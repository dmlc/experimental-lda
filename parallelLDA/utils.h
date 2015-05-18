#ifndef _UTILS_H
#define _UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <random>

#include "constants.h"
#include "strtokenizer.h"

#ifdef _MSC_VER

typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;

#endif

// map of words/terms [string => int]
typedef std::map<std::string, int> mapword2id;
// map of words/terms [int => string]
typedef std::map<int, std::string> mapid2word;

class utils
{
public:
	// reading and writing wordmaps
	static int write_wordmap(std::string wordmapfile, mapword2id * pword2id);
	static int read_wordmap(std::string wordmapfile, mapword2id * pword2id);
	static int read_wordmap(std::string wordmapfile, mapid2word * pid2word);

	// sort    
	static void sort(std::vector<double> & probs, std::vector<int> & words);
	static void quicksort(std::vector<std::pair<int, double> > & vect, int left, int right);
	static void quicksort(std::vector<std::pair<int, int> > & vect, int left, int right);
	// binary search for key index
	static int binary_search(double A[], double key, int imin, int imax);

	// random number generators
	inline static std::mt19937 & urng()
	{
		static std::mt19937 u{};
		return u;
	}

	inline static void randomize()
	{
		static std::random_device rd{};
		urng().seed(rd());
		//urng().seed(3);
	}

	inline static double unif01()
	{
		static std::uniform_real_distribution<double> d(0.0, 1.0);
		return d(urng());
	}
	
	static int coin_toss(double p);
	static int pick_a_number(int from, int thru);

	// discrete logarithm
	inline static short discreteLog(int v)
	{
		union { unsigned u[2]; double d; } t;
		t.u[1] = 0x43300000;
		t.u[0] = v;
		t.d -= 4503599627370496.0;
		//short r = (t.u[1] >> 20) - 0x3FF;
		//return r;
		short r = (t.u[1] >> 20);
		return r&0x7F;
	}
	inline static short discreteLog(double v)
	{
		int64_t L = *(int64_t*)&v;
		L = L >> 52;
		// short r = (L & 0x7FF) - 0x3FF;
		// return  r;
		short r = (L & 0x7F);
		return  r;
	}
};

#endif

