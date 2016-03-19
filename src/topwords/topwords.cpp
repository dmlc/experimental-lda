#include "topwords.h"

bool my_compare(const std::pair<float, unsigned> &i, const std::pair<float, unsigned> &j)
{
	return i.first > j.first;
}

topwords::topwords()
{
	V = 0;
	K = 0;

	phi_wk = NULL;

	vfile = "";
	dfile = "";

	n = 15;
	cmd = "";
}

topwords::~topwords()
{
	if (phi_wk)
	{
		for (unsigned w = 0; w < V; ++w)
		{
			if (phi_wk[w])
			{
				delete[] phi_wk[w];
			}
		}
		delete[] phi_wk;
	}
}

int topwords::init(int argc, char** argv)
{
	// parse arguments
	std::vector<std::string> arguments(argv, argv + argc);
	for (auto arg = arguments.begin(); arg != arguments.end(); ++arg)
	{
		if (*arg == "--dataset")
		{
			dfile = *(++arg);
			if (dfile == "")
			{
				std::cout << "Error: Invalid file path to phi matrix." << std::endl;
				return 1;
			}
			dfile += ".phi";

		}
		else if (*arg == "--vocab-file")
		{
			vfile = *(++arg);
			if (vfile == "")
			{
				std::cout << "Error: Invalid file path to testing corpus." << std::endl;
				return 1;
			}
		}
	}

	std::cout << "Loading data from: " << dfile << " and " << vfile << std::endl;

	//read phi matrix
	std::ifstream fin(dfile, std::ios::binary);
	if (!fin)
	{
		std::cout << "Error: Cannot open file to read: " << dfile << std::endl;
		return 1;
	}
	fin.read((char *)&V, sizeof(unsigned));
	fin.read((char *)&K, sizeof(unsigned short));
	std::cout << V << ", " << K << std::endl;

	phi_wk = new float*[V];
	for (unsigned w = 0; w < V; ++w)
	{
		phi_wk[w] = new float[K];
		fin.read((char *)(phi_wk[w]), sizeof(float)*K);
	}
	//fin.read((char *)(phi_wk), sizeof(float)*V*K);
	fin.close();

	//read vocab file
	utils::read_wordmap(vfile, &word2id);
	if (V != word2id.size())
	{
		std::cout << "Error: Vocab size mismatch " << V << " : " << word2id.size() << std::endl;
		return 1;
	}
	// construct the reverse map (currently stupidly by reading back)
	for (auto w : word2id)
	{
		id2word[w.second] = w.first;
	}

	return 0;
}

int topwords::interact()
{
	//loop over inputs
	while (cmd.compare("exit"))
	{
		std::cout << ">>";
		std::getline(std::cin, cmd);
		if (cmd == "sums")
		{
			for (unsigned short k = 0; k < K; ++k)
			{
				float sum = 0;
				for (unsigned w = 0; w < V; ++w)
					sum += phi_wk[w][k];
				std::cout << sum << ", ";
			}
			std::cout << std::endl;
		}
		else if (cmd.substr(0, 3) == "sum")
		{
			unsigned short k = -1;
			try{
				k = std::stoi(cmd.substr(4));
				float sum = 0;
				for (unsigned w = 0; w < V; ++w)
					sum += phi_wk[w][k];
				std::cout << sum << std::endl;
			}
			catch (std::exception& e){
				std::cout << e.what() << std::endl;
			}
		}
		else if (cmd == "V")
		{
			std::cout << "Vocab size: " << V << std::endl;
		}
		else if (cmd == "K")
		{
			std::cout << "Num topics: " << K << std::endl;
		}
		else if (cmd.substr(0, 1) == "n")
		{
			try{
				std::cout << cmd.substr(2) << std::endl;
				n = std::stoi(cmd.substr(2));
				std::cout << "Num words set to: " << n << std::endl;
			}
			catch (std::exception& e){
				std::cout << e.what() << std::endl;
			}
		}
		else if (cmd.substr(0, 5) == "topic")
		{
			unsigned short k = -1;
			try{
				k = std::stoi(cmd.substr(6));

				std::map<unsigned, std::string>::const_iterator it;

				std::vector<std::pair<float, unsigned> > words_probs(V);
				std::pair<float, unsigned> word_prob;
				for (unsigned w = 0; w < V; ++w)
				{
					word_prob.first = phi_wk[w][k];
					word_prob.second = w;
					words_probs[w] = word_prob;
				}

				// quick sort to sort word-topic probability
				std::sort(words_probs.begin(), words_probs.end(), my_compare);

				std::cout << "Topic " << k << "th:" << std::endl;
				for (unsigned i = 0; i < n; i++)
				{
					//it = id2word.find(words_probs[i].first);
					//if (it != id2word.end())
					//{
					std::cout << "\t" << words_probs[i].second << "\t" << id2word[words_probs[i].second] << "\t" << words_probs[i].first << std::endl;
					//}
				}
			}
			catch (std::exception& e){
				std::cout << e.what() << std::endl;
			}
		}
		else
		{
			std::cout << "Incorrect command. Enter <help> to list available commands or <exit> to quit." << std::endl;
		}
	}
}
