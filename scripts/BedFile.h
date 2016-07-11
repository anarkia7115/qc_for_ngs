using namespace std;

class BedFile {
    private:
        ifstream fread;

        string filePath;
        string chrName;

        long r1 = 0;
        long r2 = 0;
        long totalDiff = 0;

        bool hasMoreLine = true;


    public:
        //Constructor
        BedFile(string bedPath) {
            // get input file and open

            filePath = bedPath;

            fread.open(bedPath);
        }

        long regionDiff(string l) {
            
            istringstream iss(l);
            iss >> chrName >> r1 >> r2;
            return r2 - r1;
        }
        
        bool nextDiffRegion() {

            if (hasMoreLine) {
                // get next line, 
            }
            else {
                return false;
            }

            return rdiff;
        }
};
