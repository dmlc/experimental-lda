#ifndef	_MODEL_H
#define	_MODEL_H

#include <algorithm>
#include <atomic> 
#include <future>
#include <fstream>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>

//#include "workqueue.h"
#include "circqueue.h"
#include "constants.h"
#include "dataset.h"
#include "utils.h"
#include <assert.h>

// LDA model
class model {
public:
    // fixed options
	std::string wordmapfile;	// file that contains word map [string -> integer id]
	std::string trainlogfile;	// training log file
	std::string tassign_suffix;	// suffix for topic assignment file
	std::string theta_suffix;	// suffix for theta file
	std::string phi_suffix;		// suffix for phi file
	std::string others_suffix;	// suffix for file containing other parameters
	std::string twords_suffix;	// suffix for file containing words-per-topics
	std::string time_suffix;	// suffix for file containing time after each iteration
	std::string llh_suffix;		// suffix for file containing train log likelihood after each iteration

	std::string ddir;			// data directory
	std::string mdir;			// model directory
	std::string dfile;			// train data file    
	std::string tfile;			// test data file    
	std::string model_name;		// model name
    int model_status;			// model status:
				// MODEL_UNKNOWN: unknown status
				// MODEL_SELF_TEST: estimating from scratch
				// MODEL_SEPARATE_TEST: continue to estimate the model from a previous one

    dataset * ptrndata;	// pointer to training dataset object
    dataset * pnewdata; // pointer to new dataset object

    mapid2word id2word; // word map [int => string]
	mapword2id word2id; // word map [string => int]
    
	// locks
	struct delta
	{
		int word;
		int old_topic;
		int new_topic;
		
		delta()
		{	}
		
		delta(int a, int b, int c) : word(a), old_topic(b), new_topic(c)
		{	}
	};
	circular_queue<delta> *cbuff;
	//circular_queue<delta> cbuff[20];
		
	
	std::mutex t_mtx;	// lock for nwsum
	std::mutex* mtx;	// lock for each word
	//workqueue::threadpool *ts;	// doc sampling thread pool
	//workqueue::threadpool *tu;	// table update thread pool
	bool *done;			// flag for completion
	bool inf_stop;
	int nst; // number of sampling threads
    int ntt; // number of table updating threads
    
	
    // --- model parameters ---    
    int M; // dataset size (i.e., number of docs)
    int V; // vocabulary size
    int K; // number of topics
	double alpha, beta, Vbeta; // LDA hyperparameters 
    int niters; // number of Gibbs sampling iterations for estimation
    int savestep; // saving period
    int twords; // print out top words per each topic

	// --- model variables ---
	//std::atomic<int> ** nw;
	int ** nw; // cwt[i][j]: number of instances of word/term i assigned to topic j, size V x K
    //std::vector< std::unordered_map< int, int > > nds1; //sparse representation of nd
	std::vector< std::vector< std::pair<int, int> > > nds1; //sparse representation of nd
	//std::atomic<int> * nwsum; 
	int * nwsum; // nwsum[j]: total number of words assigned to topic j, size K
	int ** z; // topic assignments for words, size M x doc.size()
	std::vector<double> time_ellapsed; // time ellapsed after each iteration
	std::vector<double> likelihood; // likelihood after each iteration
	    
    // --- for held out set testing only ---
	int inf_niters;
    int newM;
    int ** newnw;
    int ** newnd;
    int * newnwsum;
    int ** newz;
	// --------------------------------------
    
    model()
	{
		set_default_values();
    }
          
    ~model();
    
    // set default values for variables
    void set_default_values();   

    // parse command line to get options
    int parse_args(int argc, char ** argv);
	    
    // initialize the model
    int init(int argc, char ** argv);
    
    // save LDA model to files
    // generate the model name for the current iteration // iter = -1 => final model
	std::string generate_model_name(int iter);
	// model_name.time: time at which statistics calculated
    // model_name.llh: Per word likelihood on held out documents
	// model_name.twords: Top words in each top
    // model_name.others: containing other parameters of the model (alpha, beta, M, V, K)
	// model_name.phi: topic-word distributions
	int save_model(std::string model_name);
	int save_model_time(std::string filename) const;
	int save_model_llh(std::string filename) const;
	int save_model_others(std::string filename) const;
	int save_model_twords(std::string filename);
	int save_model_phi(std::string filename) const;
	    
    // init for estimation
    int init_est();

    // init for inference
    int init_inf();

	// estimation of LDA model on training data
	int estimate();
	virtual int specific_init() { return 0; }
	virtual int sampling( int m ) { return 0; }// sampling outsourced to children
	virtual int updater(int i) //updating details outsourced to children
	{
		do
		{
			for(int tn = 0; tn<nst; ++tn)
			{
					if(!(cbuff[i*nst + tn].empty()))
					{
						delta temp = cbuff[i*nst + tn].front();
						cbuff[i*nst + tn].pop();
						nw[temp.word][temp.old_topic] -= 1;
						nw[temp.word][temp.new_topic] += 1;
						nwsum[temp.old_topic] -= 1;
						nwsum[temp.new_topic] += 1;
						//nw[temp.word][temp.old_topic].fetch_add(-1);
						//nw[temp.word][temp.new_topic].fetch_add(+1);
						//nwsum[temp.old_topic].fetch_add(-1);
						//nwsum[temp.new_topic].fetch_add(+1);
					}
			}
		} while (!done[i]); //(done.test_and_set());

		return 0;
	}
    
	// inference for new (unseen) data based on the estimated LDA model
    int inference();
    int inf_sampling(int m);
	int inf_parallel();
    
	// per word log-likelihood for new (unseen) data based on the estimated LDA model
	double newllhw() const;
	
	// per word log-likelihood for training data based on the estimated LDA model
	double llhw() const;
	
	int sanity() const;
	int dump();
	int dump2();
};

#endif
