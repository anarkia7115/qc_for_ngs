#include "./DecompressedFiles.h"




// functions
vector<string> readDir(string folderPath);

int main(int argc, char** argv) {

	string folderPath = "../data/output";

	DecompressedFiles df(folderPath);

	df.testFunc();

	return 0;
}
