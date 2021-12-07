//splay_tree.hpp
//Copyright (C) 2021 张子辰

// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.

//Revision:
//2021-12-08: Change `size_t' to `std::size_t', in case `size_t' is not in
//the global namespace.
#pragma once
#include <utility>

namespace zzc
{namespace detail{
template<typename T>
struct splay_tree_node
{
    struct construct_data_t{explicit construct_data_t()=default;};
    static constexpr construct_data_t construct_data{};
	splay_tree_node *left,*right,*parent;
	std::size_t cnt;//以该结点为根的子树的大小
    T data;
	splay_tree_node()=default;
	template<typename ...Args>
	splay_tree_node(construct_data_t,Args &&...args):data(std::forward<Args>(args)...){}
};

//伸展树
template<typename T>
class splay_tree_impl
{
public:
    using Node=splay_tree_node<T>;
    static void initalize_root(Node *root)noexcept
    {
        root->left=nullptr,root->right=nullptr,root->parent=nullptr;
        root->cnt=1;
    }

    template<typename Func,typename U>
    static Node* search(Node *root,Func &&cmp,U &&key)
    {
        auto p=root->left;
        while(p)
        {
            if(cmp(key,p->data))p=p->left;
            else if(cmp(p->data,key))p=p->right;
            else
            {
                splay(p);
                return p;
            }
        }
        return nullptr;
    }

    template<typename Func,typename U>
    static std::pair<Node*,int> search2(Node *root,Func &&cmp,U &&key)
    {
        auto p=root->left;
        if(!p)return {root,-1};
		while(true)
		{
			if(cmp(key,p->data))
			{
				if(p->left)p=p->left;
				else return {p,-1};
			}
			else if(cmp(p->data,key))
			{
				if(p->right)p=p->right;
				else return {p,1};
			}
			else return {p,0};
		}
    }

    template<typename Func,typename U>
    static std::pair<Node*,int> search2_hint(Node *root,Func &&cmp,U &&key,Node *hint)
    {
        if(hint==root)return search2(root,std::forward<Func>(cmp),std::forward<U>(key));
        //如果key<hint, 则只要pre_hint<key即可插入
        if(cmp(key,hint->data))
        {
            auto pre=predecessor_no_splay(hint);
            if(pre&&!cmp(pre->data,key))
                return search2(root,std::forward<Func>(cmp),std::forward<U>(key));
        }
        //如果hint<key，则只要key<suc_hint即可插入
        else if(cmp(hint->data,key))
        {
            auto suc=successor_no_splay(hint);
            if(suc&&!cmp(key,suc->data))
                return search2(root,std::forward<Func>(cmp),std::forward<U>(key));
        }
        else return {hint,0};//key已存在
        Node *p=hint;
		while(true)
		{
			if(cmp(key,p->data))
			{
				if(p->left)p=p->left;
				else return {p,-1};
			}
			else if(cmp(p->data,key))
			{
				if(p->right)p=p->right;
				else return {p,1};
			}
			else return {p,0};
		}
    }

    static void search2_no_insert(Node *node)
    {
        splay(node);
    }

    static Node* min(Node *root)noexcept
    {
        auto p=root;
		while(p->left)p=p->left;
        splay(p);
		return p;
    }

    static Node* predecessor(Node* p)noexcept
    {
        p=predecessor_no_splay(p);
        if(p)splay(p);
        return p;
    }

    static Node* successor(Node* p)noexcept
    {
        p=successor_no_splay(p);
        if(p)splay(p);
        return p;
    }

    static void insert(Node *p,int dire,Node *const x)noexcept
    {
        if(dire==-1)p->left=x;
        else p->right=x;
		x->parent=p;
		x->left=nullptr;
		x->right=nullptr;
		x->cnt=1;
		while(p)
		{
			++p->cnt;
			p=p->parent;
		}
        splay(x);
    }

    static void erase(Node *x)noexcept
    {
        splay(x);
        auto root=x->parent;
        if(!x->left)
		{
			root->left=x->right;
			if(x->right)x->right->parent=root;
			--root->cnt;
		}
		else if(!x->right)
		{
			root->left=x->left;
			x->left->parent=root;
			--root->cnt;
		}
		else
		{
			auto p=x->right;
			if(!p->left)
			{
				p->left=x->left;
				x->left->parent=p;
				root->left=p;
				p->parent=root;
				p->cnt+=p->left->cnt;
                --root->cnt;
			}
			else
			{
				do p=p->left; while(p->left);
				auto tmp=p->parent;
				tmp->left=p->right;
				if(p->right)p->right->parent=tmp;
				p->right=x->right;
				x->right->parent=p;
				p->left=x->left;
				x->left->parent=p;
				root->left=p;
				p->parent=root;
				p->cnt=x->cnt;
				while(tmp)--tmp->cnt,tmp=tmp->parent;
			}
		}
    }

    template<typename Func>
    static void merge(Node *root,Func &&cmp,Node *other)
    {
        auto p=min(other);
        while(p!=other)
        {
            auto [pos,dire]=search2(root,cmp,p->data);
            if(dire)
            {
                auto tmp=successor_no_splay(p);
                erase(p);
                insert(pos,dire,p);
                p=tmp;
            }
            else p=successor_no_splay(p);
        }
    }

    static std::size_t rank(Node *p)noexcept
    {
        splay(p);
        std::size_t c=p->left?(p->left->cnt):0;
		while(p->parent)
		{
			if(p==p->parent->right)
				c+=(p->parent->cnt)-(p->cnt);
			p=p->parent;
		}
		return c;
    }

    static Node* select(Node *root,std::size_t r)noexcept
    {
        auto p=root;
		while(p)
		{
			std::size_t lsize=p->left?p->left->cnt:0;
			if(r==lsize)
            {
                splay(p);
                return p;
            }
			else if(r<lsize)p=p->left;
			else r-=(lsize+1),p=p->right;
		}
		return nullptr;
    }
private:
    static Node* predecessor_no_splay(Node* p)noexcept
    {
        if(p->left)
		{
			p=p->left;
			while(p->right)
				p=p->right;
			return p;
		}
		while(p->parent)
		{
			if(p==p->parent->right)
            {
                p=p->parent;
				return p;
            }
			p=p->parent;
		}
		return nullptr;
    }

    static Node* successor_no_splay(Node* p)noexcept
    {
        if(p->right)
		{
			p=p->right;
			while(p->left)p=p->left;
			return p;
		}
		while(p->parent)
		{
			if(p==p->parent->left)
            {
                p=p->parent;
				return p;
            }
			p=p->parent;
		}
		return nullptr;
    }

    static void splay(Node *node)noexcept
    {
        if(!node->parent)return;
        while(node->parent->parent)
        {
            Node *p=node->parent;
            Node *pp=p->parent;
            if(node==p->left)
            {
                if(!pp->parent){zig(node,p);break;}
                else if(p==pp->left)zig(p,pp),zig(node,p);
                else zig(node,p),zag(node,pp);
            }
            else
            {
                if(!pp->parent){zag(node,p);break;}
                else if(p==pp->right)zag(p,pp),zag(node,p);
                else zag(node,p),zig(node,pp);
            }
        }
    }
    //node==node->parent->left, 把node旋至parent的位置
    static void zig(Node *node,Node *p)noexcept
    {
        p->left=node->right;
        std::size_t c=0,a=(p->cnt)-(node->cnt);
        if(node->right)node->right->parent=p,c=node->right->cnt;
        p->cnt=a+c;
        node->cnt+=a;
        if(p==p->parent->left)p->parent->left=node;
        else p->parent->right=node;
        node->parent=p->parent;
        node->right=p;
        p->parent=node;
    }
    //node==node->parent->right, 把node旋至parent的位置
    static void zag(Node *node,Node *p)noexcept
    {
        p->right=node->left;
        std::size_t c=0,a=(p->cnt)-(node->cnt);
        if(node->left)node->left->parent=p,c=node->left->cnt;
        p->cnt=a+c;
        node->cnt+=a;
        if(p==p->parent->left)p->parent->left=node;
        else p->parent->right=node;
        node->parent=p->parent;
        node->left=p;
        p->parent=node;
    }
};
}}//namespace zzc

#include <functional>//for std::less
#include <memory>//for std::allocator
#include "map_wrapper.hpp"

namespace zzc
{
template<typename Key,typename T,
	typename Compare=std::less<Key>,
	typename Allocator=std::allocator<std::pair<const Key,T>>>
using splay_tree=tree_map_wrapper<Key,T,Compare,Allocator,
	detail::splay_tree_impl<std::pair<const Key,T>>>;
}
