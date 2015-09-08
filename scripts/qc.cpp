#include <iostream>
#include "GZPair.h"

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

	gp.testFunc();
	vector<string> lines1 = gp.getLines1();

	QualityCheck qc = gp.returnQc();

	//qc.printMetrics();

	auto v = qc.getQualVec();

	//cout << "len: " << v.size() << endl;

	for (auto i : v) {

		cout << i << endl;
	}

	return 0;
}

pair<double, double> calcQuantile(vector<int> v) {

	n = v.size();


}
