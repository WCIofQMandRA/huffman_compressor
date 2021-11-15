//huffman_tree.cpp
//Copyright (C) 2021-2022 张子辰
//This file is part of the hmcmpsr library.

#include <huffman_tree.hpp>
#include <vector>
#include <queue>
namespace hmcmpsr
{
struct huffman_tree_node
{
    uint64_t ch;
    std::vector<huffman_tree_node*> child;
};
struct huffman_tree_impl
{
    huffman_tree_node *root;
};

huffman_tree::huffman_tree():m(std::make_unique<huffman_tree_impl>()){}
huffman_tree::~huffman_tree(){}

void huffman_tree::build_tree(const char_frequency_t &char_frequency,unsigned n_branches)
{
    std::priority_queue<std::pair<uint64_t,huffman_tree_node*>,
        std::vector<std::pair<uint64_t,huffman_tree_node*>>,
        std::greater<std::pair<uint64_t,huffman_tree_node*>>> q;
    for(auto &i:char_frequency)
    {
        auto node=new huffman_tree_node{i.first,{}};
        q.push({i.second,node});
    }
    if((char_frequency.size()-1)%(n_branches-1)!=0)
    {
        //加入权为0的填充字符
        for(int i=(char_frequency.size()-1)%(n_branches-1);i>=0;--i)
            q.push({0,nullptr});
    }
    while(q.size()!=1)
    {
        auto node=new huffman_tree_node;
        node->child.resize(n_branches);
        uint64_t weight=0;
        for(auto &i:node->child)
        {
            i=q.top().second;
            weight+=q.top().first;
            q.pop();
        }
        q.push({weight,node});
    }
    m->root=q.top().second;
}
}
