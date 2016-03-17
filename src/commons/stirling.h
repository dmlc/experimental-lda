#ifndef _STIRLING_H
#define	_STIRLING_H

#include <vector>
#include <cmath>

#define log_zero -10000.0

class Stirling
{
	std::vector<double*> log_stirling_num_;
	std::vector<double*> u_stirling_ratio_;
	double log_sum(double log_a, double log_b);

public:
	Stirling();
	double get_log_stirling_num(int n, int m);	//return the log of stirling(n,m)
	double uratio(int n, int m);	//return the ratio of stirling(n+1,m)/stirling(n,m)
	double vratio(int n, int m);	//return the ratio of stirling(n+1,m+1)/stirling(n+1,m)
	double wratio(int n, int m);	//return the ratio of stirling(n+1,m+1)/stirling(n,m)
};

#endif
