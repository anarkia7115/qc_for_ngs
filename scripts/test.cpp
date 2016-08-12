#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <map>
#include "./BedFile.h"
#include "./GuessEncoding.h"

using namespace std;

int main(int argc, char** argv) {

    string base33Fq = "../data/fq/base33.fq";
    string base64Fq = "../data/fq/base64.fq";

    string guessEncodingLine;

    // guessEncodingLine Pos from 0 to 3
    int linePos = 0;

    ifstream fqStream(base64Fq);

    GuessEncoding ge;

    string encodingKey;

    // check file open
    if (fqStream.is_open()) {

        // read quality guessEncodingLine parse to GuessEncoding
        while (getline (fqStream, guessEncodingLine)) {

            // only read quality guessEncodingLine
            if (linePos != 3) {
                linePos++;
                continue;
            }
            else {
                linePos = 0;
            }

            // parse to ge
            if (ge.findEncoding(guessEncodingLine)) {
                encodingKey = ge.getHitKey();
                break;
            }
        }
    }
    else {
        // file open fail
        cout << "Unable to open file" << endl;
    }

    if (encodingKey.length() == 0) {
        cerr << "encoding not found!"<< endl;
        exit(1);
    }
    else {
        cout << "encoding: " << encodingKey << endl;
    }

    return 0;
}
