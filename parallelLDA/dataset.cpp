#include "dataset.h"

int dataset::read_data(std::string dfile, mapword2id * pword2id)
{   
	std::ifstream fin(dfile);
    if (!fin) 
	{
		std::cout << "Cannot open file to read! " << dfile << std::endl;
		return 1;
    }   
    
    mapword2id::iterator it; 
    
	std::string line;
    
    // get the number of documents
	std::getline(fin, line);
	M = stoi(line);
    if (M <= 0)
	{
		std::cout << "No document available!" << std::endl;
		return 1;
    }
    
    // allocate memory for corpus
    if (docs)
	{
		deallocate();
    } 
	else
	{
		docs = new document*[M];
    }
    
    // set number of words to current dictionary size
    V = pword2id->size();
    
    for (int i = 0; i < M; i++)
	{
		std::getline(fin, line);
		strtokenizer strtok(line, " \t\r\n");
		int length = strtok.count_tokens();

		if (length <= 0)
		{
			std::cout << "Invalid (empty) document!" << std::endl;
			deallocate();
			M = V = 0;
			return 1;
		}
	
		// allocate new document
		document * pdoc = new document(length);
	
		for (int j = 0; j < length; j++)
		{
			it = pword2id->find(strtok.token(j));
		    if (it == pword2id->end())
			{
				// word not found, i.e., new word
				pdoc->words[j] = pword2id->size();
				pword2id->insert(std::pair<std::string, int>(strtok.token(j), pword2id->size()));
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

