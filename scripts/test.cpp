#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "./BedFile.h"

using namespace std;

int main(int argc, char** argv) {

    string bedFilePath = "../data/S03723314_Covered_hg38.bed";
    BedFile bf(bedFilePath);

    long gnomeBaseTotal = bf.sumRegionDiff();
    cout << gnomeBaseTotal << endl;
    return 0;
}
