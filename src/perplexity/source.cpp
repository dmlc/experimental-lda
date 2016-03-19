#include "test_model.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\t./perplexity --dataset <string> --testing-file <string> [--num-iterations <int>] " << std::endl;
	std::cout << "--dataset <string> :\n"
		<< "  The location where phi matrix, param file is stored.\n"
		<< "--testing-file <string> :\n"
		<< "  The testing data file after converting to data-pack\n"
		<< "--num-iterations <int> :\n"
		<< "  The number of Gibbs sampling iterations for testing. The default value is 100." << std::endl;
}

int main(int argc, char ** argv) 
{
	// initialize the model
	test_model lda;
	if (lda.init(argc, argv))
	{
		show_help();
		system("pause");
		return 1;
	}
	
	// Test the model
	lda.test();

	system("pause");
	return 0;
}



