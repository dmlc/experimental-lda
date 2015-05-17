#ifndef	_DATASET_H
#define	_DATASET_H

#include <string>
#include <vector>
#include <map>

#include "utils.h"

class document
{
public:
	int * words;
	std::string rawstr;
	int length;

	document()
	{
		words = NULL;
		rawstr = "";
		length = 0;
	}

	document(int length)
	{
		this->length = length;
		rawstr = "";
		words = new int[length];
	}

	document(int length, int * words)
	{
		this->length = length;
		rawstr = "";
		this->words = new int[length];
		for (int i = 0; i < length; i++)
		{
			this->words[i] = words[i];
		}
	}

	document(int length, int * words, std::string rawstr)
	{
		this->length = length;
		this->rawstr = rawstr;
		this->words = new int[length];
		for (int i = 0; i < length; i++)
		{
			this->words[i] = words[i];
		}
	}

	document(std::vector<int> & doc)
	{
		this->length = doc.size();
		rawstr = "";
		this->words = new int[length];
		for (int i = 0; i < length; i++)
		{
			this->words[i] = doc[i];
		}
	}

	document(std::vector<int> & doc, std::string rawstr)
	{
		this->length = doc.size();
		this->rawstr = rawstr;
		this->words = new int[length];
		for (int i = 0; i < length; i++)
		{
			this->words[i] = doc[i];
		}
	}

	~document()
	{
		if (words)
		{
			delete[] words;
		}
	}
};

class dataset
{
public:
    document ** docs;
    int M; // number of documents
    int V; // number of words
    
	dataset()
	{
		docs = NULL;
		M = 0;
		V = 0;
	}
    
	dataset(int M)
	{
		this->M = M;
		this->V = 0;
		docs = new document*[M];
	}
    
	~dataset()
	{
		if (docs)
		{
			for (int i = 0; i < M; i++)
			{
				delete docs[i];
			}
		}
		delete[] docs;
	}
    
	void deallocate()
	{
		if (docs)
		{
			for (int i = 0; i < M; i++)
			{
				delete docs[i];
			}
		}
		delete docs;
		docs = NULL;

	}
    
	void add_doc(document * doc, int idx)
	{
		if (0 <= idx && idx < M)
		{
			docs[idx] = doc;
		}
	}

	int read_data(std::string dfile, mapword2id * wordmap);
};

#endif

