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
        for (unsigned i = 0; i < M; i++)
            delete docs[i];
    }
    delete[] docs;
}

void dataset::add_doc(document * doc, unsigned idx)
{
    if (0 <= idx && idx < M)
        docs[idx] = doc;
}

int dataset::read_data(std::string dfile, std::map<std::string, unsigned> * pword2id, std::set<std::string> * stopwords)
{   
    std::ifstream fin(dfile);
    if (!fin) 
	throw std::runtime_error("Error: Invalid data file: " + dfile);

    if (stopwords == NULL)
    	stopwords = new std::set<std::string>;	 
    
    std::map<std::string, unsigned>::iterator it;

    std::string line;

    // retrieve the number of documents in dataset
    std::getline(fin, line);
    M = std::stoi(line);
    if (M <= 0)
        throw std::runtime_error( "Error: Empty corpus!" );
    
    // allocate memory for corpus
    docs = new document*[M];
    
    // set number of words to current dictionary size
    V = pword2id->size();
    
    int temp_words[1000000];
    for (unsigned i = 0; i < M; i++)
    {
	std::getline(fin, line);

	StringTokenizer strtok(line);
		
	unsigned length = strtok.count_tokens();
	if (length <= 0)
	    std::runtime_error("Error: Invalid document object! " + i);
         
	unsigned js=0;
	for (unsigned j = 0; j < length; ++j)
	{
            std::string key = strtok.nextToken();
            if(stopwords->count(key))
                continue;
            it = pword2id->lower_bound(key);
            if (it == pword2id->end() || key < it->first)
            {
		// word not found, i.e., new word
                temp_words[j] = pword2id->size();
		pword2id->insert(it, std::pair<std::string, unsigned>(key, pword2id->size()));
            }
            else
            {
                // Give the word current id
                temp_words[j] = it->second;
            }
            ++js;
	}
	
	// add new doc to the corpus
	document * pdoc = new document(js);
	for (unsigned j = 0; j < js; ++j)
            pdoc->words[j] = temp_words[j];
	add_doc(pdoc, i);
    }
    
    fin.close();
       
    // update number of words
    V = pword2id->size();
    
    return 0;
}

int dataset::write(std::ofstream& fout)
{
    if (!fout)
        throw std::runtime_error("Cannot open to write!");
    fout.write((char *)&version, sizeof(int));
    fout.write((char *)&V, sizeof(unsigned));
    fout.write((char *)&M, sizeof(unsigned));
    for (unsigned i = 0; i < M; ++i)
    {
            fout.write((char *)&(docs[i]->length), sizeof(unsigned));
            fout.write((char *)(docs[i]->words), sizeof(unsigned)*docs[i]->length);
    }

    return 0;
}

int dataset::read(std::ifstream& fin)
{
    int temp;
    if (!fin)
        throw std::runtime_error("Dataset not found!");
    fin.read((char *)&temp, sizeof(int));
    if (temp != version)
        throw std::runtime_error("Dataset version incorrect!");

    fin.read((char *)&V, sizeof(unsigned));
    fin.read((char *)&M, sizeof(unsigned));

    docs = new document*[M];
    for (unsigned i = 0; i < M; ++i)
    {
        docs[i] = new document();
        fin.read((char *)&(docs[i]->length), sizeof(unsigned));
        docs[i]->words = new unsigned[docs[i]->length];
        fin.read((char *)(docs[i]->words), sizeof(unsigned)*docs[i]->length);
    }

    return 0;
}
