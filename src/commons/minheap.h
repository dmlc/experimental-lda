#ifndef _MYHEAP_H
#define _MYHEAP_H

template<typename T>
class MinHeap
{
private:
    T* heap;
    size_t _size;

    void BubbleDown(size_t index)
    {
        size_t leftChildIndex = 2 * index + 1;
        size_t rightChildIndex = 2 * index + 2;

        if (leftChildIndex >= _size)
            return; //index is a leaf

        size_t minIndex = index;

        if (heap[index] > heap[leftChildIndex])
            minIndex = leftChildIndex;

        if ((rightChildIndex < _size) && (heap[minIndex] > heap[rightChildIndex]))
            minIndex = rightChildIndex;

	if (minIndex != index)
	{
            //need to swap
            T temp = heap[index];
            heap[index] = heap[minIndex];
            heap[minIndex] = temp;
            BubbleDown(minIndex);
        }
    }

    void BubbleUp(size_t index)
    {
        if (index == 0)
           return;

        size_t parentIndex = (index - 1) / 2;

        if (heap[parentIndex] > heap[index])
        {
            size_t temp = heap[parentIndex];
            heap[parentIndex] = heap[index];
            heap[index] = temp;
            BubbleUp(parentIndex);
        }
    }
    void Heapify()
    {
        for (size_t i = _size - 1; i >= 0; --i)
            BubbleDown(i);
    }

public:
    MinHeap()
    {
        heap = new T[4096];
        _size = 0;
    }
    MinHeap(T* a, size_t l)
    {
        heap = new T[4096];
        _size = l;
        for (size_t i = 0; i < l; ++i)
        {
                heap[i] = a[i];
        }

        Heapify();
    }

    bool empty() const
    {
        return _size==0;
    }

    size_t size() const
    {
        return _size;
    }

    T& top() const
    {
        return heap[0];
    }
    
    void push(T item)
    {
        heap[_size] = item;
        BubbleUp(_size++);
    }
        
    void pop()
    {
        heap[0] = heap[--_size];
        BubbleDown(0);
    }

    void bulk(T* a, size_t l)
    {
        for (size_t i = 0; i < l; ++i)
            heap[i + _size] = a[i];
        
        _size += l;
        Heapify();
    }

    void clear()
    {
        _size = 0;
    }

};

#endif
