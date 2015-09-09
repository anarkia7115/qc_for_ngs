#include "log.h"
#include "QualityCheck.h"

// for find_if
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};

struct MatchFileSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '.';
    }
};

class GZ 
{

public:
	// constructor
	GZ(string p1, string p2) 
	{
		setPath(p1);

		setWritePath(p2);

		initFH();
	}

	GZ(string s) 
	{
		setPath(s);
		parseName();
	}

	GZ() {}

	string getPath() 
	{
		return path;
	}

	// set path and init fileName
	void setPath(string s) 
	{
		this->path = s;
		parseName();
	}

	string getName() 
	{
		return fileName;
	}

	// init fileName through the path
	void parseName() 
	{
		fileName = basename(path);
	}

	// split file
	void splitFilesNoCompress(int lineNum) 
	{

		lineNum = lineNum / 4;
		// init first part
		int i = 0;
		string p = wpath + "/"  + getName() + "_" + to_string(i) + ".out";

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
				out.open(p);
			}

			writeLines(out);

			ln++;

			// one file generated
			if (ln >= lineNum) {
				ln = 0;

				// next part of file
				i++;
				out.close();

				// log file
				cout << "file: " << p << " done." << endl;
				p = wpath + "/"  + getName() + "_" + to_string(i) + ".out";

				// log date
				printDate();
			}
		}

		gzclose(gf1);
		out.close();
	}

	// split file
	void splitFiles(int lineNum) 
	{

		lineNum = lineNum / 4;
		// init first part
		int i = 0;
		string p = wpath + "/"  + getName() + "_" + to_string(i) + ".gz";

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
				p = wpath + "/" + getName() + "_" + to_string(i) + ".gz";

				// log date
				printDate();
			}
		}

		gzclose(gf1);
		gzclose(gfw);
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

private:
	string path;
	string fileName;

	gzFile gf1;

	QualityCheck qc;

	vector<string> lines1;

	string wpath;

	// seperator
	char sep1 = 168; 

	char eol = '\n'; 


	// write 4 lines to plain file
	void writeLines(ofstream& out)
	{
		// write
		std::ostream_iterator<std::string> output_iterator(out);
		std::copy(lines1.begin(), lines1.end(), output_iterator);
		/*
		for (auto l : lines1) 
		{
			out << l;
		}
		*/

		lines1.clear();
	}

	// write 4 lines
	void writeLines(gzFile& gfw) 
	{
		// write
		for (auto l : lines1) 
		{
			gzputs(gfw, l.c_str());
		}

		lines1.clear();
	}

	// read 4 lines
	bool readLines() 
	{

		char buf1[CHUNK];
		string s1;
		bool is_get;

		for (int i = 0; i < 4; i++) 
		{
			// read
			is_get = gzgets(gf1, buf1, CHUNK);

			chomp(buf1);

			// add end of line
			s1.assign(buf1);

			if (i == 1) {
				qc.parseReadsLine(s1);
			}

			// 4th line special
			if (i == 3) {
				qc.parseQualLine(s1);

				s1.push_back(eol);
			}
			else {
				s1.push_back(sep1);
			}

			// push lines
			lines1.push_back(s1);
		}
		return is_get;
	}

	// chomp
	void chomp(char* buf) 
	{
		// clear eol
		size_t ln = strlen(buf) - 1;
		if (buf[ln] == '\n')
			buf[ln] = '\0';

	}

	// basename of the file
	std::string
	basename( std::string const& pathname )
	{
		// return last part of the string
		// seprate by '/'
		string fileName( 
		std::find_if( pathname.rbegin(), pathname.rend(),
			      MatchPathSeparator() ).base(),
		pathname.end() );

		string fileNamePrefix( 
		fileName.begin(), 
		std::find_if( fileName.begin(), fileName.end(),
			      MatchFileSeparator() )
		 );
		return fileNamePrefix;
	}

	// init file handle
	void initFH() 
	{
		gf1 = gzopen(getPath().c_str(),  "r");
	}

	// print date
	void printDate() {

		time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime (&rawtime);
		printf ("Date: %s", asctime(timeinfo));
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

};

