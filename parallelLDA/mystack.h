#ifndef _MYSTACK_H
#define _MYSTACK_H

#include <iostream>
#define MAX1 20

template<typename T>
class my_stack
{
private:
	T *stack_arr;
	int tp;
public:
	my_stack()
	{
		stack_arr = new T[MAX1];
		tp = -1;
	}

	int size()
	{
		return tp + 1;
	}

	T& top()
	{
		return stack_arr[tp];
	}

	void push(T item)
	{
		stack_arr[++tp] = item;
	}

	void pop()
	{
		--tp;
	}
};

#endif