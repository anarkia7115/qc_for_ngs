#include <map>
#include <algorithm>

using namespace std;
class QualityCheck {

	private:

	// parameters
	string outputFolderPath;

	double rawLen 	= 0;
	long rawReads 	= 0;
	long rawBases 	= 0;
	long rawGcBases	= 0;

	string name = "default";

	long effectiveReads = 0;
	long effectiveBases = 0;

	long lineGcBases	= 0;
	long lineNBases  	= 0;
	long lineBases		= 0;

	bool isReadsOk 	= false;
	long excessNReads 	= 0;
	long passedReads 	= 0;
	double effectiveRate 	= 0;

	long lineQualSum 	= 0;
	size_t genSen = 0;

	long q30Bases	= 0;
	long q20Bases	= 0;
	long q10Bases 	= 0;
	long q4Bases	= 0;
	long lineLowQualBases = 0;

	double genomeCoverage = 0;
	long meanDepth = 0;
	double mappedRate = 0;

	map<char, vector<long>>	perPosReadsCounts;
	map<char, vector<bool>>	tmp_perPosReadsCounts;

	map<int, long, less<int>>	perGcReadsCounts;
	map<int, long>	perNsReadsCounts;
	map<int, long>	perBaseReadsCounts;
	map<int, long>	perQualBasesCounts;

	vector<char> geneType = {'G', 'C', 'A', 'T', 'N'};

	// qual
	vector<long> perPosQualCounts;
	vector<long> tmp_perPosQualCounts;

	vector<long> perQualCounts;
	map<int, long, std::less<int>> perQualReadsCounts;

	int lineMeanQual 	= 0;
	bool isQualOk		= false;
	float discard 		= 0.5;
	long lowQualReads 	= 0;
	long passedQuals 	= 0;

	// aggregate values
	struct AggVal {

		int key = 0;
		long sum = 0;
		long count = 0;
		double mean = 0;

		double median = 0;

		double lowest = 9999;
		double highest = 0;

		int q1 = 0;
		int q3 = 0;

		vector<int> qualVals;

	} ;
	int curPos = 0;

	map<int, AggVal> perPosAggVal;

	//map<char, map<int, long> > positionBaseComposition;
	//map<pair<char, int>, double> positionBaseComposition;
	map<int, map<char, double>> positionBaseComposition;
	map<int, int,  less<int>> qc_bqd_data;
	map<int, long, less<int>> qc_rqd_data;
	map<int, long, less<int>> qc_gcd_data;
	

	void addTmpPosReads(char targetKey) {

		char key;

		//cout << "perPosReadsCounts size: " << perPosReadsCounts.size() << endl;
		//cout << "key: " << endl;
		for(	auto k : geneType ) {

			key = k;

			// pos ++
			if(key == targetKey){

				tmp_perPosReadsCounts[key].push_back(true);
			}
			else{
				tmp_perPosReadsCounts[key].push_back(false);
			}
		}

	}

/*
	void flushTmpReads() {
		//int lenTot = 0;
		//int lenTmp = 0;

		char key;

		for(	auto k : geneType) {

			key = k;

			auto totIter = perPosReadsCounts[key].begin();
			auto tmpIter = tmp_perPosReadsCounts[key].begin();

			// 50% time on the for loop
			for (   ; totIter != perPosReadsCounts[key].end() 
				&& tmpIter != tmp_perPosReadsCounts[key].end()
				; totIter++) {

				// if gene is on this pos
				if(*tmpIter){
					(*totIter)++;
				}

				tmpIter++;
			}

			// push back new to total, if tmp is not over
			for (; tmpIter != tmp_perPosReadsCounts[key].end(); tmpIter++) {
				if(*tmpIter){
					perPosReadsCounts[key].push_back(1);
				}
				else {
					perPosReadsCounts[key].push_back(0);
				}
			}
		}

		//cout << "tmp size: " << tmp_perPosReadsCounts.size() << endl;
		tmp_perPosReadsCounts.clear();
	}
*/

	void addTmpPosQual(int qualVal) {

		// pos ++
		tmp_perPosQualCounts.push_back(qualVal);
	}

	void flushTmpQual() {

		auto totIter = perPosQualCounts.begin();
		auto tmpIter = tmp_perPosQualCounts.begin();

		for (   ; totIter != perPosQualCounts.end() 
			&& tmpIter != tmp_perPosQualCounts.end()
			; totIter++) {

			// if gene is on this pos
			*totIter += *tmpIter;

			tmpIter++;
		}

		// tmp is longer than tot
		// push back new to total, if tmp is not over
		for (; tmpIter != tmp_perPosQualCounts.end(); tmpIter++) {

			perPosQualCounts.push_back(*tmpIter);
		}

		tmp_perPosQualCounts.clear();
	}

	void addPerQualCounts(int qual) {

/*
		if(perQualCounts.find(qual) != perQualCounts.end()) {

			// add to orig record
			perQualCounts[qual]++;
		}
		else {

			// assign to the record directly
			perQualCounts[qual] = 1;
		}
*/
		perQualCounts[qual]++;
		
	}

	void addPerQualReadsCount() {

		// if record exists
		if(perQualReadsCounts.find(lineMeanQual) 
				!= perQualReadsCounts.end()) {
		
			perQualReadsCounts[lineMeanQual] ++;
			//cout << lineMeanQual << ": " 
			//<< perQualReadsCounts[lineMeanQual] << endl;
		}
		else {

			perQualReadsCounts[lineMeanQual] = 1;
			//cout << lineMeanQual << ": " 
			//<< perQualReadsCounts[lineMeanQual] << endl;
		}
	}

	void addPerGcReadsCounts() {
		if(perGcReadsCounts.find(lineGcBases) 
				!= perGcReadsCounts.end()) {

			// add to orig record
			perGcReadsCounts[lineGcBases] ++;
		}
		else {

			// assign to the record directly
			perGcReadsCounts[lineGcBases] = 1;
		}
	}

	void addPerNsReadsCounts() {
		if(perNsReadsCounts.find(lineNBases) != perNsReadsCounts.end()) {

			// add to orig record
			perNsReadsCounts[lineNBases] ++;
		}
		else {

			// assign to the record directly
			perNsReadsCounts[lineNBases] = 1;
		}
	}

	void addPerBaseReadsCounts() {
		if(perBaseReadsCounts.find(lineBases) != perBaseReadsCounts.end()) {

			// add to orig record
			perBaseReadsCounts[lineBases] ++;
		}
		else {

			// assign to the record directly
			perBaseReadsCounts[lineBases] = 1;
		}
	}

	void addPerPosAggVal(int qual) {
		// push value to the right position
		switch(curPos) {
			case 0:
				//cout << "in case 0: " << endl;
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			case 8:
			case 149:
				perPosAggVal[curPos].qualVals.push_back(qual);break;
			case 9:
			case 10:
			case 11:
			case 12:
			case 13:
				perPosAggVal[9].qualVals.push_back(qual);break;
			case 14:
			case 15:
			case 16:
			case 17:
			case 18:
				perPosAggVal[14].qualVals.push_back(qual);break;
			case 19:
			case 20:
			case 21:
			case 22:
			case 23:
				perPosAggVal[19].qualVals.push_back(qual);break;
			case 24:
			case 25:
			case 26:
			case 27:
			case 28:
				perPosAggVal[24].qualVals.push_back(qual);break;
			case 29:
			case 30:
			case 31:
			case 32:
			case 33:
				perPosAggVal[29].qualVals.push_back(qual);break;
			case 34:
			case 35:
			case 36:
			case 37:
			case 38:
				perPosAggVal[34].qualVals.push_back(qual);break;
			case 39:
			case 40:
			case 41:
			case 42:
			case 43:
				perPosAggVal[39].qualVals.push_back(qual);break;
			case 44:
			case 45:
			case 46:
			case 47:
			case 48:
				perPosAggVal[44].qualVals.push_back(qual);break;
			case 49:
			case 50:
			case 51:
			case 52:
			case 53:
				perPosAggVal[49].qualVals.push_back(qual);break;
			case 54:
			case 55:
			case 56:
			case 57:
			case 58:
				perPosAggVal[54].qualVals.push_back(qual);break;
			case 59:
			case 60:
			case 61:
			case 62:
			case 63:
				perPosAggVal[59].qualVals.push_back(qual);break;
			case 64:
			case 65:
			case 66:
			case 67:
			case 68:
				perPosAggVal[64].qualVals.push_back(qual);break;
			case 69:
			case 70:
			case 71:
			case 72:
			case 73:
				perPosAggVal[69].qualVals.push_back(qual);break;
			case 74:
			case 75:
			case 76:
			case 77:
			case 78:
				perPosAggVal[74].qualVals.push_back(qual);break;
			case 79:
			case 80:
			case 81:
			case 82:
			case 83:
				perPosAggVal[79].qualVals.push_back(qual);break;
			case 84:
			case 85:
			case 86:
			case 87:
			case 88:
				perPosAggVal[84].qualVals.push_back(qual);break;
			case 89:
			case 90:
			case 91:
			case 92:
			case 93:
				perPosAggVal[89].qualVals.push_back(qual);break;
			case 94:
			case 95:
			case 96:
			case 97:
			case 98:
				perPosAggVal[94].qualVals.push_back(qual);break;
			case 99:
			case 100:
			case 101:
			case 102:
			case 103:
				perPosAggVal[99].qualVals.push_back(qual);break;
			case 104:
			case 105:
			case 106:
			case 107:
			case 108:
				perPosAggVal[104].qualVals.push_back(qual);break;
			case 109:
			case 110:
			case 111:
			case 112:
			case 113:
				perPosAggVal[109].qualVals.push_back(qual);break;
			case 114:
			case 115:
			case 116:
			case 117:
			case 118:
				perPosAggVal[114].qualVals.push_back(qual);break;
			case 119:
			case 120:
			case 121:
			case 122:
			case 123:
				perPosAggVal[119].qualVals.push_back(qual);break;
			case 124:
			case 125:
			case 126:
			case 127:
			case 128:
				perPosAggVal[124].qualVals.push_back(qual);break;
			case 129:
			case 130:
			case 131:
			case 132:
			case 133:
				perPosAggVal[129].qualVals.push_back(qual);break;
			case 134:
			case 135:
			case 136:
			case 137:
			case 138:
				perPosAggVal[134].qualVals.push_back(qual);break;
			case 139:
			case 140:
			case 141:
			case 142:
			case 143:
				perPosAggVal[139].qualVals.push_back(qual);break;
			case 144:
			case 145:
			case 146:
			case 147:
			case 148:
				perPosAggVal[144].qualVals.push_back(qual);break;
			default:
				cerr << "" << endl;
				exit(-1);
		}

	}

	void checkLowQual(size_t l) {


		float part = float(lineLowQualBases) / float(l);

		if (part > discard) {

			isQualOk = false;
			lowQualReads++;
		}
		else {

			isQualOk = true;
			passedQuals++;
		}

	}

	void checkNStatus() {

		if (lineNBases > 3) {

			// too many N
			isReadsOk = false;
			excessNReads++;
		}
		else {
			isReadsOk = true;
			passedReads++;
		}
	}

	void calcMedian(AggVal& av) {

		vector<int> v = av.qualVals;

		double median;
		size_t size = v.size();

		int a = 0, b = 0;

		if (size % 2 == 0) {
			std::nth_element(v.begin()
					, v.begin() + size / 2 - 1, v.end());
			a = v[size / 2 - 1];
			b = v[size / 2];

			median = (a + b) / 2;
		}
		else {
			std::nth_element(v.begin()
					, v.begin() + size / 2, v.end());
			median = v[size / 2];
		}

		av.median  = median;
	}

	void calcMean(AggVal& av) {

		vector<int> v = av.qualVals;

		long sum = 0;

		for(auto i : v) {

			sum += i;
		}

		av.mean = double(sum) / double(v.size());
	}

	void calcQuartile(AggVal& av) {

		vector<int> v = av.qualVals;

		int n = v.size();

		vector<double> probs = {0.25, 0.75};

		vector<double> index;

		for (auto d : probs) {

			index.push_back(1 + (n - 1) * d);
		}

		vector<int> lo;
		vector<int> hi;
		vector<int> set;

		int f = 0, c = 0;

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
		map<int, int> careEle;

		// x <- sort(x, partial = unique(c(lo, hi)))
		for (auto i : set) {

			nth_element(v.begin(), v.begin() + i - 1 , v.end());
			careEle[i] = v.at(i - 1);
		}

		// qs <- x[lo]
		for (auto i : lo) {
			qs.push_back(careEle[i]);
		}

		vector<int> diffIdx;

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

		double lowest = qs.at(0) - 1.5 * IQR;
		double highest = qs.at(1) + 1.5 * IQR;

		// min and max
		nth_element(v.begin(), v.begin(), v.end());
		double lowestAbs = v.front();

		nth_element(v.begin(), v.end() - 1, v.end());
		double highestAbs = v.back();

		if(lowest < lowestAbs) {
			lowest = lowestAbs;
		}

		if(highest > highestAbs) {
			highest = highestAbs;
		}

		// q1 and q3
		av.q1 = qs.at(0);
		av.q3 = qs.at(1);

		// lowest and highest
		av.lowest = lowest;
		av.highest = highest;
	}

	public:

	// Constructor
	QualityCheck(string op) : perQualCounts(100, 0) {

		this->outputFolderPath = op;
	}

	vector<int> getQualVec(int i) {
		
		auto iter = next(perPosAggVal.begin(),i);
		cout << "key: " << iter->first << endl;
		return iter->second.qualVals;
	}

	void printMetrics() {

		cout << "rawReads:\t" << rawReads << endl;
		cout << "rawBases:\t" << rawBases << endl;
		cout << "passedReads:\t" << passedReads << endl;
		cout << "effectiveReads:\t" << effectiveReads << endl;
		cout << "effectiveBases:\t" << effectiveBases << endl;
		cout << "rawGcBases:\t" << rawGcBases << endl;
		cout << "excessNReads:\t" << excessNReads << endl;

	}

	void printPosData(char key) {

		//cout << "perPosReadsCounts[key] length: " << perPosReadsCounts[key].size() << endl;
		for (auto i : perPosReadsCounts[key]) {
			cout << i << endl;
		}

	}

	void printQualDistribution() {
	}

	bool parseGene(char gene) {
		lineBases++;
		size_t s = perPosReadsCounts[gene].size();

		switch(gene){

			//cerr << "size: " << s << endl;

			case 'G':
				lineGcBases++;
				// count++
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						perPosReadsCounts['G'].push_back(0);
					}
				}
				perPosReadsCounts['G'].at(genSen)++; 
				//addTmpPosReads('G');
				break;
			case 'C':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						perPosReadsCounts['C'].push_back(0);
					}
				}
				lineGcBases++;
				perPosReadsCounts['C'].at(genSen)++; 
				//addTmpPosReads('C');
				break;
			case 'A':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						perPosReadsCounts['A'].push_back(0);
					}
				}
				perPosReadsCounts['A'].at(genSen)++; 
				//addTmpPosReads('A');
				break;
			case 'T':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						perPosReadsCounts['T'].push_back(0);
					}
				}
				perPosReadsCounts['T'].at(genSen)++; 
				//addTmpPosReads('T');
				break;
			case 'N':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						perPosReadsCounts['N'].push_back(1);
					}
				}
				lineNBases++;
				perPosReadsCounts['N'].at(genSen)++; 
				//addTmpPosReads('N');
				break;
			default:
				cerr 	<< "Warning:\n\t" 
					<< gene
					<< "\tunknown base?" << endl;
				return false;
				break;	
		}

		genSen++;
		return true;
	}

	void parseQual(char qualCh) {


		// char to int
		int qual = qualCh - 33;

		// lowest and highest
		addPerPosAggVal(qual);

		addTmpPosQual(qual);
		addPerQualCounts(qual);

		lineQualSum += qual;

		//if(qual >= 30) {
		//	q30Bases++;
		//	q20Bases++;
		//	q10Bases++;
		//	q4Bases++;
		//}
		//else if(qual >= 20) {
		//	q20Bases++;
		//	q10Bases++;
		//	q4Bases++;
		//}
		//else if(qual >= 10) {
		//	q10Bases++;
		//	q4Bases++;
		//	lineLowQualBases++;
		//}
		//else if(qual >= 4) {
		//	q4Bases++;
		//	lineLowQualBases++;
		//}
		//else {
		//	lineLowQualBases++;
		//}

		curPos++;
	}

	void parseReadsLine(string line) {

		// init line metrics
		lineGcBases = 0;
		lineNBases  = 0;
		lineBases = 0;
		genSen = 0;

		// first iter
		auto siter = line.begin();
		parseGene(*siter);

		// parse gene by gene
		for(siter++; siter!= line.end(); siter++) {

			// shift pos
			//cout << "current gene before parse: " << *siter << endl;
			if(!parseGene(*siter)) {
				cout << line << endl;
				exit(-1);
			}
		}

		// flush tmp vector
		// 60% of the time
		//flushTmpReads();
		//tmp_perPosReadsCounts.clear();

		// add line metrics to total
		rawReads++;
		rawBases	+= lineBases;
		rawGcBases	+= lineGcBases;

		// add occuring frequency
		addPerGcReadsCounts();
		addPerNsReadsCounts();
		addPerBaseReadsCounts();

		checkNStatus();
	}

	void parseQualLine(string line) {
		//cout << "in parseQualLine." << endl;

		// init
		lineQualSum  = 0;
		lineMeanQual = 0;
		lineLowQualBases = 0;
		curPos = 0;

		// first iter
		auto siter = line.begin();
		parseQual(*siter);

		// parse gene by gene
		for(siter++; siter!= line.end(); siter++) {

			// shift pos
			//cout 
			//<< "current gene before parse: " 
			//<< *siter << endl;
			parseQual(*siter);
		}

		// flush tmp
		flushTmpQual();

		lineMeanQual 
			= round(float(lineQualSum) 
			  / float(line.size()));

		addPerQualReadsCount();
		checkLowQual(line.size());
	}

	void sumUp() {

		genData_qc_pqd();
		genData_qc_pbc();
		genData_qc_bqd();
		genData_qc_rqd();
		genData_qc_gcd();
	}

	void genFiles() {
		genFile_qc_pqd();
		genFile_qc_pbc();
		genFile_qc_bqd();
		genFile_qc_rqd();
		genFile_qc_gcd();
	}

	void genData_qc_pqd() {

		for(auto &av : perPosAggVal) {

			// key
			av.second.key = av.first;
			// calculate median
			calcMedian(av.second);
			// calculate mean
			calcMean(av.second);
			// calculate lowest, highest, q1 and q3
			calcQuartile(av.second);
		}
	}

	void genFile_qc_pqd() {

		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_pqd_data-" + name + ".txt");

		ofile	<< "graph_title" 	<< "\t" << "quality scores across all bases"
			<< endl
			<< "minY" 		<< "\t" << 0
			<< endl
			<< "maxY" 		<< "\t" << 41
			<< endl
			<< "labelX" 		<< "\t" << "position in read (bp)" 
			<< "\n"
			<< endl
			<< "Xlabels\t" 
			<< "Mean\t"
			<< "Median\t"
			<< "Lower Quartile\t"
			<< "Upper Quartile\t"
			<< "lowest\t"
			<< "highest"
			<< endl;

		for (auto av: perPosAggVal) {

			ofile 	<< av.second.key +1   << "\t"
				<< av.second.mean     << "\t"
				<< av.second.median   << "\t"
				<< av.second.q1       << "\t"
				<< av.second.q3       << "\t"
				<< av.second.lowest   << "\t"
				<< av.second.highest  
				<< endl;
		}

		ofile.close();
	}

	map<int, map<char, double>> getPositionBaseComposition() {

		return positionBaseComposition;
	}

	vector<long> getPerQualCounts() {

		return perQualCounts;
	}

	void genData_qc_pbc() {

		//map<char, vector<long>>	perPosReadsCounts;
		map<char, map<int, long>> v1;
		//map<pair<char, int>, double> positionBaseComposition
		//map<int, map<char, double>> positionBaseComposition

		char gene;
		int  key;
		vector<long> v;

		int sen = 0;

		for (auto g : perPosReadsCounts) {

			gene = g.first;
			v   = g.second;

			for (int i = 0; i < 9; i++) {

				v1[gene].insert(
						make_pair(i, v[i]));
			}

			for (int i = 9; i < 149; i++) {

				if(sen == 0) {
					key = i;
					sen++;
					v1[gene].insert(
						make_pair(key, 0));
				}
				else if(sen == 4){
					sen = 0;
				}
				else {
					sen++;
				}

				v1[gene][key] 
					+= v[i];
			}

			v1[gene].insert(
					make_pair(149, v[149]));

		}

		map<int, map<int, long>> v2;
		for (auto m : v1) {

			gene = m.first;

			for (auto l : m.second) {

				key = l.first;

				v2[key][gene] = l.second;
			}
		}

		long sum = 0;

		for (auto m : v2) {

			sum = 0;
			key = m.first;

			for (auto l : m.second) {

				sum += l.second;
			}

			for (auto l : m.second) {
				gene = l.first;

				positionBaseComposition[key][gene] 
					= (double) l.second
					/ (double) sum
					* 100;
			}
		}
	}

	void genFile_qc_pbc() {

		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_pbc_data-" + name + ".txt");

		ofile 	<< "graph_title\tbase percentage composition along reads"
			<< endl
			<< "allX\t5"
			<< endl
			<< "allY\t38"
			<< endl
			<< "labelX\tposition along reads"
			<< endl
			<< "labelY\tpercentage"
			<< endl
			<< "legend\tACGTN"
			<< "\n"
			<< endl
			<< "Base\t"
			<< "G\t"
			<< "A\t"
			<< "T\t"
			<< "C\t"
			<< "N"
			<< endl;

		auto g = positionBaseComposition;

		map<int, map<char, double>>::iterator it;

		for (int i = 0; i < 150; i++) {

			it = g.find(i);
			if(it != g.end()) {

				ofile	<< i +1 << "\t"
					<< it->second['G'] << "\t"
					<< it->second['A'] << "\t"
					<< it->second['T'] << "\t"
					<< it->second['C'] << "\t"
					<< it->second['N'] << "\t"
					<< endl;
			}
		}

	}

	void genData_qc_bqd() {

		long sum = 0;
		long left = 0;
		//map<int, long> perQualCounts;
		//map<int, long> perQualReadsCounts;
		vector<int> quals;
		//map<int, int> qc_bqd_data;

		for(auto l : perQualCounts) {

			sum += l;
		}

		left = sum;

		int i = 0;
		for(auto l : perQualCounts) {

			qc_bqd_data[i] = (double(left) / double(sum)) * 100;

			//cout << qc_bqd_data[l.first] << endl;
			//cout << l.first << " fraq: " << double(left)/double(sum) << endl;
			left -= l;
			i++;
		}
	}

	void genFile_qc_bqd() {

		//map<int, int> qc_bqd_data;

		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_bqd_data-" + name + ".txt");

		ofile 	<< "graph_title\taccumulated base quality over all bases\n"
			<< "allX\t" << qc_bqd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tbase quality\n"
			<< "labelY\tpercentage\n"
			<< endl

			<< "Quality\t"
			<< "Count"
			<< endl;

		for (auto i : qc_bqd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
	}

	void genData_qc_rqd() {
		qc_rqd_data = perQualReadsCounts;
	}

	void genFile_qc_rqd() {
		
		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_rqd_data-" + name + ".txt");

		ofile 	<< "graph_title\tquality score distribution over all sequences\n"
			<< "allX\t" << qc_rqd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tbase quality\n"
			<< "labelY\tpercentage\n"
			<< endl

			<< "Quality\t"
			<< "Count"
			<< endl;

		for (auto i : qc_rqd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
	}

	void genData_qc_gcd() {

		qc_gcd_data = perQualReadsCounts;
	}

	void genFile_qc_gcd() {
		
		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_gcd_data-" + name + ".txt");

		ofile 	<< "graph_title\tGC distribution over all sequences\n"
			<< "allX\t" << qc_gcd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tmean GC content (%)\n"
			<< "labelY\tGC count per read\n"
			<< endl

			<< "GC Counts\t"
			<< "Count"
			<< endl;

		for (auto i : qc_gcd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
	}

};
