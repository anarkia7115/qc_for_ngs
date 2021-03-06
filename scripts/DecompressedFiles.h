#include <string>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>

using namespace std;

class DecompressedFiles {
	private:

		string 		folderPath;
		vector<string> 	fileNames;
		ifstream 	fread;

		bool isDouble = false;
		bool hasMoreLines = true;

		size_t 	curFileNo = 0;
		size_t	curLineNo = 0;
		string 	curLine;
		string	readsLine1;
		string	readsLine2;
		string	qualLine1;
		string	qualLine2;

		// seperator
		const char sep1 = 168;
		const char sep2 = 250;

		// get all file names in folder
		void getFileNames() {

			DIR *dir;
			struct dirent *ent;

			string fn;

			if ((dir = opendir (folderPath.c_str())) != NULL) {
				/* print all the files 
				 * and directories within directory */

				while ((ent = readdir (dir)) != NULL) {

					// add folder path
					fn = folderPath + "/" + ent->d_name;

					// match ".out" file
					if(fn.find("halvade") != string::npos) {

						fileNames.push_back(fn);
					}
				}
				closedir (dir);

			} else {
				/* could not open directory */
				perror ("Error in read Dir");
			}
			if (fileNames.size() == 0) {
				perror ("no files read!");
			}
		}

		// read next raw line
		void readNextLine() {

			// get next line
			string l;
			if(!getline(fread, l)) {
			// if file has no more lines

				// move to next file
				curFileNo++;
				// if has no more files
				if (curFileNo > fileNames.size() - 1) {
					hasMoreLines = false;
					return;
				}
				// else get next file
				else{
					fread.close();
					fread.open(fileNames[curFileNo]);
					readNextLine();
				}
			}
			// if readLine success 
			/*
			else
			{
				curLine.push_back(l);
				for (int i = 1; i < 4; i++) {
					getline(fread, l);
				}
				parseCurLine();
			}
			*/

		}

		// parse single end
		void parseSingle(size_t startPos, string &rl, string &ql) {
			// grab reads
			//cout << "curLine: " << curLine << endl;
			size_t readsBegin 	= curLine.find(sep1, startPos);
			size_t readsEnd	= curLine.find(sep1, readsBegin + 1);
			//cout << "readsBegin: " << readsBegin << endl;
			//cout << "readsEnd: " << readsEnd << endl;
			rl = curLine.substr(
					readsBegin + 1
					, readsEnd - readsBegin - 1);

			// grab qual
			size_t qualBegin	= curLine.find(sep1, readsEnd + 1);
			size_t qualEnd	= curLine.find(sep2, qualBegin + 1);

			// if \n not sep2
			if(qualEnd == string::npos) {

				qualEnd	= curLine.size();
			}

			ql = curLine.substr(
					qualBegin + 1
					, qualEnd - qualBegin - 1);
		}

		// parse single/double end
		void parseCurLine() {

			// is single end
			if (!isDouble) {

				parseSingle(0, this->readsLine1, this->qualLine1);
			}
			// is double end
			else {

				parseSingle(0, this->readsLine1, this->qualLine1);

				size_t sep2Pos 	= curLine.find(sep2, 0);

				parseSingle(sep2Pos, this->readsLine2, this->qualLine2);
			}
		}

	public:
		//Constructor
		DecompressedFiles(string folderPath) {

			// get folder path
			this->folderPath = folderPath;

			// get all file names in folder
			getFileNames();

			// open file handle
			fread.open(fileNames[curFileNo]);

			// parse first 8 line;
			//parseHead();

			// parse first line
			//readFirstLine();
		}

		// parse first 8 lines
		bool parseHead() {

			string l;
			vector<string> head;

			for (int i = 0; i < 8; i++) {

				if(getline(fread, l)){
					head.push_back(l);
				}
				else {
					// current file empty
					// next file
					fread.close();
					curFileNo++;
					fread.open(fileNames[curFileNo]);
					i--;
				}
			}

			// 0, 4
			string info1 = head.at(0);
			string info2 = head.at(4);
			string sub1 = info1.substr(0, info1.find(' '));
			string sub2 = info2.substr(0, info2.find(' '));
			
			//cout << sub1 << endl << sub2 << endl;

			//cout << head.at(0) << endl << head.at(4) << endl;

			if (sub1.compare(sub2) == 0) {

				return true;
			}
			else {

				return false;
			}
		}

		// parse head
		// depend is double or single
		void parseHead() {

			vector<string> head;

			string l;

			for (int i = 0; i < 8; i++) {
				if (getline(fread, l)) {

					head.push_back(l);
					//cout << l << endl;
				}
				else {

					//cout << "read failed" << endl;
					fread.close();
					curFileNo++;
					fread.open(fileNames[curFileNo]);
					i--;
				}
			}

			string info1 = head.at(0);
			string info2 = head.at(4);
			string sub1 = info1.substr(0, info1.find(' '));
			string sub2 = info2.substr(0, info2.find(' '));

			if (sub1.compare(sub2) == 0) {
				isDouble = true;
			}
			else {
				isDouble = false;
			}
		}

		// read first line of the file, 
		// only trigger once at the beginning
		void readFirstLine() {

			if(!getline(fread, curLine)) {
				hasMoreLines = false;
			}

			//if found sep2
			if(curLine.find(sep2) != string::npos) {

				// double
				this->isDouble = true;
			}

			// parse first line
			parseCurLine();
		}

		// print current line
		void printCurLine() {

			cout << curLine << endl;
		}

		// test
		void testFunc() {

			readFirstLine();
			cout << "1: " << readsLine1 << endl;
			cout << "2: " << qualLine1 << endl;
			cout << "3: " << readsLine2 << endl;
			cout << "4: " << qualLine2 << endl;
		}

		// get next formatted line
		bool nextLine(string &rtLine) {

			if(hasMoreLines == false) {

				return false;
			}

			// first 2 lines
			switch(curLineNo) {
				case 0:
					rtLine = readsLine1;
					break;
				case 1:
					rtLine = qualLine1;
					break;
				case 2:
					rtLine = readsLine2;
					break;
				case 3:
					rtLine = qualLine2;
					break;
				default :
					cerr << "curLineNo Exceed!" << endl;
					exit(-1);
			}

			curLineNo++;

			// all lines read
			// line number exceed
			// if is single
			if(isDouble == false) {

				if(curLineNo > 1) {
					curLineNo = 0;
					readNextLine();
				}
			}
			// double
			else {

				if(curLineNo > 3) {
					curLineNo = 0;
					readNextLine();
				}
			}

			return true;
		}

};
