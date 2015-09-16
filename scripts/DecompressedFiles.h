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

		int 	curFileNo = 0;
		int	curLineNo = 1;
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
					if(fn.find(".out") != string::npos) {

						fileNames.push_back(fn);
					}
				}
				closedir (dir);

			} else {
				/* could not open directory */
				perror ("Error in read Dir");
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

			// parse first line
			readFirstLine();
		}

		// read first line of the file, 
		// only trigger once at the beginning
		void readFirstLine() {

			getline(fread, curLine);

			//if found sep2
			if(curLine.find(sep2) != string::npos) {

				// double
				this->isDouble = true;
			}
		}

		// print current line
		void printCurLine() {

			cout << curLine << endl;
		}

		// test
		void testFunc() {

			printCurLine();
		}

		// parse single end
		void parseSingle() {
			// grab reads
			int readsBegin 	= curLine.find(sep1, 0);
			int readsEnd	= curLine.find(sep1, readsBegin);
			readsLine1 = curLine.substr(
					readsBegin
					, readsEnd - readsBegin + 1);

			// grab qual
			int qualBegin	= curLine.find(sep1, readsEnd);
			int qualEnd	= curLine.find(sep1, qualBegin);
			qualLine1 = curLine.substr(
					qualBegin
					, qualEnd - qualBegin + 1);
		}

};
