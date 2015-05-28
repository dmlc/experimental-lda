#include "dataset.h"

dataset::dataset()
{
	docs = NULL;
	M = 0;
	V = 0;
}

dataset::~dataset()
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

void dataset::add_doc(document * doc, int idx)
{
	if (0 <= idx && idx < M)
	{
		docs[idx] = doc;
	}
}

int dataset::read_data(std::string dfile, std::map<std::string, int> * pword2id)
{   
	std::ifstream fin(dfile);
    if (!fin) 
	{
		std::cout << "Error: Invalid data file: " << dfile << std::endl;
		return 1;
    }   
    
	std::map<std::string, int>::iterator it;
    
	std::string line;
    
	// retrieve the number of documents in dataset
	std::getline(fin, line);
	M = stoi(line);
    if (M <= 0)
	{
		std::cout << "Error: Empty corpus!" << std::endl;
		return 1;
    }
    
    // allocate memory for corpus
    docs = new document*[M];
    
    // set number of words to current dictionary size
    V = pword2id->size();
    
    for (int i = 0; i < M; i++)
	{
		std::getline(fin, line);

		StringTokenizer strtok(line);
		
		int length = strtok.count_tokens();
		if (length <= 0)
		{
			std::cout << "Error: Invalid document object!" << std::endl;
			M = V = 0;
			return 1;
		}
	
		// allocate new document
		document * pdoc = new document(length);
	
		for (int j = 0; j < length; j++)
		{
			std::string key = strtok.nextToken();
			it = pword2id->lower_bound(key);
			if (it == pword2id->end() || key < it->first)
			{
				// word not found, i.e., new word
				pdoc->words[j] = pword2id->size();
				pword2id->insert(it, std::pair<std::string, int>(key, pword2id->size()));
			}
			else
			{
				pdoc->words[j] = it->second;
			}
		}
	
		// add new doc to the corpus
		add_doc(pdoc, i);
    }
    
	fin.close();
       
    // update number of words
    V = pword2id->size();
    
    return 0;
}

int dataset::write_wordmap(std::string wordmapfile, std::map<std::string, int> * pword2id)
{
	std::ofstream fout(wordmapfile);
	if (!fout)
	{
		std::cout << "Error: Unable to write to file: " << wordmapfile << std::endl;
		return 1;
	}

	std::map<std::string, int>::iterator it;
	fout << pword2id->size() << std::endl;
	for (it = pword2id->begin(); it != pword2id->end(); it++)
	{
		fout << it->first << " " << it->second << std::endl;
	}

	fout.close();

	return 0;
}