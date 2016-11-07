class GuessEncoding {

    // max quality parses
    const int MAX_ITER = 100;
    const int MIN_INT = 0;
    const int MAX_INT = 200;

    // current iterator
    int curIter = 0;

    // Phred bundaries
    map<string, pair<int, int> > phredRange;

    // quality min and max
    int qmin = MAX_INT;
    int qmax = MIN_INT;

    // hit key
    string hitKey;

    public :
        GuessEncoding() {
            phredRange["base33"] = make_pair(33, 83);
            phredRange["base64"] = make_pair(64, 105);
        }

        bool findEncoding(string qual_str) {

            //cout << qual_str << endl;
            // narrow range
            getQualRange(qual_str);

            // find encoding?
            int hitNum = getEncodingsInRange();
            if (hitNum == 1) {
                // stop read line
                return true;
            }
            else if (hitNum == 0) {
                cerr << "no hit" << endl;
                exit(1);
            }
            else{
                // not hit, continue reading
                return false;
            }
        }

        string getHitKey() {

            if (hitKey.length() == 0) {
                cerr << "get key before find it." << endl;
                exit(1);
            }
            else {
                return hitKey;
            }
        }

    private :

    void getQualRange(string qual_str){

        if (curIter > MAX_ITER) {
            cerr << "cannot decide Quality Encoding" << endl;
            exit(1);
        }

        curIter++;

        // refresh min and max
        for (int c : qual_str) {
            //cout << c << endl;

            if (c < qmin) {
                qmin = c;
            }
            if (c > qmax) {
                qmax = c;
            }
        }
	//cout << "qmin: " << qmin << endl;
	//cout << "qmax: " << qmax << endl;
    }

    int getEncodingsInRange() {

        // hit num
        int iHit = 0;
        string hitPhred;
        // loop all phred range
        int emin = 0;
        int emax = 0;
        string key;
        for (auto iter = phredRange.begin(); iter != phredRange.end(); iter++) {

            emin = iter->second.first;
            emax = iter->second.second;

            if (qmin >= emin && qmax <= emax) {
                // quality in current phred range
                iHit++;
                key = iter->first;
                cout << "hit " << key << endl;
            }
        }

        if (iHit == 1) {
            hitKey = key;
        }

        return iHit;
    }

};

