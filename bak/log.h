#include "std.h"

extern string log_path;
extern string call_id;

void logMsg(const char* szString)
{
	cout << "log:" << szString << endl;

	ofstream logStrm;
	logStrm.open(log_path, std::ofstream::out | std::ofstream::app);
	cout << log_path << endl;
	logStrm << szString << endl;
	logStrm.close();
}

void logMsg(string s)
{
	cout << "log:" << s << endl;

	ofstream logStrm;
	logStrm.open(log_path, std::ofstream::out | std::ofstream::app);
	cout << log_path << endl;
	logStrm << s << endl;
	logStrm.close();
}

void initLogPath()
{
	if (log_path.size() == 0)
	{
		// no log_path
		log_path = "/tmp/gcbi_file_preproc" 
			+ to_string(::getpid())
			+ ".log"
			;
		cout 	<< "log path: " + log_path
			<< endl;
	}
	else if(call_id.size() == 0)
	{
		// has log_path, no call_id
		boost::filesystem::create_directories(
				boost::filesystem::path(log_path.c_str()));
		log_path += "/gcbi_file_preproc"
			+ to_string(::getpid())
			+ ".log"
			;
		cout 	<< "log path: " + log_path
			<< endl;
	}
	else {
		// has log_path, has call_id
		boost::filesystem::create_directories(boost::filesystem::path(log_path.c_str()));
		log_path += "/"
			+ call_id
			+ ".log"
			;
		cout 	<< "log path: " + log_path
			<< endl;
	}
}
