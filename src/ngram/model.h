#ifndef	_MODEL_H
#define	_MODEL_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include "../commons/dataset.h"
#include "../commons/my_rand.h"
#include "../commons/utils.h"

class model {
    
public:
    /****** constructor/destructor ******/
    model();
    virtual ~model();

    /****** interacting functions ******/
    static model* init(int, char**);// initialize the model randomly
    int train();					// train LDA using prescribed algorithm on training data
    int test();						// test LDA according to specified method

protected:
    /****** Enums for testing type model status  ******/
    enum {							// testing types:
            INVALID,					// model not initialised
            NO_TEST,					// do not report any likelihood
            SEPARATE_TEST                               // report likelihood on a held-out testing set
    } testing_type;

    /****** DATA ******/
    dataset *trngdata;				// training dataset
    dataset *testdata;				// test dataset

    std::map<unsigned, std::string> id2word;			// word map [int => string]
    std::map<std::string, unsigned> word2id;			// word map [string => int]

    /****** Model Parameters ******/
    unsigned M;							// Number of documents
    unsigned V; 							// Number of words in dictionary
    unsigned T;

    /****** Model Hyper-Parameters ******/
    double beta, Vbeta;				// Dirichlet language model

    /****** Model variables ******/
    unsigned * n_w;					// number of times word w assigned to topic k
    //std::atomic<int> * n_wk;		// if on target system atomics are lock_free()

    /****** Initialisation aux ******/
    int read_data();				// Read training (and testing) data
    int parse_args(std::vector<std::string> arguments);	// parse command line inputs

    /****** Training aux ******/
    unsigned n_topWords; 				// Number of top words to be printed per topic
    int init_train();					// init for training

    /****** Testing aux ******/
    unsigned test_M;
    int init_test();				// init for testing

    /****** Performance computations ******/
    double time_ellapsed;                       // time ellapsed after each iteration
    double likelihood;                          // likelihood after each iteration
    double newllhw() const;			// per word log-likelihood for new (unseen) data based on the estimated LDA model
    double llhw() const;			// per word log-likelihood for training data based on the estimated LDA model

    /****** File and Folder Paths ******/
    std::string name;				// dataset name
    std::string ddir;				// data directory
    std::string mdir;				// model directory
    std::string dfile;				// train data file    
    std::string tfile;				// test data file
    std::string vfile;				// vocabulary file

    /****** save LDA model to files ******/
    int save_model(unsigned iter) const;						// save model: call each of the following:		
    int save_model_time(std::string filename) const;	// model_name.time: time at which statistics calculated
    int save_model_llh(std::string filename) const;		// model_name.llh: Per word likelihood on held out documents
    int save_model_params(std::string filename) const;	// model_name.params: containing other parameters of the model (alpha, beta, M, V, K)
    int save_model_topWords(std::string filename) const;// model_name.twords: Top words in each top
    int save_model_phi(std::string filename) const;		// model_name.phi: topic-word distributions
};

#endif
