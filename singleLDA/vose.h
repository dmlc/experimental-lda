#ifndef _VOSE_H
#define _VOSE_H

#include <queue>
#include <random>
#include <algorithm>
#include "utils.h"

class voseAlias
{
public:
	double *w;		//Contains proportions and alias probabilities
	double wsum;		//Sum of proportions
	std::pair<double, int> *table; //Alias probabilities and indices
	int n;				//Dimension
	int noSamples;		// number of samples between replenishment

	voseAlias();
	voseAlias(double* weights, double T, int num);
	~voseAlias();
	
	void init(int num);
	void copy(const voseAlias &other);
	void recompute(double* p, double T);
	void constructTable();	
	inline int sample()
	{
		//1. Generate a fair die roll from an n-sided die; call the side i.
		int fair_die = (int)(utils::unif01()*n); // utils::pick_a_number(0, n - 1);
		//2. Flip a biased coin that comes up heads with probability Prob[i].
		bool res = utils::unif01() < table[fair_die].first; // Prob[fair_die]; //
		//3. If the coin comes up "heads," return i. Otherwise, return Alias[i].
        return res ? fair_die : table[fair_die].second; //Alias[fair_die];
    }
};

#endif
