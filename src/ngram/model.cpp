#include "model.h"
#include <sstream>

model::model()
{
    testing_type = INVALID;

    trngdata = NULL;
    testdata = NULL;

    M = 0;
    V = 0;
    T = 0;

    beta = 0.1;
    Vbeta = 0;

    n_w = NULL;

    n_topWords = 0;

    test_M = 0;
    
    time_ellapsed = 0;
    likelihood = 0;

    ddir = "./";
    mdir = "./";
    dfile = "";
    tfile = "";
}

model::~model()
{
    if (trngdata) delete trngdata;
    if (testdata) delete testdata;

    if (n_w) delete[] n_w;
}

model* model::init(int argc, char ** argv)
{
    model *ngram = NULL;

    std::vector<std::string> arguments(argv, argv + argc);
    for (auto arg = arguments.begin(); arg != arguments.end() - 1; ++arg)
    {
        if (*arg == "--method")
        {
            if (*(arg + 1) == "unigram")
            {
                ngram = new model;
                std::cout << "Running inference using unigram" << std::endl;
            }
            else
            {
                throw std::runtime_error("Error: Invalid inference algorithm!");
            }
        }
    }
    
    if (ngram != NULL)
    {
	// call for parsing arguments
        ngram->parse_args(arguments);

	// read data
	ngram->read_data();
    }
    else
    {
	throw std::runtime_error("Error: Inference algorithm not specified!");
    }

    return ngram;
}

int model::train()
{
    // for each document
    std::chrono::high_resolution_clock::time_point ts, tn; ts=tn;
    ts = std::chrono::high_resolution_clock::now();
    for (unsigned m = 0; m < M; m++)
    {
        for (unsigned n = 0; n < trngdata->docs[m]->length; n++)
        {
            unsigned w = trngdata->docs[m]->words[n];
            ++n_w[w];
            ++T;
        }
    }
    tn = std::chrono::high_resolution_clock::now();
    return 0;
}

int model::test()
{
    if( testing_type == SEPARATE_TEST )
    {
        likelihood = newllhw();
        std::cout << "Likelihood on held out documents: " << likelihood << " at time " << time_ellapsed << std::endl;
    }
    return 0;
}

int model::read_data()
{
    //utils::randomize();
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
    std::cout << "beta = " << beta << std::endl;
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
            
            else if (*arg == "net")            
                testing_type = SEPARATE_TEST;
        }
        else if (*arg == "--output-model")
        {
            mdir = *(++arg);
            if (mdir == "")
                mdir = "./";
        }
        else if (*arg == "--dataset")
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
        else if (*arg == "--beta")
        {
            double _beta = std::stod(*(++arg));
            if (_beta >= 0.0)
                beta = _beta;
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
    Vbeta = V * beta;
    // allocate heap memory for model variables
    n_w = new unsigned[V];
    //n_w = new std::atomic<int>*[V];
    std::fill(n_w, n_w+V, 0);
    
}

int model::init_test()
{
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
            unsigned w = testdata->docs[m]->words[n];
            dsum += log ( (n_w[w] + beta) / (T + Vbeta) );
        }
        sum += dsum;
    }
    return exp( -sum / num_tokens );
}

double model::llhw() const
{
    double sum = 0;
    for (unsigned m = 0; m < M; m++)
    {
        double dsum = 0;
        for (unsigned n = 0; n < trngdata->docs[m]->length; n++)
        {
            unsigned w = trngdata->docs[m]->words[n];
            dsum += log( (n_w[w] + beta) / (T + Vbeta) );
        }
        sum += dsum;
    }
    return exp( -sum / T );
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
    save_model_llh(mdir + model_name + ".llh");
    save_model_time(mdir + model_name + ".time");
    if (testing_type == SEPARATE_TEST)  save_model_llh(mdir + model_name + ".llh");
    if (n_topWords > 0) save_model_topWords(mdir + model_name + ".twords");
	
    return 0;
}

int model::save_model_time(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename);
		
    fout << time_ellapsed << std::endl;

    fout.close();
    std::cout << "time done" << std::endl;

    return 0;
}

int model::save_model_llh(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );
    
    fout << likelihood << std::endl;
	
    fout.close();
    std::cout << "llh done" << std::endl;

    return 0;
}

int model::save_model_params(std::string filename) const
{
    std::ofstream fout(filename);
    if (!fout)
        throw std::runtime_error( "Error: Cannot open file to save: " + filename );

    fout << "beta=" << beta << std::endl;
    fout << "num-docs=" << M << std::endl;
    fout << "num-words=" << V << std::endl;
    
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

    
    std::vector<std::pair<unsigned, unsigned> > words_probs(V);
    std::pair<unsigned, unsigned> word_prob;
    for (int w = 0; w < V; w++)
    {
        word_prob.first = w;
        word_prob.second = n_w[w];
        words_probs[w] = word_prob;
    }

    // quick sort to sort word-topic probability
    std::sort(words_probs.begin(), words_probs.end(), [](auto &left, auto &right){return left.second > right.second;});

    for (unsigned i = 0; i < _n_topWords; i++)
    {
        it = id2word.find(words_probs[i].first);
        if (it != id2word.end())
            fout << "\t" << it->second << "   " << words_probs[i].second << std::endl;
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

    unsigned short K=1;
    fout.write((char *)&V, sizeof(unsigned));
    fout.write((char *)&K, sizeof(unsigned short));
    
    float phi_w;
    for (unsigned w = 0; w < V; ++w)
    {
        phi_w = (n_w[w] + beta) / (T + Vbeta);
        fout.write((char *)&phi_w, sizeof(float));
    }

    fout.close();
    std::cout << "phi done" << std::endl;

    return 0;
}
