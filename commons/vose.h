#ifndef _VOSE_H
#define _VOSE_H

#include <atomic>
#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include "utils.h"

class voseAlias
{
public:
	std::vector<double> w;			//Contains proportions and alias probabilities
	double wsum;		//Sum of proportions
	std::vector<std::pair<double, int>> table; //Alias probabilities and indices
	int n;				//Dimension
	int noSamples;		// number of samples between replenishment

	voseAlias() { }
	voseAlias(const voseAlias & c) { };
	voseAlias(voseAlias && m) { };
		
	void init(int num);
	void recompute(double* p, double T);
	void constructTable();	
	inline int sample(double u1, double u2) const
	{
		//1. Generate a fair die roll from an n-sided die; call the side i.
		int fair_die = (int)(u1*n); // utils::pick_a_number(0, n - 1);
		//2. Flip a biased coin that comes up heads with probability Prob[i].
		bool res = u2 < table[fair_die].first; // Prob[fair_die]; //
		//3. If the coin comes up "heads," return i. Otherwise, return Alias[i].
        return res ? fair_die : table[fair_die].second; //Alias[fair_die];
    }
};

#endif
