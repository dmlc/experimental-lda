#include "vose.h"

voseAlias::voseAlias()
{
	n = 0;
	w = NULL;
	table = NULL;
	wsum = 0;
	noSamples = -1;
}
voseAlias::voseAlias(double* weights, double T, int num)
{
	n = num;
	//Create arrays Alias and Prob, each of size n.
	table = new std::pair<double, int>[n];
	w = weights;
	wsum = T;
	constructTable();
	noSamples = -1;
}
voseAlias::~voseAlias()
{
	if (w)
		delete[] w;
	if (table)
		delete[] table;
}
void voseAlias::init(int num)
{
	n = num;
	w = new double[n];
	wsum = 0;
	table = new std::pair<double, int>[n];
	noSamples = -1;
}
void voseAlias::copy(const voseAlias &other)
{
	n = other.n;
	wsum = other.wsum;
	noSamples = other.noSamples;
	std::copy(&other.w[0], &other.w[n], w);
    	std::copy(&other.table[0], &other.table[n], table);
}
void voseAlias::recompute(double* weights, double T)
{
	w = weights;
	wsum = T;
	constructTable();
}
void voseAlias::constructTable()
{
	double *p = new double[n];
	
	//1. Create two worklists, Small and Large.
	std::queue<int> Small, Large;

	//2. Multiply each probability by n.
	for (int i = 0; i < n; ++i)
	{
		p[i] = (w[i] * n) / wsum;
	}

	//3. For each scaled probability pi:
	//		a. If pi<1, add i to Small.
	//		b. Otherwise(pi≥1), add i to Large.
	for (int i = 0; i < n; ++i)
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
		int l = Small.front(); Small.pop();
		int g = Large.front(); Large.pop();
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
		int g = Large.front(); Large.pop();
		table[g].first = 1;	//Prob[g] = 1;
	}

	//6. While Small is not empty : This is only possible due to numerical instability.
	//		a. Remove the first element from Small; call it l.
	//		b. Set Prob[l] = 1.
	while (!Small.empty())
	{
		int l = Small.front(); Small.pop();
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
