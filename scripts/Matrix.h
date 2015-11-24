// 1. read line by line
// 2. split words by \t
// 3. first word as key
#include "AggVal.h"
#include <fstream>
#include <sstream>

using namespace std;

class Matrix {
	private:

		string 		matFilePath;
		ifstream 	fread;
		stringstream	buffer;

		vector<string>	fstring;
		map<int, AggVal>	
				matrix;

		string keyDel 	= ":\t";
		char tokenDel 	= '\t';


		// file to vector string
		void readFile() {

			string curLine;
			while(getline(fread, curLine)) {

				fstring.push_back(curLine);
				makeMat(curLine);
			}
		}

		// construct Matrix
		void makeMat(string s) {

			// tokenize
			// find key
			size_t tokenBeg = 0;
			size_t tokenEnd = s.find(keyDel);

			size_t curKey = 
				stoi(s.substr(tokenBeg, tokenEnd));
			// find left tokens

			// first iter
			tokenBeg = tokenEnd + 2;
			tokenEnd = s.find(tokenDel, tokenBeg);

			// clear inited vector
			matrix[curKey].qualVals.clear();

			// do while
			do {
				matrix[curKey].qualVals.push_back(
					stol(
					s.substr(
					tokenBeg,
					tokenEnd)));

				tokenBeg = tokenEnd + 1;
				tokenEnd = s.find(tokenDel, tokenBeg);

			} while(tokenEnd != string::npos);
		}

		// calc lowest and highest
		// calc count
		void calcMetrics() {

			for(auto &pa : matrix) {

				auto &a = pa.second;

				// parse throught vector 
				// 1. record min/max 
				// non-zero index
				// 2. sum
				long sum = 0;
				int lowest = 9999;
				int highest = 0;
				int i = 0;

				for(auto l : a.qualVals) {

					sum += l;
					if (l != 0 && lowest == 9999) {

						lowest = i;
					}

					if (l != 0 && i > highest) {
						highest = i;
					}

					i++;
				}

				a.count = sum;
				a.lowest = lowest;
				a.highest = highest;
			}
		}


	public:

		// Constructor
		Matrix(string fp) {

			// read path
			this->matFilePath = fp;

			// open file
			fread.open(matFilePath);

			// read file line by line
			readFile();
			fread.close();

			calcMetrics();
		}

		// return matrix
		map<int, AggVal>	
			returnMat() {

			return matrix;
		}

};
