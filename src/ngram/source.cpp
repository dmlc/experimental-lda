/* 
 * File:   main.cpp
 * Author: Manzil
 *
 * Created on February 28, 2016, 11:12 PM
 */

#include "model.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\thdp -nt -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string>" << std::endl;
	std::cout << "\thdp -set -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string>" << std::endl;
	std::cout << "\thdp -net -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string> -tfile <string>" << std::endl;
}

/*
 * 
 */
int main(int argc, char** argv) {
    
    model *ngram = NULL;
    
    // initialize the model
    try {
        ngram = model::init(argc, argv);
    } catch(std::exception& e) {
        show_help();
        throw;
    }
            
    // Train the model
    try {
        ngram->train();
    } catch(std::exception& e) {
        std::cout << "Error: There exists a Bug in training part!" << std::endl;
	throw;
    }

    // Finally test the model	
    try {
        ngram->test();
    } catch (std::exception& e) {
    	std::cout << "Error: There exists a Bug in testing part!" << std::endl;
	throw;
    }

    system("pause");
    return 0;
}


