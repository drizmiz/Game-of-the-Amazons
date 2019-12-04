
#pragma once

#include "std.h"

#ifndef _BOTZONE_ONLINE
template <typename T>
class Stack
{
	template <typename Ty>
	using sptr = std::shared_ptr<Ty>;
private:
	struct Node
	{
		T value;
		sptr<Node> next;
		Node(T invalue, sptr<Node> innext)
		{
			value = invalue;
			next = innext;
		}
	};
	sptr<Node> top;
	int N;
public:
	Stack() { top = nullptr; N = 0; }
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void push_back(T input)
	{
		sptr<Node> newtopptr = std::make_shared<Node>(input, top);
		top = newtopptr;
		N++;
	}
	T pop_back()
	{
		if (isEmpty())
			throw std::exception("Stack empty!");
		T re = top->value;
		top = top->next;
		N--;
		return re;
	}
};

template <typename T>
class Queue
{
	template <typename Ty>
	using sptr = std::shared_ptr<Ty>;
private:
	struct Node
	{
		T value;
		sptr<Node> next;
		Node(T invalue, sptr<Node> innext)
		{
			value = invalue;
			next = innext;
		}
		Node& operator=(const T& val)
		{
			this->value = val;
		}
		operator T()
		{
			return this->value;
		}
	};
	sptr<Node> first,last;
	int N;
public:
	Queue() { first = last = nullptr; N = 0; }
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void enqueue(T input)
	{
		sptr<Node> newlast = std::make_shared<Node>(input, nullptr);
		if (isEmpty())
			first = newlast;
		else last->next = newlast;
		last = newlast;
		N++;
	}
	T dequeue()
	{
		if (isEmpty())
			throw std::exception("Queue empty!");
		T tmp = first->value;
		first = first->next;
		N--;
		if (isEmpty())last = nullptr;
		return tmp;
	}
	struct iterator : public std::iterator<Queue, T>
	{
	private:
		sptr<Node> pointer;
	public:
		iterator(sptr<Node> input)
		{
			pointer = input;
		}
		iterator& operator++()
		{
			if (pointer->next != nullptr)
				pointer = pointer->next;
			else pointer = nullptr;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp(this->pointer);
			operator++();
			return tmp;
		}
		bool operator==(iterator other)
		{
			return this->pointer == other->pointer;
		}
		Node& operator*()
		{
			return *(this->pointer);
		}
	};
	iterator begin()
	{
		return iterator(first);
	}
	iterator end()
	{
		return iterator(nullptr);
	}
};

// 习题1.3.31&1.3.33
template <typename T>
class Deque 
{
	template <typename Ty>
	using sptr = std::shared_ptr<Ty>;
private:
	struct DoubleNode
	{
		T value;
		sptr<DoubleNode> left;
		sptr<DoubleNode> right;
		DoubleNode(T value, sptr<DoubleNode> left, sptr<DoubleNode> right)
		{
			this->value = value;
			this->left = left;
			this->right = right;
		}
		DoubleNode& operator=(T input)
		{
			value = input;
		}
		operator T()
		{
			return value;
		}
	};
	sptr<DoubleNode> first, last;
	int N;
public:
	Deque() { first = last = nullptr; N = 0; }
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void pushLeft(T input)
	{
		sptr<DoubleNode> newfirst = std::make_shared<DoubleNode>(input, nullptr, first);
		if (isEmpty())
			last = newfirst;
		else first->left = newfirst;
		first = newfirst;
		N++;
	}
	void pushRight(T input)
	{
		sptr<DoubleNode> newlast = std::make_shared<DoubleNode>(input, last, nullptr);
		if (isEmpty())
			first = newlast;
		else last->right = newlast;
		last = newlast;
		N++;
	}
	T popLeft()
	{
		if (isEmpty())
			throw std::exception("Deque empty!");
		T tmp = first->value;
		first = first->right;
		first->left = nullptr;
		N--;
		if (isEmpty())last = nullptr;
		return tmp;
	}
	T popRight()
	{
		if(isEmpty())
			throw std::exception("Deque empty!");
		T tmp = last->value;
		last = last->left;
		last->right = nullptr;
		N--;
		if (isEmpty())first = nullptr;
		return tmp;
	}
	struct iterator : public std::iterator<Deque, T>
	{
	private:
		sptr<DoubleNode> pointer;
	public:
		iterator(sptr<DoubleNode> input)
		{
			pointer = input;
		}
		iterator& operator++()
		{
			if (pointer->right != nullptr)
				pointer = pointer->right;
			else pointer = nullptr;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp(this->pointer);
			operator++();
			return tmp;
		}
		iterator& operator--()
		{
			if (pointer->left != nullptr)
				pointer = pointer->left;
			else pointer = nullptr;
			return *this;
		}
		iterator operator--(int)
		{
			iterator tmp(this->pointer);
			operator--();
			return tmp;
		}
		bool operator==(iterator other)
		{
			return this->pointer == other->pointer;
		}
		DoubleNode& operator*()
		{
			return *(this->pointer);
		}
	};
	iterator begin()
	{
		return iterator(first);
	}
	iterator end()
	{
		return iterator(nullptr);
	}
};
template <typename T>
class RAStack
{
private:
	static const int initialN = static_cast<int>(1e+4);
	static const int expandrate = 3;
	std::allocator<T> space;
	T* initialptr, * nowptr;
	// nowptr指向最后一个元素之后的位置
	int N, allspace;
	int iteratorcode;
public:
	RAStack(int initial = initialN)
	{
		initialptr = space.allocate(initial);
		nowptr = initialptr;
		allspace = initial; N = 0;
		std::srand(1);
		iteratorcode = std::rand();
	}
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void resize(int size)
	{
		std::allocator<T> newspace;
		T* newinitialptr = newspace.allocate(allspace = size);
		T* newptr = newinitialptr;
		for (iterator i = begin(); i != end(); i++)
		{
			newspace.construct(newptr, *i);
			newptr++;
		}
		while (nowptr != initialptr)
			space.destroy(--nowptr);
		space.deallocate(initialptr, allspace / expandrate);
		space = newspace;
		initialptr = newinitialptr;
		nowptr = newptr;
		iteratorcode = std::rand();
	}
	void push_back(T input)
	{
		if (N >= allspace)
			resize(allspace * expandrate);
		space.construct(nowptr++, input);
		N++;
	}
	T pop_back()
	{
		if (isEmpty())
			throw std::exception("RAStack empty!");
		if (N < allspace / expandrate / expandrate)
			resize(allspace / expandrate);
		T tmp = *(--nowptr);
		space.destroy(nowptr);
		N--;
		return tmp;
	}
	T& operator[](int n)
	{
		return initialptr + n;
	}

	struct iterator : public std::iterator<RAStack, T>
	{
	private:
		T* pointer;
		int code;
		RAStack& parent;
	public:
		iterator(T* input, RAStack& parent0) :parent(parent0)
		{
			pointer = input;
			code = parent.iteratorcode;
		}
		iterator& operator++()
		{
			++pointer;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp(this->pointer, parent);
			operator++();
			return tmp;
		}
		iterator operator+(int input)
		{
			return iterator(pointer + input, parent);
		}
		bool operator==(iterator other)
		{
			return this->pointer == other->pointer;
		}
		T& operator*()
		{
			return *(this->pointer);
		}
		operator T* ()
		{
			return pointer;
		}
		bool isValid()
		{
			return iteratorcode == code;
		}
	};
	iterator begin()
	{
		return iterator(initialptr, *this);
	}
	iterator end()
	{
		return iterator(nowptr, *this);
	}
};
template <typename T>
class RADeque
{
private:
	static const int initialN = static_cast<int>(1e+3);
	static const int expandrate = 2;
	std::allocator<T> space;
	T* initialptr, *beginptr,* endptr;
	// endptr指向最后一个元素之后的位置
	int N, allspace;
	int iteratorcode;
public:
	RADeque(int initial = initialN)
	{
		initialptr = space.allocate(initial);
		allspace = initial; N = 0;
		beginptr = endptr = initialptr + initial / 4;
		std::srand(1);
		iteratorcode = std::rand();
	}
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void resize(int size)
	{
		std::allocator<T> newspace;
		T* newinitialptr = newspace.allocate(size);
		T* newptr = newinitialptr + size / 4;;
		for (iterator i = begin(); i != end(); i++)
		{
			newspace.construct(newptr, *i);
			newptr++;
		}
		while (endptr != beginptr)
			space.destroy(--endptr);
		space.deallocate(initialptr, allspace);
		space = newspace;
		initialptr = newinitialptr;
		beginptr = newinitialptr + size / 4;
		endptr = newptr;
		allspace = size;
		iteratorcode = std::rand();
	}
	void rearrange()
	{
		resize(allspace);
	}
	void push_back(T input)
	{
		if (endptr >= initialptr + allspace)
			resize(allspace * expandrate);
		space.construct(endptr++, input);
		N++;
	}
	void push_front(T input)
	{
		if (beginptr == initialptr)
		{
			if (N >= allspace / expandrate / expandrate)
				resize(allspace * expandrate);
			else rearrange();
		}
		space.construct(--beginptr, input);
		N++;
	}
	T pop_back()
	{
		if (isEmpty())
			throw std::exception("RADeque empty!");
		/*
		if (N < allspace / expandrate / expandrate)
			resize(allspace / expandrate);
		*/
		T tmp = *(--endptr);
		space.destroy(endptr);
		N--;
		return tmp;
	}
	T pop_front()
	{
		if (isEmpty())
			throw std::exception("RADeque empty!");
		T tmp = *beginptr;
		space.destroy(beginptr++);
		N--;
		return tmp;
	}
	T& operator[](int n)
	{
		return *(beginptr + n);
	}

	struct iterator : public std::iterator<RADeque, T>
	{
	private:
		T* pointer;
		int code;
		RADeque& parent;
	public:
		iterator(T* input, RADeque& parent0) :parent(parent0)
		{
			pointer = input;
			code = parent.iteratorcode;
		}
		iterator& operator++()
		{
			++pointer;
			return *this;
		}
		iterator operator++(int)
		{
			iterator tmp(this->pointer, parent);
			operator++();
			return tmp;
		}
		iterator operator+(int input)
		{
			return iterator(pointer + input, parent);
		}
		iterator operator-(int input)
		{
			return operator+(-input);
		}
		int operator-(iterator input)
		{
			return (this->pointer - input.pointer);
		}
		bool operator==(iterator other)
		{
			return this->pointer == other->pointer;
		}
		T& operator*()
		{
			return *(this->pointer);
		}
		operator T* ()
		{
			return pointer;
		}
		bool isValid()
		{
			return iteratorcode == code;
		}
	};
	iterator begin()
	{
		return iterator(beginptr, *this);
	}
	iterator end()
	{
		return iterator(endptr, *this);
	}
};

template<typename _Ty>
class Sort;

template <typename T, class Container = std::vector<T> >
class my_priority_queue					// 最大值在树顶
{
	typedef unsigned uint;
private:
	Container& pq;						// ！不使用pq[0]
	void swim(uint index)
	{
		for (; index > 1 && pq[index / 2] < pq[index]; index /= 2)
			std::swap(pq[index / 2], pq[index]);
	}
	void sink(uint index, uint N)
	{
		for (; 2 * index <= N;)
		{
			uint kid_index = 2 * index;
			if (kid_index < N && pq[kid_index] < pq[kid_index + 1])
				++kid_index;
			if (pq[index] >= pq[kid_index])
				break;
			std::swap(pq[index], pq[kid_index]);
			index = kid_index;
		}
	}
public:
	friend class Sort<T>;
	my_priority_queue(Container& inpq) :pq(inpq) {}
	my_priority_queue(uint N) :pq(Container()) { pq.reserve(N); }
	void push(T in)
	{
		pq.push_back(in);
		swim(pq.size());
	}
	void emplace(uint index, T in)
	{
		pq[index] = in;
		// do sth
	}
	void insert(uint index, T in)
	{
		// do sth
	}
	T pop()
	{
		std::swap(pq[1], pq[pq.size()]);
		T ret = pq.pop_back();
		sink(1);
		return ret;
	}
};

#endif

template <typename T>
class resizing_queue
{
private:
	static const int initialN = static_cast<int>(1e+3);
	static const int expandrate = 2;
	std::allocator<T> space;
	T* initialptr, * beginptr, * endptr;
	// endptr指向最后一个元素之后的位置
	int N, allspace;
public:
	resizing_queue(int initial = initialN)
	{
		initialptr = space.allocate(initial);
		allspace = initial; N = 0;
		beginptr = endptr = initialptr + initial / 4;
	}
	bool isEmpty() { return N == 0; }
	int currentSize() { return N; }
	void resize(int size)
	{
		std::allocator<T> newspace;
		T* newinitialptr = newspace.allocate(size);
		T* newptr = newinitialptr + size / 4;;
		for (auto i = beginptr; i != endptr; i++)
		{
			newspace.construct(newptr, *i);
			newptr++;
		}
		while (endptr != beginptr)
			space.destroy(--endptr);
		space.deallocate(initialptr, allspace);
		space = newspace;
		initialptr = newinitialptr;
		beginptr = newinitialptr + size / 4;
		endptr = newptr;
		allspace = size;
	}
	void rearrange()
	{
		resize(allspace);
	}
	void push_back(T input)
	{
		if (endptr >= initialptr + allspace)
			resize(allspace * expandrate);
		space.construct(endptr++, input);
		N++;
	}
	void push_front(T input)
	{
		if (beginptr == initialptr)
		{
			if (N >= allspace / expandrate / expandrate)
				resize(allspace * expandrate);
			else rearrange();
		}
		space.construct(--beginptr, input);
		N++;
	}
	T pop_back()
	{
		//if (isEmpty())
			//throw std::exception("RADeque empty!");
		/*
		if (N < allspace / expandrate / expandrate)
			resize(allspace / expandrate);
		*/
		T tmp = *(--endptr);
		space.destroy(endptr);
		N--;
		return tmp;
	}
	T pop_front()
	{
		//if (isEmpty())
			//throw std::exception("RADeque empty!");
		T tmp = *beginptr;
		space.destroy(beginptr++);
		N--;
		return tmp;
	}
	T& operator[](int n)
	{
		return *(beginptr + n);
	}

	void push(T value) { push_back(value); }
	T pop() { return pop_front(); }

	bool empty() { return isEmpty(); }
};
