#include <string>
#include <iostream>
#include <fstream>
#include <dirent.h>
#include <vector>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/filter/gzip.hpp>

using namespace std;

class HalvadeFiles {
	private:

		string 		folderPath;
		vector<string> 	fileNames;
		ifstream 	fread;
		std::ifstream 	gzfile;
		boost::iostreams::filtering_istream gzfread;

		bool isGz = false;
		bool isDouble = false;
		bool isReadsLine = true;
		bool hasMoreLines = true;

		size_t 	curFileNo = 0;
		//size_t	curLineNo = 0;
		vector<string> 	curLine;
		string readsLine;
		string  qualLine;

        // lineType in {r, q}
        char lineType = 'r';

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

					//cout << ent->d_name << endl;
					// match ".out" file
					if(string(ent->d_name).find("fq") != string::npos) {

						//cout << fn << endl;
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
			if(!this->getline(l)) {
			// if file has no more lines

				// move to next file
				curFileNo++;
				// if has no more files
				if (curFileNo > fileNames.size() - 1) {
					hasMoreLines = false;
                    cout << "parsed final file, exit" << endl;
					return;
				}
				// else get next file
				else{
					this->closeFile();

					string curFileName = fileNames[curFileNo];
					this->openFile(curFileName);
                    cout << "parsing " << fileNames[curFileNo] << endl;
					readNextLine();
				}
			}
			// if readLine success 
			else
			{
				curLine.push_back(l);
				for (int i = 1; i < 4; i++) {

					this->getline(l);
					curLine.push_back(l);
				}
				parseCurLine();
			}
		}

		// parse single end
		void parseSingle(size_t startPos, string &rl, string &ql) {
			// grab reads
			rl = curLine.at(1);

			// grab qual
			ql = curLine.at(3);
		}

		// parse single/double end
		void parseCurLine() {

			// is single end
			parseSingle(0, this->readsLine, this->qualLine);
			curLine.clear();
		}

		bool hasEnding (std::string const &fullString, std::string const &ending) {
		    if (fullString.length() >= ending.length()) {
			return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
		    } else {
			return false;
		    }
		}

	public:
		//Constructor
		HalvadeFiles(string folderPath) {

			// get folder path
			this->folderPath = folderPath;

			// get all file names in folder
			getFileNames();

			// open file handle
			string curFileName = fileNames[curFileNo];

			if (hasEnding(curFileName, ".gz")) {
				this->isGz = true;
			}
			else {
				this->isGz = false;
			}

			this->openFile(curFileName);
			cout << "parsing " << fileNames[curFileNo] << endl;

			// parse first 8 line;
			//parseHead();
		}

	istream& getline(string& l) {
		if (this->isGz == true) {
			cout << "curFileNo: " << this->curFileNo << endl;
			cout << "in it!" << endl;
			return std::getline(gzfread, l);
		}
		else {
			cout << "wrong in it!" << endl;
			return std::getline(fread, l);
		}
	}

	void openFile(string fileName) {
		if (this->isGz == true) {
			gzfile.open(fileName, std::ios_base::in | std::ios_base::binary);
			gzfread.push(boost::iostreams::gzip_decompressor());
			gzfread.push(gzfile);
		}
		else {
			fread.open(fileName);
		}
	}

	void closeFile() {
		if (this->isGz == true) {
			gzfile.close();
			gzfread.pop();
		}
		else {
			fread.close();
		}
	}

        void rewind() {
            curFileNo = 0;
	    this->closeFile();
	    this->openFile(fileNames[curFileNo]);
            cout << "parsing " << fileNames[curFileNo] << endl;
        }


		// parse head
		// depend is double or single
		void parseHead() {

			vector<string> head;

			string l;

			for (int i = 0; i < 8; i++) {
				if (this->getline(l)) {

					head.push_back(l);
					//cout << l << endl;
				}
				else {

					//cout << "read failed" << endl;
					curFileNo++;
					this->closeFile();
					this->openFile(fileNames[curFileNo]);
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

		// print current line
		void printCurLine() {

			cout << readsLine << endl;
			cout << qualLine << endl;
		}

		// test
		void testFunc() {
		}

		// get next formatted line
		bool nextLine(string &rtLine, char &lt) {

            // return current line type read
            lt = lineType;

            // read line of current line type
            // set line type pointing to next line
            switch(lineType) {
                case 'r':
                    readNextLine();
                    lineType = 'q';
                    rtLine = readsLine;
                    break;

                case 'q':
                    lineType = 'r';
                    rtLine = qualLine;
                    break;

                default:
                    perror ("Unknown Line Type.");
                    
            }

			if(hasMoreLines == false) {

				return false;
			}
			else {

				return true;
			}
		}

};
