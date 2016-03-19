#include "test_model.h"

test_model::test_model()
{
	testdata = NULL;

	V = 0;
	K = 100;

	alpha = 50.0 / K;
	beta = 0.1;

	//n_wk = NULL;
	//n_k = NULL;
	phi_wk = NULL;

	test_n_iters = 100;
	test_M = 0;
	test_z = NULL;
	//test_n_wk = NULL;
	test_n_mk = NULL;
	test_n_k = NULL;

	likelihood=-1e300;

	dfile = "";
	tfile = "";
	pfile = "";
	lfile = "";
}

test_model::~test_model()
{
	if (testdata) delete testdata;

	//if (n_wk)	delete[] n_wk;

	//if (n_k)	delete[] n_k;

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

	/*if (test_n_wk)
	{
		for (int w = 0; w < V; w++)
		{
			if (test_n_wk[w])
			{
				delete[] test_n_wk[w];
			}
		}
		delete[] test_n_wk;
	}*/

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

int test_model::init(int argc, char ** argv)
{
	// call parse_args
	std::vector<std::string> arguments(argv, argv + argc);	
	if (parse_args(arguments))
		return 1;

	// read data
	if (read_data())
		return 1;

	//display all configurations
	std::cout << "We will perform a test on a heldout set." << std::endl;
	std::cout << "Phi matrix file = " << dfile << std::endl;
	std::cout << "Testing file = " << tfile << std::endl;
	std::cout << "test_n_iters = " << test_n_iters << std::endl;
	std::cout << "alpha = " << alpha << std::endl;
	std::cout << "beta = " << beta << std::endl;
	std::cout << "K = " << K << std::endl;
	std::cout << "V = " << V << std::endl;

	// initialise aux variables for testing
	if (init_test())
	{
		std::cout << "Error: Failed to initialise testing variables!" << std::endl;
		return 1;
	}


	return 0;
}

int test_model::test()
{
	for (int iter = 1; iter <= test_n_iters; ++iter)
	{
		//std::cout << iter << std::endl;
		// for each doc
		for (int m = 0; m < test_M; m++)
			vanilla_sampling(m);
	}
	likelihood = newllhw();
	std::cout << "Likelihood on held out documents: " << likelihood <<std::endl;
	save_llh(lfile);
	return 0;
}

int test_model::parse_args(std::vector<std::string> arguments)
{
	double _alpha = -1.0;

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
			
			pfile = dfile + ".params";
			lfile = dfile + ".llh";
			dfile += ".phi";
			
		}
		else if (*arg == "--testing-file")
		{
			tfile = *(++arg);
			if (tfile == "")
			{
				std::cout << "Error: Invalid file path to testing corpus." << std::endl;
				return 1;
			}
		}
		else if (*arg == "--num-iterations")
		{
			int _n_iters = std::stoi(*(++arg));
			if (_n_iters > 0)	test_n_iters = _n_iters;
		}
	}

	return 0;
}

int test_model::init_test()
{
	//sum to get n_k
	/*n_k = new int[K];
	for (int k = 0; k < K; ++k)
	{
		n_k[k] = 0;
	}
	for (int w = 0; w < V; w++)
	{
		int ptr = w*K;
		for (int k = 0; k < K; k++)
		{
			n_k[k] += n_wk[ptr + k];
		}
	}*/

	// initialise variables for testing
	/*test_n_wk = new int*[V];
	for (int w = 0; w < V; w++)
	{
		test_n_wk[w] = new int[K];
		for (int k = 0; k < K; k++)
		{
			test_n_wk[w][k] = 0;
		}
	}*/

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
	#pragma omp parallel for
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
			//test_n_wk[w][topic] += 1;
			// number of words in document i assigned to topic j
			test_n_mk[m][topic] += 1;
			// total number of words assigned to topic j
			test_n_k[topic] += 1;
		}
	}

	return 0;
}

int test_model::vanilla_sampling(int m)
{
	double *p = new double[K];
	for (int n = 0; n < testdata->docs[m]->length; n++)
	{
		int w = testdata->docs[m]->words[n];
		int ptr = w*K;

		// remove z_i from the count variables
		int topic = test_z[m][n];
		//test_n_wk[w][topic] -= 1;
		test_n_mk[m][topic] -= 1;
		test_n_k[topic] -= 1;

		double psum = 0;
		// do multinomial sampling via cumulative method
		for (int k = 0; k < K; k++)
		{
			psum += (test_n_mk[m][k] + alpha) * phi_wk[ptr + k];
			p[k] = psum;
		}

		// scaled sample because of unnormalized p[]
		double u = utils::unif01() * psum;
		topic = std::lower_bound(p, p + K, u) - p;

		// add newly estimated z_i to count variables
		//test_n_wk[w][topic] += 1;
		test_n_mk[m][topic] += 1;
		test_n_k[topic] += 1;
		test_z[m][n] = topic;
	}
	delete[] p;
	return 0;
}

double test_model::newllhw() const
{
	double sum = 0;
	int num_tokens = 0;

	#pragma omp parallel for reduction(+:sum)
	for (int m = 0; m < test_M; ++m)
	{
		double dsum = 0;
		num_tokens += testdata->docs[m]->length;
		for (int n = 0; n < testdata->docs[m]->length; n++)
		{
			double wsum = 0;
			int w = testdata->docs[m]->words[n];
			int ptr = w*K;
			for (int k = 0; k<K; k++)
			{
				wsum += (test_n_mk[m][k] + alpha) * phi_wk[ptr + k];
			}
			dsum += log(wsum);
		}
		sum += dsum - testdata->docs[m]->length*log(testdata->docs[m]->length + K * alpha);
	}
	return sum / num_tokens;
}

int test_model::read_data()
{
	utils::randomize();
	std::ifstream fin;

	//check if llh already computer
    	fin.open(lfile);
    	if (fin.good())
	{
        	std::cout << "Error: LLH file already exists" << std::endl;
		fin.close();
        	return 1;
    	}
	fin.close();   

	// read params for model
	if (read_model_params(pfile))
	{
		std::cout << "Error: Failed to read model parameters!" << std::endl;
		return 1;
	}

	// read phi matrix
	if (read_phi(dfile))
	{
		std::cout << "Error: Failed to read phi matrix!" << std::endl;
		return 1;
	}

	// read held-out testing data
	testdata = new dataset;
	fin.open(tfile, std::ios::binary);
	if (testdata->read(fin))
	{
		std::cout << "Error: Failed to read testing corpus!" << std::endl;
		return 1;
	}
	fin.close();
	test_M = testdata->M;

	return 0;
}

int test_model::read_model_params(std::string filename)
{
	std::ifstream fin(filename);
	if (!fin)
	{
		std::cout << "Error: Cannot open model parameters file: " << filename << std::endl;
		return 1;
	}

	std::string temp;
	
	std::getline(fin, temp);
	alpha = std::stod(temp.substr(6));
	
	std::getline(fin, temp);
	beta = std::stod(temp.substr(5));
	
	std::getline(fin, temp);
	K = std::stoi(temp.substr(11));

	std::getline(fin, temp);	//num_docs to be ignored

	std::getline(fin, temp);
	V = std::stoi(temp.substr(10));
	
	fin.close();

	return 0;
}

int test_model::read_phi(std::string filename)
{
	std::ifstream fin(filename, std::ios::binary);
	if (!fin)
	{
		std::cout << "Error: Cannot open file to read: " << filename << std::endl;
		return 1;
	}

	int _V, _K;
	fin.read((char *)&_V, sizeof(int));
	fin.read((char *)&_K, sizeof(int));

	if ( (V!=_V) || (K!=_K) )
	{
		std::cout << "Parameters and phi do not match!";
		return 1;
	}

	phi_wk = new float[V*K];
	fin.read((char *)(phi_wk), sizeof(float)*V*K);

	fin.close();

	return 0;
}

int test_model::save_llh(std::string filename) const
{
	std::ofstream fout(filename);
	if (!fout)
	{
		std::cout << "Error: Cannot open file to save: " << filename << std::endl;
		return 1;
	}

	fout << likelihood << std::endl;

	fout.close();

	return 0;
}
