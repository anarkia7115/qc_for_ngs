#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
//#include "./BedFile.h"

using namespace std;

int main(int argc, char** argv) {

    string bedFilePath = "../data/S03723314_Covered_hg38.bed";
    //BedFile bf(bedFilePath);

    istringstream iss;
    ifstream fread;
    fread.open(bedFilePath);

    string chrName;
    string r1;
    string r2;
    //long r1 = 0;
    //long r2 = 0;

    string line;
    while(getline(fread, line)){
        cout << line << endl;
    }

    return 0;
}
