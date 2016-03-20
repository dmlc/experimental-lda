#ifndef	_MODEL_H
#define	_MODEL_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>

#include <atomic> 
#include <future>
#include <mutex>
#include <thread>

#include "../commons/circqueue.h"
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
    enum {							// testing types:
            INVALID,					// model not initialised
            NO_TEST,					// do not report any likelihood
            SEPARATE_TEST					// report likelihood on a held-out testing set
    } testing_type;

    /****** DATA ******/
    dataset *trngdata;				// training dataset
    dataset *testdata;				// test dataset

    std::map<unsigned, std::string> id2word;			// word map [int => string]
    std::map<std::string, unsigned> word2id;			// word map [string => int]

    /****** Model Parameters ******/
    unsigned M;							// Number of documents
    unsigned V; 							// Number of words in dictionary
    unsigned short K; 						// Number of topics

    /****** Model Hyper-Parameters ******/
    double alpha, alphaK;					// per document Topic proportions dirichlet prior
    double beta, Vbeta;				// Dirichlet language model

    /****** Model variables ******/
    unsigned short ** z;						// topic assignment for each word
    unsigned ** n_wk;					// number of times word w assigned to topic k
    //std::atomic<int> ** n_wk;		// if on target system atomics are lock_free()
    std::vector< spvector > n_mks;  //sparse representation of n_mk: number of words assigned to topic k in document m
    //std::vector< std::vector< std::pair<int, int> > > n_mks; //sparse representation of n_mk: number of words assigned to topic k in document m
    unsigned * n_k;						// number of words assigned to topic k = sum_w n_wk = sum_m n_mk
    //std::atomic<int> * n_k;		// if on target system atomics are lock_free()

    /****** Initialisation aux ******/
    int read_data();				// Read training (and testing) data
    int parse_args(std::vector<std::string> arguments);	// parse command line inputs

    /****** Training aux ******/
    unsigned n_iters;	 				// Number of Gibbs sampling iterations
    unsigned n_save;			 			// Number of iters in between saving
    unsigned n_topWords; 				// Number of top words to be printed per topic
    int init_train();					// init for training
    virtual int specific_init() { return 0; }	// if sampling algo need some specific inits
    virtual int sampling(unsigned) { return 0; }		// sampling on machine i outsourced to children
    virtual int updater(unsigned i)					// updating sufficient statistics, can be outsourced to children
    {
        do
        {
            for (unsigned tn = 0; tn<nst; ++tn)
            {
                if (!(cbuff[i*nst + tn].empty()))
                {
                    delta temp = cbuff[i*nst + tn].front();
                    cbuff[i*nst + tn].pop();
                    --n_wk[temp.word][temp.old_topic];
                    ++n_wk[temp.word][temp.new_topic];
                    --n_k[temp.old_topic];
                    ++n_k[temp.new_topic];
                    //n_wk[temp.word][temp.old_topic].fetch_add(-1);
                    //n_wk[temp.word][temp.new_topic].fetch_add(+1);
                    //n_k[temp.old_topic].fetch_add(-1);
                    //n_k[temp.new_topic].fetch_add(+1);
                }
            }
        } while (!inf_stop); //(!done[i]);

        return 0;
    }			

    /****** Testing aux ******/
    unsigned test_n_iters;
    unsigned test_M;
    unsigned short ** test_z;
    unsigned ** test_n_mk;
    int init_test();				// init for testing
    int async_test();				// test asynchronously to the training procedure

    /****** Concurency parameters ******/
    unsigned nst;						// number of sampling threads
    unsigned ntt;						// number of table updating threads
    volatile bool inf_stop;					// flag for stopping inference
    unsigned *current_iter;					// current iteration of some thread
    std::mutex t_mtx;				// lock for n_k
    std::mutex* mtx;				// lock for data-structures involving n_wk
    struct delta					// sufficient statistic update message
    {
        unsigned word;
        unsigned short old_topic;
        unsigned short new_topic;

        delta()
        {	}

        delta(unsigned a, unsigned short b, unsigned short c) : word(a), old_topic(b), new_topic(c)
        {	}
    };
    circular_queue<delta> *cbuff;	// buffer for messages NST*NTT

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

};

#endif
