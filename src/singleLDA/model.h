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
#include "../commons/spvector.h"
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
    enum {						// testing types:
            INVALID,					// model not initialised
            NO_TEST,					// do not report any likelihood
            SELF_TEST,					// report training likelihood
            SEPARATE_TEST				// report likelihood on a held-out testing set
    } testing_type;

    /****** DATA ******/
    dataset *trngdata;				// training dataset
    dataset *testdata;				// test dataset

    std::map<unsigned, std::string> id2word;			// word map [int => string]
    std::map<std::string, unsigned> word2id;			// word map [string => int]
    
    /****** Model Parameters ******/
    unsigned M;							// Number of documents
    unsigned V; 							// Number of words in dictionary
    unsigned short K; 							// Number of topics

    /****** Model Hyper-Parameters ******/
    double alpha, alphaK;					// per document Topic proportions dirichlet prior
    double beta, Vbeta;				// Dirichlet language model

    /****** Model variables ******/
    unsigned short ** z;						// topic assignment for each word
    unsigned ** n_wk;					// number of times word w assigned to topic k
    std::vector< spvector > n_mks; //sparse representation of n_mk: number of words assigned to topic k in document m
    unsigned * n_k;						// number of words assigned to topic k = sum_w n_wk = sum_m n_mk
		
    /****** Temporary variables ******/
    double * p;
    unsigned *nd_m;
    unsigned short *rev_mapper;
      
    /****** Initialisation aux ******/
    int read_data();				// Read training (and testing) data
    int parse_args(std::vector<std::string> arguments);	// parse command line inputs
	        
    /****** Training aux ******/
    unsigned n_iters;	 				// Number of Gibbs sampling iterations
    unsigned n_save;			 			// Number of iters in between saving
    unsigned n_topWords; 				// Number of top words to be printed per topic
    int init_train();					// init for training
    virtual int specific_init() { return 0; }	// if sampling algo need some specific inits
    virtual int sampling(int m) { return 0; }	// sampling doc m outsourced to children
    
    /****** Testing aux ******/
    unsigned test_n_iters;
    unsigned test_M;
    unsigned short ** test_z;
    unsigned ** test_n_mk;
    int init_test();				// init for testing
    
    /****** Functions to update sufficient statistics ******/
    inline int add_to_topic(unsigned w, unsigned m, unsigned short topic)
    {
        ++n_k[topic];
        ++n_wk[w][topic];
        ++nd_m[topic];
        if(rev_mapper[topic] == K)
            rev_mapper[topic] = n_mks[m].push_back(topic, 1);
        else
            n_mks[m].increment(rev_mapper[topic]);

        return 0;
    }
    inline int remove_from_topic(unsigned word, unsigned doc, unsigned short topic)
    {
        --n_k[topic];
        --n_wk[word][topic];
        --nd_m[topic];
        unsigned short old_topic_pos = rev_mapper[topic];
        n_mks[doc].decrement(old_topic_pos);
        if (n_mks[doc].val_in(old_topic_pos) == 0)
        {
            unsigned short replacement_topic = n_mks[doc].erase_pos(old_topic_pos);
            rev_mapper[replacement_topic] = old_topic_pos;
            rev_mapper[topic] = K;
        }

        return 0;
    }

    /****** Performance computations ******/
    std::vector<double> time_ellapsed; // time ellapsed after each iteration
    std::vector<double> likelihood; // likelihood after each iteration
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

    /******* random number generator ******/
    xorshift128plus rng_;
};

#endif