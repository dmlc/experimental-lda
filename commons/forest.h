#ifndef _FOREST_H
#define _FOREST_H

#include <vector>
#include <unordered_map>
#include "mystack.h"
#include "utils.h"

struct carry
{
	char task=0;
	short level=0, pos=0, addr=0;
	double dw=0;
};

class forestSample
{
	short N;
		
	struct node
	{
		double weight = 0;
		short childNum = -1;
		std::vector<short> children;
	};
	node *levels;
	//std::vector<unordered_map<short,node>> forest;

	void processQueue();
	

public:
	
	double wsum;
	//node **forest;
	std::vector<std::vector<node>> forest;

	forestSample();

	bool preprocess(short n, double* w);

	short generate();

	void update(short pos, double newW);

	void add(double newW);

	void remove(short pos);

};

#endif