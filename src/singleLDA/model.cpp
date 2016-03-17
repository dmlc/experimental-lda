#include "model.h"
#include "lda.h"
#include <sstream>

#define COMP_LLH 1

model::model() 
{
	testing_type = INVALID;
	
	trngdata = NULL;
	testdata = NULL;

	M = 0;
	V = 0;
	K = 100;

	alpha = 50.0 / K;
	beta = 0.1;

	z = NULL;
	n_wk = NULL;
	n_k = NULL;

	p = NULL;
	nd_m = NULL;
	rev_mapper = NULL;

	n_iters = 1000;
	n_save = 200;
	n_topWords = 0;

	test_n_iters = 10;
	test_M = 0;
	test_z = NULL;
	test_n_wk = NULL;
	test_n_mk = NULL;
	test_n_k = NULL;

	time_ellapsed.reserve(50);
	likelihood.reserve(50);

    ddir = "./";
	mdir = "./";
	dfile = "";
	tfile = "";
}

model::~model()
{
	if (trngdata) delete trngdata;
	if (testdata) delete trngdata;
	
	
	if (z)
	{
		for (int m = 0; m < M; m++)
		{
			if (z[m])
			{
				delete[] z[m];
			}
		}
		delete[] z;
	}

	if (n_wk)
	{
		for (int w = 0; w < V; ++w)
		{
			if (n_wk[w])
			{
				delete[] n_wk[w];
			}
		}
		delete[] n_wk;
	}

	if (n_k)	delete[] n_k;


	if (p)		delete[] p;
	if (nd_m)	delete[] nd_m;
	if (rev_mapper)	delete[] rev_mapper;


	if (test_z)
	{
		for (int m = 0; m < test_M; m++)
		{
			if (test_z[m])
			{
				delete[] test_z[m];
			}
		}
		delete[] test_z;
	}

	if (test_n_wk)
	{
		for (int w = 0; w < V; w++)
		{
			if (test_n_wk[w])
			{
				delete[] test_n_wk[w];
			}
		}
		delete[] test_n_wk;
	}

	if (test_n_mk)
	{
		for (int m = 0; m < test_M; m++)
		{
			if (test_n_mk[m])
			{
				delete[] test_n_mk[m];
			}
		}
		delete[] test_n_mk;
	}

	if (test_n_k)	delete[] test_n_k;

}

model* model::init(int argc, char ** argv)
{
	model *lda = NULL;

	std::vector<std::string> arguments(argv, argv + argc);
	for (auto arg = arguments.begin(); arg != arguments.end() - 1; ++arg)
	{
		if (*arg == "--method")
		{
			if (*(arg + 1) == "simpleLDA")
			{
				lda = new simpleLDA;
				std::cout << "Running LDA inference using simpleLDA" << std::endl;
			}
			else if (*(arg + 1) == "unifLDA")
			{
				lda = new unifLDA;
				std::cout << "Running LDA inference using unifLDA" << std::endl;
			}
			else if (*(arg + 1) == "sparseLDA")
			{
				lda = new sparseLDA;
				std::cout << "Running LDA inference using sparseLDA" << std::endl;
			}
			else if (*(arg + 1) == "aliasLDA")
			{
				lda = new aliasLDA;
				std::cout << "Running LDA inference using aliasLDA" << std::endl;
			}
			else if (*(arg + 1) == "FTreeLDA")
			{
				lda = new FTreeLDA;
				std::cout << "Running LDA inference using FTreeLDA" << std::endl;
			}
			else if (*(arg + 1) == "forestLDA")
			{
				lda = new forestLDA;
				std::cout << "Running LDA inference using forestLDA" << std::endl;
			}
			else if (*(arg + 1) == "lightLDA")
			{
				lda = new lightLDA;
				std::cout << "Running LDA inference using lightLDA" << std::endl;
			}
			else
			{
				lda = NULL;
				std::cout << "Error: Invalid method algorithm!" << std::endl;
				return NULL;
			}
		}
	}

	if (lda != NULL)
	{
		// call parse_args
		if (lda->parse_args(arguments))
		{
			return NULL;
		}

		// read data
		if (lda->read_data())
		{
			return NULL;
		}
	}
	else
	{
		lda = NULL;
		std::cout << "Error: Method algorithm not specified!" << std::endl;
		return NULL;
	}

	return lda;
}

int model::train()
{
	if (specific_init())
		return 1;

	std::chrono::high_resolution_clock::time_point ts, tn;
	std::cout << "Sampling " << n_iters << " iterations!" << std::endl;

	for (int iter = 1; iter <= n_iters; ++iter)
	{
		std::cout << "Iteration " << iter << " ..." << std::endl;
		ts = std::chrono::high_resolution_clock::now();

		// for each document
		for (int m = 0; m < M; ++m)
			sampling(m);

		tn = std::chrono::high_resolution_clock::now();
		time_ellapsed.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count());

#if COMP_LLH
		test();
#endif

		if (n_save > 0)
		{
			if (iter % n_save == 0)
			{
				// saving the model
				std::cout << "Saving the model at iteration " << iter << "..." << std::endl;
				save_model(iter);
			}
		}
	}

	std::cout << "Gibbs sampling completed!" << std::endl;
	std::cout << "Saving the final model!" << std::endl;
	save_model(-1);

	return 0;
}

int model::test()
{
	if (testing_type == SELF_TEST)
	{
		// just do MAP estimates
		likelihood.push_back(llhw());
		std::cout << "Likelihood on training documents: " << likelihood.back() << " at time " << time_ellapsed.back() << std::endl;
	}
	else if (testing_type == SEPARATE_TEST)
	{
		for (int iter = 1; iter <= test_n_iters; ++iter)
		{
			// for each doc
			for (int m = 0; m < test_M; m++)
				vanilla_sampling(m);
		}
		likelihood.push_back(newllhw());
		std::cout << "Likelihood on held out documents: " << likelihood.back() << " at time " << time_ellapsed.back() << std::endl;
	}
	return 0;
}

int model::read_data()
{
	utils::randomize();

	// read training data
	trngdata = new dataset;
	if (trngdata->read_data(ddir + dfile, &word2id))
	{
		std::cout << "Fail to read training data!\n";
		return 1;
	}

	// according to testing type initialise
	if (testing_type == NO_TEST || testing_type == SELF_TEST)
	{
		M = trngdata->M;
		V = trngdata->V;

		// randomly initialise model variables for training
		std::cout << "Now randomly initialising model variables for training" << std::endl;
		if (init_train())
		{
			std::cout << "Error: Failed to allocate model variables!" << std::endl;
			return 1;
		}
	}
	else if (testing_type == SEPARATE_TEST)
	{
		// read held-out testing data
		testdata = new dataset;
		if (testdata->read_data(ddir + tfile, &word2id))
		{
			std::cout << "Error: Failed to read training corpus!" << std::endl;
			return 1;
		}
		M = trngdata->M;
		test_M = testdata->M;
		V = testdata->V;
		trngdata->V = V;

		// randomly initialise model variables for training
		std::cout << "Now randomly initialising model variables for training" << std::endl;
		if (init_train())
		{
			std::cout << "Error: Failed to allocate model variables!" << std::endl;
			return 1;
		}
		// initialise aux variables for testing
		if (init_test())
		{
			std::cout << "Error: Failed to initialise testing variables!" << std::endl;
			return 1;
		}
	}

	// write word map to file
	if (dataset::write_wordmap(mdir + "wordmap.txt", &word2id))
	{
		return 1;
	}

	// construct the reverse map (currently stupidly by reading back)
	for (auto w : word2id)
	{
		id2word[w.second] = w.first;
	}

	return 0;
}

int model::parse_args(std::vector<std::string> arguments)
{
	double _alpha = -1.0;

	for (auto arg = arguments.begin(); arg != arguments.end(); ++arg)
	{
		if (*arg == "--testing-mode")
		{
			++arg;
			if (*arg == "nt")
			{
				testing_type = NO_TEST;
			}
			else if (*arg == "set")
			{
				testing_type = SELF_TEST;
			}
			else if (*arg == "net")
			{
				testing_type = SEPARATE_TEST;
			}
		}
		else if (*arg == "--output-model")
		{
			mdir = *(++arg);
			if (mdir == "")		mdir = "./";
		}
		else if (*arg == "--training-file")
		{
			dfile = *(++arg);
			if (dfile == "")
			{
				std::cout << "Error: Invalid file path to training corpus." << std::endl;
				return 1;
			}
			std::string::size_type idx = dfile.find_last_of("/");
			if (idx == std::string::npos)
			{
				ddir = "./";
			}
			else
			{
				ddir = dfile.substr(0, idx + 1);
				dfile = dfile.substr(idx + 1, dfile.size() - ddir.size());
				
			}
		}
		else if (*arg == "--testing-file")
		{
			tfile = *(++arg);
		}
		else if (*arg == "--alpha")
		{
			_alpha = std::stod(*(++arg));
		}
		else if (*arg == "--beta")
		{
			double _beta = std::stod(*(++arg));
			if (_beta >= 0.0)	beta = _beta;
		}
		else if (*arg == "--num-topics")
		{
			int _K = std::stoi(*(++arg));
			if (_K > 0)	K = _K;
		}
		else if (*arg == "--num-iterations")
		{
			int _n_iters = std::stoi(*(++arg));
			if (_n_iters > 0)	n_iters = _n_iters;
		}
		else if (*arg == "--output-state-interval")
		{
			int _n_save = std::stoi(*(++arg));
			if (_n_save > 0)	n_save = _n_save;
		}
		else if (*arg == "--num-top-words")
		{
			int _n_topWords = std::stoi(*(++arg));
			if (_n_topWords > 0) n_topWords = _n_topWords;
		}
	}


	//Check common parameters and accept only if valid
	if (_alpha >= 0.0)
	{
		alpha = _alpha;
	}
	else
	{
		// default value for alpha
		alpha = 50.0 / K;
	}

	//Check specific parameter    
	if (testing_type == SEPARATE_TEST)
	{
		if (tfile == "")
		{
			std::cout << "Error: Invalid file path to test corpus." << std::endl;
			return 1;
		}
	}

	if (testing_type == INVALID)
	{
		std::cout << "Error: Please choose the task you would like to perform (-nt/-set/-net)!" << std::endl;
		return 1;
	}

	//display all configurations
	std::cout << "We will perform ";
	switch (testing_type)
	{
	case NO_TEST:
		std::cout << "no testing." << std::endl;
		break;
	case SELF_TEST:
		std::cout << "a test on training data." << std::endl;
		break;
	case SEPARATE_TEST:
		std::cout << "a test on a heldout set." << std::endl;
		break;
	default:
		break;
	}
	std::cout << "data dir = " << ddir << std::endl;
	std::cout << "Training file = " << dfile << std::endl;
	if (testing_type == SEPARATE_TEST)
		std::cout << "Testing file = " << tfile << std::endl;
	std::cout << "model dir = " << mdir << std::endl;
	std::cout << "n_iters = " << n_iters << std::endl;
	std::cout << "alpha = " << alpha << std::endl;
	std::cout << "beta = " << beta << std::endl;
	std::cout << "K = " << K << std::endl;

	return 0;
}

int model::init_train()
{
	Vbeta = V * beta;

	// allocate heap memory for model variables
	n_wk = new int*[V];
	for (int w = 0; w < V; w++)
	{
		n_wk[w] = new int[K];
		for (int k = 0; k < K; k++)
		{
			n_wk[w][k] = 0;
		}
	}

	n_mks.resize(M);

	n_k = new int[K];
	for (int k = 0; k < K; k++)
	{
		n_k[k] = 0;
	}

	// random consistent assignment for model variables
	z = new int*[M];
	for (int m = 0; m < trngdata->M; m++)
	{
		int N = trngdata->docs[m]->length;
		std::map<int, int > map_nd_m;
		z[m] = new int[N];

		// initialize for z
		for (int n = 0; n < N; n++)
		{
			int topic = utils::pick_a_number(0, K - 1);
			z[m][n] = topic;
			int w = trngdata->docs[m]->words[n];

			// number of instances of word i assigned to topic j
			n_wk[w][topic] += 1;
			// number of words in document i assigned to topic j
			map_nd_m[topic] += 1;
			// total number of words assigned to topic j
			n_k[topic] += 1;
		}
		// transfer to sparse representation
		for (auto myc : map_nd_m)
			n_mks[m].push_back(myc);
	}

	time_ellapsed.reserve(n_iters);
	likelihood.reserve(n_iters);

	// allocate heap memory for temporary variables
	p = new double[K];
	nd_m = new int[K];
	rev_mapper = new int[K];
	for (int k = 0; k < K; ++k)
	{
		nd_m[k] = 0;
		rev_mapper[k] = -1;
	}

	return 0;
}

int model::init_test()
{
	// initialise variables for testing
	test_n_wk = new int*[V];
	for (int w = 0; w < V; w++)
	{
		test_n_wk[w] = new int[K];
		for (int k = 0; k < K; k++)
		{
			test_n_wk[w][k] = 0;
		}
	}

	test_n_mk = new int*[test_M];
	for (int m = 0; m < test_M; m++)
	{
		test_n_mk[m] = new int[K];
		for (int k = 0; k < K; k++)
		{
			test_n_mk[m][k] = 0;
		}
	}

	test_n_k = new int[K];
	for (int k = 0; k < K; k++)
	{
		test_n_k[k] = 0;
	}

	test_z = new int*[test_M];
	for (int m = 0; m < testdata->M; m++)
	{
		int N = testdata->docs[m]->length;
		test_z[m] = new int[N];

		// assign values for n_wk, n_mk, n_k
		for (int n = 0; n < N; n++)
		{
			int w = testdata->docs[m]->words[n];
			int topic = utils::pick_a_number(0, K - 1);
			test_z[m][n] = topic;

			// number of instances of word i assigned to topic j
			test_n_wk[w][topic] += 1;
			// number of words in document i assigned to topic j
			test_n_mk[m][topic] += 1;
			// total number of words assigned to topic j
			test_n_k[topic] += 1;
		}
	}

	return 0;
}

int model::vanilla_sampling(int m)
{
	for (int n = 0; n < testdata->docs[m]->length; n++)
	{
		int w = testdata->docs[m]->words[n];

		// remove z_i from the count variables
		int topic = test_z[m][n];
		test_n_wk[w][topic] -= 1;
		test_n_mk[m][topic] -= 1;
		test_n_k[topic] -= 1;

		double psum = 0;
		// do multinomial sampling via cumulative method
		for (int k = 0; k < K; k++)
		{
			psum += (test_n_mk[m][k] + alpha) * (n_wk[w][k] + test_n_wk[w][k] + beta) / (n_k[k] + test_n_k[k] + Vbeta);
			p[k] = psum;
		}

		// scaled sample because of unnormalized p[]
		double u = utils::unif01() * psum;
		topic = std::lower_bound(p, p+K, u) - p;

		// add newly estimated z_i to count variables
		test_n_wk[w][topic] += 1;
		test_n_mk[m][topic] += 1;
		test_n_k[topic] += 1;
		test_z[m][n] = topic;
	}

	return 0;
}

double model::newllhw() const
{
	double sum = 0;
	int num_tokens = 0;
	for (int m = 0; m < test_M; ++m)
	{
		double dsum = 0;
		num_tokens += testdata->docs[m]->length;
		for (int n = 0; n < testdata->docs[m]->length; n++)
		{
			double wsum = 0;
			int w = testdata->docs[m]->words[n];
			for (int k = 0; k<K; k++)
			{
				wsum += (test_n_mk[m][k] + alpha) * (n_wk[w][k] + test_n_wk[w][k] + beta) / (n_k[k] + test_n_k[k] + Vbeta);
			}
			dsum += log(wsum);
		}
		sum += dsum - testdata->docs[m]->length*log(testdata->docs[m]->length + K * alpha);
	}
	return sum / num_tokens;
}

double model::llhw() const
{
	double sum = 0;
	int num_tokens = 0;
	for (int m = 0; m < M; m++)
	{
		for (auto k = n_mks[m].begin(); k != n_mks[m].end(); ++k)
		{
			nd_m[k->first] = k->second;
		}

		double dsum = 0;
		num_tokens += trngdata->docs[m]->length;
		for (int n = 0; n < trngdata->docs[m]->length; n++)
		{
			double wsum = 0;
			int w = trngdata->docs[m]->words[n];
			for (int k = 0; k<K; k++)
			{
				wsum += (nd_m[k] + alpha) * (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
			}
			wsum /= (trngdata->docs[m]->length + K * alpha);
			dsum += log(wsum);
		}
		sum += dsum;
		for (auto k = n_mks[m].begin(); k != n_mks[m].end(); ++k)
		{
			nd_m[k->first] = 0;
		}
	}
	return sum / num_tokens;
}

int model::save_model(int iter) const
{
	std::string model_name = dfile + "-";
	if (iter >= 0)
	{
		std::ostringstream sstr1;
		sstr1 << std::setw(5) << std::setfill('0') << iter;
		model_name += sstr1.str();
	}
	else
	{
		model_name += "final";
	}
	
	if (save_model_time(mdir + model_name + ".time"))
	{
		return 1;
	}
	std::cout << "time done" << std::endl;
	if (save_model_llh(mdir + model_name + ".llh"))
	{
		return 1;
	}
	std::cout << "llh done" << std::endl;
	if (save_model_params(mdir + model_name + ".params")) 
	{
		return 1;
    }
	std::cout << "others done" << std::endl;
    if (n_topWords > 0)
	{
		//if (save_model_twords(mdir + model_name + ".twords")) 
		//ff{
		//	return 1;
		//}
		//std::cout << "twords done" << std::endl;
    }
	//if (model_status == MODEL_SELF_TEST)
	//{
	//	if (save_model_phi(mdir + model_name + ".phi"))
	//	{
	//		return 1;
	//	}
	//	std::cout << "phi done" << std::endl;
	//}
    return 0;
}

int model::save_model_time(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (unsigned r = 0; r < time_ellapsed.size(); ++r)
	{
		fout << time_ellapsed[r] << std::endl;
	}

	fout.close();

	return 0;
}

int model::save_model_llh(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (unsigned r = 0; r < likelihood.size(); ++r)
	{
		fout << likelihood[r] << std::endl;
	}

	fout.close();

	return 0;
}

int model::save_model_params(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	fout << "alpha=" << alpha << std::endl;
	fout << "beta=" << beta << std::endl;
	fout << "num-topics=" << K << std::endl;
	fout << "num-docs=" << M << std::endl;
	fout << "num-words=" << V << std::endl;
	fout << "num-iters=" << n_iters << std::endl;
	fout.close();
    
    return 0;
}

int model::save_model_topWords(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}
   
	int _n_topWords = n_topWords;
    if (_n_topWords > V)	_n_topWords = V;
 
	std::map<int, std::string>::const_iterator it;
    
    for (int k = 0; k < K; k++)
	{
		std::vector<std::pair<int, int> > words_probs(V);
		std::pair<int, int> word_prob;
		for (int w = 0; w < V; w++)
		{
			word_prob.first = w;
			word_prob.second = n_wk[w][k];
			words_probs[w] = word_prob;
		}
    
        // quick sort to sort word-topic probability
		std::sort(words_probs.begin(), words_probs.end());
	
		fout << "Topic " << k << "th:" << std::endl;
		for (int i = 0; i < _n_topWords; i++)
		{
			it = id2word.find(words_probs[i].first);
			if (it != id2word.end()) 
			{
				fout << "\t" << it->second << "   " << words_probs[i].second << std::endl;
			}
		}
    }
    
    fout.close();    
    
    return 0;    
}

int model::save_model_phi(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (int k = 0; k < K; k++)
	{
		for (int w = 0; w < V; w++)
		{
			fout << (n_wk[w][k] + beta) / (n_k[k] + Vbeta) << " ";
		}
		fout << std::endl;
	}

	fout.close();

	return 0;
}

int model::sanity() const
{
	long tott = 0;
        for(int m = 0; m < M; ++m)
	{
		int sumd = 0;
                for(const auto &t:n_mks[m])
			sumd += t.second;
		if (sumd == trngdata->docs[m]->length)
			tott += sumd;
		else
			std::cout<<"Length mismatch at doc: "<<m<<std::endl;
        }
	std::cout<<"Total number of training tokens: "<<tott <<std::endl;
	return 0;
}

int model::dump()
{
	std::ifstream fin("phi.txt");

	if (!fin.is_open()) {
		std::cout << "Cannot open file to read! " << std::endl;
		return 0;
	}

	std::string line;
	int countv = 0;

	while (!fin.eof())
	{
		std::string word_text;
		long wordID, topic, count;
		getline(fin, line);
		std::stringstream ss(line);
		ss >> wordID;
		ss >> word_text;
		//ss >> wordID;
		while (ss >> topic)
		{
			ss >> count;
			//std::cout << wordID << count << std::endl;
			wordID = word2id[word_text];
			n_wk[wordID][topic] = count;
		}
		countv++;
	}

	std::cout << "Done with reading document data with words:" << countv << std::endl;
	fin.close();

	return 0;
}

int model::dump2()
{
	std::ifstream fin("new-state.txt");

	if (!fin.is_open()) {
		std::cout << "Cannot open file to read! " << std::endl;
		return 0;
	}

	for (int v = 0; v < V; ++v)
	{
		for (int k = 0; k < K; ++k)
			n_wk[v][k] = 0;
	}
	for (int k = 0; k < K; ++k)
		n_k[k] = 0;

	std::string line;
	while (!fin.eof())
	{
		std::string word_text;
		long docID, pos, wordID, topic;
		getline(fin, line);
		std::stringstream ss(line);
		ss >> docID;
		ss >> word_text;
		ss >> pos;
		ss >> topic;
		ss >> word_text;
		ss >> topic;
		wordID = word2id[word_text];
		//std::cout << docID << " " << pos << " " << word_text << " " << topic << std::endl;
		z[docID][pos] = topic;
		n_wk[wordID][topic]++;
		n_k[topic]++;
	}

	int countv = 0;
	for (int k = 0; k < K; ++k)
		countv += n_k[k];


	std::cout << "Done with reading document data with words:" << countv << std::endl;
	fin.close();

	return 0;
}
