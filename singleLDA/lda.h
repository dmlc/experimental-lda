#ifndef _LDA_H
#define _LDA_H

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include "model.h"
#include "utils.h"
#include "vose.h"
#include "fTree.h"
#include "forest.h"

class simpleLDA : public model
{
public:
	// estimate LDA model using Gibbs sampling
	int specific_init();
	int sampling(int m);
};

class unifLDA : public model
{
public:
	// estimate LDA using Metropolis within Gibbs with uniform proposal
	int specific_init();
	int sampling(int m);
};

class sparseLDA : public model
{
public:
	std::vector< std::vector< std::pair<int, int> > > nws;
	
	double ssum, rsum, qsum;
	double *q1;

	// estimate LDA model using sparse strategy of Yao09
	int specific_init();
	int sampling(int m);
};

class aliasLDA : public model
{
public:
	std::vector<voseAlias*> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int m);
	void generateQtable(int i);

	~aliasLDA() 
	{
		for(int w=0; w<V; ++w)
			delete q[w];
	}
};

class FTreeLDA : public model
{
public:
	fTree *trees;

	// estimate LDA model using F+ Tree
	int specific_init();
	int sampling(int m);
};

class forestLDA : public model
{
public:
	forestSample *q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int m);
};

class lightLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int m);
	void generateQtable(int i);
};
#endif
