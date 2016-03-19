#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "../commons/utils.h"

class topwords
{
public:
	/****** constructor/destructor ******/
	topwords();
	~topwords();

	/****** interacting functions ******/
	int init(int argc, char** argv);			// initialize the model and read data
	int interact();

protected:
	/****** DATA ******/
	unsigned V, K;									// Model parameters
	float **phi_wk;								// probability of word given topic
	std::map<unsigned, std::string> id2word;			// word map [int => string]
	std::map<std::string, unsigned> word2id;			// word map [string => int]

	/****** File and Folder Paths ******/
	std::string vfile;		// vocabulary file
	std::string dfile;		// phi matrix file
	
	/****** Interaction variable ******/
	unsigned n;					// number of top words to display
	std::string cmd;		// current command entered
};
