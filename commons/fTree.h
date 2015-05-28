#ifndef _FTREE_H
#define _FTREE_H

#include "utils.h"

class fTree
{
public:
	double *w;	//tree structure
	
	int T;		//Dimension

	fTree();	
	fTree(double* weights, int num);
	~fTree();
	void init(int num);
	//void copy(const fTree &other);
	void recompute(double* weights);
	void update(int t, double new_w);
	double getComponent(int t);
	void constructTree(double* weights);
	int sample(double u);
};

#endif
