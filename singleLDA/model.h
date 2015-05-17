#ifndef	_MODEL_H
#define	_MODEL_H

#include <algorithm>
#include <functional>
#include <fstream>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "constants.h"
#include "dataset.h"
#include "utils.h"

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
    
    // --- model parameters ---    
    int M; // dataset size (i.e., number of docs)
    int V; // vocabulary size
    int K; // number of topics
    double alpha, beta, Vbeta; // LDA hyperparameters 
    int niters; // number of Gibbs sampling iterations for estimation
    int savestep; // saving period
    int twords; // print out top words per each topic

	// --- model variables ---
	int ** nw; // cwt[i][j]: number of instances of word/term i assigned to topic j, size V x K
    //std::vector< std::unordered_map< int, int > > nds1; //sparse representation of nd
	std::vector< std::vector< std::pair<int, int> > > nds1; //sparse representation of nd
	int * nwsum; // nwsum[j]: total number of words assigned to topic j, size K
	int ** z; // topic assignments for words, size M x doc.size()
	std::vector<double> time_ellapsed; // time ellapsed after each iteration
	std::vector<double> likelihood; // likelihood after each iteration
	
	// --- temporary variables ---
	double * p;
	int *nd_m;
	int *rev_mapper;
    
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
    
	// inference for new (unseen) data based on the estimated LDA model
    int inference();
    int inf_sampling(int m);
    
	// per word log-likelihood for new (unseen) data based on the estimated LDA model
	double newllhw() const;
	
	// per word log-likelihood for training data based on the estimated LDA model
	double llhw() const;

	// functions to update sufficient statistics
	inline int add_to_topic( int w, int m, int topic, int old_topic )
	{
		nw[w][topic] += 1;
		if (topic!=old_topic && nd_m[topic] == 0)
		{
			rev_mapper[topic] = nds1[m].size();
			nds1[m].push_back(std::pair<int, int>(topic, 1));
		}
		else
			nds1[m][rev_mapper[topic]].second += 1;
		nd_m[topic] += 1;
		if (nd_m[old_topic] == 0)
		{
			nds1[m][rev_mapper[old_topic]].first = nds1[m].back().first;
			nds1[m][rev_mapper[old_topic]].second = nds1[m].back().second;
			rev_mapper[nds1[m].back().first] = rev_mapper[old_topic];
			nds1[m].pop_back();
			rev_mapper[old_topic] = -1;
		}
		nwsum[topic] += 1;
		
		//nw[word][topic] += 1;
		//nds1[doc][topic] += 1;
		//if (nds1[doc][old_topic] == 0)
		//	nds1[doc].erase(old_topic);
		//nwsum[topic] += 1;
		return 0;
	}
	inline int remove_from_topic( int word, int doc, int topic)
	{
		nw[word][topic] -= 1;
		nd_m[topic] -= 1;
		nds1[doc][rev_mapper[topic]].second -= 1;
		nwsum[topic] -= 1;
		
		//nw[word][topic] -= 1;
		//nds1[doc][topic] -= 1;
		//nwsum[topic] -= 1;
		return 0;
	}
	
	int sanity() const;
	int dump();
	int dump2();
};

#endif

