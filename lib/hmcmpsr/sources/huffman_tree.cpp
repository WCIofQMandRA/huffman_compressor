//huffman_tree.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.
// This library is free software, you can use and 
// distrubute it under the term of the BSD 3-Clause
// License, see the file “LICENSE” for detail.

#include <hmcmpsr/huffman_tree.hpp>
#include <hmcmpsr/char_frequency.hpp>
#include <hmcmpsr/custream.hpp>
#include <hmcmpsr/genbitstream.hpp>
#include <vector>
#include <queue>
#include <tuple>
#include <iostream>

namespace hmcmpsr
{
std::unique_ptr<huffman_tree> huffman_tree::construct(unsigned n_branches)
{
    return std::make_unique<huffman_tree_g>();
}

struct huffman_tree_node
{
    huffman_tree_node()=default;
    huffman_tree_node(uint64_t ch):ch(ch){}
    uint64_t ch;
    std::vector<huffman_tree_node*> child;
};
struct huffman_tree_impl
{
    huffman_tree_node* root=nullptr;
    std::map<uint64_t,std::vector<uint8_t>> encoding;
    unsigned code_unit_length;
    void print(std::ostream&,huffman_tree_node *,int)const;
    void free(huffman_tree_node*);
    void get_encoding(huffman_tree_node *node);
    void output_dfs(huffman_tree_node *node,ocustream &os,std::vector<huffman_tree_node*> &leaves_list);
    huffman_tree_node* input_dfs(unsigned n_branches,icustream &is,std::vector<huffman_tree_node*> &leaves_list);
};

huffman_tree_g::huffman_tree_g():m(std::make_unique<huffman_tree_impl>()){}
huffman_tree_g::~huffman_tree_g()
{
    m->free(m->root);
}

void huffman_tree_g::build_tree(const char_frequency_t &char_frequency,unsigned n_branches)
{
    using qtype=std::tuple<uint64_t,uint32_t,huffman_tree_node*>;
    m->code_unit_length=char_frequency.get_code_unit_length();
    //weight,deepth,node
    //权相同时，深度小的优先，这样可以保证Huffman树的高度是O(ln(n_chars))，防止递归算法引起栈溢出
    std::priority_queue<qtype,std::vector<qtype>,std::greater<qtype>> q;
    std::map<uint64_t,huffman_tree_node*> position;
    for(auto &i:char_frequency)
    {
        auto node=new huffman_tree_node(i.first);
        q.push({i.second,0,node});
        position[i.first]=node;
    }
    if((char_frequency.size()-1)%(n_branches-1)!=0)
    {
        //加入权为0的填充字符
        for(int i=(char_frequency.size()-1)%(n_branches-1);i<n_branches-1;++i)
            q.push({0,0,nullptr});
    }
    while(q.size()!=1)
    {
        auto node=new huffman_tree_node;
        node->child.resize(n_branches);
        uint64_t weight=0;
        uint32_t deepth=0;
        for(unsigned i=0;i<n_branches;++i)
        {
            auto [w,d,n]=q.top();
            node->child[i]=n;
            weight+=w;
            deepth=std::max(deepth,d);
            q.pop();
        }
        q.push({weight,deepth+1,node});
    }
    m->root=std::get<2>(q.top());
    q.pop();
    m->get_encoding(m->root);
}

void huffman_tree_impl::get_encoding(huffman_tree_node *node)
{
    static thread_local std::vector<uint8_t> current_encoding;
    if(!node)return;
    if(node->child.empty())
    {
        encoding[node->ch]=current_encoding;
    }
    else
    {
        current_encoding.push_back(0);
        for(uint8_t i=0;i<node->child.size();++i)
        {
            *--current_encoding.end()=i;
            get_encoding(node->child[i]);
        }
        current_encoding.pop_back();
    }
}

void huffman_tree_g::output_impl(std::ostream &os)const
{
    m->print(os,m->root,0);
}

void huffman_tree_impl::print(std::ostream &os,huffman_tree_node *node,int deepth)const
{
    static thread_local std::vector<bool> is_last_child;
    //if(deepth==0)is_last_child.clear();
    for(int i=0;i<deepth-1;++i)
    {
        os<<(is_last_child[i]?" ":"│");
        os<<" ";
    }
    if(deepth)
    {
        os<<(is_last_child[deepth-1]?"└":"├");
        os<<"─";
    }

    if(node==nullptr)
        os<<"虚拟字符\n";
    else if(node->child.empty())
    {
        os<<node->ch<<"\n";
    }
    else
    {
        os<<"*\n";
        is_last_child.push_back(false);
        for(unsigned i=0;i<node->child.size();++i)
        {
            if(i==node->child.size()-1)
                is_last_child[deepth]=true;
            print(os,node->child[i],deepth+1);
        }
        is_last_child.pop_back();
    }
}

void huffman_tree_impl::free(huffman_tree_node *node)
{
    if(node)
    {
        for(auto i:node->child)
            this->free(i);
        delete node;
    }
}

void huffman_tree_g::encode(genbitsaver &os,icustream &is,size_t n_chars)
{
    while(n_chars)
    {
        uint64_t ch;
        is>>ch;
        if(!is)break;
        auto it=m->encoding.find(ch);
        if(it==m->encoding.end())
            throw std::runtime_error("huffman_tree_g::encode: 输入流中含有不在Huffman树中的字符.");
        for(auto j:it->second)
            os.putbit(j);
        --n_chars;
    }
}

void huffman_tree_g::decode(ocustream &os,genbitloader &is)
{
    while(true)
    {
        auto node=m->root;
        while(node->child.size())
        {
            unsigned bit=is.getbit();
            if(bit==GENBITSTREAM_EOF)return;
            node=node->child[bit];
            if(!node)
                throw std::runtime_error("huffman_tree_g::decode: 输入流中含有不在Huffman树中的编码");
        }
        os<<node->ch;
    }
}

void huffman_tree_g::load_tree(std::istream &is)
{
    unsigned n_branches=(uint8_t)is.get();
    m->code_unit_length=(uint8_t)is.get();
    std::vector<huffman_tree_node*> leaves;
    if(m->root)
        m->free(m->root);
    {
        icustream_124 stream(2,is);
        m->root=m->input_dfs(n_branches,stream,leaves);
    }
    {
        auto stream=icustream::construct(m->code_unit_length,is);
        for(auto i:leaves)
        {
            *stream>>(i->ch);
            if(!*stream)
                throw std::runtime_error("huffman_tree_g::load_tree: Huffman树格式错误-字符");
        }
    }
    m->get_encoding(m->root);
}

void huffman_tree_g::save_tree(std::ostream &os)
{
    uint8_t n_branches=m->root->child.size();
    uint8_t code_unit_length=m->code_unit_length;
    os.put(n_branches);
    os.put(code_unit_length);
    std::vector<huffman_tree_node*> leaves;
    {
        ocustream_124 stream(2,os);
        m->output_dfs(m->root,stream,leaves);
        stream.sync();
    }
    {
        auto stream=ocustream::construct(m->code_unit_length,os);
        for(auto i:leaves)
            *stream<<(i->ch);
        stream->sync();
    }
}

void huffman_tree_impl::output_dfs(huffman_tree_node *node,ocustream &os,std::vector<huffman_tree_node*> &leaves_list)
{
    if(!node)os<<3;
    else if(node->child.empty())
    {
        os<<2;
        leaves_list.push_back(node);
    }
    else
    {
        os<<0;
        for(auto i:node->child)
            output_dfs(i,os,leaves_list);
    }
}

huffman_tree_node* huffman_tree_impl::input_dfs(unsigned n_branches,icustream &is,std::vector<huffman_tree_node*> &leaves_list)
{
    uint64_t ch;
    is>>ch;
    if(!is)throw std::runtime_error("huffman_tree_impl::input_dfs: Huffman树格式错误-形态_0");
    if(ch==3)return nullptr;
    else if(ch==2)
    {
        huffman_tree_node *node=new huffman_tree_node;
        leaves_list.push_back(node);
        return node;
    }
    else if(ch==0)
    {
        huffman_tree_node *node=new huffman_tree_node;
        node->child.resize(n_branches);
        for(auto &i:node->child)
            i=input_dfs(n_branches,is,leaves_list);
        return node;
    }
    else throw std::runtime_error("huffman_tree_impl::input_dfs: Huffman树格式错误-形态_1");
}

unsigned huffman_tree_g::get_code_unit_length()const
{
    return m->code_unit_length;
}

unsigned huffman_tree_g::get_n_branches()const
{
    return m->root->child.size();
}

}
