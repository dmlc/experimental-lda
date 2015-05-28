#ifndef _CIRCQUEUE_H
#define _CIRCQUEUE_H

#define MAX 4096

#include <atomic>
#include <iostream>
#include <chrono>
#include <thread>

template<typename T>
class circular_queue
{
private:
	T *cqueue_arr;
	volatile char pad1[64];
	int fp;
	volatile char pad2[64];
	int rp;
	
	//std::atomic<int> fp{ 0 };
	//std::atomic<int> rp{ 0 };
	//const int MAX = 512;
	
public:
	circular_queue()
	{
		//MAX = 512;
		cqueue_arr = new T[MAX];
		fp = rp = 0;
	}
	
	circular_queue(int n)
	{
		//MAX = n;
		cqueue_arr = new T[MAX];
		fp = rp = 0;
	}

	circular_queue(const circular_queue& c)
	{
		cqueue_arr = c.cqueue_arr;
	}
	
	~circular_queue()
	{
		delete[] cqueue_arr;
	}
	
	void init()
	{
		//MAX = 512;
		cqueue_arr = new T[MAX];
		fp = rp = 0;
	}

	int size() const
	{
		int result;
		//int trp = rp.load();
		//int tfp = fp.load();
		int trp = rp;
		int tfp = fp;
		if (trp >= tfp)
			result = trp - tfp;
		else
			result = (MAX - tfp) + trp;
		return result;
	}
	
	bool empty() const
	{
		return (rp == fp);
	}

	T& front()
	{
		return cqueue_arr[fp];
	}

	T& back()
	{
		return cqueue_arr[(rp - 1 + MAX) % MAX];
	}

	// Insert into Circular Queue
	void push(T item)
	{
		/*int trp = rp.load();
		while ((trp + 1) % MAX == fp.load())
		{
			    std::this_thread::sleep_for (std::chrono::milliseconds(1000));
				std::cout << "Contention: I am crazy ... well not too much" << std::endl;
		}
		cqueue_arr[trp] = item;
		rp.store((trp + 1) % MAX);*/

		while ((rp + 1) % MAX == fp)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			//std::cout << "Contention: Probably you should increase number of updating threads!" << std::endl;
		}
		cqueue_arr[rp] = item;
		rp =(rp + 1) % MAX;
	}

	//Delete from Circular Queue
	void pop()
	{
		//int tfp = fp.load();
		//fp.store((tfp + 1) % MAX);
		fp = (fp + 1) % MAX;
	}
};

#endif
