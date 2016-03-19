#ifndef SPVECTOR_H_
#define SPVECTOR_H_

class spvector
{
protected:    
    struct sp_container
    {
	unsigned short idx;  //array containing the id
	unsigned val;        //alias of i

	sp_container() 
	{   }

	sp_container(const sp_container &obj) : idx(obj.idx), val(obj.val)
	{   }

	sp_container& operator= (const sp_container &obj)
	{
            idx = obj.idx;
            val = obj.val;
            return *this;
	}
    }
    *_data;
    unsigned short _size;
    unsigned short _capacity;


public:
    /*** constructor ***/
    spvector(unsigned short cap = 32)
    {
	_size = 0;
	_capacity = cap;
	_data = new sp_container[_capacity];
    }

    /*** destructor ***/
    ~spvector()
    {
	if(_data) delete[] _data;
    }

    /*** capacity ***/
    inline unsigned short size() const
    {
	return _size;
    }

    unsigned short max_size() const
    {
	//what to do here???
	return 32767;
    }

    void resize(unsigned short num)
    {
	//std::cout<<"In resize of spvector!!!"<<std::endl;
	if (num < _capacity)
	    _size = num;
	else
        {
            // _capacity = 2*_capacity;
            _capacity = (unsigned short)(_capacity + 16);
            sp_container* temp = new sp_container[_capacity];
            for (unsigned short i = 0; i < _size; ++i)
		temp[i] = _data[i];
		
            delete[] _data;
            _data = temp;
            _size = num;
	}
    }

    inline unsigned short capacity() const
    {
	return _capacity;
    }

    inline bool empty() const
    {
        return _size == 0;
    }

    void reserve(unsigned short num)
    {
	if (num > _capacity)
	{
            _capacity = num;
            sp_container* temp = new sp_container[_capacity];
            for (unsigned i = 0; i < _size; ++i)
		temp[i] = _data[i];
            delete[] _data;
            _data = temp;
	}
    }

    void shrink_to_fit()
    {
	if (_capacity > _size)
        {
            _capacity = _size;
            sp_container* temp = new sp_container[_capacity];
            for (unsigned i = 0; i < _size; ++i)
                temp[i] = _data[i];
	
            delete[] _data;
            _data = temp;
	}
    }

    // The c++ shrink to fit is exactly this -- totally empty
    //public void shrink_to_fit() {
    //    
    //}

    /*** Iterator access ***/

    inline sp_container* begin()
    {
	return _size>0 ? &_data[0] : nullptr; 
    }

    inline sp_container* end()
    {
	return _size>0 ? &_data[_size] : nullptr; 
    }

    inline const sp_container* begin() const
    {
	return _size>0 ? &_data[0] : nullptr;
    }

    inline const sp_container* end() const
    {
	return _size>0 ? &_data[_size] : nullptr;
    }


    /*** Element access ***/

    sp_container* data()
    {
	return _data;
    }

    unsigned at(unsigned short idx)
    {
	//costly typically avoid
	unsigned result = 0;
	for (unsigned i = 0; i<_size; ++i)
        {
            if (_data[i].idx == idx)
		result = _data[i].val;
	}
	return result;
    }

	
    inline unsigned short idx_in(unsigned short pos) const
    {
	return _data[pos].idx;
    }

    inline unsigned val_in(unsigned short pos) const
    {
	return _data[pos].val;
    }

    inline unsigned short idx_front()
    {
	return _data[0].idx;
    }

    inline unsigned val_front()
    {
	return _data[0].val;
    }

    inline unsigned short idx_back()
    {
	return _data[_size - 1].idx;
    }

    inline unsigned val_back()
    {
	return _data[_size - 1].val;
    }

    /*** Modifiers ***/
    unsigned short push_back(unsigned short idx, unsigned val)
    {
	if (_size == _capacity) 
            resize(_size);
		
	unsigned short pos = _size;
	_data[_size].idx = idx;
	_data[_size].val = val;
	_size++;

	return pos;
    }

    inline void pop_back()
    {
	_size--;
    }

    unsigned short insert(unsigned short idx, unsigned val)
    {
	return push_back(idx, val);
    }

    bool erase_idx(unsigned short idx)
    {
	//costly typically avoid
	for (unsigned i = 0; i<_size; ++i)
        {
            if (_data[i].idx == idx)
            {
		_data[i] = _data[_size - 1];
		_size--;
		return true;
            }
	}
	return false;
    }

    inline unsigned short erase_pos(unsigned short pos)
    {
        //if (0 <= pos && pos < _size) {
        _size--;
	_data[pos] = _data[_size];
	return _data[pos].idx;
	//}
	//return false;
    }

    void clear_to_capacity(unsigned short cap)
    {
	_size = 0;
	if (_capacity != cap)
	{
            delete[] _data;
            _capacity = cap;
            _data = new sp_container[_capacity];
	}
    }

    void clear()
    {
	_size = 0;
	//for (short i = 0; i < _capacity; ++i) {
	//Arrays.fill(_idx, (short)0);
	//Arrays.fill(_val, (byte)0);
	//}
    }

    inline void increment(unsigned short pos)
    {
	//if (0 <= pos && pos < _size) {
	_data[pos].val++;
	//_val[pos] = csuros::increment(_val[pos], r);
	//}
	//else {
    	//	throw new Error("Invalid position");
	//}
    }

    inline void decrement(unsigned short pos)
    {
	//if (0 <= pos && pos < _size) {
	_data[pos].val--;
	//	}
    }

    inline void update(unsigned short pos, unsigned val)
    {
	//if (0 <= pos && pos < _size) {
	_data[pos].val = val;
	//}
    }
};

class spvector_sorted : public spvector
{
public:
    unsigned short sorted_insert(unsigned short idx, unsigned val)
    {
	if (_size == _capacity) 
            resize(_size);
        
        unsigned short pos=0;
        /* find correct position to insert */
        while( (pos <_size) && (val <= _data[pos].val) )
            ++pos;
        
        for (unsigned short k=_size; k > pos; --k)
            _data[k] = _data[k-1];
		
	_data[pos].idx = idx;
	_data[pos].val = val;
	++_size;

	return pos;
    }
    
    int sorted_inc(unsigned short idx)
    {
        unsigned short k=0;
        /* increment after which it is sorted as well */
        while( (k <_size) && (idx != _data[k].idx) )
            ++k;
        
        if (k >= _size)	// if not found in existing topics
            this->push_back(idx, 1);
        else
        {
            _data[k].val += 1;
            while( (k > 0) && (_data[k - 1].val < _data[k].val) )
            {
                std::swap(_data[k-1], _data[k]);
                --k;
            }
        }
        
        return 0;
    }
    
    int sorted_dec(unsigned short idx)
    {
        unsigned short k=0;
        /* decrement after which it is sorted as well */
        while( (k <_size) && (idx != _data[k].idx) )
            ++k;
        
        _data[k++].val -= 1;
        
        while( (k < _size) && (_data[k - 1].val < _data[k].val) )
        {
            std::swap(_data[k-1], _data[k]);
            ++k;
        }
        
        if (_data[_size-1].val == 0)	/* If zeroRemove zeros */
            --_size;

        return 0;
    }
};

#endif
