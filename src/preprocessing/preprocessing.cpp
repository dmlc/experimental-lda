/**
 * @author: Manzil
 * @date: 01/01/2015
 * This code is to pre-process the C-LDA/UCI data format
 * - replace the wordID with real word
 * - shuffle randomly the words within each document
 */
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <stdexcept>

class preprocess
{
protected:
    struct document
    {
        std::vector<unsigned> words;
        std::vector<unsigned> counts;
        unsigned docLength;
    
        document() : docLength(0)  {  }
        
    };

    unsigned D;
    unsigned V;
    unsigned long NNZ;
    std::vector< std::string > mapID2W;
    std::vector< document > docs;
    
    std::string dfilename;
    std::string vfilename;
    std::string ofilename;
    unsigned num_test;
    unsigned shuffle;
    
    int shuffleAndOutput()
    {	
	std::default_random_engine generator;
        std::cout << "Keeping aside " << num_test << "test docs." << std::endl;

	std::ofstream fout;	
        fout.open(ofilename+".test");
        //fout << num_test << std::endl;
        unsigned d=0;
	for (auto & doc : docs )
        {
            if (d == num_test)
            {
                fout.close();
                fout.open(ofilename+".train");
                //fout << D-num_test << std::endl;
            }
            if((d++)%1000==0)
		std::cout << "Processing document: " << d << " with " << doc.docLength << " tokens. " << std::endl;

            int pos = 0;
            std::string newLine = "";
            while (doc.docLength > 0)
            {
                if(shuffle)
                {
                    // Randomly draw a word based on its occurences in the document
                    std::discrete_distribution<unsigned> distribution (doc.counts.begin(), doc.counts.end());
                    pos = distribution(generator);
                }
                else
                {
                    // Same order as in input document
                    if(doc.counts[pos]<=0)
                            pos++;
                }
                newLine += mapID2W[doc.words[pos]]+" ";
                doc.counts[pos]--;
                doc.docLength--;
            }
            fout << newLine << std::endl;
	}
	fout.close();
	
	return 0;	
    }
    
    virtual int readWordMap() { return 0; }
    virtual int readDocumentData() { return 0; }
    
public:
    preprocess() : D(0), V(0), NNZ(0)   {   }
    
    preprocess(const std::vector<std::string> & argument) : D(0), V(0), NNZ(0)
    {
	for( const auto & a : argument)
		std::cout << a << " ";
	std::cout<<std::endl;


        this->dfilename = argument[2];
        this->vfilename = argument[3];
        this->ofilename = argument[4];
        this->num_test = std::stoi(argument[5]);
        if (argument.size() > 6)
            this->shuffle = std::stoi(argument[5]);
        else
            this->shuffle = 0;
    }
    
    int start()
    {
        readDocumentData();
        readWordMap();
        shuffleAndOutput();
    }
    
};

class clda : public preprocess
{
protected:
    int readWordMap()
    {    
        std::ifstream fin (vfilename);
        if (!fin.is_open())
        {
            std::string msg = "Cannot open vocab file to read! " + vfilename;
            throw std::runtime_error(msg);
        }

        std::string line;
        while ( fin >> line )
        {
            line.erase(std::remove_if(line.begin(), line.end(), [](int c)->bool{return !isalnum(c);}), line.end());
            mapID2W.push_back(line);

            fin >> line; // =
            fin >> line; // number
            if(std::stoi(line)!=V)
            {
                throw std::runtime_error("Some error in reading vocab file");
            }
            fin >> line; // =
            fin >> line; // freq1
            fin >> line; // freq2

            ++V;
        }

        std::cout << "There are " << V << " words in the dictionary." << std::endl;

        fin.close();
        return 0;
    }
    
    int readDocumentData()
    {
        std::ifstream fin (dfilename);

        if (!fin.is_open())
        {
            std::string msg = "Cannot open doc file to read! " + dfilename;
            throw std::runtime_error(msg);
        }

        std::string line;
        while (fin >> line)
        {	
            document doc;
            unsigned docLength, wordID, count;
            if(line.empty())
                continue;
            docLength = std::stoi(line);
            for(unsigned i=0; i<docLength; ++i)
            {
                fin >> line;
                unsigned divider = line.find(':');
                wordID = std::stoi(line.substr(0,divider));
                count = std::stoi(line.substr(divider+1));

                doc.words.push_back(wordID);
                doc.counts.push_back(count);
                doc.docLength+=count;
            }

            NNZ += doc.docLength;
            docs.push_back(doc);
            ++D;
        }

        std::cout << "Done with reading " << D << " document data having " << NNZ << " tokens in total." << std::endl;
        fin.close();
        return 0;
    }
    using preprocess::preprocess;
};

class uci : public preprocess
{
protected:
    int readWordMap()
    {
	std::ifstream fin (vfilename);
	if (!fin.is_open())
        {
            std::string msg = "Cannot open vocab file to read! " + vfilename;
            throw std::runtime_error(msg);
	}

	std::string line;
	mapID2W.resize(V+1);
	long idW = 0;
	while (	fin >> mapID2W[idW] )
        {
            mapID2W[idW].erase(std::remove_if(mapID2W[idW].begin(), mapID2W[idW].end(), [](int c)->bool{return !isalnum(c);}), mapID2W[idW].end());
            ++idW;
        }
    
        if (V!=idW)
            throw std::runtime_error("Size of vocab do not match!");
            
        std::cout << "There are " << V << " words in the dictionary." << std::endl;

	fin.close();
	return 0;
    }

    int readDocumentData()
    {
        std::ifstream fin (dfilename);
        if (!fin.is_open())
        {
            std::string msg = "Cannot open data file to read! " + dfilename;
            throw std::runtime_error(msg);
        }

        std::string line;	

        // Read no of docs	
        fin >> line; 
        D = stol(line);

        // Read no of unique words
        fin >> line; 
        V = stol(line);

        // Read no of non-zero token (not important)
        fin >> line; 
        NNZ = stol(line);

        std::cout << D << " " << V << " " << NNZ << std::endl;
        docs.resize(D);

	NNZ = 0;
        while (!fin.eof())
        {	
            long docID, wordID, count;		
            fin >> docID >> wordID >> count;

            if (docID > D || docID <1)
                throw std::runtime_error("Invalid document ID.");
            
            docs[docID-1].words.push_back(wordID-1);
            docs[docID-1].counts.push_back(count);
            docs[docID-1].docLength +=count;
            NNZ += count;
        }

        std::cout << "Done with reading " << D << " document data having " << NNZ << " tokens in total." << std::endl;
        fin.close();
        return 0;
    }
public:
    using preprocess::preprocess;
};

void show_help()
{
    std::cout << "Command line should be: processing <format: UCI/cLDA> <document_file> <dictionary_file> <output_file> <number_of_test_docs> [shuffle]"
            << std::endl;
}

int main(int argc, char** argv) {

    if (argc < 6)
    {
        show_help();
        return 1;
    }

    std::vector<std::string> argument(argv, argv + argc);
    
    preprocess * data = NULL;
    if (argument[1] == "UCI")
        data = new uci(argument);
    else if (argument[1] == "cLDA")
        data = new clda(argument);
    else
        show_help();
    
    if(data != NULL)
    {
        data->start();
        std::cout << "DONE." << std::endl;
    }

    return 0;
}
