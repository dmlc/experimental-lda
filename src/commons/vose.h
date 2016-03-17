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
	
    unsigned short n;				//Dimension
    double wsum;		//Sum of proportions
    std::vector<std::pair<double, unsigned short>> table; //Alias probabilities and indices

    std::vector<double> w;			//Contains proportions and alias probabilities

    unsigned noSamples;		// number of samples between replenishment

    voseAlias() { }
    voseAlias(const voseAlias &) { };
    voseAlias(voseAlias &&) { };
		
    void init(unsigned short num);
    void recompute(double* p, double T);
    void constructTable();	
    inline unsigned short sample(unsigned short fair_die , double u) const
    {
        //1. Generate a fair die roll from an n-sided die; call the side i.
        //int fair_die = (int)(u1*n); // utils::pick_a_number(0, n - 1);
        //2. Flip a biased coin that comes up heads with probability Prob[i].
        bool res = u < table[fair_die].first; // Prob[fair_die]; //
        //3. If the coin comes up "heads," return i. Otherwise, return Alias[i].
        return res ? fair_die : table[fair_die].second; //Alias[fair_die];
    }
};

#endif
