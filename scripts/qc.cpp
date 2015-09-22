#include "./DecompressedFiles.h"
#include "./QualityCheck.h"
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

bool isReadsLine = true;
void pMkdir(const char *dir);

int main(int argc, char** argv) {

	// input and output path
	string inputPath  = "../data/output/decompressed_files/";
	string outputPath = "../data/output/qc_result/";
	struct stat info;

	// check arguments
	if (argc != 3){
		cerr 	<< "Argument Number Error!\n"
			<< "Current Arg Number: " << argc << "\n"
			<< "Usage:\n"
			<< "\tqc <input_folder_path> <output_folder_path>"
			<< endl;
		exit(-1);
	}
	else {
		inputPath = argv[1];
		outputPath = argv[2];
	}

	// check output folder status
	if( stat( outputPath.c_str(), &info ) != 0 ) {
		// no such folder
		cerr << "creating folder " << outputPath << " ..."<< endl;
		pMkdir(outputPath.c_str());
		cerr << "Done." << endl;
	}
	else if( info.st_mode & S_IFDIR ) {

		// folder exists
		cerr << "Dir: " << outputPath << " Exists." << endl;
	}
	else {

		// folder err
		cerr << outputPath << " is no directory. " << endl;
		exit(-1);
	}

	// init 2 classes
	DecompressedFiles df(inputPath);
	QualityCheck qc(outputPath);

	// current string to be overwrited
	string curLine;

	while(df.nextLine(curLine)) {

		// parse lines to qc
		if (isReadsLine) {

			qc.parseReadsLine(curLine);
		}
		else {

			qc.parseQualLine(curLine);
		}

		isReadsLine = !isReadsLine;
	}

	// generate data and files
	qc.sumUp();

	qc.genFiles();

	//qc.listQualVec();

	


	return 0;
}

void pMkdir(const char *dir) {
	char tmp[256];
	char *p = NULL;
	size_t len;

	snprintf(tmp, sizeof(tmp),"%s",dir);
	len = strlen(tmp);
	if(tmp[len - 1] == '/')
		tmp[len - 1] = 0;
	for(p = tmp + 1; *p; p++)
		if(*p == '/') {
			*p = 0;
			mkdir(tmp, S_IRWXU);
			*p = '/';
		}
	mkdir(tmp, S_IRWXU);
}
