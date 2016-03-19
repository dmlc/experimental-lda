#include "stirling.h"
#include <iostream>

using namespace std;

double Stirling::log_sum(double log_a, double log_b)
{
	double v;

	if (log_a < log_b) {
		if (exp(log_a - log_b) <= -1)
			std::cout << "Negative log..." << endl;			
		v = log_b + log(1 + exp(log_a - log_b));
	}
	else {
		if (exp(log_b - log_a) <= -1)
			std::cout << "Negative log..." << endl;
		v = log_a + log(1 + exp(log_b - log_a));
	}
	return v;
}

Stirling::Stirling() 
{
	log_stirling_num_.reserve(200);
	double* u = new double[1];
	u_stirling_ratio_.push_back(u);
	u_stirling_ratio_[0][0] = INFINITY;
	u = new double[2];
	u_stirling_ratio_.push_back(u);
	u_stirling_ratio_[1][0] = INFINITY;
	u_stirling_ratio_[1][1] = 1;
}


/*
* return the log of the stirling number log(s(n,m))
* s(n, n) = 1
* s(n, 0) = 0 if n > 0
* s(n, m) = 0 if n < m
* s(n+1, m) = s(n, m-1) + ns(n, m)
*/
double Stirling::get_log_stirling_num(int n, int m)
{
	double answer;
	if (n < m)
	{
		answer = log_zero;
	}
	else
	{
		int start = log_stirling_num_.size();
		for (int i = start; i <= n ; ++i)
		{
			double* v = new double[i + 1];
			for (int j = 0; j <= i; ++j)
			{
				v[j] = log_zero;
			}
			log_stirling_num_.push_back(v);
			log_stirling_num_[i][i] = 0.0;
			for (int j = 1; j < i; ++j)
			{
				log_stirling_num_[i][j] = log_sum(log_stirling_num_[i - 1][j - 1],	log(i - 1) + log_stirling_num_[i - 1][j]);
			}
		}
		answer = log_stirling_num_[n][m];
	}
	return answer;
}

double Stirling::uratio(int n, int m)
{
	double answer;
	if (n < m)
	{
		answer = INFINITY;
	}
	else
	{
		int start = u_stirling_ratio_.size();
		for (int i = start; i <= n; ++i)
		{
			double* u = new double[i + 1];
			u_stirling_ratio_.push_back(u);
			u_stirling_ratio_[i][0] = INFINITY;
			u_stirling_ratio_[i][1] = i;
			for (int j = 2; j < i; ++j)
			{
				u_stirling_ratio_[i][j] = u_stirling_ratio_[i - 1][j - 1] + i - (i - 1)*u_stirling_ratio_[i - 1][j-1] / u_stirling_ratio_[i - 1][j];
			}
			u_stirling_ratio_[i][i] = u_stirling_ratio_[i - 1][i - 1] + i;
		}
		answer = u_stirling_ratio_[n][m];
	}
	return answer;
}

double Stirling::vratio(int n, int m)
{
	double answer;
	if (n < m)
	{
		answer = 0;
	}
	else
	{
		answer = 1 / (uratio(n + 1, m + 1) - (n+1));
	}
	return answer;
}

double Stirling::wratio(int n, int m)
{
	double answer;
	if (n <= m)
	{
		answer = 1;
	}
	else
	{
		answer = uratio(n, m + 1);
		answer = answer / (answer - n);
	}
	return answer;
}