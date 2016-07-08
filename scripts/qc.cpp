#include "./HalvadeFiles.h"
#include "./QualityCheck.h"
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

//bool isReadsLine = true;
void pMkdir(const char *dir);

int main(int argc, char** argv) {

	// input and output path
	string inputPath; // = "../data/halvade_input/";
	string outputPath; // = "../data/output/qc_result/";
	string gcsId;
	struct stat info;

	// check arguments
	if (argc != 4){
		cerr 	<< "Argument Number Error!\n"
			<< "Current Arg Number: " << argc << "\n"
			<< "Usage:\n"
			<< "\tqc <input_folder_path> <output_folder_path> <GCS_ID>"
			<< endl;
		exit(-1);
	}
	else {
		inputPath = argv[1];
		outputPath = argv[2];
		gcsId = argv[3];
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
	HalvadeFiles df(inputPath);
	QualityCheck qc(outputPath, gcsId);

	//cout << inputPath << endl;

	// current string to be overwrited
	string curLine;
    char lt;

	while(df.nextLine(curLine, lt)) {

		// parse lines to qc
        cout << curLine << endl;

        switch(lt) {
            case 'r':
                qc.parseReadsLine(curLine);
                break;

            case 'q':
                qc.parseQualLine(curLine);
                break;

            default:
                cerr << "Unknown Line Type." << endl;
        }
	}

	// generate data and files
	qc.sumUp();

	qc.genFiles();

	qc.printMatrix();

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
