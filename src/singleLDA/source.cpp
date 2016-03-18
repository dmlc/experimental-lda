#include "lda.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\t./LDA --method xxx --testing-mode yyy [--alpha <real+>] [--beta <real+>] [--num-topics <int>] [--num-iterations <int>] [-odir <string>] [-savestep <int>] [-twords <int>] --training-file <string> [--testing-file <string>]" << std::endl;
	std::cout << "--method xxx :\n"
		<< "  xxx can be{ simpleLDA, sparseLDA, aliasLDA, FTreeLDA, forestLDA, lightLDA } corresponding to the six methods described above.\n"
		<< "--testing - mode yyy :\n"
		<< "  yyy can be{ nt, set, net } corresponding to the three modes described in readme.\n"
		<< "--alpha <real+> :\n"
		<< "  The value of alpha, hyper-parameter of LDA. The default value of alpha is 50 / num-topics.\n"
		<< "--beta <real+> :\n"
		<< "  The value of beta, hyper-parameter of LDA. The default value of beta is 0.1.\n"
		<< "--num-topics <int> :\n"
		<< "  The number of topics.The default value is 100.\n"
		<< "--num-iterations <int> :\n"
		<< "  The number of Gibbs sampling iterations. The default value is 2000.\n"
		<< "--output-model <string> :\n"
		<< "  The location where statistics and trained topics will be stored. The default location is the current working directory.\n"
		<< "--output-state-interval <int> :\n"
		<< "  The step(counted by the number of Gibbs sampling iterations) at which the LDA model is saved to hard disk. The default value is 200.\n"
		<< "--num-top-words <int> :\n"
		<< "  The number of most likely words for each topic. The default value is zero.\n"
		<< "--training-file <string> :\n"
		<< "  The input training data file. Data format described in readme under data folder.\n"
		<< "--testing-file <string> :\n"
		<< "  The input test data file. Data format described in readme under data folder." << std::endl;
}

int main(int argc, char ** argv) 
{
    model *lda = NULL;

    // initialize the model
    try {
        lda = model::init(argc, argv);
    } catch(std::exception& e) {
        show_help();
        throw;
    }

    // Train the model
    try {
        lda->train();
    } catch(std::exception& e) {
        std::cout << "Error: There exists a Bug in training part!" << std::endl;
        throw;
    }

    // Finally test the model	
    try {
        lda->test();
    } catch (std::exception& e) {
        std::cout << "Error: There exists a Bug in testing part!" << std::endl;
        throw;
    }

    delete lda;	

    system("pause");
    return 0;
}



