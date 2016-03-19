#include "vose.h"

void voseAlias::init(unsigned short num)
{
    n = num;
    w.resize(n);
    wsum = 0;
    table.resize(n);
    noSamples = -1;
}
void voseAlias::recompute(double* weights, double T)
{
    for (unsigned short i = 0; i < n; ++i)
            w[i] = weights[i];
    wsum = T;
    constructTable();
}
void voseAlias::constructTable()
{
	double *p = new double[n];
	
	//1. Create two worklists, Small and Large.
	std::queue<unsigned short> Small, Large;

	//2. Multiply each probability by n.
	for (unsigned short i = 0; i < n; ++i)
	{
		p[i] = (w[i] * n) / wsum;
	}

	//3. For each scaled probability pi:
	//		a. If pi<1, add i to Small.
	//		b. Otherwise(pi≥1), add i to Large.
	for (unsigned short i = 0; i < n; ++i)
	{
		if (p[i] < 1)
			Small.push(i);
		else
			Large.push(i);
	}

	//4. While Small and Large are not empty : (Large might be emptied first)
	//		a. Remove the first element from Small; call it l.
	//		b. Remove the first element from Large; call it g.
	//		c. Set Prob[l] = pl.
	//		d. Set Alias[l] = g.
	//		e. Set pg : = (pg + pl)−1. (This is a more numerically stable option.)
	//		f. If pg<1, add g to Small.
	//		g. Otherwise(pg≥1), add g to Large.
	while (!(Small.empty() || Large.empty()))
	{
		unsigned short l = Small.front(); Small.pop();
		unsigned short g = Large.front(); Large.pop();
		table[l].first = p[l]; //Prob[l] = p[l];
		table[l].second = g;   //Alias[l] = g;
		p[g] = (p[g] + p[l]) - 1;
		if (p[g] < 1)
			Small.push(g);
		else
			Large.push(g);
	}

	//5. While Large is not empty :
	//		a. Remove the first element from Large; call it g.
	//		b. Set Prob[g] = 1.
	while (!Large.empty())
	{
		unsigned short g = Large.front(); Large.pop();
		table[g].first = 1;	//Prob[g] = 1;
	}

	//6. While Small is not empty : This is only possible due to numerical instability.
	//		a. Remove the first element from Small; call it l.
	//		b. Set Prob[l] = 1.
	while (!Small.empty())
	{
		unsigned short l = Small.front(); Small.pop();
		table[l].first = 1;	//Prob[l] = 1;
	}

	noSamples = 0;

	delete[] p;
	
}

//int voseAlias::sampleVose()
//{
//	double u = utils::unif01() * n;
//	//1. Generate a fair die roll from an n-sided die; call the side i.
//	int fair_die = (int)(u); // utils::pick_a_number(0, n - 1);
//	//2. Flip a biased coin that comes up heads with probability Prob[i].
//	bool res = u - fair_die > table[fair_die].first; //Prob[fair_die];
//	//3. If the coin comes up "heads," return i. Otherwise, return Alias[i].
//	return res ? fair_die : table[fair_die].second;
//}
