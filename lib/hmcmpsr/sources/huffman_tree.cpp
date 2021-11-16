//huffman_tree.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <huffman_tree.hpp>
#include <char_frequency.hpp>
#include <genbitstream.hpp>
#include <vector>
#include <queue>
#include <tuple>
#include <iostream>

namespace hmcmpsr
{
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
};

huffman_tree::huffman_tree():m(std::make_unique<huffman_tree_impl>()){}
huffman_tree::~huffman_tree()
{
    m->free(m->root);
}

void huffman_tree::build_tree(const char_frequency_t &char_frequency,unsigned n_branches)
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
        for(int i=(char_frequency.size()-1)%(n_branches-1);i>0;--i)
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

void huffman_tree::output_impl(std::ostream &os)const
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
        os<<"   ";
    }
    if(deepth)
    {
        os<<(is_last_child[deepth-1]?"└":"├");
        os<<"── ";
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

void huffman_tree::encode(ogenbitstream &os,std::istream &is,size_t n_chars)
{
    if(m->code_unit_length==8)return encode8(os,is,n_chars);
    //TODO
}

void huffman_tree::decode(std::ostream &os,igenbitstream &is,size_t n_chars)
{
    if(m->code_unit_length==8)return decode8(os,is,n_chars);
    //TODO
}

void huffman_tree::encode8(ogenbitstream &os,std::istream &is,size_t n_chars)
{
    while(n_chars)
    {
        uint64_t ch=is.get();
        if(ch>=256)break;
        {
            auto it=m->encoding.find(ch);
            for(auto j:it->second)
            {
                os.putbit(j);
            }
        }
        n_chars--;
    }
}

void huffman_tree::decode8(std::ostream &os,igenbitstream &is,size_t n_chars)
{
    while(n_chars)
    {
        auto node=m->root;
        while(node->child.size())
        {
            unsigned bit=is.getbit();
            if(bit==GENBITSTREAM_EOF)return;
            node=node->child[bit];
        }
        os.put((unsigned char)node->ch);
        --n_chars;
    }
}

//TODO
void huffman_tree::load_tree(std::istream &is,unsigned n_branches,unsigned code_unit_length){}
void huffman_tree::save_tree(std::ostream &os){}

}
