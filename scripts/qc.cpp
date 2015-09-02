#include <iostream>
#include "GZPair.h"
#include "QualityCheck.h"

string call_id;
string sample;

string gz1_path;
string gz2_path;
string output_path;
string log_path;
string sample_name;
int row_num = 0;
bool is_compress;

int main(int argc, char** argv) {

	string p1 = "../data/sample_n1.gz";
	string p2 = "../data/sample_n2.gz";
	string outputPath = "../data/output";
	GZPair gp(p1, p2,outputPath);

	/*
	gp.testFunc();
	vector<string> lines1 = gp.getLines1();

	QualityCheck qc;

	cout << lines1.at(1) << endl;

	qc.parseReadsLine(lines1.at(1));
	*/

	vector<int> vi;
	vi.push_back(0);
	vector<int>::iterator iter = vi.begin();
	cout << *iter << endl;
	vi.push_back(0);
	iter++;
	cout << *iter << endl;
	//iter++;

	return 0;
}


