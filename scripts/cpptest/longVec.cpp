#include <iostream>
#include <vector>

using namespace std;

int main(int argc, char** argv) {

	long line = 41250000;
	int genePerLine = 150;

	long result = line * genePerLine;

	cout << result << endl;

	vector<int>i(line*10,100);

	getchar();

	return 0;
}
