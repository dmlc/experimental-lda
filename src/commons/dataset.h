#ifndef	_DATASET_H
#define	_DATASET_H

#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "stringtokenizer.h"

class dataset
{
    const int version = 1;

public:

    struct document
    {
    public:
   	unsigned *words;
	unsigned length;

        document()
	{
            words = NULL;
            length = 0;
	}

	document(unsigned length)
	{
            this->length = length;
            words = new unsigned[length];
	}

	~document()
	{
            if (words)
		delete[] words;
	
	}
    };
    document ** docs;
    unsigned M; // number of documents
    unsigned V; // number of words
    
    dataset();
    ~dataset();
    
    void add_doc(document * doc, unsigned idx);

    int read_data(std::string dfile, std::map<std::string, unsigned> * wordmap, std::set<std::string> * stopwords);

    int write(std::ofstream& fout);
    int read(std::ifstream& fin);
};

#endif

