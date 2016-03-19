#include "model.h"
#include "lda.h"
#include <sstream>

model::model() 
{
    testing_type = INVALID;

    trngdata = NULL;
    testdata = NULL;

    M = 0;
    V = 0;
    K = 100;

    alpha = 50.0;
    alphaK = 0;
    beta = 0.1;
    Vbeta = 0;

    z = NULL;
    n_wk = NULL;
    n_k = NULL;

    p = NULL;
    nd_m = NULL;
    rev_mapper = NULL;

    n_iters = 100;
    n_save = 200;
    n_topWords = 0;

    test_n_iters = 10;
    test_M = 0;
    test_z = NULL;
    test_n_mk = NULL;

    time_ellapsed.reserve(50);
    likelihood.reserve(50);

    name = "";
    ddir = "./";
    mdir = "./";
    dfile = "";
    tfile = "";
    vfile = "";	
}

model::~model()
{
    if (trngdata) delete trngdata;
    if (testdata) delete testdata;
	
	
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
    if (nd_m)           delete[] nd_m;
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
                throw std::runtime_error("Error: Invalid inference algorithm!");
            }
        }
    }

    if (lda != NULL)
    {
        // call parse_args
        lda->parse_args(arguments);
            
        // read data
        lda->read_data();
    }
    else
    {
        throw std::runtime_error("Error: Inference algorithm not specified!");
    }

    return lda;
}

int model::train()
{
    std::cout << "Testing at start" << std::endl;
    std::fill(rev_mapper,rev_mapper+K, K);

    std::chrono::high_resolution_clock::time_point ts, tn; ts=tn;
    std::cout << "Sampling " << n_iters << " iterations!" << std::endl;
    for (unsigned iter = 0; iter < n_iters; ++iter)
    {
        if (iter % n_save == 0)
        {
            time_ellapsed.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(tn - ts).count());
            test();
            // saving the model
            std::cout << "Saving the model at iteration " << iter << "..." << std::endl;
            save_model(iter);
        }
        
        std::cout << "Iteration " << iter << " ..." << std::endl;
        ts = std::chrono::high_resolution_clock::now();

        // for each document
        for (unsigned m = 0; m < M; ++m)
                sampling(m);

        tn = std::chrono::high_resolution_clock::now();
        //std::cout << "\rLDA: M = " << M << ", K = " << K << ", V = " << V << ", alpha = "
	//	  << alpha << ", beta = " << beta << std::endl; //
	}
        test();
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
        for (unsigned iter = 0; iter < test_n_iters; ++iter)
        {
            // for each doc
            for (int m = 0; m < test_M; m++)
            {
                for (int n = 0; n < testdata->docs[m]->length; n++)
                {
                    unsigned w = testdata->docs[m]->words[n];

                    // remove z_i from the count variables
                    int topic = test_z[m][n];
                    test_n_mk[m][topic] -= 1;

                    double psum = 0;
                    // do multinomial sampling via cumulative method
                    for (int k = 0; k < K; k++)
                    {
                        psum += (test_n_mk[m][k] + alphaK) * (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
                        p[k] = psum;
                    }

                    // scaled sample because of unnormalized p[]
                    double u = rng_.rand_double() * psum;
                    topic = std::lower_bound(p, p+K, u) - p;

                    // add newly estimated z_i to count variables
                    test_n_mk[m][topic] += 1;
                    test_z[m][n] = topic;
                }
            }
        }
        likelihood.push_back(newllhw());
        std::cout << "Likelihood on held out documents: " << likelihood.back() << " at time " << time_ellapsed.back() << std::endl;
    }
    return 0;
}

int model::read_data()
{
    std::ifstream fin;
	
    // read training data
    trngdata = new dataset;
    fin.open(ddir + dfile, std::ios::binary);
    trngdata->read(fin);
    fin.close();
    M = trngdata->M;
    utils::read_wordmap(ddir + vfile, &word2id);
    V = word2id.size();

    // randomly initialise model variables for training
    std::cout << "Now randomly initialising model variables for training" << std::endl;
    init_train();
    
    if (testing_type == SEPARATE_TEST)
    {
        // read held-out testing data
        testdata = new dataset;
        fin.open(ddir + tfile, std::ios::binary);
        testdata->read(fin);
        fin.close();
        test_M = testdata->M;
        
        // initialise aux variables for testing
        init_test();
    }
    
    // construct the reverse map (currently stupidly by reading back)
    for (auto w : word2id)
            id2word[w.second] = w.first;
    
    //display all configurations
    std::cout << "We will perform ";
    switch (testing_type)
    {
    case NO_TEST:
            std::cout << "no testing." << std::endl;
            break;
    case SELF_TEST:
            std::cout << "report training set likelihood." << std::endl;
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
    std::cout << "V = " << V << std::endl;
	
    return 0;
}

int model::parse_args(std::vector<std::string> arguments)
{
    for (auto arg = arguments.begin(); arg != arguments.end(); ++arg)
    {
        if (*arg == "--testing-mode")
        {
            ++arg;
            if (*arg == "nt")
                testing_type = NO_TEST;
            else if (*arg == "set")
                testing_type = SELF_TEST;
            else if (*arg == "net")
                testing_type = SEPARATE_TEST;
        }
        else if (*arg == "--output-model")
	{
            mdir = *(++arg);
            if (mdir == "")
                mdir = "./";
	}
        else if (*arg == "--training-file")
        {
            name = *(++arg);
            if (name == "")
		throw std::runtime_error( "Error: Invalid file path to training corpus." );
            std::string::size_type idx = name.find_last_of("/");
            if (idx == std::string::npos)
		ddir = "./";
            else
            {
		ddir = name.substr(0, idx + 1);
		name = name.substr(idx + 1, dfile.size() - ddir.size());
                vfile = name + ".vocab";
		tfile = name + "-test.dat";
		dfile = name + "-0" + ".dat";
            }    
        }
        else if (*arg == "--alpha")
        {
            double _alpha = std::stod(*(++arg));
            if (_alpha >= 0.0)
                alpha = _alpha;
        }
        else if (*arg == "--beta")
        {
            double _beta = std::stod(*(++arg));
            if (_beta >= 0.0)
                beta = _beta;
        }
        else if (*arg == "--num-topics")
        {
            int _K = std::stoi(*(++arg));
            if (_K > 0)
                K = _K;
        }
        else if (*arg == "--num-iterations")
        {
            int _n_iters = std::stoi(*(++arg));
            if (_n_iters > 0)
                n_iters = _n_iters;
        }
        else if (*arg == "--output-state-interval")
        {
            int _n_save = std::stoi(*(++arg));
            if (_n_save > 0)
                n_save = _n_save;
        }
        else if (*arg == "--num-top-words")
        {
            int _n_topWords = std::stoi(*(++arg));
            if (_n_topWords > 0) 
                n_topWords = _n_topWords;
        }
    }


    //Check specific parameter    
    if (testing_type == SEPARATE_TEST)
    {
        if (tfile == "")
            throw std::runtime_error( "Error: Invalid file path to test corpus." );
    }

    if (testing_type == INVALID)
        throw std::runtime_error( "Error: Please choose the task you would like to perform (-nt/-net)!" );

    return 0;
}

int model::init_train()
{
    alphaK = alpha/K;
    Vbeta = V * beta;

    // allocate heap memory for model variables
    n_wk = new unsigned*[V];
    for (unsigned w = 0; w < V; w++)
    {
        n_wk[w] = new unsigned[K];
        for (unsigned short k = 0; k < K; k++)
        {
            n_wk[w][k] = 0;
        }
    }

    n_mks.resize(M);

    n_k = new unsigned[K];
    for (unsigned short k = 0; k < K; k++)
    {
            n_k[k] = 0;
    }

    // random consistent assignment for model variables
    z = new unsigned short*[M];
    for (int m = 0; m < trngdata->M; m++)
    {
        unsigned N = trngdata->docs[m]->length;
        std::map<unsigned short, unsigned> map_nd_m;
        z[m] = new unsigned short[N];

        // initialize for z
        for (unsigned n = 0; n < N; n++)
        {
            unsigned w = trngdata->docs[m]->words[n];
            unsigned short topic = rng_.rand_k(K);
            z[m][n] = topic;
			
            // number of instances of word i assigned to topic j
            n_wk[w][topic] += 1;
            // number of words in document i assigned to topic j
            map_nd_m[topic] += 1;
            // total number of words assigned to topic j
            n_k[topic] += 1;
        }
        // transfer to sparse representation
        for (auto myc : map_nd_m)
                n_mks[m].push_back(myc.first, myc.second);
    }

    time_ellapsed.reserve(n_iters);
    likelihood.reserve(n_iters);

    // allocate heap memory for temporary variables
    p = new double[K];
    nd_m = new unsigned[K];
    rev_mapper = new unsigned short[K];
    for (unsigned short k = 0; k < K; ++k)
    {
        nd_m[k] = 0;
        rev_mapper[k] = K;
    }
        
    specific_init();

    return 0;
}

int model::init_test()
{
    // initialise variables for testing
    test_n_mk = new unsigned*[test_M];
    for (unsigned m = 0; m < test_M; m++)
    {
        test_n_mk[m] = new unsigned[K];
        for (int k = 0; k < K; k++)
        {
                test_n_mk[m][k] = 0;
        }
    }

    test_z = new unsigned short*[test_M];
    for (unsigned m = 0; m < testdata->M; m++)
    {
	int N = testdata->docs[m]->length;
	test_z[m] = new unsigned short[N];

        // assign values for n_wk, n_mk, n_k
        for (int n = 0; n < N; n++)
        {
            int w = testdata->docs[m]->words[n];
            int topic = rng_.rand_k(K);
            test_z[m][n] = topic;
            // number of words in document i assigned to topic j
            test_n_mk[m][topic] += 1;
        }
    }

    return 0;
}

double model::newllhw() const
{
    double sum = 0;
    unsigned num_tokens = 0;
    for (unsigned m = 0; m < test_M; ++m)
    {
        double dsum = 0;
        num_tokens += testdata->docs[m]->length;
        for (unsigned n = 0; n < testdata->docs[m]->length; n++)
        {
            double wsum = 0;
            unsigned w = testdata->docs[m]->words[n];
            for (unsigned short k = 0; k<K; k++)
                wsum += (test_n_mk[m][k] + alphaK) * (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
            dsum += log(wsum);
        }
	sum += dsum - testdata->docs[m]->length*log(testdata->docs[m]->length + alpha);
    }
    return ( sum / num_tokens );
}

double model::llhw() const
{
    double sum = 0;
    unsigned num_tokens = 0;
    std::vector<unsigned> nd_m(K);
    std::fill(nd_m.begin(), nd_m.end(),0);
    for (unsigned m = 0; m < M; m++)
    {
        for (const auto& k : n_mks[m])
            nd_m[k.idx] = k.val;

        double dsum = 0;
        num_tokens += trngdata->docs[m]->length;
        for (unsigned n = 0; n < trngdata->docs[m]->length; n++)
        {
            double wsum = 0;
            unsigned w = trngdata->docs[m]->words[n];
            for (int k = 0; k<K; k++)
                wsum += (nd_m[k] + alphaK) * (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
            dsum += log(wsum);
	}
	sum += dsum - trngdata->docs[m]->length * log(trngdata->docs[m]->length + alpha);
        for (const auto& k : n_mks[m])
            nd_m[k.idx] = 0;
    }
    return ( sum / num_tokens );
}

int model::save_model(unsigned iter) const
{
    std::string model_name = name + "-" + std::to_string(iter);

    if (iter >= 0)
    {
        std::ostringstream sstr1;
	sstr1 << std::setw(5) << std::setfill('0') << iter;
	model_name += sstr1.str();
    }
    else
    	model_name += "final";

    save_model_params(mdir + model_name + ".params");
    save_model_phi(mdir + model_name + ".phi");
    save_model_time(mdir + model_name + ".time");
    if (testing_type == SEPARATE_TEST || testing_type == SELF_TEST) 
        save_model_llh(mdir + model_name + ".llh");    
    if (n_topWords > 0)
        save_model_topWords(mdir + model_name + ".twords");
	
    return 0;
}

int model::save_model_time(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename);
		
    for (unsigned r = 0; r < time_ellapsed.size(); ++r)
        fout << time_ellapsed[r] << std::endl;

    fout.close();
    std::cout << "time done" << std::endl;

    return 0;
}

int model::save_model_llh(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );
    
    for (unsigned r = 0; r < likelihood.size(); ++r)
        fout << likelihood[r] << std::endl;
	
    fout.close();
    std::cout << "llh done" << std::endl;

    return 0;
}

int model::save_model_params(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );

    fout << "alpha=" << alpha << std::endl;
    fout << "beta=" << beta << std::endl;
    fout << "num-topics=" << K << std::endl;
    fout << "num-docs=" << M << std::endl;
    fout << "num-words=" << V << std::endl;
    fout << "num-iters=" << n_iters << std::endl;
    
    fout.close();
    std::cout << "others done" << std::endl;
    
    return 0;
}

int model::save_model_topWords(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );

    unsigned _n_topWords = n_topWords;
    if (_n_topWords > V)	_n_topWords = V;

    std::map<unsigned, std::string>::const_iterator it;

    for (unsigned short k = 0; k < K; k++)
    {
        std::vector<std::pair<unsigned, unsigned> > words_probs(V);
        std::pair<unsigned, unsigned> word_prob;
        for (int w = 0; w < V; w++)
        {
            word_prob.first = w;
            word_prob.second = n_wk[w][k];
            words_probs[w] = word_prob;
	}

	// quick sort to sort word-topic probability
	std::sort(words_probs.begin(), words_probs.end(), [](std::pair<unsigned, unsigned> &left, std::pair<unsigned, unsigned> &right){return left.second > right.second;});

	fout << "Topic " << k << "th:" << std::endl;
	for (unsigned i = 0; i < _n_topWords; i++)
	{
            it = id2word.find(words_probs[i].first);
            if (it != id2word.end())
		fout << "\t" << it->second << "   " << words_probs[i].second << std::endl;
	}
    }

    fout.close();
    std::cout << "twords done" << std::endl;

    return 0;
}

int model::save_model_phi(std::string filename) const
{
    std::ofstream fout(filename, std::ios::binary);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );

    fout.write((char *)&V, sizeof(unsigned));
    fout.write((char *)&K, sizeof(unsigned short));

    float *phi_w = new float[K];
    for (unsigned w = 0; w < V; ++w)
    {
	for (unsigned short k = 0; k < K; ++k)
            phi_w[k] = (n_wk[w][k] + beta) / (n_k[k] + Vbeta);
        fout.write((char *)(phi_w), sizeof(float)*K);
    }
    delete[] phi_w;

    fout.close();
    std::cout << "phi done" << std::endl;
    
    return 0;
}
