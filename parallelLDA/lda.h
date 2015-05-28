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
#include "shared_mutex.h"

class simpleLDA : public model
{
public:
	// estimate LDA model using Gibbs sampling
	int specific_init();
	int sampling(int i);
};

class unifLDA : public model
{
public:
	// estimate LDA using Metropolis within Gibbs with uniform proposal
	int specific_init();
	int sampling(int i);
};

class sparseLDA : public model
{
public:
	shared_mutex* smtx;
	std::vector< std::vector< std::pair<int, int> > > nws;
	
	// estimate LDA model using sparse strategy of Yao09
	int specific_init();
	int sampling(int i);
	int updater(int i);
};

class aliasLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int i);
	void generateQtable(int i);
};

class FTreeLDA : public model
{
public:
	fTree *trees;

	// estimate LDA model using F+ Tree
	int specific_init();
	int sampling(int i);
	int updater(int i);
};

class forestLDA : public model
{
public:
	forestSample *q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int i);
	int updater(int i);
};

class lightLDA : public model
{
public:
	std::vector<voseAlias> q;

	// estimate LDA model using alias sampling
	int specific_init();
	int sampling(int i);
	void generateQtable(int i);
};
#endif
