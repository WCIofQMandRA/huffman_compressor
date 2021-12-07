//queue.hpp: 队列和优先队列
//Author: 张子辰
//本文件原本是 电梯模拟器 的一部分，而 电梯模拟器 被发布
//到了公有领域.
//修订:
//2021-12-07: 补充 void zzc::queue::push(T &&x)

#pragma once
#include <cstdint>
#include <utility>
#include <stdexcept>
#include <functional>

namespace zzc
{
template<typename T>
class queue
{
public:
	constexpr queue()=default;
	queue(const queue &other)
	{
		if(!other.m_size)return;
		elem=new T[other.m_size+1];
		m_size=other.m_size;
		m_capcity=other.m_size+1;
		for(size_t i=0,j=other.m_front;j!=other.m_back;++i,j=j==other.m_capcity?0:j+1)
		{
			elem[i]=other.elem[j];
		}
	}
	constexpr queue(queue &&other)noexcept:
		elem(other.elem),m_size(other.m_size),m_capcity(other.m_capcity),
		m_front(other.m_front),m_back(other.m_back)
	{
		other.elem=nullptr;
		other.m_size=0;
		other.m_capcity=0;
		other.m_front=0,other.m_back=0;
	}
	queue& operator=(const queue &other)
	{
		m_size=0,m_front=0,m_back=0;
		if(m_capcity<=other.m_size)
			re_allocate(other.m_size+1);
		for(size_t i=0,j=other.m_front;j!=other.m_back;++i,j=j==other.m_capcity?0:j+1)
		{
			elem[i]=other.elem[j];
		}
		m_back=other.m_size;
		return *this;
	}
	constexpr queue& operator=(queue &&other)noexcept
	{
		std::swap(elem,other.elem);
		std::swap(m_size,other.m_size);
		std::swap(m_capcity,other.m_capcity);
		std::swap(m_front,other.m_front);
		std::swap(m_back,other.m_back);
		return *this;
	}
	~queue()noexcept
	{
		try{delete[] elem;}
		catch(...){}
	}
	
	T& front()
	{
		if(!m_size)throw std::out_of_range("zzc::queue::front(): The queue is empty.");
		return elem[m_front];
	}

	const T& front()const
	{
		if(!m_size)throw std::out_of_range("zzc::queue::front(): The queue is empty.");
		return elem[m_front];
	}
	
	void pop()
	{
		++m_front;
		if(m_front==m_capcity)m_front=0;
		--m_size;
	}
	
	void push(const T &x)
	{
		if(m_size+1>=m_capcity)re_allocate(m_capcity==0?8:m_capcity*2);
		elem[m_back++]=x;
		if(m_back==m_capcity)m_back=0;
		++m_size;
	}
	
	void push(T &&x)
	{
		if(m_size+1>=m_capcity)re_allocate(m_capcity==0?8:m_capcity*2);
		elem[m_back++]=std::move(x);
		if(m_back==m_capcity)m_back=0;
		++m_size;
	}
	
	constexpr size_t size()const noexcept
	{
		return m_size;
	}
	
	constexpr bool empty()const noexcept
	{
		return m_size==0;
	}
private:
	void re_allocate(size_t new_capcity)
	{
		T *new_elem=new T[new_capcity];
		for(size_t i=0,j=m_front;j!=m_back;++i,j=j==m_capcity-1?0:j+1)
			new_elem[i]=elem[j];
		delete[] elem;
		m_front=0;m_back=m_size;
		m_capcity=new_capcity;
		elem=new_elem;
	}
private:
	T *elem=nullptr;
	size_t m_size=0,m_capcity=0;
	size_t m_front=0,m_back=0;
};

template<typename T,typename Compare=std::less<T>>
class priority_queue
{
public:
	constexpr priority_queue()=default;
	priority_queue(const priority_queue &other)
	{
		if(!other.m_size)return;
		heap=new T[other.m_size];
		m_size=other.m_size,m_capcity=other.m_capcity;
		for(size_t i=0;i<m_size;++i)
			heap[i]=other.heap[i];
	}
	constexpr priority_queue(priority_queue &&other)noexcept:
		heap(other.heap),m_size(other.m_size),m_capcity(other.m_capcity)
	{
		other.heap=nullptr,other.m_size=0,other.m_capcity=0;
	}
	~priority_queue()
	{
		try{delete[] heap;}
		catch(...){}
	}
	const T& top()const
	{
		return heap[0];
	}
	void pop()
	{
		size_t i=0;
		--m_size;
		heap[0]=std::move(heap[m_size]);
		while(true)
		{
			if(i*2+1<m_size)
			{
				if(i*2+2<m_size)
				{
					if(cmp(heap[i],heap[i*2+1]))
					{
						if(cmp(heap[i],heap[i*2+2]))
						{
							if(cmp(heap[i*2+1],heap[i*2+2]))
							{
								std::swap(heap[i],heap[i*2+2]);
								i=i*2+2;
							}
							else
							{
								std::swap(heap[i],heap[i*2+1]);
								i=i*2+1;
							}
						}
						else
						{
							std::swap(heap[i],heap[i*2+1]);
							i=i*2+1;
						}
					}
					else if(cmp(heap[i],heap[i*2+2]))
					{
						std::swap(heap[i],heap[i*2+2]);
						i=i*2+2;
					}
					else break;
				}
				else if(cmp(heap[i],heap[i*2+1]))
				{
					std::swap(heap[i],heap[i*2+1]);
					i=i*2+1;
				}
				else break;
			}
			else break;
		}
	}
	void push(const T &x)
	{
		if(m_size==m_capcity)
			re_allocate(m_capcity==0?8:m_capcity*2);
		heap[m_size++]=x;
		for(size_t i=m_size-1;i;i=(i-1)/2)
		{
			if(cmp(heap[(i-1)/2],heap[i]))
				std::swap(heap[(i-1)/2],heap[i]);
			else break;
		}
	}
	void push(T &&x)
	{
		if(m_size==m_capcity)
			re_allocate(m_capcity==0?8:m_capcity*2);
		heap[m_size++]=std::move(x);
		for(size_t i=m_size-1;i;i=(i-1)/2)
		{
			if(cmp(heap[(i-1)/2],heap[i]))
				std::swap(heap[(i-1)/2],heap[i]);
			else break;
		}
	}
	constexpr size_t size()const noexcept
	{
		return m_size;
	}
	constexpr bool empty()const noexcept
	{
		return m_size==0;
	}
private:
	void re_allocate(size_t new_capcity)
	{
		T *new_heap=new T[new_capcity];
		for(size_t i=0;i<m_size;++i)
			new_heap[i]=std::move(heap[i]);
		delete[] heap;
		m_capcity=new_capcity;
		heap=new_heap;
	}
private:
	T *heap=nullptr;
	size_t m_size=0,m_capcity=0;
	Compare cmp;
};
}
