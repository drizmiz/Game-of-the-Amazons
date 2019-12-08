
#pragma once

#include "std.h"

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
