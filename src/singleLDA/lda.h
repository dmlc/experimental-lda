#ifndef _LDA_H
#define _LDA_H

#include <iostream>
#include <thread>
#include <chrono>

#include "../commons/utils.h"
#include "../commons/vose.h"
#include "../commons/fTree.h"
#include "../commons/forest.h"

#include "model.h"

class simpleLDA : public model
{
public:
	// estimate LDA model using Gibbs sampling
	int specific_init();
	int sampling(unsigned m);
};

class unifLDA : public model
{
public:
	// estimate LDA using Metropolis within Gibbs with uniform proposal
	int specific_init();
	int sampling(unsigned m);
};

class sparseLDA : public model
{
public:
	std::vector< std::vector< std::pair<int, int> > > nws;
	
	double ssum, rsum, qsum;
	double *q1;

	// estimate LDA model using sparse strategy of Yao09
	int specific_init();
	int sampling(unsigned m);
};

class aliasLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(unsigned m);
	void generateQtable(unsigned i);
};

class FTreeLDA : public model
{
public:
	fTree *trees;

	// estimate LDA model using F+ Tree
	int specific_init();
	int sampling(unsigned m);
};

class forestLDA : public model
{
public:
	forestSample *q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(unsigned m);
};

class lightLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(unsigned m);
	void generateQtable(unsigned i);

};
#endif
