#include "./DecompressedFiles.h"
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {

	string fp = "../data/halvade_input";
	//string fn = "../data/halvade_input/halvade-0-0";
	DecompressedFiles df(fp);
	//vector<string> vs;
	
	string l;
	while(df.nextLine(l)) {

		df.printCurLine();
	}

	return 0;
}
