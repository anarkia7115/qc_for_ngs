#include "QualityCheck.h"
#include "./Matrix.h"

int main() {

	string matrixFilePath = "./input/matrix-default.txt";

	Matrix matrix(matrixFilePath);

	map<int, AggVal> mat = matrix.returnMat();

	// output result
	string ofp = "./output/qc_result";
	string name = "test";

	/*
	 * test
	cout << "mat size: " << mat.size() << endl;
	int lineNum = 2;
	cout << mat[lineNum].qualVals[0] << endl;
	cout << mat[lineNum].qualVals[1] << endl;
	cout << mat[lineNum].qualVals[2] << endl;
	cout << mat[lineNum].qualVals[3] << endl;
	cout << mat[lineNum].qualVals[4] << endl;
	*/

	QualityCheck qc(ofp, name);

	qc.setPerPosAggVal(mat);

	qc.genData_qc_pqd();
	qc.genFile_qc_pqd();

	return 0;
}
