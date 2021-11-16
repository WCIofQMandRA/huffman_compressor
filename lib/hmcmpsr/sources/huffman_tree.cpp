//huffman_tree.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <huffman_tree.hpp>
#include <char_frequency.hpp>
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
    huffman_tree_node *parent=nullptr;
    unsigned which_child=0;//this==this->parent->child[this->which_child]
    std::vector<huffman_tree_node*> child;
};
struct huffman_tree_impl
{
    huffman_tree_node* root;
    std::map<uint64_t,huffman_tree_node*> position;
    std::map<uint64_t,std::vector<bool>> encoding;
    void print(std::ostream&,huffman_tree_node *,int)const;
    void free(huffman_tree_node*);
};

huffman_tree::huffman_tree():m(std::make_unique<huffman_tree_impl>()){}
huffman_tree::~huffman_tree()
{
    m->free(m->root);
}

void huffman_tree::build_tree(const char_frequency_t &char_frequency,unsigned n_branches)
{
    using qtype=std::tuple<uint64_t,uint32_t,huffman_tree_node*>;
    //weight,deepth,node
    //权相同时，深度小的优先，这样可以保证Huffman树的高度是O(ln(n_chars))，防止递归算法引起栈溢出
    std::priority_queue<qtype,std::vector<qtype>,std::greater<qtype>> q;
    for(auto &i:char_frequency)
    {
        auto node=new huffman_tree_node(i.first);
        q.push({i.second,0,node});
        m->position[i.first]=node;
    }
    if((char_frequency.size()-1)%(n_branches-1)!=0)
    {
        //加入权为0的填充字符
        for(int i=(char_frequency.size()-1)%(n_branches-1);i>=0;--i)
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
            n->parent=node;
            n->which_child=i;
            weight+=w;
            deepth=std::max(deepth,d);
            q.pop();
        }
        q.push({weight,deepth+1,node});
    }
    m->root=std::get<2>(q.top());
    q.pop();
}

std::ostream& operator<<(std::ostream &os,const huffman_tree &t)
{
    t.m->print(os,t.m->root,0);
    return os;
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

}
