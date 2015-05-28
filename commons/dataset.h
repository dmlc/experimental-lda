#ifndef	_DATASET_H
#define	_DATASET_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "stringtokenizer.h"

class dataset
{
public:

	struct document
	{
	public:
		int *words;
		int length;

		document()
		{
			words = NULL;
			length = 0;
		}

		document(int length)
		{
			this->length = length;
			words = new int[length];
		}

		~document()
		{
			if (words)
			{
				delete[] words;
			}
		}
	};
    document ** docs;
    int M; // number of documents
    int V; // number of words
    
	dataset();
	~dataset();
    
	void add_doc(document * doc, int idx);

	int read_data(std::string dfile, std::map<std::string, int> * wordmap);
	static int write_wordmap(std::string wordmapfile, std::map<std::string, int> * wordmap);
};

#endif

