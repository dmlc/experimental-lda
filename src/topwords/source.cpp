#include <iostream>
#include <map>
#include <string>

#include "topwords.h"

void show_help()
{
	std::cout << "Command line usage:" << std::endl;
	std::cout << "\t./topwords --dataset <string> --vocab-file <string>" << std::endl;
	std::cout << "--dataset <string> :\n"
		<< "  The location where phi matrix, param file is stored.\n"
		<< "--voab-file <string> :\n"
		<< "  The location where phi matrix, param file is stored." << std::endl;
}


int main(int argc, char ** argv)
{
	topwords tw = topwords();

	//initialize
	if (tw.init(argc, argv))
	{
		show_help();
		system("pause");
		return 1;
	}
	
	tw.interact();

	return 0;
}