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

        long sumRegionDiff() {
            long diff = 0;
            long sum = 0;
            string line;
            //while (fread >> chrName >> r1 >> r2) {
            while (getline(fread, line)) {
                istringstream iss(line);
                iss >> chrName >> r1 >> r2;

                diff = r2 - r1;
                //cout << diff << endl;
                sum += diff;
            }
            return sum;
        }
        
};
