#ifndef _FTREE_1_H
#define _FTREE_1_H

#include "utils.h"

class fTree_1
{
public:
    double *w;	//tree structure
    unsigned short T;		//Dimension

    fTree_1()
    {
        T = 0;
        w = NULL;
    }
	
    fTree_1(double* weights, unsigned short num)
    {
        init(num);
        constructTree(weights);
    }

    ~fTree_1()
    {
        if (w)
            delete[] w;
    }
		
    inline void init(unsigned short num)
    {
        T = num;
	if(w) delete[] w;
        w = new double[2*T];
    }

    //void copy(const fTree &other);

    inline void recompute(double* weights)
    {
        constructTree(weights);
    }
	
    inline void update(unsigned short t, double delta)
    {
        short i = t + T;
        while (i > 0)
        {
            w[i] += delta;
            i = i >> 1;
        }
    }

    inline float getComponent(unsigned short t)
    {
        return w[t + T];
    }

    inline void constructTree(double* weights)
    {
        // Reversely initialize elements
        unsigned i = 2 * T - 1;
	while (i >= T)
	{
		w[i] = weights[i - T];
		--i;
	}

	while(i > 0)
	{
		w[i] = w[2 * i] + w[2 * i + 1];
		--i;
	}
    }
	
    inline short sample(double u)
    {
        unsigned short i = 1;
        u = u * w[i];
        while (i < T)
        {
	    i <<= 1;
            if (u >=  w[i])
            {
                u -= w[i];
                ++i;
            }
        }
        return i - T;
    }

};

#endif
