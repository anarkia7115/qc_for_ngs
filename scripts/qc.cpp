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

pair<double, double> calcQuantile(vector<int> v);


struct myclass {
	  bool operator()(int i,int j) { return (i<j);}
} myobject;


int main(int argc, char** argv) {

	//int colNum = 0;

	//if(argc > 1) {

	//	colNum = atoi(argv[1]);
	//}

	string p1 = "../data/WGC_20m_n1.fq.gz";
	string p2 = "../data/WGC_20m_n2.fq.gz";
	string outputPath = "../data/output";
	GZPair gp(p1, p2,outputPath);

	gp.testFunc();
	//vector<string> lines1 = gp.getLines1();

	QualityCheck qc = gp.returnQc();

	//qc.printMetrics();

	qc.sumUp();

	qc.genFile_position_quality_distribution();
	qc.genFile_position_base_composition();
	qc.genFile_qc_bqd_data();
	qc.genFile_qc_rqd_data();
	qc.genFile_qc_gcd_data();


	return 0;
}


