#include "utils.h"

int utils::write_wordmap(std::string wordmapfile, mapword2id * pword2id)
{
	std::ofstream fout(wordmapfile);
	if (!fout)
	{
		std::cout << "Cannot open file to write!: " << wordmapfile << std::endl;
		return 1;
	}

	mapword2id::iterator it;
	fout << pword2id->size() << std::endl;
	for (it = pword2id->begin(); it != pword2id->end(); it++)
	{
		fout << it->first << " " << it->second << std::endl;
	}

	fout.close();

	return 0;
}

int utils::read_wordmap(std::string wordmapfile, mapword2id * pword2id)
{
	pword2id->clear();

	std::ifstream fin(wordmapfile);
	if (!fin)
	{
		std::cout << "Cannot open file to read! " << wordmapfile << std::endl;
		return 1;
	}

	std::string line;
	int nwords;
	std::getline(fin, line);
	nwords = stoi(line);

	for (int i = 0; i < nwords; i++)
	{
		std::getline(fin, line);

		strtokenizer strtok(line, " \t\r\n");
		if (strtok.count_tokens() != 2)
		{
			continue;
		}

		pword2id->insert(std::pair<std::string, int>(strtok.token(0), stoi(strtok.token(1))));
	}

	fin.close();

	return 0;
}

int utils::read_wordmap(std::string wordmapfile, mapid2word * pid2word)
{
	pid2word->clear();

	std::ifstream fin(wordmapfile);
	if (!fin)
	{
		std::cout << "Cannot open file to read! " << wordmapfile << std::endl;
		return 1;
	}

	std::string line;
	int nwords;
	std::getline(fin, line);
	nwords = stoi(line);

	for (int i = 0; i < nwords; i++)
	{
		std::getline(fin, line);


		strtokenizer strtok(line, " \t\r\n");
		if (strtok.count_tokens() != 2)
		{
			continue;
		}

		pid2word->insert(std::pair<int, std::string>(stoi(strtok.token(1)), strtok.token(0)));
	}

	fin.close();

	return 0;
}

void utils::sort(std::vector<double> & probs, std::vector<int> & words)
{
    for (unsigned i = 0; i < probs.size() - 1; i++) 
	{
		for (unsigned j = i + 1; j < probs.size(); j++) 
		{
			if (probs[i] < probs[j]) 
			{
				double tempprob = probs[i];
				int tempword = words[i];
				probs[i] = probs[j];
				words[i] = words[j];
				probs[j] = tempprob;
				words[j] = tempword;
			}
		}
    }
}

void utils::quicksort(std::vector<std::pair<int, double> > & vect, int left, int right)
{
    int l_hold, r_hold;
	std::pair<int, double> pivot;
    
    l_hold = left;
    r_hold = right;    
    int pivotidx = left;
    pivot = vect[pivotidx];

    while (left < right)
	{
		while (vect[right].second <= pivot.second && left < right)
		{
			right--;
		}
		if (left != right)
		{
		    vect[left] = vect[right];
			left++;
		}
		while (vect[left].second >= pivot.second && left < right)
		{
			left++;
		}
		if (left != right) 
		{
			vect[right] = vect[left];
			right--;
		}
    }

    vect[left] = pivot;
    pivotidx = left;
    left = l_hold;
    right = r_hold;
    
    if (left < pivotidx)
	{
		quicksort(vect, left, pivotidx - 1);
    }
    if (right > pivotidx) 
	{
		quicksort(vect, pivotidx + 1, right);
    }    
}

void utils::quicksort(std::vector<std::pair<int, int> > & vect, int left, int right)
{
	int l_hold, r_hold;
	std::pair<int, int> pivot;

	l_hold = left;
	r_hold = right;
	int pivotidx = left;
	pivot = vect[pivotidx];

	while (left < right)
	{
		while (vect[right].second <= pivot.second && left < right)
		{
			right--;
		}
		if (left != right)
		{
			vect[left] = vect[right];
			left++;
		}
		while (vect[left].second >= pivot.second && left < right)
		{
			left++;
		}
		if (left != right)
		{
			vect[right] = vect[left];
			right--;
		}
	}

	vect[left] = pivot;
	pivotidx = left;
	left = l_hold;
	right = r_hold;

	if (left < pivotidx)
	{
		quicksort(vect, left, pivotidx - 1);
	}
	if (right > pivotidx)
	{
		quicksort(vect, pivotidx + 1, right);
	}
}

int utils::binary_search(double A[], double key, int imin, int imax)
{
	// continue searching while [imin,imax] is not empty
	while (imax >= imin)
	{
		// calculate the midpoint for roughly equal partition
		int imid = imin + (imax - imin) / 2;
		if (A[imid] == key)
			// key found at index imid
			return imid;
		// determine which subarray to search
		else if (A[imid] < key)
			// change min index to search upper subarray
			imin = imid + 1;
		else
			// change max index to search lower subarray
			imax = imid - 1;
	}
	// key was not found
	return (A[imin]<=key?imin+1:imin);
}

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
