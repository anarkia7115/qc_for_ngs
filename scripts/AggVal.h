// aggregate values
class AggVal {

	int key = 0;
	long sum = 0;
	long count = 0;
	double mean = 0;

	double median = 0;

	int lowest = 9999;
	int highest = 0;

	double q1 = 0;
	double q3 = 0;

	vector<long> qualVals;

    vector<long> merge(vector<long> v1, vector<long> v2) {
        
        // check length
        if (v1.size() != v2.size()) {

            cerr << "Error: trying to merge 2 vectors that not having the same size." << endl;
            exit(1);
        }

        // add 2 qual vectors
        for (unsigned int i = 0; i < v1.size(); i++) {
            v1.at(i) += v2.at(i);
        }

        return v1;
    }

	// return qual
	int getPosVal(vector<long> v, long pos) {

		long sumIdx = 0;
		int qual = 0;

		// check lower bound
		if (pos < 0) {
			cerr << "quality pos exceed!" << endl;
			cerr << "position number < 0" << endl;
			exit(-1);
		}

		for(auto i : v) {

			sumIdx += i;
			//cout << "i: " << i << endl;
			//cout << "sumIdx: " << sumIdx << endl;
			//cout << "pos: " << pos << endl;
			if(sumIdx > pos) {
				//cout << "returned" << endl;
				//cout << qual << endl;

				return qual;
			}

			// this qual passed
			// next quality
			qual++;
		}

		cerr << "quality pos exceed!" << endl;
		cerr << "vector size: " << v.size() << endl;
		cerr << "finding pos: " << pos << endl;
		cerr << "max index: " << sumIdx << endl;
		exit(-1);
	}

	void calcMedian() {

		vector<long> v = this->qualVals;

		double median;
		size_t size = this->count;

		double a = 0, b = 0;

		if (size % 2 == 0) {

			//std::nth_element(v.begin()
			//		, v.begin() + size / 2 - 1, v.end());
			//a = v[size / 2 - 1];
			//b = v[size / 2];

			a = getPosVal(v, size / 2 - 1);
			b = getPosVal(v, size / 2);
			median = (a + b) / 2;
		}
		else {
			//std::nth_element(v.begin()
			//		, v.begin() + size / 2, v.end());
			//median = v[size / 2];
			median = getPosVal(v, size / 2);
		}

		this->median  = median;
	}

	void calcMean() {

		vector<long> v = this->qualVals;

		long sum = 0;
		int qual = 0;

		for(auto i : v) {

			sum += i * qual;

			// next qual
			qual++;
		}

        this->sum = sum;
		this->mean = double(sum) / double(this->count);
	}

	void calcQuartile() {

		vector<long> v = this->qualVals;

		long n = this->count;
		//cout << "size: " << n << endl;

		vector<double> probs = {0.25, 0.75};

		vector<double> index;

		for (auto d : probs) {

			index.push_back(1 + (n - 1) * d);
		}

		vector<long> lo;
		vector<long> hi;
		vector<long> set;

		long f = 0, c = 0;

		for (auto d : index) {
			f = floor(d);
			c = ceil(d);
			lo.push_back(f);
			hi.push_back(c);

			set.push_back(f);
			set.push_back(c);
		}

		unique(set.begin(), set.end());

		vector<double> qs;
		map<long, long> careEle;

		// x <- sort(x, partial = unique(c(lo, hi)))
		for (auto i : set) {

			//nth_element(v.begin(), v.begin() + i - 1 , v.end());
			//careEle[i] = v.at(i - 1);
			careEle[i] = getPosVal(v, i-1);
		}

		// qs <- x[lo]
		for (auto i : lo) {
			qs.push_back(careEle[i]);
		}

		vector<long> diffIdx;

		for (size_t idx = 0; idx < index.size(); idx++) {

			double diff = index.at(idx) - double(lo.at(idx));

			if (diff > 0) {

				// qs[i] <- (1 - h) * qs[i] + h * x[hi[i]]
				qs.at(idx) = 
				(1 - diff) * qs.at(idx) 
				+ diff * careEle[hi.at(idx)];
			}
		}

		double IQR = qs.at(1) - qs.at(0);

		//cout	<< "qs size: " << qs.size() << endl;
		//cout	<< "qs : " << qs[0] << ", " << qs[1] << endl;
		double lowest = qs.at(0) - 1.5 * IQR;
		double highest = qs.at(1) + 1.5 * IQR;
		//cout 	<< "lowest: "  << lowest  << "\t"
		//	<< "highest: " << highest << "\t"
		//	<< endl;

		// min and max
		//nth_element(v.begin(), v.begin(), v.end());
		//double lowestAbs = v.front();
		double lowestAbs = this->lowest;

		//nth_element(v.begin(), v.end() - 1, v.end());
		//double highestAbs = v.back();
		double highestAbs = this->highest;

		//cout 	<< "lowest: "  << lowestAbs  << "\t"
		//	<< "highest: " << highestAbs << "\t"
		//	<< endl;

		if(lowest < lowestAbs) {
			lowest = lowestAbs;
		}

		if(highest > highestAbs) {
			highest = highestAbs;
		}

		// q1 and q3
		this->q1 = qs.at(0);
		this->q3 = qs.at(1);

		// lowest and highest
		this->lowest = lowest;
		this->highest = highest;
	}

public:
    // Constructor
	AggVal() :qualVals(51,0){}

    void reduce() {
        this->calcMedian();
        this->calcMean();
        this->calcQuartile();
    }

    void merge(AggVal a2) {

        this->qualVals = this->merge(this->qualVals, a2.qualVals);
        this->count += a2.count;
    }

    void add(int qual) {
        this->qualVals.at(qual)++;
        count++;

		if (qual < lowest) {

			lowest = qual;
		}

		if (qual > highest) {

			highest = qual;
		}
    }
    
    void setKey(int k) {
        this->key = k;
    }
    int getKey() {
        return this->key;
    }
    double getMean() {
        return mean;
    }
    double getMedian() {
        return median;
    }
    double getQ1() {
        return q1;
    }
    double getQ3() {
        return q3;
    }
    double getLowest() {
        return lowest;
    }
    double getHighest() {
        return highest;
    }


} ;
