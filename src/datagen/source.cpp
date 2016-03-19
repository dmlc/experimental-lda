#include "../commons/dataset.h"
#include "../commons/utils.h"
#include <set>
#include <string>

int main(int argc, char ** argv)
{
	if(argc < 3)
	{
		std::cout << "Command line should be: ./datagen <dataset> <num_packs>" << std::endl;
		return 1;
	}
	std::string data_name = argv[1];
	unsigned num_packs = std::stoi(argv[2]);


	//setup streams
	std::ifstream fin;
	std::ofstream fout;
	std::string temp;
	dataset *dp;

	//word to id map
	std::map<std::string, unsigned> word2id;
	std::map<unsigned, std::string> id2word;

	//read stop-words
	std::set<std::string> stopwords;
	fin.open("data/stopwords.txt");
	while (std::getline(fin, temp))
		stopwords.insert(temp);
	fin.close();
	std::cout << "Number of stopwords detected: " << stopwords.size() << std::endl;
	
	//read each text file and write binary datapack
	for (unsigned i = 0; i < num_packs; ++i)
	{
		std::cout<< data_name <<"-"<<i<<std::endl;
		temp = data_name + "-" + std::to_string(i) + ".txt";
		dp = new dataset;
		dp->read_data(temp, &word2id, &stopwords);
		temp = data_name + "-" + std::to_string(i) + ".dat";
		fout.open(temp, std::ios::binary);
		dp->write(fout);
		fout.close();
		delete dp;
	}

	// test_data set
	{
		std::cout<< data_name << "-test"<<std::endl;	
		temp = data_name + "-test.txt";
		dp = new dataset;
		dp->read_data(temp, &word2id, &stopwords);
		temp = data_name + "-test.dat";
		fout.open(temp, std::ios::binary);
		dp->write(fout);
		fout.close();
		delete dp;
	}

	//reverse map
	utils::write_wordmap(data_name + ".vocab", &word2id);
	for (auto w : word2id)
	{
		id2word[w.second] = w.first;
	}
	fout.close();
	
	dataset test2;
	fin.open(data_name + "-0.dat", std::ios::binary);
	test2.read(fin);
	fin.close();
	std::cout << "Read the test dataset" << std::endl;

	std::cout << "Displaying document: " << 456 << std::endl;
	for (unsigned j = 0; j < test2.docs[456]->length; ++j)
	{
		std::cout << id2word[test2.docs[456]->words[j]] << " ";
	}
	std::cout << std::endl;

	system("pause");
	return 0;
}
