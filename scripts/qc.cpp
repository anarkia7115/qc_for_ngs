#include "./HalvadeFiles.h"
#include "./QualityCheck.h"
#include "./GuessEncoding.h"
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
    string bedFilePath;


    bool hasBed = false;

	struct stat info;

	// check arguments
	if (argc > 5 || argc < 4){
		cerr 	<< "Argument Number Error!\n"
			<< "Current Arg Number: " << argc << "\n"
			<< "Usage:\n"
			<< "\tqc <input_folder_path> <output_folder_path> <GCS_ID> [<Bed file>]"
			<< endl;
		exit(-1);
	}
	else if (argc == 4){
		inputPath = argv[1];
		outputPath = argv[2];
		gcsId = argv[3];
        hasBed = false;
	}
	else if (argc == 5){
		inputPath = argv[1];
		outputPath = argv[2];
		gcsId = argv[3];
        hasBed = true;
        bedFilePath = argv[4];
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

	// init file read classes
	HalvadeFiles df(inputPath);

	// current string to be overwrited
	string curLine;
    char lt;

    // guess encoding

    GuessEncoding ge;

    string encodingKey;

	while(df.nextLine(curLine, lt)) {
        if (lt == 'q') {
            if (ge.findEncoding(curLine)) {
                encodingKey = ge.getHitKey();
                break;
            }
        }
    }

    if (encodingKey.length() == 0) {
        cerr << "encoding not found!"<< endl;
        exit(1);
    }
    else {
        cout << "encoding: " << encodingKey << endl;
    }

    // rewind after guess
    df.rewind();

    // start qc process
    QualityCheck qc = hasBed ? 
        QualityCheck(outputPath, gcsId, encodingKey, bedFilePath) : 
        QualityCheck(outputPath, gcsId, encodingKey);

	while(df.nextLine(curLine, lt)) {

        //cout << "in qc loop" << endl;
        switch(lt) {
            case 'r':
                qc.parseReadsLine(curLine);
                break;

            case 'q':
                qc.parseQualLine(curLine);
                break;

            default:
                cerr << "Unknown Line Type." << endl;
                exit(-1);
        }
	}

	// generate data and files
	qc.sumUp();

	qc.genFiles();

	//qc.printMatrix();

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
