#include <string>
#include <iostream>
#include <fstream>
#include "ParseNode.h"
#include "polylex.h"

using namespace std;

int currentLine = 0;
int globalErrorCount = 0;

int main(int argc, char *argv[]) {
	ifstream file;
	bool use_stdin = true;

	for( int i=1; i<argc; i++ ) {
		string arg = argv[i];

		if( !use_stdin ) {
			cout << "Too many file names" << endl;
			return 1;
		}
		use_stdin = false;

		file.open(arg);
		if( !file.is_open() ) {
			cout << "Could not open " << arg << endl;
			return 1;
		}
	}

	istream& in = use_stdin ? cin : file;

	ParseNode *program = Prog(in);

	if( program == 0 || globalErrorCount > 0 ) {
		cout << "Program failed!" << endl;
		return 0;
	}

    char c;
    //while (in.get(c)){ cout << c; }

	return 0;
}
