#include <iostream>
#include <iterator>
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

pair<double, double> calcQuantile(vector<int> v);

int main(int argc, char** argv) {

	string p1 = "../data/sample_n1.gz";
	string p2 = "../data/sample_n2.gz";
	string outputPath = "../data/output";
	GZPair gp(p1, p2,outputPath);

	gp.testFunc();
	vector<string> lines1 = gp.getLines1();

	QualityCheck qc = gp.returnQc();

	//qc.printMetrics();

	//auto v = qc.getQualVec();

	//cout << "len: " << v.size() << endl;

	//for (auto i : v) {

	//	cout << i << endl;
	//}
	
	vector<int> v;
	for (int i = 100; i > 0; i--) {

		v.push_back(i);
	}

	pair<double, double> qs = calcQuantile(v);

	cout << qs.first << ", " << qs.second << endl;


	return 0;
}

pair<double, double> calcQuantile(vector<int> v) {

	int n = v.size();

	vector<double> probs = {0.25, 0.75};

	vector<double> index;

	for (auto d : probs) {

		index.push_back(1 + (n - 1) * d);
	}

	vector<int> lo;
	vector<int> hi;
	vector<int> set;

	int f = 0, c = 0;

	for (auto d : index) {
		f = floor(d);
		c = ceil(d);
		lo.push_back(f);
		hi.push_back(c);

		set.push_back(f);
		set.push_back(c);
	}

	unique(set.begin(), set.end());

	vector<double> qs;
	map<int, int> careEle;

	// x <- sort(x, partial = unique(c(lo, hi)))
	for (auto i : set) {

		nth_element(v.begin(), v.begin() + i - 1 , v.end());
		careEle[i] = v.at(i - 1);
	}

	// qs <- x[lo]
	for (auto i : lo) {
		qs.push_back(careEle[i]);
	}

	cout << "first qs" << endl;

	for(auto i : qs) {
		cout << i << endl;
	}

	cout << "end first qs" << endl;

	vector<int> diffIdx;

	for (size_t idx = 0; idx < index.size(); idx++) {

		double diff = index.at(idx) - double(lo.at(idx));

		if (diff > 0) {

			// qs[i] <- (1 - h) * qs[i] + h * x[hi[i]]
			qs.at(idx) = (1 - diff) * qs.at(idx) + diff * careEle[hi.at(idx)];
		}
	}

	cout << "second qs" << endl;

	for(auto i : qs) {
		cout << i << endl;
	}

	cout << "end second qs" << endl;

	double IQR = qs.at(1) - qs.at(0);

	double lowest = qs.at(0) - 1.5 * IQR;
	double highest = qs.at(1) + 1.5 * IQR;

	return make_pair(lowest, highest);
}
