#include "lda.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\t./LDA --method xxx --testing-mode yyy [--num-sampling-threads <int>] [--num-table-threads <int>] [--alpha <real+>] [--beta <real+>] [--num-topics <int>] [--num-iterations <int>] [-odir <string>] [-savestep <int>] [-twords <int>] --training-file <string> [--testing-file <string>]" << std::endl;
	std::cout << "--method xxx :\n"
		<< "  xxx can be{ simpleLDA, sparseLDA, aliasLDA, FTreeLDA, forestLDA, lightLDA } corresponding to the six methods described above.\n"
		<< "--testing - mode yyy :\n"
		<< "  yyy can be{ nt, set, net } corresponding to the three modes described in readme.\n"
		<< "--num-sampling-threads <int> :\n"
		<< "  The number of sampling threads. The default value is 4.\n"
		<< "--num-table-threads <int> :\n"
		<< "  The number of updater threads. The default value is 1.\n"
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
	if (!(lda = model::init(argc, argv)))
	{
		show_help();
		system("pause");
		return 1;
	}

	// Train the model
	if (lda->train())
	{
		std::cout << "Error: There exists a Bug in training part!" << std::endl;
		return 1;
	}

	// Finally test the model	
	if (lda->test())
	{
		std::cout << "Error: There exists a Bug in testing part!" << std::endl;
		return 1;
	}

	system("pause");
	return 0;
}



