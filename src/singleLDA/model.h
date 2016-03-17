#ifndef	_MODEL_H
#define	_MODEL_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include "dataset.h"
#include "utils.h"

class model {
public:

	/****** constructor/destructor ******/
	model();
	~model();

	/****** interacting functions ******/
	static model* init(int, char**);// initialize the model randomly
	int train();					// train LDA using prescribed algorithm on training data
	int test();						// test LDA according to specified method

protected:
	/****** Enums for testing type model status  ******/
	enum {							// testing types:
		INVALID,					// model not initialised
		NO_TEST,					// do not report any likelihood
		SELF_TEST,					// report training likelihood
		SEPARATE_TEST				// report likelihood on a held-out testing set
	} testing_type;

	/****** DATA ******/
	dataset *trngdata;				// training dataset
	dataset *testdata;				// test dataset

	std::map<int, std::string> id2word;			// word map [int => string]
	std::map<std::string, int> word2id;			// word map [string => int]
    
    /****** Model Parameters ******/
	int M;							// Number of documents
	int V; 							// Number of words in dictionary
	int K; 							// Number of topics

	/****** Model Hyper-Parameters ******/
	double alpha;					// per document Topic proportions dirichlet prior
	double beta, Vbeta;				// Dirichlet language model

	/****** Model variables ******/
	int ** z;						// topic assignment for each word
	int ** n_wk;					// number of times word w assigned to topic k
	std::vector< std::vector< std::pair<int, int> > > n_mks; //sparse representation of n_mk: number of words assigned to topic k in document m
	int * n_k;						// number of words assigned to topic k = sum_w n_wk = sum_m n_mk
		
	/****** Temporary variables ******/
	double * p;
	int *nd_m;
	int *rev_mapper;
      
	/****** Initialisation aux ******/
	int read_data();				// Read training (and testing) data
    int parse_args(std::vector<std::string> arguments);	// parse command line inputs
	        
	/****** Training aux ******/
	int n_iters;	 				// Number of Gibbs sampling iterations
	int n_save;			 			// Number of iters in between saving
	int n_topWords; 				// Number of top words to be printed per topic
	int init_train();					// init for training
	virtual int specific_init() { return 0; }	// if sampling algo need some specific inits
	virtual int sampling(int m) { return 0; }	// sampling doc m outsourced to children
    
	/****** Testing aux ******/
	int test_n_iters;
	int test_M;
	int ** test_z;
	int ** test_n_wk;
	int ** test_n_mk;
	int * test_n_k;
	int init_test();				// init for testing
	int vanilla_sampling(int m);	// vanila sampling doc m for testing
    
	/****** Functions to update sufficient statistics ******/
	inline int add_to_topic(int w, int m, int topic, int old_topic)
	{
		n_wk[w][topic] += 1;
		if (topic != old_topic && nd_m[topic] == 0)
		{
			rev_mapper[topic] = n_mks[m].size();
			n_mks[m].push_back(std::pair<int, int>(topic, 1));
		}
		else
			n_mks[m][rev_mapper[topic]].second += 1;
		nd_m[topic] += 1;
		if (nd_m[old_topic] == 0)
		{
			n_mks[m][rev_mapper[old_topic]].first = n_mks[m].back().first;
			n_mks[m][rev_mapper[old_topic]].second = n_mks[m].back().second;
			rev_mapper[n_mks[m].back().first] = rev_mapper[old_topic];
			n_mks[m].pop_back();
			rev_mapper[old_topic] = -1;
		}
		n_k[topic] += 1;

		return 0;
	}
	inline int remove_from_topic(int word, int doc, int topic)
	{
		n_wk[word][topic] -= 1;
		nd_m[topic] -= 1;
		n_mks[doc][rev_mapper[topic]].second -= 1;
		n_k[topic] -= 1;

		return 0;
	}

	/****** Performance computations ******/
	std::vector<double> time_ellapsed; // time ellapsed after each iteration
	std::vector<double> likelihood; // likelihood after each iteration
	double newllhw() const;			// per word log-likelihood for new (unseen) data based on the estimated LDA model
	double llhw() const;			// per word log-likelihood for training data based on the estimated LDA model

	/****** File and Folder Paths ******/
	std::string ddir;				// data directory
	std::string mdir;				// model directory
	std::string dfile;				// train data file    
	std::string tfile;				// test data file    

	/****** save LDA model to files ******/
	int save_model(int iter) const;						// save model: call each of the following:		
	int save_model_time(std::string filename) const;	// model_name.time: time at which statistics calculated
	int save_model_llh(std::string filename) const;		// model_name.llh: Per word likelihood on held out documents
	int save_model_params(std::string filename) const;	// model_name.params: containing other parameters of the model (alpha, beta, M, V, K)
	int save_model_topWords(std::string filename) const;// model_name.twords: Top words in each top
	int save_model_phi(std::string filename) const;		// model_name.phi: topic-word distributions

	/****** Some functions for debugging ******/
	int sanity() const;
	int dump();
	int dump2();
};

#endif