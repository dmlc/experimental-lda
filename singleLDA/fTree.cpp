#include "fTree.h"

fTree::fTree()
{
	T = 0;
	w = NULL;
}

fTree::fTree(double* weights, int num)
{
	init(num);
	constructTree(weights);
}

fTree::~fTree()
{
	if (w)
		delete[] w;
}

void fTree::init(int num)
{
	T = num;
	w = new double[2*T];
}

void fTree::copy(const fTree &other)
{
	T = other.T;
	std::copy(&other.w[0], &other.w[T], w);
}

void fTree::recompute(double* weights)
{
	constructTree(weights);
}

void fTree::constructTree(double* weights)
{
	// Reversely initialize elements
	for (int i = 2 * T - 1; i > 0; --i)
	{
		if (i >= T)
			w[i] = weights[i - T];
		else
			w[i] = w[2 * i] + w[2 * i + 1];
	}
}
int fTree::sample()
{
	int i = 1;
	double u = utils::unif01() * w[i];
	while (i < T)
	{				
		if (u < w[2 * i])
		{
			i = 2 * i;
		}
		else
		{ 			
			u = u - w[2 * i];
			i = 2 * i + 1;
		}
	}

	return i - T;
}
void fTree::update(int t, double delta)
{
	// t = 0..T-1, 
	int i = t + T;
	while (i > 0) {
		w[i] += delta;
		i = i / 2;
	}
}
double fTree::getComponent(int t)
{
	// t = 0..T-1
	return w[t + T];
}