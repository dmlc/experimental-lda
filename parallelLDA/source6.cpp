#include "lda.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\tlda -nt -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string>" << std::endl;
	std::cout << "\tlda -set -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string>" << std::endl;
	std::cout << "\tlda -net -alpha <double> -beta <double> -ntopics <int> -niters <int> -odir <string> -savestep <int> -twords <int> -dfile <string> -tfile <string>" << std::endl;
}

int main(int argc, char ** argv) 
{
	lightLDA lda;

	// parse arguments
    if (lda.init(argc, argv))
	{
		show_help();
		system("pause");
		return 1;
    }
    
    // parameter estimation
	if(lda.estimate())
	{
		std::cout << "There exists a Bug in estimate part!" << std::endl;
		return 1;
	}
    
	// do inference	
	if(lda.inference())
	{
		std::cout << "There exists a Bug in inference part!" << std::endl;
		return 1;;
    }

	//system("pause");
    
    return 0;
}



