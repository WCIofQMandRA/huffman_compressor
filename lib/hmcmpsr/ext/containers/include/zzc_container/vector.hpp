//vector.hpp
//Copyright (C) 2021 张子辰

// Copying and distribution of this file, with or without modification,
// are permitted in any medium without royalty provided the copyright
// notice and this notice are preserved.  This file is offered as-is,
// without any warranty.

//Revision:
//2021-12-08: Change `size_t' to `std::size_t', in case `size_t' is not in
//the global namespace.

#pragma once
#include <stdexcept>
#include <memory>
#include <iterator>
#include <type_traits>
#include <utility>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <limits>

namespace zzc
{
template<typename T,typename Allocator=std::allocator<T>>
class vector
{
//类型
public:
    using value_type=T;
    using allocator_type=Allocator;
    using size_type=std::size_t;
    using difference_type=std::ptrdiff_t;
    using reference=T&;
    using const_reference=const T&;
    using pointer=typename std::allocator_traits<Allocator>::pointer;
    class const_iterator;
    class iterator
    {
        using self_type=iterator;
        friend class vector;
        friend class const_iterator;
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=T;
        using difference_type=std::ptrdiff_t;
        using pointer=value_type*;
        using reference=value_type&;
        iterator()=default;
        reference operator*()const noexcept{return *p;}
        pointer operator->()const noexcept{return p;}
        bool operator==(self_type other)const noexcept{return p==other.p;}
        bool operator!=(self_type other)const noexcept{return p!=other.p;}
        
        self_type& operator++()noexcept
        {
            ++p;
            return *this;
        }
        self_type operator++(int)noexcept
        {
            auto tmp=*this;
            ++p;
            return tmp;
        }
        self_type& operator--()noexcept
        {
            --p;
            return *this;
        }
        self_type operator--(int)noexcept
        {
            auto tmp=*this;
            ++p;
            return tmp;
        }
        
        self_type& operator+=(difference_type n)noexcept
        {
            p+=n;
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
            p-=n;
            return *this;
        }
        self_type operator-(difference_type n)const noexcept
        {
            auto x=*this;
            return x-=n;
        }
        difference_type operator-(self_type other)const noexcept
        {
            return p-other.p;
        }
        reference operator[](size_type n)const noexcept
        {
            return p[n];
        }
    private:
        constexpr explicit iterator(T *p):p(p){}
        T *p;
    };
    class const_iterator
    {
        using self_type=const_iterator;
        friend class vector;
    public:
        using iterator_category=std::random_access_iterator_tag;
        using value_type=T;
        using difference_type=std::ptrdiff_t;
        using pointer=const value_type*;
        using reference=const value_type&;
        const_iterator()=default;
        const_iterator(const iterator &it):p(it.p){}
        reference operator*()const noexcept{return *p;}
        pointer operator->()const noexcept{return p;}
        bool operator==(self_type other)const noexcept{return p==other.p;}
        bool operator!=(self_type other)const noexcept{return p!=other.p;}
        
        self_type& operator++()noexcept
        {
            ++p;
            return *this;
        }
        self_type operator++(int)noexcept
        {
            auto tmp=*this;
            ++p;
            return tmp;
        }
        self_type& operator--()noexcept
        {
            --p;
            return *this;
        }
        self_type operator--(int)noexcept
        {
            auto tmp=*this;
            ++p;
            return tmp;
        }
        
        self_type& operator+=(difference_type n)noexcept
        {
            p+=n;
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
            p-=n;
            return *this;
        }
        self_type operator-(difference_type n)const noexcept
        {
            auto x=*this;
            return x-=n;
        }
        difference_type operator-(self_type other)const noexcept
        {
            return p-other.p;
        }
        reference operator[](size_type n)const noexcept
        {
            return p[n];
        }
    private:
        constexpr explicit const_iterator(const T *p):p(p){}
        const T *p;
    };
    using reverse_iterator=std::reverse_iterator<iterator>;
    using const_reverse_iterator=std::reverse_iterator<const_iterator>;
//构造函数
public:
    //(1)
    vector() noexcept(noexcept(Allocator()))=default;
    //(2)
    explicit vector(const Allocator& alloc)noexcept:
        m_allocator(alloc){}
    //(3)
    vector(size_type count,const T &value,const Allocator &alloc=Allocator()):
        m_allocator(alloc)
    {
        m_elem=allocate(count);
        m_size=m_capacity=count;
        for(std::size_t i=0;i<count;++i)
            construct(m_elem+i,value);
    }
    //(4)
    explicit vector(std::size_t count,const Allocator &alloc=Allocator()):
        m_allocator(alloc)
    {
        m_elem=allocate(count);
        m_size=m_capacity=count;
        for(std::size_t i=0;i<count;++i)
            construct(m_elem+i);
    }
    //(5)
    template<typename InputIt,
    std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag,typename std::iterator_traits<InputIt>::iterator_category>,int> U=0>
    vector(InputIt first,InputIt last,const Allocator &alloc=Allocator()):m_allocator(alloc)
    {
        m_size=m_capacity=last-first;
        m_elem=allocate(m_capacity);
        std::size_t i=0;
        for(auto it=first;it!=last;++it,++i)
            construct(m_elem+i,*it);
    }
    template<typename InputIt>
    vector(InputIt first,InputIt last,const Allocator &alloc=Allocator()):m_allocator(alloc)
    {
        for(auto it=first;it!=last;++it)
            push_back(*it);
    }
    //(6)
    vector(const vector &other):m_allocator(other.m_allocator)
    {
        m_size=m_capacity=other.m_size;
        m_elem=allocate(m_capacity);
        for(std::size_t i=0;i<m_size;++i)
            construct(m_elem+i,other.m_elem[i]);
    }
    //(7)
    vector(const vector &other,const Allocator &alloc):m_allocator(alloc)
    {
        m_size=m_capacity=other.m_size;
        m_elem=allocate(m_capacity);
        for(std::size_t i=0;i<m_size;++i)
            construct(m_elem+i,other.m_elem[i]);
    }
    //(8)
    vector(vector &&other)noexcept:
        m_allocator(std::move(other.m_allocator)),m_elem(other.m_elem),m_size(other.m_size),m_capacity(other.m_capacity)
    {
        other.m_elem=nullptr;
        other.m_size=other.m_capacity=0;
    }
    //(9)
    vector(vector &&other,const Allocator &alloc):m_allocator(alloc)
    {
        if(m_allocator==other.alloc)
        {
            m_elem=other.m_elem;
            m_size=other.m_size;
            m_capacity=other.m_capacity;
            other.m_elem=nullptr;
            other.m_size=0;
            other.m_capacity=0;
        }
        else
        {
            m_size=m_capacity=other.m_size;
            m_elem=allocate(m_capacity);
            for(std::size_t i=0;i<m_size;++i)
                construct(m_elem+i,other.m_elem[i]);
        }
    }
    //(10)
    vector(std::initializer_list<T> list,const Allocator &alloc=Allocator()):vector(list.begin(),list.end(),alloc){}
//析构函数
public:
    ~vector()
    {
        for(std::size_t i=0;i<m_size;++i)
            destroy(m_elem+i);
        deallocate(m_elem,m_capacity);
    }
//operator=
public:
    //(1)
    vector& operator=(const vector &other)
    {
        if(m_capacity<other.m_capacity)
            reset_capacity(std::max(other.m_capacity,m_capacity*2));
        if(m_size<=other.m_size)
        {
            std::size_t i;
            for(i=0;i<m_size;++i)
                m_elem[i]=other.m_elem[i];
            for(;i<other.m_size;++i)
                construct(m_elem+i,other.m_elem[i]);
        }
        else
        {
            std::size_t i;
            for(i=0;i<other.m_size;++i)
                m_elem[i]=other.m_elem[i];
            for(;i<m_size;++i)
                destroy(m_elem+i);
        }
        m_size=other.m_size;
        return *this;
    }
    //(2)
    vector& operator=(vector &&other)noexcept(std::allocator_traits<Allocator>::propagate_on_container_move_assignment::value
        || std::allocator_traits<Allocator>::is_always_equal::value)
    {
        std::swap(m_allocator,other.m_allocator);
        std::swap(m_elem,other.m_elem);
        std::swap(m_size,other.m_size);
        std::swap(m_capacity,other.m_capacity);
        return *this;
    }
    //(3)
    vector& operator=(std::initializer_list<T> ilist)
    {
        assign(ilist);
        return *this;
    }
//assign
public:
    //(1)
    void assign(size_type count,const T &value)
    {
        if(m_capacity<count)
            reset_capacity(std::max(count,m_capacity*2));
        if(m_size<=count)
        {
            std::size_t i;
            for(i=0;i<m_size;++i)
                m_elem[i]=value;
            for(;i<count;++i)
                construct(m_elem+i,value);
        }
        else
        {
            std::size_t i;
            for(i=0;i<count;++i)
                m_elem[i]=value;
            for(;i<m_size;++i)
                destroy(m_elem+i);
        }
        m_size=count;
    }
    //(2)
    template<class InputIt,
    std::enable_if_t<std::is_base_of_v<std::random_access_iterator_tag,typename std::iterator_traits<InputIt>::iterator_category>,int> U=0>
    void assign(InputIt first,InputIt last)
    {
        std::size_t count=last-first;
        if(m_capacity<count)
            reset_capacity(std::max(count,m_capacity*2));
        if(m_size<=count)
        {
            std::size_t i=0;auto it=first;
            for(;i<m_size;++i,++it)
                m_elem[i]=*it;
            for(;it!=last;++it,++i)
                construct(m_elem+i,*it);
        }
        else
        {
            std::size_t i=0;
            for(auto it=first;it!=last;++it,++i)
                m_elem[i]=*it;
            for(;i<m_size;++i)
                destroy(m_elem+i);
        }
        m_size=count;
    }
    template<class InputIt>
    void assign(InputIt first,InputIt last)
    {
        clear();
        for(auto i=first;i!=last;++i)
            push_back(*i);
    }
    //(3)
    void assign(std::initializer_list<T> ilist)
    {
        if(m_capacity<ilist.size())
            reset_capacity(std::max(ilist.size(),m_capacity*2));
        if(m_size<=ilist.size())
        {
            T *p,*it;
            for(p=m_elem,it=ilist.begin();p!=m_elem+m_size;++p,++it)
                *p=*it;
            for(;it!=ilist.end();++p,++it)
                construct(p,*it);
        }
        else
        {
            T *p,*it;
            for(p=m_elem,it=ilist.begin();it!=ilist.end();++p,++it)
                *p=*it;
            for(;p<m_elem+m_size;++p)
                destroy(p);
        }
        m_size=ilist.size();
    }
//get_allocator
public:
    Allocator get_allocator()const noexcept
    {
        return m_allocator;
    }
//元素访问
public:
    T& at(std::size_t pos)
    {
        if(pos>=m_size)
            throw std::out_of_range("zzc::vector::at(size_t): pos>m_size. pos="+std::to_string(pos)+" m_size="+std::to_string(m_size));
        return m_elem[pos];
    }
    const T& at(std::size_t pos)const
    {
        if(pos>=m_size)
            throw std::out_of_range("zzc::vector::at(size_t): pos>m_size. pos="+std::to_string(pos)+" m_size="+std::to_string(m_size));
        return m_elem[pos];
    }
    T& operator[](std::size_t pos)               {return m_elem[pos];}
    const T& operator[](std::size_t pos)const    {return m_elem[pos];}
    T& front()                              {return *m_elem;}
    const T& front()const                   {return *m_elem;}
    T& back()                               {return m_elem[m_size-1];}
    const T& back()const                    {return m_elem[m_size-1];}
    T* data()noexcept                       {return m_elem;}
    const T* data()const noexcept           {return m_elem;}
//迭代器
public:
    iterator begin()                        {return iterator(m_elem);}
    const_iterator begin()const             {return const_iterator(m_elem);}
    const_iterator cbegin()const            {return const_iterator(m_elem);}
    iterator end()                          {return iterator(m_elem+m_size);}
    const_iterator end()const               {return const_iterator(m_elem+m_size);}
    const_iterator cend()const              {return const_iterator(m_elem+m_size);}
    reverse_iterator rbegin()               {return reverse_iterator(iterator(m_elem+m_size));}
    const_reverse_iterator rbegin()const    {return const_reverse_iterator(const_iterator(m_elem+m_size));}
    const_reverse_iterator crbegin()const   {return const_reverse_iterator(const_iterator(m_elem+m_size));}
    reverse_iterator rend()                 {return reverse_iterator(iterator(m_elem));}
    const_reverse_iterator rend()const      {return const_reverse_iterator(const_iterator(m_elem));}
    const_reverse_iterator crend()const     {return const_reverse_iterator(const_iterator(m_elem));}
//容量
public:
    bool empty()const noexcept{return m_size==0;}
    size_type size()const noexcept{return m_size;}
    size_type max_size()const noexcept{return std::numeric_limits<size_type>::max()/sizeof(T);}
    void reverse(size_type new_cap)
    {
        if(new_cap>m_capacity)
            reset_capacity(new_cap);
    }
    void shrink_to_fit()
    {
        reset_capacity(m_size);
    }
//修改器
public:
    void clear()
    {
        for(std::size_t i=0;i<m_size;++i)
            destroy(m_elem+i);
        m_size=0;
    }
    //TODO: insert, emplace, erase
    void push_back(const T &value)
    {
        if(m_capacity==m_size)
            reset_capacity(m_capacity?m_capacity*2:4);
        construct(m_elem+m_size,value);
        ++m_size;
    }
    void push_back(T &&value)
    {
        if(m_capacity==m_size)
            reset_capacity(m_capacity?m_capacity*2:4);
        construct(m_elem+m_size,std::move(value));
        ++m_size;
    }
    template<typename ...Args>
    T& emplace_back(Args &&...args)
    {
        if(m_capacity==m_size)
            reset_capacity(m_capacity?m_capacity*2:4);
        construct(m_elem+m_size,std::forward<Args>(args)...);
        ++m_size;
        return m_elem[m_size-1];
    }
    void pop_back()
    {
        m_size--;
        destroy(m_elem+m_size);
    }
    void resize(std::size_t count)
    {
        if(count<=m_size)
        {
            for(std::size_t i=m_size;i>count;--i)
                destroy(m_elem+(i-1));
        }
        else
        {
            if(count>m_capacity)
                reset_capacity(std::max(m_capacity*2,count));
            for(std::size_t i=m_size;i<count;++i)
                construct(m_elem+i);
        }
        m_size=count;
    }
    void resize(std::size_t count,const T &value)
    {
        if(count<=m_size)
        {
            for(std::size_t i=m_size;i>count;--i)
                destroy(m_elem+(i-1));
        }
        else
        {
            if(count>m_capacity)
                reset_capacity(std::max(m_capacity*2,count));
            for(std::size_t i=m_size;i<count;++i)
                construct(m_elem+i,value);
        }
        m_size=count;
    }
    void swap(vector &other)noexcept(std::allocator_traits<Allocator>::propagate_on_container_swap::value
    ||std::allocator_traits<Allocator>::is_always_equal::value)
    {
        std::swap(m_allocator,other.m_allocator);
        std::swap(m_elem,other.m_elem);
        std::swap(m_capacity,other.m_capacity);
        std::swap(m_size,other.m_size);
    }
private:
    inline T* allocate(std::size_t s)
    {
        return s?std::allocator_traits<allocator_type>::allocate(m_allocator,s):nullptr;
    }
    inline void deallocate(T *p,std::size_t s)
    {
        if(s)std::allocator_traits<allocator_type>::deallocate(m_allocator,p,s);
    }
    template<typename... Args>
    inline void construct(T *p,Args &&...args)
    {
        std::allocator_traits<allocator_type>::construct(m_allocator,p,std::forward<Args>(args)...);
    }
    inline void destroy(T *p)
    {
        std::allocator_traits<allocator_type>::destroy(m_allocator,p);
    }
    
    void reset_capacity(std::size_t new_capacity)
    {
        T *new_elem=allocate(new_capacity);
        for(std::size_t i=0;i<m_size;++i)
        {
           construct(new_elem+i,std::move(m_elem[i]));
           destroy(m_elem+i);
        }
        deallocate(m_elem,m_capacity);
        m_elem=new_elem;
        m_capacity=new_capacity;
    }
    Allocator m_allocator;
    T *m_elem=nullptr;
    std::size_t m_size=0,m_capacity=0;
};
}
