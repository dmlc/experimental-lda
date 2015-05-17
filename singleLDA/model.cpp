#include "model.h"
#include <sstream>

model::~model()
{
    if (p)
	{
		delete[] p;
    }
	
	if (nd_m)
	{
		delete[] nd_m;
    }
	
	if (rev_mapper)
	{
		delete[] rev_mapper;
    }

    if (ptrndata)
	{
		delete ptrndata;
    }
    
    if (pnewdata) 
	{
		delete pnewdata;
    }

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
    
    if (nw) 
	{
		for (int w = 0; w < V; ++w) 
		{
			if (nw[w]) 
			{
				delete[] nw[w];
			}
		}
		delete[] nw;
    }

    if (nwsum)
	{
		delete[] nwsum;
	}   
    
    // only for inference
    if (newz)
	{
		for (int m = 0; m < newM; m++)
		{
		    if (newz[m]) 
			{
				delete[] newz[m];
		    }
		}
		delete[] newz;
    }
    
    if (newnw) 
	{
		for (int w = 0; w < V; w++) 
		{
			if (newnw[w]) 
			{
				delete[] newnw[w];
			}
		}
		delete[] newnw;
    }

    if (newnd) 
	{
		for (int m = 0; m < newM; m++)
		{
			if (newnd[m])
			{
				delete[] newnd[m];
			}
		}
		delete[] newnd;
    } 
    
    if (newnwsum)
	{
		delete[] newnwsum;
    }   
        
}

void model::set_default_values() 
{
    wordmapfile = "wordmap.txt";
    trainlogfile = "trainlog.txt";
    tassign_suffix = ".tassign";
    theta_suffix = ".theta";
    phi_suffix = ".phi";
    others_suffix = ".others";
    twords_suffix = ".twords";
	time_suffix = ".time";
	llh_suffix = ".llh";
    
    ddir = "./";
    dfile = "trndocs.dat";
	mdir = "./";
    model_name = "model-final";    
    model_status = MODEL_UNKNOWN;
    
    ptrndata = NULL;
    pnewdata = NULL;
	time_ellapsed.reserve(50);
	likelihood.reserve(50);

    M = 0;
    V = 0;
    K = 100;
    alpha = 50.0 / K;
    beta = 0.1;
    niters = 2000;
    savestep = 200;    
    twords = 0;
    
    p = NULL;
    nd_m = NULL; 
    rev_mapper = NULL;
    
    z = NULL;
    nw = NULL;
    nwsum = NULL;
    
    newM = 0;
    newz = NULL;
	newnd = NULL;
    newnw = NULL;
    newnwsum = NULL;
    
}

int model::parse_args(int argc, char ** argv) 
{
	int _model_status = MODEL_UNKNOWN;
	std::string _ddir = "";
	std::string _mdir = "";
	std::string _dfile = "";
	std::string _tfile = "";
	double _alpha = -1.0;
	double _beta = -1.0;
	int _K = 0;
	int _niters = 0;
	int _inf_niters = 0;
	int _savestep = 0;
	int _twords = 0;
	int _withrawdata = 0;
	int _nst = 0;
	int _ntt = 0;

	std::vector<std::string> arguments(argv, argv + argc);

	int i = 0;

	for (auto arg = arguments.begin(); arg != arguments.end(); ++arg)
	{
		if (*arg == "-nt")
		{
			_model_status = MODEL_NO_TEST;
		}
		else if (*arg == "-set")
		{
			_model_status = MODEL_SELF_TEST;
		}
		else if (*arg == "-net")
		{
			_model_status = MODEL_SEPARATE_TEST;
		}
		else if (*arg == "-odir")
		{
			_mdir = *(++arg);
		}
		else if (*arg == "-dfile")
		{
			_dfile = *(++arg);
		}
		else if (*arg == "-tfile")
		{
			_tfile = *(++arg);
		}
		else if (*arg == "-alpha")
		{
			_alpha = std::stod(*(++arg));
		}
		else if (*arg == "-beta")
		{
			_beta = std::stod(*(++arg));
		}
		else if (*arg == "-ntopics")
		{
			_K = std::stoi(*(++arg));
		}
		else if (*arg == "-niters")
		{
			_niters = std::stoi(*(++arg));
		}
		else if (*arg == "-savestep")
		{
			_savestep = std::stoi(*(++arg));
		}
		else if (*arg == "-twords")
		{
			_twords = std::stoi(*(++arg));
		}
		else
		{
			// any more?
		}
	}


	//Check common parameters and accept only if valid
	if (_dfile == "")
	{
		std::cout << "Please specify the train data file for model estimation!" << std::endl;
		return 1;
	}

	if (_mdir == "")
	{
		mdir = "./";
	}
	else
	{
		mdir = _mdir;
		std::cout << "model dir = " << mdir << std::endl;
	}

	model_status = _model_status;

	if (_K > 0)
	{
		K = _K;
	}

	if (_alpha >= 0.0)
	{
		alpha = _alpha;
	}
	else
	{
		// default value for alpha
		alpha = 50.0 / K;
	}

	if (_beta >= 0.0)
	{
		beta = _beta;
	}

	if (_niters > 0)
	{
		niters = _niters;
	}

	if (_savestep > 0)
	{
		savestep = _savestep;
	}

	if (_twords > 0)
	{
		twords = _twords;
	}
	
	dfile = _dfile;

	std::string::size_type idx = _dfile.find_last_of("/");
	if (idx == std::string::npos)
	{
		ddir = "./";
	}
	else
	{
		ddir = _dfile.substr(0, idx + 1);
		dfile = _dfile.substr(idx + 1, _dfile.size() - ddir.size());
		std::cout << "data dir = " << ddir << std::endl;
		std::cout << "dfile = " << dfile << std::endl;
	}

	//Check specific parameter    
	if (model_status == MODEL_SEPARATE_TEST)
	{
		if (_tfile == "")
		{
			std::cout << "Please specify the test data file for model estimation!" << std::endl;
			return 1;
		}
		else
		{
			tfile = _tfile;
			std::cout << "tfile = " << tfile << std::endl;
		}

		if (_inf_niters > 0)
		{
			inf_niters = _inf_niters;
		}
		else
		{
			// default number of Gibbs sampling iterations for doing inference
			inf_niters = 10;
		}
	}

	if (model_status == MODEL_UNKNOWN)
	{
		std::cout << "Please specify the task you would like to perform (-nt/-set/-net)!" << std::endl;
		return 1;
	}

	return 0;
}

int model::init(int argc, char ** argv) 
{
	utils::randomize();
	// call parse_args
    if (parse_args(argc, argv)) 
	{
		return 1;
    }

	// + read training data
	ptrndata = new dataset;
	if (ptrndata->read_data(ddir + dfile, &word2id))
	{
		std::cout << "Fail to read training data!\n";
		return 1;
	}
    
	if (model_status == MODEL_NO_TEST || model_status == MODEL_SELF_TEST)
	{
		M = ptrndata->M;
		V = ptrndata->V;

		// estimating the model from scratch
		if (init_est()) 
		{
		    return 1;
		}
    }
	else if (model_status == MODEL_SEPARATE_TEST) 
	{
		// read new data for inference
		pnewdata = new dataset;
		if (pnewdata->read_data(ddir + tfile, &word2id))
		{
			std::cout << "Fail to read new data!" << std::endl;
			return 1;
		}
		M = ptrndata->M;
		newM = pnewdata->M;
		V = pnewdata->V;
		ptrndata->V = V;

		// estimating the model from scratch
		if (init_est()) 
		{
			return 1;
		}
		// do inference
		if (init_inf())
		{
			return 1;
		}
    } 

	// write word map to file
	if (utils::write_wordmap(mdir + wordmapfile, &word2id))
	{
		return 1;
	}
	
	// construct the reverse map (currently stupidly by reading back)
	if (utils::read_wordmap(mdir + wordmapfile, &id2word))
	{
		return 1;
	}

    return 0;
}

std::string model::generate_model_name(int iter)
{
	std::string model_name = dfile + "-";

	std::string buff = "";

	if (0 <= iter && iter < 10)
	{
		buff += "0000";
		buff += std::to_string(iter);
	}
	else if (10 <= iter && iter < 100)
	{
		buff += "000";
		buff += std::to_string(iter);
	}
	else if (100 <= iter && iter < 1000)
	{
		buff += "00";
		buff += std::to_string(iter);
	}
	else if (1000 <= iter && iter < 10000)
	{
		buff += "0";
		buff += std::to_string(iter);
	}
	else
	{
		buff += std::to_string(iter);
	}

	if (iter >= 0)
	{
		model_name += buff;
	}
	else
	{
		model_name += "final";
	}

	return model_name;
}

int model::save_model(std::string model_name)
{
	if (save_model_time(mdir + model_name + time_suffix))
	{
		return 1;
	}
	std::cout << "time done" << std::endl;
	if (save_model_llh(mdir + model_name + llh_suffix))
	{
		return 1;
	}
	std::cout << "llh done" << std::endl;
	if (save_model_others(mdir + model_name + others_suffix)) 
	{
		return 1;
    }
	std::cout << "others done" << std::endl;
    if (twords > 0)
	{
		//if (save_model_twords(mdir + model_name + twords_suffix)) 
		//{
		//	return 1;
		//}
		//std::cout << "twords done" << std::endl;
    }
	//if (model_status == MODEL_SELF_TEST)
	//{
	//	if (save_model_phi(mdir + model_name + phi_suffix))
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
		std::cout << "Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (int r = 0; r < time_ellapsed.size(); ++r)
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
		std::cout << "Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (int r = 0; r < likelihood.size(); ++r)
	{
		fout << likelihood[r] << std::endl;
	}

	fout.close();

	return 0;
}

int model::save_model_others(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	fout << "alpha=" << alpha << std::endl;
	fout << "beta=" << beta << std::endl;
	fout << "ntopics=" << K << std::endl;
	fout << "ndocs=" << M << std::endl;
	fout << "nwords=" << V << std::endl;
	fout << "niters=" << niters << std::endl;
	fout.close();
    
    return 0;
}

int model::save_model_twords(std::string filename)
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Cannot open file to save: " << filename << std::endl;
		return 1;
	}
   
    if (twords > V)
	{
		twords = V;
    }
    mapid2word::iterator it;
    
    for (int k = 0; k < K; k++)
	{
		std::vector<std::pair<int, int> > words_probs(V);
		std::pair<int, int> word_prob;
		for (int w = 0; w < V; w++)
		{
			word_prob.first = w;
			word_prob.second = nw[w][k];
			words_probs[w] = word_prob;
		}
    
        // quick sort to sort word-topic probability
		utils::quicksort(words_probs, 0, words_probs.size() - 1);
	
		fout << "Topic " << k << "th:" << std::endl;
		for (int i = 0; i < twords; i++)
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
		std::cout << "Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	for (int k = 0; k < K; k++)
	{
		for (int w = 0; w < V; w++)
		{
			fout << (nw[w][k] + beta) / (nwsum[k] + Vbeta) << " ";
		}
		fout << std::endl;
	}

	fout.close();

	return 0;
}

int model::init_est()
{
	// + allocate memory and assign values for variables
		
	// --- model parameters ---  	
	Vbeta = V * beta;

	
	// --- model variables ---
    nw = new int*[V];
    for (int w = 0; w < V; w++)
	{
        nw[w] = new int[K];
        for (int k = 0; k < K; k++)
		{
    	    nw[w][k] = 0;
        }
    }
	
	nds1.resize(M);
		
    nwsum = new int[K];
    for (int k = 0; k < K; k++)
	{
		nwsum[k] = 0;
    }
    
	z = new int*[M];
    for (int m = 0; m < ptrndata->M; m++)
	{
		int N = ptrndata->docs[m]->length;
		std::map<int, int > map_nd_m;
		z[m] = new int[N];
	
        // initialize for z
        for (int n = 0; n < N; n++)
		{
			int topic = utils::pick_a_number(0, K-1);
    	    z[m][n] = topic;
			int w = ptrndata->docs[m]->words[n];
    	    
    	    // number of instances of word i assigned to topic j
    	    nw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
			map_nd_m[topic] += 1;
    	    // total number of words assigned to topic j
    	    nwsum[topic] += 1;
        } 
		// transfer to sparse representation
		for (auto myc : map_nd_m)
			nds1[m].push_back(myc);
    }
    
	time_ellapsed.reserve(niters);
	likelihood.reserve(niters);
	
	// temporary
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

int model::init_inf()
{
    // initialize variables for inference
    
    newnw = new int*[V];
    for (int w = 0; w < V; w++)
	{
        newnw[w] = new int[K];
        for (int k = 0; k < K; k++)
		{
    	    newnw[w][k] = 0;
        }
    }
	
    newnd = new int*[newM];
    for (int m = 0; m < newM; m++)
	{
        newnd[m] = new int[K];
        for (int k = 0; k < K; k++)
		{
    	    newnd[m][k] = 0;
        }
    }
	
    newnwsum = new int[K];
    for (int k = 0; k < K; k++)
	{
		newnwsum[k] = 0;
    }
    
    newz = new int*[newM];
    for (int m = 0; m < pnewdata->M; m++)
	{
		int N = pnewdata->docs[m]->length;
		newz[m] = new int[N];

		// assign values for nw, nd, nwsum	
        for (int n = 0; n < N; n++)
		{
    	    int w = pnewdata->docs[m]->words[n];
			int topic = utils::pick_a_number(0, K-1);
    	    newz[m][n] = topic;
    	    
    	    // number of instances of word i assigned to topic j
    	    newnw[w][topic] += 1;
    	    // number of words in document i assigned to topic j
    	    newnd[m][topic] += 1;
    	    // total number of words assigned to topic j
    	    newnwsum[topic] += 1;
        } 
    }  
   
    return 0;        
}

int model::estimate()
{
	if(specific_init())
		return 1;
	
	std::chrono::high_resolution_clock::time_point ts, tn;
	std::cout << "Sampling " << niters << " iterations!" << std::endl;

	for (int iter = 1; iter <= niters; ++iter)
	{
		std::cout << "Iteration " << iter << " ..." << std::endl;
		ts = std::chrono::high_resolution_clock::now();

		// for each document
		for (int m = 0; m < M; ++m)
		{
			if( sampling(m) )
				return 1;
		}

		tn = std::chrono::high_resolution_clock::now();
		time_ellapsed.push_back( std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count() );
		
#if COMP_LLH
		inference();
#endif

		if (savestep > 0)
		{
			if (iter % savestep == 0)
			{
				// saving the model
				std::cout << "Saving the model at iteration " << iter << "..." << std::endl;
				save_model(generate_model_name(iter));
			}
		}
	}

	std::cout << "Gibbs sampling completed!" << std::endl;
	std::cout << "Saving the final model!" << std::endl;
	save_model(generate_model_name(-1));
	
	return 0;
}

int model::inference()
{
	if(model_status==MODEL_SELF_TEST)
	{
		// just do MAP estimates
		likelihood.push_back(llhw());
		std::cout << "Likelihood on training documents: " << likelihood.back() <<" at time " << time_ellapsed.back() << std::endl;
	}
	else if(model_status == MODEL_SEPARATE_TEST)
	{
		for (int iter = 1; iter <= inf_niters; ++iter)
		{
			// for each doc
			for (int m = 0; m < newM; m++)
			{
				if(inf_sampling(m))
					return 1;
			}
		}
		likelihood.push_back(newllhw());
		std::cout << "Likelihood on held out documents: " << likelihood.back() <<" at time " << time_ellapsed.back() << std::endl;
	}
	return 0;
}

int model::inf_sampling(int m)
{
	for (int n = 0; n < pnewdata->docs[m]->length; n++)
	{
		int w = pnewdata->docs[m]->words[n];
		
		// remove z_i from the count variables
		int topic = newz[m][n];
		newnw[w][topic] -= 1;
		newnd[m][topic] -= 1;
		newnwsum[topic] -= 1;
		
		double psum = 0;
		// do multinomial sampling via cumulative method
		for (int k = 0; k < K; k++)
		{
			psum += (nw[w][k] + newnw[w][k] + beta) * (newnd[m][k] + alpha) / (nwsum[k] + newnwsum[k] + Vbeta);
			p[k] = psum;
		}
		
		// scaled sample because of unnormalized p[]
		double u = utils::unif01() * psum;
		topic = utils::binary_search(p, u, 0, K - 1);
		
		// add newly estimated z_i to count variables
		newnw[w][topic] += 1;
		newnd[m][topic] += 1;
		newnwsum[topic] += 1;
		newz[m][n] = topic;
	}
	
	return 0;
}

double model::newllhw() const
{
	double sum = 0;
	int num_tokens = 0;
	for (int m = 0; m < newM; ++m)
	{
		double dsum = 0;
		num_tokens += pnewdata->docs[m]->length;
		for (int n = 0; n < pnewdata->docs[m]->length; n++)
		{
			double wsum = 0;
			int w = pnewdata->docs[m]->words[n];
			for (int k = 0; k<K; k++)
			{
				wsum += (newnd[m][k] + alpha) * (nw[w][k] + newnw[w][k] + beta) / (nwsum[k] + newnwsum[k] + Vbeta);
			}
			dsum += log(wsum);
		}
		sum += dsum - pnewdata->docs[m]->length*log(pnewdata->docs[m]->length + K * alpha);
	}
	return sum/num_tokens;
}

double model::llhw() const
{
	double sum = 0;
	int num_tokens = 0;
	for (int m = 0; m < M; m++)
	{
		for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
		{
			nd_m[k->first] = k->second;
		}

		double dsum = 0;
		num_tokens += ptrndata->docs[m]->length;
		for (int n = 0; n < ptrndata->docs[m]->length; n++)
		{
			double wsum = 0;
			int w = ptrndata->docs[m]->words[n];
			for (int k = 0; k<K; k++)
			{
				wsum += (nd_m[k] + alpha) * (nw[w][k] + beta) / (nwsum[k] + Vbeta);
			}
			wsum /= (ptrndata->docs[m]->length + K * alpha);
			dsum += log(wsum);
		}
		sum += dsum;
		for (auto k = nds1[m].begin(); k != nds1[m].end(); ++k)
		{
			nd_m[k->first] = 0;
		}
	}
	return sum / num_tokens;
}

/*int model::add_to_topic( int word, int doc, int topic, int old_topic )
{
	nw[word][topic] += 1;
	nds1[doc][topic] += 1;
	if (nds1[doc][old_topic] == 0)
		nds1[doc].erase(old_topic);
	nwsum[topic] += 1;
}
int model::remove_from_topic( int word, int doc, int topic)
{
	nw[word][topic] -= 1;
	nds1[doc][topic] -= 1;
	nwsum[topic] -= 1;
}*/

int model::sanity() const
{
	long tott = 0;
        for(int m = 0; m < M; ++m)
	{
		int sumd = 0;
                for(const auto &t:nds1[m])
			sumd += t.second;
		if (sumd == ptrndata->docs[m]->length)
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
			nw[wordID][topic] = count;
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
			nw[v][k] = 0;
	}
	for (int k = 0; k < K; ++k)
		nwsum[k] = 0;

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
		nw[wordID][topic]++;
		nwsum[topic]++;
	}

	int countv = 0;
	for (int k = 0; k < K; ++k)
		countv += nwsum[k];


	std::cout << "Done with reading document data with words:" << countv << std::endl;
	fin.close();

	return 0;
}
