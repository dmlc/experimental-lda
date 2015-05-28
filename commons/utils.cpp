#include "utils.h"

//int utils::binary_search(double A[], double key, int imin, int imax)
//{
//	// continue searching while [imin,imax] is not empty
//	while (imax >= imin)
//	{
//		// calculate the midpoint for roughly equal partition
//		int imid = imin + (imax - imin) / 2;
//		if (A[imid] == key)
//			// key found at index imid
//			return imid;
//		// determine which subarray to search
//		else if (A[imid] < key)
//			// change min index to search upper subarray
//			imin = imid + 1;
//		else
//			// change max index to search lower subarray
//			imax = imid - 1;
//	}
//	// key was not found
//	return (A[imin]<=key?imin+1:imin);
//}

// std::mt19937 & utils::urng()
// {
	// static std::mt19937 u{};
	// return u;
// }

// void utils::randomize()
// {
	// static std::random_device rd{};
	// urng().seed(rd());
	// //urng().seed(3);
// }

// double utils::unif01()
// {
	// static std::uniform_real_distribution<double> d(0.0, 1.0);
	// return d(urng());
// }

int utils::coin_toss(double p)
{
	static std::bernoulli_distribution d{};
	using parm_t = decltype(d)::param_type;
	return d(urng(), parm_t{ p });
}

int utils::pick_a_number(int from, int thru)
{
	static std::uniform_int_distribution<> d{};
	using parm_t = std::uniform_int_distribution<>::param_type;
	return d(urng(), parm_t{ from, thru });
}


/*template<class iterator_type>
int die_throw(iterator_type  begin, iterator_type  end)
{
	static std::discrete_distribution<> d{};
	using parm_t = decltype(d)::param_type;
	return d(global_urng(), parm_t{ begin, end });
}*/

//short utils::discreteLog(int v)
//{
//	union { unsigned int u[2]; double d; } t;
//	t.u[1] = 0x43300000;
//	t.u[0] = v;
//	t.d -= 4503599627370496.0;
//	//short r = (t.u[1] >> 20) - 0x3FF;
//	//return r;
//	short r = (t.u[1] >> 20);
//	return r&0x3F;
//}
//short utils::discreteLog(double v)
//{
//	__int64 L = *(__int64*)&v;
//	L = L >> 52;
//	//short r = (L & 0x7FF) - 0x3FF;
//	//return  r;
//	short r = (L & 0x7FF);
//	return  r&0x3F;
//}
