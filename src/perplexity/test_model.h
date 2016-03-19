#ifndef	_TEST_MODEL_H
#define	_TEST_MODEL_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../commons/dataset.h"
#include "../commons/utils.h"

class test_model {
public:

public:

	/****** constructor/destructor ******/
	test_model();
	~test_model();

	/****** interacting functions ******/
	int init(int, char**);
	int test();						// test LDA according to specified method

protected:
	/****** DATA ******/
	dataset *testdata;				// test dataset

	/****** Model Parameters ******/
	int V; 							// Number of words in dictionary
	int K; 							// Number of topics

	/****** Model Hyper-Parameters ******/
	double alpha;					// per document Topic proportions dirichlet prior
	double beta, Vbeta;				// Dirichlet language model

	/****** Model variables ******/
	//int * n_wk;						// number of times word w assigned to topic k
	//int * n_k;						// number of words assigned to topic k = sum_w n_wk = sum_m n_mk
	float * phi_wk;

	/****** Initialisation aux ******/
	int parse_args(std::vector<std::string> arguments);	// parse command line inputs	

	/****** Testing aux ******/
	int test_n_iters;
	int test_M;
	int ** test_z;
	//int ** test_n_wk;
	int ** test_n_mk;
	int * test_n_k;
	int init_test();				// init for testing
	int vanilla_sampling(int m);	// vanila sampling doc m for testing
	int async_test();				// test asynchronously to the training procedure

	/****** Performance computations ******/
	double likelihood;				// likelihood after each iteration
	double newllhw() const;			// per word log-likelihood for new (unseen) data based on the estimated LDA model

	/****** File and Folder Paths ******/
	std::string ddir;				// data directory
	std::string dfile;				// phi matrix file
	std::string pfile;				// params file
	std::string lfile;				// likelihood file
	std::string tfile;				// test data file

	/******	Read and write ******/
	int read_data();								// Read testing data
	int read_phi(std::string filename);				// model_name.phi: topic-word distributions
	int read_model_params(std::string filename);
	int save_llh(std::string filename) const;		// model_name.llh: Per word likelihood on held out documents
};

#endif
