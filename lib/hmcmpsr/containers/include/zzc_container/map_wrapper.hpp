//map_wrapper.hpp
//Copyright (C) 2021 张子辰

// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.
#pragma once
#include <memory> // allocator
#include <utility> //pair, swap, forward, move
#include <iterator>
#include <tuple> // forward_as_tuple
#include <type_traits>
#include <initializer_list>
#include <stdexcept> // out_of_range
#include <limits> // numeric_limits

namespace zzc
{
//接口与C++17中的结点柄相同
template<typename Node,typename Alloc,typename Friend>
class __node_type_map
{
public:
    friend Friend;
    using key_type=std::remove_const_t<decltype(std::declval<Node>().data.first)>;
    using mapped_type=decltype(std::declval<Node>().data.second);
    using allocator_type=Alloc;
    constexpr __node_type_map()noexcept:
        real_node(nullptr){}
    __node_type_map(__node_type_map &&other)noexcept:
        real_node(other.real_node)
    {
        other.real_node=nullptr;
    }
    __node_type_map& operator=(__node_type_map &&other)
    {
        if(real_node)my_delete(real_node);
        real_node=other.real_node;
        other.real_node=nullptr;
        return *this;
    }
    ~__node_type_map()
    {
        if(real_node)my_delete(real_node);
    }
    bool empty()const noexcept
    {
        return real_node==nullptr;
    }
    explicit operator bool()const noexcept
    {
        return real_node!=nullptr;
    }
    Alloc get_allocator()const
    {
        return alloc;
    }
    key_type& key()const
    {
        return real_node->data.first;
    }
    mapped_type& mapped()const
    {
        return real_node->data.second;
    }
    void swap(__node_type_map &other)
        noexcept(std::allocator_traits<allocator_type>::propagate_on_container_swap::value ||
        std::allocator_traits<allocator_type>::is_always_equal::value)
    {
        if(this==&other)return;
        std::swap(alloc,other.alloc);
        std::swap(real_node,other.real_node);
    }
    friend void swap(__node_type_map &x,__node_type_map &y)noexcept(noexcept(x.swap(y)))
    {
        x.swap(y);
    }
private:
    inline void my_delete(Node *x)
    {
        std::allocator_traits<Alloc>::destroy(alloc,x);
        std::allocator_traits<Alloc>::deallocate(alloc,x,1);
    }
    __node_type_map(Node *real_node,const Alloc &alloc=Alloc()):
        real_node(real_node),alloc(alloc){}
    Node *real_node;
    Alloc alloc;
};

//
template<typename Iter,typename NodeType>
struct __insert_return_type
{
    Iter position;
    bool inserted;
    NodeType node;
};

template<typename Key,typename T,typename Cmp,typename Alloc,typename Impl>
class tree_multimap_wrapper;

//接口与std::map相同，除了迭代器为 遗留随机访问迭代器
// Key T Cmp的含义与std::map相同
// Alloc: 分配器，要求分配的类型必须是Impl::Node
// Impl: 搜索树的实现
template<typename Key,typename T,typename Cmp,typename Alloc,typename Impl>
class tree_map_wrapper
{
    using Node=typename Impl::Node;
//成员类型
public:
    using key_type=Key;
    using mapped_type=T;
    using value_type=std::pair<const Key,T>;
    using size_type=std::size_t;
    using difference_type=std::ptrdiff_t;
    using key_compare=Cmp;
    using allocator_type=typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
    using reference=value_type&;
    using const_reference=const value_type&;
    using pointer=typename std::allocator_traits<Alloc>::pointer;
    using const_pointer=typename std::allocator_traits<Alloc>::const_pointer;
    using node_type=__node_type_map<Node,Alloc,tree_map_wrapper>;

    class value_compare
    {
    public:
        friend class tree_map_wrapper;
        value_compare()=default;
        explicit value_compare(const Cmp &c):cmp(c){}
        template<typename U,typename V>
        bool operator()(const std::pair<const Key,U> &x,const std::pair<const Key,V> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y.first);
        }
        template<typename U,typename V>
        bool operator()(const std::pair<Key,U> &x,const std::pair<const Key,V> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y.first);
        }
        template<typename U,typename V>
        bool operator()(const std::pair<const Key,U> &x,const std::pair<Key,V> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y.first);
        }
        template<typename U,typename V>
        bool operator()(const std::pair<Key,U> &x,const std::pair<Key,V> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y.first);
        }
        template<typename U>
        bool operator()(const Key &x,const std::pair<const Key,U> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x,y.first);
        }
        template<typename U>
        bool operator()(const Key &x,const std::pair<Key,U> &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x,y.first);
        }
        template<typename U>
        bool operator()(const std::pair<const Key,U> &x,const Key &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y);
        }
        template<typename U>
        bool operator()(const std::pair<Key,U> &x,const Key &y)const
            noexcept(std::is_nothrow_invocable_v<Cmp,const Key,const Key>)
        {
            return cmp(x.first,y);
        }
    private:
        Cmp cmp;
    };
    
//迭代器
public:
    class const_iterator;
    //遗留随机访问迭代器 LegacyRandomAccessIterator
    class iterator
    {
        using self_type=iterator;
        friend class tree_map_wrapper;
        friend class const_iterator;
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=std::pair<const Key,T>;
        using difference_type=std::ptrdiff_t;
        using pointer=value_type*;
        using reference=value_type&;

        iterator()=default;
        reference operator*()const noexcept{return node->data;}
        pointer operator->()const noexcept{return &node->data;}
        bool operator==(self_type other)const noexcept{return node==other.node;}
        bool operator!=(self_type other)const noexcept{return node!=other.node;}
        
        self_type& operator++()noexcept
        {
            node=Impl::successor(node);
            return *this;
        }
        self_type operator++(int)noexcept
        {
            auto tmp=*this;
            node=Impl::successor(node);
            return tmp;
        }
        self_type& operator--()noexcept
        {
            node=Impl::predecessor(node);
            return *this;
        }
        self_type operator--(int)noexcept
        {
            auto tmp=*this;
            node=Impl::predecessor(node);
            return tmp;
        }
        
        self_type& operator+=(difference_type n)noexcept
        {
            size_type i=Impl::rank(node)+n;
            node=Impl::select(root,i);
            return *this;
        }
        self_type operator+(difference_type n)const noexcept
        {
            auto x=*this;
            return x+=n;
        }
        friend self_type operator+(difference_type n,self_type x)noexcept
        {
            return x+=n;
        }
        self_type& operator-=(difference_type n)noexcept
        {
            size_type i=Impl::rank(node)-n;
            node=Impl::select(root,i);
            return *this;
        }
        self_type operator-(difference_type n)const noexcept
        {
            auto x=*this;
            return x-=n;
        }
        difference_type operator-(self_type other)const noexcept
        {
            return static_cast<difference_type>(Impl::rank(node)-Impl::rank(other.node));
        }
        reference operator[](size_type n)const noexcept
        {
            return *(*this+n);
        }

        bool operator<(self_type other)const noexcept{return Impl::rank(node)<Impl::rank(other.node);}
        bool operator>(self_type other)const noexcept{return Impl::rank(node)>Impl::rank(other.node);}
        bool operator<=(self_type other)const noexcept{return Impl::rank(node)<=Impl::rank(other.node);}
        bool operator>=(self_type other)const noexcept{return Impl::rank(node)>=Impl::rank(other.node);}
    private:
        Node *node=nullptr, *root=nullptr;
        iterator(Node *p,Node *root):node(p),root(root){}
    };
    class const_iterator
    {
        using self_type=const_iterator;
        friend class tree_map_wrapper;
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=std::pair<const Key,T>;
        using difference_type=std::ptrdiff_t;
        using pointer=const value_type*;
        using reference=const value_type&;

        const_iterator()=default;
        const_iterator(const iterator &it):
            node(it.node),root(it.root){}
        reference operator*()const noexcept{return node->data;}
        pointer operator->()const noexcept{return &node->data;}
        bool operator==(self_type other)const noexcept{return node==other.node;}
        bool operator!=(self_type other)const noexcept{return node!=other.node;}
        
        self_type& operator++()noexcept
        {
            node=Impl::successor(node);
            return *this;
        }
        self_type operator++(int)noexcept
        {
            auto tmp=*this;
            node=Impl::successor(node);
            return tmp;
        }
        self_type& operator--()noexcept
        {
            node=Impl::predecessor(node);
            return *this;
        }
        self_type operator--(int)noexcept
        {
            auto tmp=*this;
            node=Impl::predecessor(node);
            return tmp;
        }
        
        self_type& operator+=(difference_type n)noexcept
        {
            size_type i=Impl::rank(node)+n;
            node=Impl::select(root,i);
            return *this;
        }
        self_type operator+(difference_type n)const noexcept
        {
            auto x=*this;
            return x+=n;
        }
        friend self_type operator+(difference_type n,self_type x)noexcept
        {
            return x+=n;
        }
        self_type& operator-=(difference_type n)noexcept
        {
            size_type i=Impl::rank(node)-n;
            node=Impl::select(root,i);
            return *this;
        }
        self_type operator-(difference_type n)const noexcept
        {
            auto x=*this;
            return x-=n;
        }
        difference_type operator-(self_type other)const noexcept
        {
            return static_cast<difference_type>(Impl::rank(node)-Impl::rank(other.node));
        }
        reference operator[](size_type n)const noexcept
        {
            return *(*this+n);
        }

        bool operator<(self_type other)const noexcept{return Impl::rank(node)<Impl::rank(other.node);}
        bool operator>(self_type other)const noexcept{return Impl::rank(node)>Impl::rank(other.node);}
        bool operator<=(self_type other)const noexcept{return Impl::rank(node)<=Impl::rank(other.node);}
        bool operator>=(self_type other)const noexcept{return Impl::rank(node)>=Impl::rank(other.node);}
    private:
        Node *node=nullptr, *root=nullptr;
        const_iterator(Node *p,Node *root):node(p),root(root){}
    };
    using reverse_iterator=std::reverse_iterator<iterator>;
    using const_reverse_iterator=std::reverse_iterator<const_iterator>;

    using insert_return_type=__insert_return_type<iterator,node_type>;
//构造函数
public:
    //(1)
    tree_map_wrapper(){Impl::initalize_root(root);}
    //(1)
    explicit tree_map_wrapper(const key_compare &comp,const allocator_type &alloc=allocator_type()):
        cmp(comp),alloc(alloc){Impl::initalize_root(root);}
    //(1) C++11起
    explicit tree_map_wrapper(const allocator_type &alloc):
        alloc(alloc){Impl::initalize_root(root);}
    //(2)
    template<typename InputIt>
    tree_map_wrapper(InputIt first,InputIt last,const key_compare &comp=key_compare(),const allocator_type &alloc=allocator_type()):
        cmp(comp),alloc(alloc)
    {
        Impl::initalize_root(root);
        for(auto it=first;it!=last;++it)
        {
            if(auto [pos,dire]=Impl::search2(root,cmp,*it);dire)
            {
                auto node=my_new(Node::construct_data,*it);
                Impl::insert(pos,dire,node);
            }
            else Impl::search2_no_insert(pos);
        }
    }
    //(2) C++14起
    template<typename InputIt>
    tree_map_wrapper(InputIt first,InputIt last,const allocator_type &alloc):
        tree_map_wrapper(first,last,key_compare(),alloc){}
    //(3)
    tree_map_wrapper(const tree_map_wrapper &other):
        tree_map_wrapper(other,other.alloc){}
    //(3) C++11起
    tree_map_wrapper(const tree_map_wrapper &other,const allocator_type &alloc):
        cmp(other.cmp),alloc(alloc)
    {
        root=copy_subtree(other.root);
    }
    //(4) C++11起
    tree_map_wrapper(tree_map_wrapper &&other):
        cmp(std::move(other.cmp)),alloc(std::move(other.alloc)),root(other.root)
    {
        other.root=other.my_new_raw();
        Impl::initalize_root(other.root);
    }
    //(4) C++11起
    tree_map_wrapper(tree_map_wrapper &&other,const allocator_type &alloc):
        cmp(std::move(other.cmp)),alloc(alloc),root(other.root)
    {
        other.root=other.my_new_raw();
        Impl::initalize_root(other.root);
    }
    //(5) C++11起
    tree_map_wrapper(std::initializer_list<value_type> init,const key_compare &comp=key_compare(),
        const allocator_type &alloc=allocator_type()):
        tree_map_wrapper(init.begin(),init.end(),comp,alloc){}
    //(5) C++14起
    tree_map_wrapper(std::initializer_list<value_type> init,const allocator_type &alloc):
        tree_map_wrapper(init.begin(),init.end(),key_compare(),alloc){}
//析构函数
public:
    ~tree_map_wrapper()
    {
        delete_subtree(root->left);
        my_delete_raw(root);
    }
//operator=
public:
    //(1)
    tree_map_wrapper& operator=(const tree_map_wrapper &other)
    {
        delete_subtree(root->left);
        cmp=other.cmp;
        alloc=other.alloc;
        root=copy_subtree(other.root);
        return *this;
    }
    //(2) C++17 起
    tree_map_wrapper& operator=(tree_map_wrapper &&other)
        noexcept(std::allocator_traits<Alloc>::is_always_equal::value
        && std::is_nothrow_move_assignable<Cmp>::value)
    {
        delete_subtree(root->left);
        my_delete_raw(root);
        cmp=std::move(other.cmp);
        alloc=std::move(other.alloc);
        root=std::move(other.root);
        other.root=other.my_new_raw();
        Impl::initalize_root(other.root);
        return *this;
    }
    //(3) C++11 起
    tree_map_wrapper& operator=(std::initializer_list<value_type> ilist)
    {
        delete_subtree(root->left);
        Impl::initalize_root(root);
        for(auto &i:ilist)
        {
            if(auto [pos,dire]=Impl::search2(root,cmp,i);dire!=0)
            {
                auto node=my_new(Node::construct_data,i);
                insert(pos,dire,node);
            }
            else Impl::search2_no_insert(pos);
        }
        return *this;
    }
public:
    //(1) C++11起
    allocator_type get_allocator() const noexcept
    {
        return alloc;
    }
//元素访问
public:
    //at (1)
    T& at(const Key &key)
    {
        auto p=Impl::search(root,cmp,key);
        if(!p)throw std::out_of_range("tree_map_wrapper::at()");
        return p->data.second;
    }
    //at (2)
    const T& at(const Key &key)const
    {
        auto p=Impl::search(root,cmp,key);
        if(!p)throw std::out_of_range("tree_map_wrapper::at()const");
        return p->data.second;
    }
    //operator[] (1)
    T& operator[](const key_type &key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        if(dire)
        {
            auto node=my_new(Node::construct_data,key,T());
            Impl::insert(pos,dire,node);
            return node->data.second;
        }
        else
        {
            Impl::search2_no_insert(pos);
            return pos->data.second;
        }
    }
    //operator[] (2) C++11 起
    T& operator[](key_type &&key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::move(key),T());
            Impl::insert(pos,dire,node);
            return node->data.second;
        }
        else
        {
            Impl::search2_no_insert(pos);
            return pos->data.second;
        }
    }
//迭代器
public:
    iterator begin()noexcept{return iterator(Impl::min(root),root);}
    const_iterator begin()const noexcept{return const_iterator(Impl::min(root),root);}
    const_iterator cbegin()const noexcept{return const_iterator(Impl::min(root),root);}
    iterator end()noexcept{return iterator(root,root);}
    const_iterator end()const noexcept{return const_iterator(root,root);}
    const_iterator cend()const noexcept{return const_iterator(root,root);}
    reverse_iterator rbegin()noexcept{return reverse_iterator(iterator(root,root));}
    const_reverse_iterator rbegin()const noexcept{return const_reverse_iterator(const_iterator(root,root));}
    const_reverse_iterator crbegin()const noexcept{return const_reverse_iterator(const_iterator(root,root));}
    reverse_iterator rend()noexcept{return reverse_iterator(iterator(Impl::min(root),root));}
    const_reverse_iterator rend()const noexcept{return const_reverse_iterator(const_iterator(Impl::min(root),root));}
    const_reverse_iterator crend()const noexcept{return const_reverse_iterator(const_iterator(Impl::min(root),root));}
//容量
public:
    bool empty()const noexcept{return root->cnt==1;}
    size_type size()const noexcept{return (root->cnt)-1;}
    constexpr size_type max_size()const noexcept
    {
        return std::numeric_limits<size_type>::max()/sizeof(Node);
    }
//修改器
public:
    //clear C++11起
    void clear()noexcept
    {
        delete_subtree(root->left);
        Impl::initalize_root(root);
    }
    //insert (1)
    std::pair<iterator,bool> insert(const value_type &value)
    {
        auto [pos,dire]=Impl::search2(root,cmp,value);
        if(dire)
        {
            auto node=my_new(Node::construct_data,value);
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //insert (2) C++11起
    template<typename P,typename=std::enable_if_t<std::is_constructible_v<value_type,P&&>>>
    std::pair<iterator,bool> insert(P &&value)
    {
        auto [pos,dire]=Impl::search2(root,cmp,value);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::forward<P>(value));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //insert (3) C++17起
    std::pair<iterator,bool> insert(value_type &&value)
    {
        auto [pos,dire]=Impl::search2(root,cmp,value);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::move(value));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //insert (4) C++11起
    iterator insert(const_iterator hint,const value_type &value)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,value,hint.node);
        if(dire)
        {
            auto node=my_new(Node::construct_data,value);
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //insert (5) C++11起
    template<typename P,typename=std::enable_if_t<std::is_constructible_v<value_type,P&&>>>
    iterator insert(const_iterator hint,P &&value)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,value,hint.node);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::forward<P>(value));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //insert (6) C++17起
    iterator insert(const_iterator hint,value_type &&value)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,value,hint.node);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::move(value));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //insert (7)
    template<typename InputIt>
    void insert(InputIt first,InputIt last)
    {
        for(auto i=first;i!=last;++i)
            insert(*i);
    }
    //insert (8) C++11起
    void insert(std::initializer_list<value_type> ilist)
    {
        for(auto &i:ilist)
            insert(i);
    }
    //insert (9) C++17起
    insert_return_type insert(node_type &&nh)
    {
        if(!nh.real_node)
            return insert_return_type{end(),false,node_type()};
        auto [pos,dire]=Impl::search2(root,cmp,nh.real_node);
        if(dire)
        {
            Impl::insert(pos,dire,nh.real_node);
            pos=nh.real_node;
            nh.real_node=nullptr;
            return insert_return_type{iterator(pos,root),true,node_type()};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return insert_return_type{iterator(pos,root),false,std::move(nh)};
        }
    }
    //insert (10) C++17起
    iterator insert(const_iterator hint,node_type &&nh)
    {
        if(!nh.real_node)return end();
        auto [pos,dire]=Impl::search2_hint(root,cmp,nh.real_node,hint);
        if(dire)
        {
            Impl::insert(pos,dire,nh.real_node);
            pos=nh.real_node;
            nh.real_node=nullptr;
            return iterator(pos,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return end();
        }
    }
    //insert_or_assign (1) C++17起
    template<typename M>
    std::pair<iterator,bool> insert_or_assign(const key_type &k,M &&obj)
    {
        auto [pos,dire]=Impl::search2(root,cmp,k);
        if(dire)
        {
            auto node=my_new(Node::construct_data,k,std::forward<M>(obj));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            pos->data.second=std::forward<M>(obj);
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //insert_or_assign (2) C++17起
    template<typename M>
    std::pair<iterator,bool> insert_or_assign(key_type &&k,M &&obj)
    {
        auto [pos,dire]=Impl::search2(root,cmp,k);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::move(k),std::forward<M>(obj));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            pos->data.second=std::forward<M>(obj);
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //insert_or_assign (3) C++17起
    template<typename M>
    iterator insert_or_assign(const_iterator hint,const key_type &k,M &&obj)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,k,hint);
        if(dire)
        {
            auto node=my_new(Node::construct_data,k,std::forward<M>(obj));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            pos->data.second=std::forward<M>(obj);
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //insert_or_assign (4) C++17起
    template<typename M>
    iterator insert_or_assign(const_iterator hint,key_type &&k,M &&obj)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,k,hint.node);
        if(dire)
        {
            auto node=my_new(Node::construct_data,std::move(k),std::forward<M>(obj));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            pos->data.second=std::forward<M>(obj);
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //emplace C++11起
    template<typename ...Args>
    std::pair<iterator,bool> emplace(Args &&...args)
    {
        auto node=my_new(Node::construct_data,std::forward<Args>(args)...);
        auto [pos,dire]=Impl::search2(root,cmp,node->data);
        if(dire)
        {
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            my_delete(node);
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //emplace_hint C++11起
    template<typename ...Args>
    iterator emplace_hint(const_iterator hint,Args &&...args)
    {
        auto node=my_new(Node::construct_data,std::forward<Args>(args)...);
        auto [pos,dire]=Impl::search2_hint(root,cmp,node->data,hint.node);
        if(dire)
        {
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            my_delete(node);
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //try_emplace (1) C++17起
    template<typename ...Args>
    std::pair<iterator,bool> try_emplace(const key_type &k,Args &&...args)
    {
        auto [pos,dire]=Impl::search2(root,cmp,k);
        if(dire)
        {
            auto node=my_new(Node::construct_data,
                std::piecewise_construct,std::forward_as_tuple(k),
                std::forward_as_tuple(std::forward<Args>(args)...));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //try_emplace (2) C++17起
    template<typename ...Args>
    std::pair<iterator,bool> try_emplace(key_type &&k,Args &&...args)
    {
        auto [pos,dire]=Impl::search2(root,cmp,k);
        if(dire)
        {
            auto node=my_new(Node::construct_data,
                std::piecewise_construct,std::forward_as_tuple(std::move(k)),
                std::forward_as_tuple(std::forward<Args>(args)...));
            Impl::insert(pos,dire,node);
            return {iterator(node,root),true};
        }
        else
        {
            Impl::search2_no_insert(pos);
            return {iterator(pos,root),false};
        }
    }
    //try_emplace (3) C++17起
    template<typename ...Args>
    iterator try_emplace(const_iterator hint,const key_type &k,Args &&...args)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,k,hint);
        if(dire)
        {
            auto node=my_new(Node::construct_data,
                std::piecewise_construct,std::forward_as_tuple(k),
                std::forward_as_tuple(std::forward<Args>(args)...));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //try_emplace (4) C++17起
    template<typename ...Args>
    iterator try_emplace(const_iterator hint,key_type &&k,Args &&...args)
    {
        auto [pos,dire]=Impl::search2_hint(root,cmp,k,hint);
        if(dire)
        {
            auto node=my_new(Node::construct_data,
                std::piecewise_construct,std::forward_as_tuple(std::move(k)),
                std::forward_as_tuple(std::forward<Args>(args)...));
            Impl::insert(pos,dire,node);
            return iterator(node,root);
        }
        else
        {
            Impl::search2_no_insert(pos);
            return iterator(pos,root);
        }
    }
    //erase (1) C++11起
    iterator erase(const_iterator pos)
    {
        auto succ=Impl::successor(pos.node);
        Impl::erase(pos.node);
        my_delete(pos.node);
        return iterator(succ,root);
    }
    //erase (1) C++17起
    iterator erase(iterator pos)
    {
        auto succ=Impl::successor(pos.node);
        Impl::erase(pos.node);
        my_delete(pos.node);
        return iterator(succ,root);
    }
    //erase (2) C++11起
    iterator erase(const_iterator first,const_iterator last)
    {
        auto node=first.node;
        while(node!=last.node)
        {
            auto succ=Impl::successor(node);
            Impl::erase(node);
            my_delete(node);
            node=succ;
        }
        return iterator(node,root);
    }
    //erase (3)
    std::size_t erase(const key_type &key)
    {
        auto node=Impl::search(root,cmp,key);
        if(node)
        {
            Impl::erase(node);
            my_delete(node);
            return 1;
        }
        else return 0;
    }
    //swap C++17起
    void swap(tree_map_wrapper &other)
        noexcept(std::allocator_traits<Alloc>::is_always_equal::value
        && std::is_nothrow_swappable<Cmp>::value)
    {
        std::swap(cmp,other.cmp);
        std::swap(alloc,other.alloc);
        std::swap(root,other.root);
    }
    //extract (1) C++17起
    node_type extract(const_iterator position)
    {
        Impl::erase(position.node);
        return node_type(position.node,alloc);
    }
    //extract (2) C++17起
    node_type extract(const key_type &x)
    {
        auto node=Impl::search(root,cmp,x);
        if(!node)return node_type(nullptr,alloc);
        Impl::erase(node);
        return node_type(node,alloc);
    }
    //merge (1) C++17起
    template<typename C2>
    void merge(tree_map_wrapper<Key,T,C2,Alloc,Impl> &source)
    {
        Impl::merge(root,cmp,source.root);
    }
    //merge (2) C++17起
    template<typename C2>
    void merge(tree_map_wrapper<Key,T,C2,Alloc,Impl> &&source)
    {
        Impl::merge(root,cmp,source.root);
    }
    //merge (3) C++17起
    template<typename C2,typename I2>
    void merge(tree_multimap_wrapper<Key,T,C2,Alloc,I2> &source);
    //merge (4) C++17起
    template<typename C2,typename I2>
    void merge(tree_multimap_wrapper<Key,T,C2,Alloc,I2> &&source);
//查找
public:
    //count (1)
    std::size_t count(const Key &key)const
    {
        return Impl::search(root,cmp,key)==nullptr?0:1;
    }
    //count (2) C++14起
    template<typename K>
    std::size_t count(const K &key)const
    {
        return Impl::search(root,cmp,key)==nullptr?0:1;
    }
    //find (1)
    iterator find(const Key &key)
    {
        auto node=Impl::search(root,cmp,key);
        if(!node)node=root;
        return iterator(node,root);
    }
    //find (2)
    const_iterator find(const Key &key)const
    {
        auto node=Impl::search(root,cmp,key);
        if(!node)node=root;
        return const_iterator(node,root);
    }
    //find (3) C++14起
    template<typename K>
    iterator find(const K &key)
    {
        auto node=Impl::search(root,cmp,key);
        if(!node)node=root;
        return iterator(node,root);
    }
    //find (4) C++14起
    template<typename K>
    const_iterator find(const K &key)const
    {
        auto node=Impl::search(root,cmp,key);
        if(!node)node=root;
        return const_iterator(node,root);
    }
    //contains (1) C++20起
    bool contains(const Key &key)const
    {
        return Impl::search(root,cmp,key)!=nullptr;
    }
    //contains (2) C++20起
    template<typename K>
    bool contains(const K &key)const
    {
        return Impl::search(root,cmp,key)!=nullptr;
    }
    //equal_range (1)
    std::pair<iterator,iterator> equal_range(const Key &key)
    {
        return {lower_bound(key),upper_bound(key)};
    }
    //equal_range (2)
    std::pair<const_iterator,const_iterator> equal_range(const Key &key)const
    {
        return {lower_bound(key),upper_bound(key)};
    }
    //equal_range (3) C++14起
    template<typename K>
    std::pair<iterator,iterator> equal_range(const K &key)
    {
        return {lower_bound(key),upper_bound(key)};
    }
    //equal_range (4) C++14起
    template<typename K>
    std::pair<const_iterator,const_iterator> equal_range(const K &key)const
    {
        return {lower_bound(key),upper_bound(key)};
    }
    //lower_bound (1)
    iterator lower_bound(const Key& key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<=0)return iterator(pos,root);
        else return iterator(Impl::successor(pos),root);
    }
    //lower_bound (2)
    const_iterator lower_bound(const Key& key)const
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<=0)return const_iterator(pos,root);
        else return const_iterator(Impl::successor(pos),root);
    }
    //lower_bound (3) C++14起
    template<typename K>
    iterator lower_bound(const K& key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<=0)return iterator(pos,root);
        else return iterator(Impl::successor(pos),root);
    }
    //lower_bound (4) C++14起
    template<typename K>
    const_iterator lower_bound(const K& key)const
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<=0)return const_iterator(pos,root);
        else return const_iterator(Impl::successor(pos),root);
    }
    //upper_bound (1)
    iterator upper_bound(const Key& key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<0)return iterator(pos,root);
        else return iterator(Impl::successor(pos),root);
    }
    //upper_bound (2)
    const_iterator upper_bound(const Key& key)const
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<0)return const_iterator(pos,root);
        else return const_iterator(Impl::successor(pos),root);
    }
    //upper_bound (3) C++14起
    template<typename K>
    iterator upper_bound(const K& key)
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<0)return iterator(pos,root);
        else return iterator(Impl::successor(pos),root);
    }
    //upper_bound (4) C++14起
    template<typename K>
    const_iterator upper_bound(const K& key)const
    {
        auto [pos,dire]=Impl::search2(root,cmp,key);
        Impl::search2_no_insert(pos);
        if(dire<0)return const_iterator(pos,root);
        else return const_iterator(Impl::successor(pos),root);
    }
//观察器
public:
    key_compare key_comp()const{return cmp.cmp;}
    value_compare value_comp()const{return cmp;}
//operator == != < <= > >=
public:
    inline friend bool operator==(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {
        if(m1.size()!=m2.size())return false;
        for(auto i=m1.begin(),j=m2.begin();i!=m1.end();++i,++j)
        {
            //cppreference: 此比较忽略 map 的定序 Compare 
            if(*i!=*j)return false;
        }
        return true;
    }
    inline friend bool operator<(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {
        for(auto i=m1.begin(),j=m2.begin();i!=m1.end()&&j!=m2.end();++i,++j)
        {
            if(*i<*j)return true;
            else if(*i>*j)return false;
        }
        return m1.size()<m2.size();
    }
    inline friend bool operator!=(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {return !(m1==m2);}
    inline friend bool operator>(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {return m2<m1;}
    inline friend bool operator<=(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {return !(m2<m1);}
    inline friend bool operator>=(const tree_map_wrapper &m1,const tree_map_wrapper &m2)
    {return !(m1<m2);}
private:
    value_compare cmp;
    allocator_type alloc;
    Node *root=my_new_raw();
    inline Node* my_new_raw()
    {
        return std::allocator_traits<allocator_type>::allocate(alloc,1);
    }

    template<class... Args>
    inline Node* my_new(Args &&...args)
    {
        auto x=my_new_raw();
        std::allocator_traits<allocator_type>::construct(alloc,x,std::forward<Args>(args)...);
        return x;
    }

    inline void my_delete_raw(Node *x)
    {
        std::allocator_traits<allocator_type>::deallocate(alloc,x,1);
    }

    inline void my_delete(Node *x)
    {
        std::allocator_traits<allocator_type>::destroy(alloc,x);
        my_delete_raw(x);
    }

    // Node* copy_subtree(Node *node,Node *parent)
    // {
    //  if(node==nullptr)return nullptr;
    //  Node *new_node=my_new(node->data);
    //  new_node->cnt=node->cnt;
    //  new_node->parent=parent;
    //  new_node->left=copy_subtree(node->left,new_node);
    //  new_node->right=copy_subtree(node->right,new_node);
    //  return new_node;
    // }

    Node* copy_subtree(Node *root)
    {
        Node *new_root=my_new(*root);
        new_root->cnt=root->cnt;
        Node *new_p=new_root,*p=root;
        //非递归的先序遍历，防止栈溢出
        while(true)
        {
            if(p->left)
            {
                new_p->left=my_new(*(p->left));
                new_p->left->parent=new_p;
                new_p=new_p->left,p=p->left;
                new_p->cnt=p->cnt;
            }
            else
            {
                new_p->left=nullptr;
                while(!p->right)
                {
                    new_p->right=nullptr;
                    if(!p->parent)goto done;
                    while(p==p->parent->right)
                    {
                        p=p->parent;
                        new_p=new_p->parent;
                    }
                    p=p->parent;
                    new_p=new_p->parent;
                }
                new_p->right=my_new(*(p->right));
                new_p->right->parent=new_p;
                new_p=new_p->right,p=p->right;
            }
        }
        done:;
        return new_root;
    }

    void delete_subtree(Node *node)
    {
        if(!node)return;
        node->parent=nullptr;
        while(true)
        {
            if(node->left)node=node->left;
            else if(node->right)node=node->right;
            else
            {
                Node *p=node->parent;
                if(p)
                {
                    if(node==p->left)p->left=nullptr;
                    else p->right=nullptr;
                    my_delete(node);
                    node=p;
                }
                else
                {
                    my_delete(node);
                    break;
                }
            }
        }
    }
};
}

namespace std
{
//std::swap C++17起
template<typename Key,typename T,typename Cmp,typename Alloc,typename Impl>
inline void swap(zzc::tree_map_wrapper<Key,T,Cmp,Alloc,Impl> &lhs,
    zzc::tree_map_wrapper<Key,T,Cmp,Alloc,Impl> &rhs)
    noexcept(noexcept(lhs.swap(rhs)))
{
    lhs.swap(rhs);
}
}
