// basic file operations
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

int main () {
	ifstream myfile;
	myfile.open ("../../data/a");

	string line;
	int sen = 0;
	int qual = 0;

	vector<int> vq;
	map<int, long> mq;

	while(getline(myfile, line)) {
		if(sen == 3) {
			
			for(auto ch : line) {
				qual = ch - 33;

				vq.push_back(qual);
			}

			if(mq.find(qual) != mq.end()) {
				cout << "qual1: " <<  qual << endl;
				mq[qual]++;
			}
			else {
				cout << "qual2: " <<  qual << endl;
				mq[qual] = 1;
			}

			sen = 0;
		}
		else{
			sen++;
		}
	}

	//cout << "size: " << vq.size() << endl;

	//for (auto q : mq) {

	//	cout << q.first << ", " << q.second << endl;
	//}

	return 0;
}
