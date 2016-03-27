#ifndef _LDA_H
#define _LDA_H

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>
#include <shared_mutex>

#include "model.h"
#include "../commons/utils.h"
#include "../commons/vose.h"
#include "../commons/fTree.h"
//#include "../commons/forest.h"
#include "../commons/shared_mutex.h"

class simpleLDA : public model
{
public:
	// estimate LDA model using Gibbs sampling
	int specific_init();
	int sampling(unsigned i);
};

class sparseLDA : public model
{
public:
	shared_mutex* smtx;
	std::vector< spvector_sorted > nws;
	//std::vector< std::vector< std::pair<unsigned short, unsigned> > > nws;
	
	// estimate LDA model using sparse strategy of Yao09
	int specific_init();
	int sampling(unsigned i);
	int updater(unsigned i);
	unsigned num_table_threads(unsigned nt) { return nt/4; }
};

class aliasLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(unsigned i);
	void generateQtable(unsigned i);
};

class FTreeLDA : public model
{
public:
	fTree *trees;

	// estimate LDA model using F+ Tree
	int specific_init();
	int sampling(unsigned i);
	int updater(unsigned i);
	unsigned num_table_threads(unsigned nt) { return 3*nt/16; }

	FTreeLDA()
	{
		trees = NULL;
	}
	~FTreeLDA()
	{
		if(trees) delete[] trees;
	}

};

class lightLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(unsigned i);
	void generateQtable(unsigned i);
};
#endif
