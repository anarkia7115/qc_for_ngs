#include "GZ.h"

class GZPair 
{

public:
	// constructor
	GZPair(string p1, string p2, string p3) 
	{
		gz1.setPath(p1);
		gz2.setPath(p2);

		setWritePath(p3);

		gzPair = make_pair(gz1, gz2);
		initFH();
	}

	// print lines
	void printLines()
	{

		vector<string>::iterator iter1 = lines1.begin(); 
		vector<string>::iterator iter2 = lines2.begin(); 

		while(iter1 != lines1.end()) {

			cout << *iter1;
			iter1++;
		}

		while(iter2 != lines2.end()) {

			cout << *iter2;
			iter2++;
		}

		lines1.clear();
		lines2.clear();
		
	}

	vector<string> getLines1() {
		return lines1;
	}

	
	// set path
	void setPath(string p1, string p2) 
	{
		gz1.setPath(p1);
		gz2.setPath(p2);

		gzPair = make_pair(gz1, gz2);

	}

	// get pair
	pair<GZ, GZ> getPair() 
	{
		return this->gzPair;
	}

	QualityCheck returnQc() {
		return this->qc;
	}

	// test
	void testFunc(int i, bool isCompress) 
	{
		if(isCompress) {
		        splitFiles(i);
		}
		else {
		        splitFilesNoCompress(i);
		}
	}

	// split file
	void splitFilesNoCompress(int lineNum) 
	{

		lineNum = lineNum / 8;
		// init first part
		int i = 0;
		string p = wpath + "/"  + gz1.getName() + "_" + to_string(i) + ".out";

		// init line number
		int ln = 0;

		// open write file handle
		ofstream out;

		// loop until eof
		while(readLines()) 
		{

			// open new write file
			if(ln == 0) 
			{
				out.open(p, std::ofstream::out);
			}

			writeLines(out);

			ln++;

			// one file generated
			// close and open a new file
			if (ln >= lineNum) {
				ln = 0;

				// next part of file
				i++;
				out.close();

				// log file
				//cout << "file: " << p << " done." << endl;
				logMsg("file: " + p + " done.\n");
				p = wpath + "/"  + gz1.getName() + "_" + to_string(i) + ".out";

				// log date
				printDate();
			}
		}

		gzclose(gf1);
		gzclose(gf2);
		out.close();
	}

	// split file
	void splitFiles(int lineNum) 
	{

		lineNum = lineNum / 8;
		// init first part
		int i = 0;
		string p = wpath + "/" + gz1.getName() + "_" + to_string(i) + ".gz";

		// init line number
		int ln = 0;

		// open write file handle
		gzFile gfw;

		// loop until eof
		while(readLines()) 
		{

			// open new write file
			if(ln == 0) 
			{
				gfw = gzopen(p.c_str(), "w");
			}

			writeLines(gfw);

			ln++;

			// one file generated
			if (ln >= lineNum) {
				ln = 0;

				// next part of file
				i++;
				gzclose(gfw);

				// log file
				cout << "file: " << p << " done." << endl;
				p = wpath + "/" + gz1.getName() + "_" + to_string(i) + ".gz";

				// log date
				printDate();
			}
		}

		gzclose(gf1);
		gzclose(gf2);
		gzclose(gfw);
	}

private:
	GZ gz1;
	GZ gz2;

	QualityCheck qc;

	vector<string> lines1;
	vector<string> lines2;

	gzFile gf1, gf2;

	pair<GZ, GZ> gzPair;

	string wpath;

	// seperator
	char sep1 = '\n'; 
	char sep2 = '\n'; 

	char eol = '\n'; 

	// Methods

	// write 4 lines to plain file
	void writeLines(ofstream& out)
	{
		// write
		std::ostream_iterator<std::string> output_iterator(out);
		std::copy(lines1.begin(), lines1.end(), output_iterator);
		std::copy(lines2.begin(), lines2.end(), output_iterator);

		lines1.clear();
		lines2.clear();
	}

	// write 4 lines and compress
	void writeLines(gzFile& gfw) 
	{
		// write
		for (auto l : lines1) 
		{
			gzputs(gfw, l.c_str());
		}

		for (auto l : lines2) 
		{
			gzputs(gfw, l.c_str());
		}

		lines1.clear();
		lines2.clear();
	}

	// read 4 lines
	bool readLines() 
	{

		char buf1[CHUNK];
		char buf2[CHUNK];
		string s1;
		string s2;
		bool is_get;

		for (int i = 0; i < 4; i++) 
		{
			// read
			is_get = gzgets(gf1, buf1, CHUNK);
			gzgets(gf2, buf2, CHUNK);
			if (!is_get) {
				break;
			}

			chomp(buf1);
			chomp(buf2);

			// add end of line
			s1.assign(buf1);
			s2.assign(buf2);

			if (i == 1) {
				qc.parseReadsLine(s1);
				qc.parseReadsLine(s2);
			}

			// 4th line special
			if (i == 3) {
				//qc.parseQualLine(s1);
				//qc.parseQualLine(s2);

				s1.push_back(sep2);
				s2.push_back(eol);
			}
			else {
				s1.push_back(sep1);
				s2.push_back(sep1);
			}

			// push lines
			lines1.push_back(s1);
			lines2.push_back(s2);
		}
		return is_get;
	}

	void chomp(char* buf) {
		// clear eol
		size_t ln = strlen(buf) - 1;
		if (buf[ln] == '\n')
			buf[ln] = '\0';

	}

	// set write path
	void setWritePath(string p) 
	{
		if ( !boost::filesystem::exists( p.c_str() ) )
		{
			std::cout 
			  << "Folder does not exists.\n" 
			     "Create Folder: "
			     + p
			  << std::endl;
		   	boost::filesystem::create_directories(
				boost::filesystem::path(p.c_str()));
		}

		this->wpath = p;
	}

	// init file handle
	void initFH() 
	{
		gf1 = gzopen(gzPair.first.getPath().c_str(),  "r");
		gf2 = gzopen(gzPair.second.getPath().c_str(), "r");

		// set buff                            
		int setBuf1 = gzbuffer(gf1, CHUNK);  
		int setBuf2 = gzbuffer(gf2, CHUNK);  
						       
		if (setBuf1 || setBuf2)                
		{                                      
			cerr << "setBuf failed!\n";    
			exit(setBuf1 || setBuf2);      
		}                                      

	}

	// print date
	void printDate() {

		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		printf ("Date: %s", asctime(timeinfo));
	}
};

