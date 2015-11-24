#include "./DecompressedFiles.h"
#include <string>
#include <fstream>

using namespace std;

int main(int argc, char** argv) {

	string fp = "../data/halvade_input";
	string fn = "../data/halvade_input/halvade-0-0";
	DecompressedFiles df(fp);
	bool isDouble = df.parseHead();

	if (isDouble) {
		cout << "double" << endl;
	}
	else {
		cout << "single" << endl;
	}

	//vector<string> vs;

	ifstream f(fn);

	string l;
	while(getline(f, l)) {

		cout << l << endl;
	}

	f.close();


	return 0;
}
