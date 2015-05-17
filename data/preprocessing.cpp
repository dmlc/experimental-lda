/**
 * @author: Loc Do
 * @modified: manzil
 * @date: 01/01/2015
 * This code is to pre-process the UCI data format with several changes
 * - replace the wordID with real word
 * - shuffle randomly the words within each document
 */
#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <random>
#include <algorithm>

struct document{
	std::vector<long> wordIDArr;
	std::vector<long> counterArr;
} *docs;

long D = 0, W = 0, N = 0, NNZ = 0;

std::string *mapID2W = nullptr;

long *pNNZ = nullptr;

int readWordMap(const std::string & filename) {
	std::ifstream fin (filename);

	if (!fin.is_open()) {
		std::cout << "Cannot open file to read! " << filename << std::endl;
		return 0;
	}

	std::string line;	
	
	if (W == 0)	{
		std::cout << "Notice: readWordMap func should be run after reading the document data"
			 << std::endl;
	}

	mapID2W = new std::string[W+1];
	long idW = 1;
	while (	fin >> mapID2W[idW] ) {
		mapID2W[idW].erase(std::remove_if(mapID2W[idW].begin(), mapID2W[idW].end(), [](int c)->bool{return !isalnum(c);}), mapID2W[idW].end());
		++idW;
    }
    
    std::cout << "There are " << idW << " words in the dictionary." 
    	 << std::endl;

	fin.close();
	return 0;
}

int readDocumentData(const std::string & filename) {
	std::ifstream fin (filename.c_str());

	if (!fin.is_open()) {
		std::cout << "Cannot open file to read! " << filename << std::endl;
		return 0;
	}

    std::string line;	

	// Read no of docs	
	fin >> line; 
	D = stol(line);
	
	// Read no of unique words
	fin >> line; 
	W = stol(line);

	// Read no of non-zero token (not important)
	fin >> line; 
	NNZ = stol(line);

	std::cout << D << " " << W << " " << NNZ << std::endl;

	docs = new document[D+1];
	
	pNNZ = new long[D+1];
	for (int i = 0; i <= D; ++i)
		pNNZ[i] = 0;
	//memset(pNNZ, 0, sizeof(long)*(D+1));

	while (!fin.eof())
	{	
		long docID, wordID, count;		
		fin >> docID >> wordID >> count;
		
		if (docID >= D+1 || docID <1) {
			std::cout << "Invalid document ID." << std::endl;
			break;
		}
		
		docs[docID].wordIDArr.push_back(wordID);
		docs[docID].counterArr.push_back(count);
		pNNZ[docID]+=count;
    }
    
    std::cout << "Done with reading document data." << std::endl;
	fin.close();
	return 0;
}

int shuffleAndOutput(const std::string& filename, const std::string& nr_test, const std::string& shuf = "0") {
	
	std::default_random_engine generator;
	
	int num_test = std::stoi(nr_test);
	int shuffle = std::stoi(shuf);
	std::ofstream fout;
	
	fout.open(filename+".train");
	fout << D-num_test << std::endl;
	for (long idDoc = 1; idDoc <= D-num_test; idDoc++) {
		
		if(idDoc%1000==0)
			std::cout << "Processing document: " << idDoc << " with " << pNNZ[idDoc] << " tokens. " << std::endl;

		int pos = 0;
		std::string newLine = "";
		while (pNNZ[idDoc] > 0)
		{
			if(shuffle)
			{
				// Randomly draw a word based on its occurences in the document
				std::discrete_distribution<long> distribution (docs[idDoc].counterArr.begin(), docs[idDoc].counterArr.end());
				pos = distribution(generator);
			}
			else
			{
				// Same order as in input document
				if(docs[idDoc].counterArr[pos]<=0)
					pos++;
			}
			newLine += mapID2W[docs[idDoc].wordIDArr[pos]]+" ";
			docs[idDoc].counterArr[pos]--;
			pNNZ[idDoc]--;
		}
		fout << newLine << std::endl;
	}
	fout.close();
	
	fout.open(filename+".test");
	fout << num_test << std::endl;
	for (long idDoc = D-num_test+1; idDoc <= D; idDoc++) {
		
		if(idDoc%1000==0)
			std::cout << "Processing document: " << idDoc << " with " << pNNZ[idDoc] << " tokens. " << std::endl;

		std::string newLine = "";
		while (pNNZ[idDoc] > 0)
		{
			int pos = 0;
			if(shuffle)
			{
				// Randomly draw a word based on its occurences in the document
				std::discrete_distribution<long> distribution (docs[idDoc].counterArr.begin(), docs[idDoc].counterArr.end());
				pos = distribution(generator);
			}
			else
			{
				// Same order as in input document
				if(docs[idDoc].counterArr[pos]<=0)
					pos++;
			}
			newLine += mapID2W[docs[idDoc].wordIDArr[pos]]+" ";
			docs[idDoc].counterArr[pos]--;
			pNNZ[idDoc]--;
		}
		fout << newLine << std::endl;
	}
	fout.close();
	return 0;	
}

int main(int argc, char** argv) {

	if (argc < 5) {
		std::cout << "Command line should be: processing <document_file> <dictionary_file> <output_file> <number_of_test_docs> [shuffle]"
			 << std::endl;
		return -1;
	}

	std::vector<std::string> arguments(argv, argv + argc);

	readDocumentData(arguments[1]);
	readWordMap(arguments[2]);
	if(argc==5)
		shuffleAndOutput(arguments[3],arguments[4]);
	else
		shuffleAndOutput(arguments[3],arguments[4], arguments[5]);

	std::cout << "DONE." << std::endl;

	return 0;
}
