#include <iostream>
#include <string>

#include "translator.h"

int main(int argc, char **argv) {
	
	std::string input_file;
	std::string output_file;

	std::cout << "Input file name: ";
	std::cin >> input_file;

	std::cout << "Output file name: ";
	std::cin >> output_file;

	Translator *tr = new Translator();

	tr->Initialize( input_file, output_file );
	tr->Run();

	return 0;
}