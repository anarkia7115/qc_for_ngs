#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include "./BedFile.h"
#include "AggVal.h"

using namespace std;
class QualityCheck {

	private:

	// parameters
	string outputFolderPath;
	string gnomeVersion = "hg38";

    // phred encoding key
    string encodingKey;
    int phredShift = 0;

	long rawLen 	= 0;
	long rawReads 	= 0;
	long rawBases 	= 0;
	long rawGcBases	= 0;
	long rawNBases 	= 0;
	long gnomeBaseTotal = 0;

	string name = "default";

	long effectiveReads = 0;
	long effectiveBases = 0;

	long lineGcBases	= 0;
	long lineNBases  	= 0;
	long lineBases		= 0;

	bool isFirstRead = true;
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

	double q30Rate = 0;
	double q20Rate = 0;
	double gcRate = 0;
	double genomeCoverage = 0;
	double meanDepth = 0;
	double mappedRate = 0;

	map<char, vector<long>>	mPerPosReadsCounts;
	map<char, vector<bool>>	mTmp_perPosReadsCounts;

	map<int, long, less<int>>	mPerGcReadsCounts;
	map<int, long>	mPerNsReadsCounts;
	map<int, long>	mPerBaseReadsCounts;
	//map<int, long>	mPerQualBasesCounts;

	vector<char> geneType = {'G', 'C', 'A', 'T', 'N'};

	// qual
	vector<long> perPosQualCounts;
	vector<long> tmp_perPosQualCounts;

	vector<long> perQualCounts;
	map<int, long, std::less<int>> mPerQualReadsCounts;

	int lineMeanQual 	= 0;
	bool isQualOk		= false;
	float discard 		= 0.5;
	long lowQualReads 	= 0;
	long passedQuals 	= 0;

	unsigned int curPos = 0;

	vector<AggVal> vPos2AggVal;

	//map<char, map<int, long> > mPositionBaseComposition;
	//map<pair<char, int>, double> mPositionBaseComposition;
	map<int, map<char, double>> mPositionBaseComposition;
	map<int, int,  less<int>> mQc_bqd_data;
	map<int, long, less<int>> mQc_rqd_data;
	map<int, long, less<int>> mQc_gcd_data;
	

	void addTmpPosReads(char targetKey) {

		char key;

		//cout << "mPerPosReadsCounts size: " << mPerPosReadsCounts.size() << endl;
		//cout << "key: " << endl;
		for(	auto k : geneType ) {

			key = k;

			// pos ++
			if(key == targetKey){

				mTmp_perPosReadsCounts[key].push_back(true);
			}
			else{
				mTmp_perPosReadsCounts[key].push_back(false);
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

			auto totIter = mPerPosReadsCounts[key].begin();
			auto tmpIter = mTmp_perPosReadsCounts[key].begin();

			// 50% time on the for loop
			for (   ; totIter != mPerPosReadsCounts[key].end() 
				&& tmpIter != mTmp_perPosReadsCounts[key].end()
				; totIter++) {

				// if gene is on this pos
				if(*tmpIter){
					(*totIter)++;
				}

				tmpIter++;
			}

			// push back new to total, if tmp is not over
			for (; tmpIter != mTmp_perPosReadsCounts[key].end(); tmpIter++) {
				if(*tmpIter){
					mPerPosReadsCounts[key].push_back(1);
				}
				else {
					mPerPosReadsCounts[key].push_back(0);
				}
			}
		}

		//cout << "tmp size: " << mTmp_perPosReadsCounts.size() << endl;
		mTmp_perPosReadsCounts.clear();
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

		perQualCounts.at(qual)++;
	}

	void addPerQualReadsCount() {

		// if record exists
		if(mPerQualReadsCounts.find(lineMeanQual) 
				!= mPerQualReadsCounts.end()) {

			mPerQualReadsCounts[lineMeanQual] ++;
			//cout << lineMeanQual << ": " 
			//<< mPerQualReadsCounts[lineMeanQual] << endl;
		}
		else {

			mPerQualReadsCounts[lineMeanQual] = 1;
			//cout << lineMeanQual << ": " 
			//<< mPerQualReadsCounts[lineMeanQual] << endl;
		}
	}

	void addPerGcReadsCounts() {
		if(mPerGcReadsCounts.find(lineGcBases) 
				!= mPerGcReadsCounts.end()) {

			// add to orig record
			mPerGcReadsCounts[lineGcBases] ++;
		}
		else {

			// assign to the record directly
			mPerGcReadsCounts[lineGcBases] = 1;
		}
	}

	void addPerNsReadsCounts() {
		if(mPerNsReadsCounts.find(lineNBases) != mPerNsReadsCounts.end()) {

			// add to orig record
			mPerNsReadsCounts[lineNBases] ++;
		}
		else {

			// assign to the record directly
			mPerNsReadsCounts[lineNBases] = 1;
		}
	}

	void addPerBaseReadsCounts() {
		if(mPerBaseReadsCounts.find(lineBases) != mPerBaseReadsCounts.end()) {

			// add to orig record
			mPerBaseReadsCounts[lineBases] ++;
		}
		else {

			// assign to the record directly
			mPerBaseReadsCounts[lineBases] = 1;
		}
	}

	void addPerPosAggVal(int qual) {
		// push value to the right position
		//int intervalNum = posToInterval(curPos);

        // construct if not long enough
        if (vPos2AggVal.size() < curPos + 1) {
            for (unsigned int i = vPos2AggVal.size(); i < curPos + 1; i++) {
                vPos2AggVal.emplace_back();
            }
        }
		AggVal &curAggVal = vPos2AggVal.at(curPos);

        if (qual < 50) {
            curAggVal.add(qual);
        }
        else {
            cerr << "qual exceed 49: current qual val [" << qual << "] adding to qual=50" << endl;
            curAggVal.add(50);
        }
	}

	int posToInterval(int pos) {
		// push value to the right position
        if ((pos <= 9 && pos >= 0) || (pos >= rawLen - 10)) {
            // if first 10 head and last 10 head
            return pos;
        }
        else if (pos >= 0 && pos <= rawLen) {
            // if in the middle
            return pos - pos % 5;
        }
        else {
            cerr << "raw length: " << rawLen << endl;
            cerr << "pos: " << pos << endl;
			cerr << "unknown position!" << endl;
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
			effectiveReads++;
		}
	}

	public:

	// Constructor
	QualityCheck(string op) : perQualCounts(100, 0) {

		this->outputFolderPath = op;
        // default gnomeBaseTotal
		if (gnomeVersion == "hg38") {

			gnomeBaseTotal = 3209286105;
		}
		else if (gnomeVersion == "hg19") {
			
			gnomeBaseTotal = 3137161264;
		}
		else {

			cerr << "Unknown Gnome Version: " << gnomeVersion << endl;
			exit(-1);
		}
	}

	QualityCheck(string op, string n, string ek) : QualityCheck(op) {

		this->name = n;
        this->encodingKey = ek;

        if (this->encodingKey == "base33"){
                phredShift = 33;
        }
        else if (this->encodingKey == "base64"){
                phredShift = 64;
        }
        else {
				cerr 	<< "Error:\n\t" 
					<< this->encodingKey
					<< "\tunknown phred encoding?" << endl;
				exit(1);
        }
	}

	QualityCheck(string op, string n, string ek, string bfp) : QualityCheck(op, n, ek) {

        BedFile bf(bfp);

        gnomeBaseTotal = bf.sumRegionDiff();
	}

	void listQualVec() {
		
		// map<int, AggVal> vPos2AggVal;
		cout << "a size: " << vPos2AggVal.size() << endl;
		for(auto a : vPos2AggVal) {
			cout << "key: " << a.getKey() << endl;
			//cout << "qual count: " << a.second.getCount() << endl;
			//cout << "vector size: " << a.second.qualVals.size() << endl;
		}
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

		//cout << "mPerPosReadsCounts[key] length: " << mPerPosReadsCounts[key].size() << endl;
		for (auto i : mPerPosReadsCounts[key]) {
			cout << i << endl;
		}

	}

	void printQualDistribution() {
	}

	bool parseGene(char gene) {
		lineBases++;
		size_t s = mPerPosReadsCounts[gene].size();

		switch(gene){

			//cerr << "size: " << s << endl;

			case 'G':
				lineGcBases++;
				// count++
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						mPerPosReadsCounts['G'].push_back(0);
					}
				}
				mPerPosReadsCounts['G'].at(genSen)++; 
				//addTmpPosReads('G');
				break;
			case 'C':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						mPerPosReadsCounts['C'].push_back(0);
					}
				}
				lineGcBases++;
				mPerPosReadsCounts['C'].at(genSen)++; 
				//addTmpPosReads('C');
				break;
			case 'A':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						mPerPosReadsCounts['A'].push_back(0);
					}
				}
				mPerPosReadsCounts['A'].at(genSen)++; 
				//addTmpPosReads('A');
				break;
			case 'T':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						mPerPosReadsCounts['T'].push_back(0);
					}
				}
				mPerPosReadsCounts['T'].at(genSen)++; 
				//addTmpPosReads('T');
				break;
			case 'N':
				if(s < genSen + 1) {
					//cerr << genSen<< "size not full" << endl;
					for(size_t i = 0; i < genSen - s + 1; i++) {
						mPerPosReadsCounts['N'].push_back(1);
					}
				}
				lineNBases++;
				mPerPosReadsCounts['N'].at(genSen)++; 
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
		int qual = qualCh - phredShift;

		addPerPosAggVal(qual);

		addTmpPosQual(qual);
		addPerQualCounts(qual);

		lineQualSum += qual;

		if(qual >= 30) {
			q30Bases++;
			q20Bases++;
			q10Bases++;
			q4Bases++;
		}
		else if(qual >= 20) {
			q20Bases++;
			q10Bases++;
			q4Bases++;
		}
		else if(qual >= 10) {
			q10Bases++;
			q4Bases++;
			lineLowQualBases++;
		}
		else if(qual >= 4) {
			q4Bases++;
			lineLowQualBases++;
		}
		else {
			lineLowQualBases++;
		}

		curPos++;
	}

	void parseReadsLine(string line) {

		// init line metrics
		lineGcBases = 0;
		lineNBases  = 0;
		lineBases = 0;
		genSen = 0;

		// first line
		if (isFirstRead) {

			rawLen = line.size();
			isFirstRead = !isFirstRead;
		}

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
		//mTmp_perPosReadsCounts.clear();

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
		genData_qc_summary();
	}

	void genFiles() {
		genFile_qc_pqd();
		genFile_qc_pbc();
		genFile_qc_bqd();
		genFile_qc_rqd();
		genFile_qc_gcd();
		genFile_qc_summary();
	}

	void genData_qc_pqd() {

        int maxReadsLen = vPos2AggVal.size();
        AggVal* stopAggVal = NULL;
        int stopKey = 0;
        int stopInterval = 5;

        // first loop set keys, and merge intervals
        for (int pos = 0; pos < maxReadsLen; pos++) {
            if ( (pos >=0 && pos < 10) || (pos >= maxReadsLen - 10 && pos < maxReadsLen) ) {
                // first 10 and last 10
                vPos2AggVal.at(pos).setKey(pos);
            }
            else if ( pos % stopInterval == 0 ) {
                // interval stop
                stopKey = pos;
                stopAggVal = &vPos2AggVal.at(pos);
                vPos2AggVal.at(pos).setKey(stopKey);
            }
            else if (pos >=0 && pos < maxReadsLen) {
                // between intervals
                vPos2AggVal.at(pos).setKey(-1);
                // merge current pos value to stop AggVal
                stopAggVal->merge(vPos2AggVal.at(pos));
            }
            else {
                cerr << "unknown pos: " << pos << endl;
                exit(1);
            }
        }
        // second loop to erase? TODO
        for (auto& av : vPos2AggVal) {
            if (av.getKey() != -1){
                av.reduce();
            }
        }

	}

	static void genFile_qc_pqd(vector<AggVal> mat, string ofp, string n) {

		ofstream ofile;
		ofile.open (ofp + "/qc_pqd_data-" + n + ".txt");

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

		for (auto av: mat) {

            // pass -1 keys
            if(av.getKey() == -1) {
                continue;
            }

			ofile 	<< av.getKey() +1   << "\t"
				<< av.getMean()     << "\t"
				<< av.getMedian()   << "\t"
				<< av.getQ1()       << "\t"
				<< av.getQ3()       << "\t"
				<< av.getLowest()   << "\t"
				<< av.getHighest()  
				<< endl;
		}

		ofile.close();
	}

	void genFile_qc_pqd() {

		genFile_qc_pqd(this->vPos2AggVal, this->outputFolderPath, this->name);
	}

    /*
	void printMatrix() {

		ofstream ofile;
		ofile.open (outputFolderPath + "/matrix-" + name + ".txt");


		for (auto a : vPos2AggVal) {

			ofile << a.first << ":\t";
			auto qv = a.second.qualVals;

			for (auto c : qv) {

				ofile << c << "\t";
			}

			ofile << endl;
		}
		ofile.close();
	}
    */

	map<int, map<char, double>> getPositionBaseComposition() {

		return mPositionBaseComposition;
	}

	vector<long> getPerQualCounts() {

		return perQualCounts;
	}

	void genData_qc_pbc() {

		//map<char, vector<long>>	mPerPosReadsCounts;
		map<char, map<int, long>> v1;
		//map<pair<char, int>, double> mPositionBaseComposition
		//map<int, map<char, double>> mPositionBaseComposition

		char gene;
		int  key;
		vector<long> v;

		int sen = 0;

		for (auto g : mPerPosReadsCounts) {

			gene = g.first;
			v   = g.second;

			for (unsigned int i = 0; i < 9; i++) {

				if (v.size() <= i) {
				v1[gene].insert( make_pair(i, 0));
				}
				else {
				v1[gene].insert( make_pair(i, v.at(i)));
				}
			}

			for (unsigned int i = 9; i < v.size(); i++) {

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
					+= v.at(i);
			}

            //cout << "gene: " <<             gene << endl;
            //cout << "v.size(): " <<         v.size() << endl;
            //cout << "rawLen - 1: " <<       rawLen - 1 << endl;
            //cout << "v.at(rawLen - 1): " << v.at(rawLen - 1) << endl;
            //cout << "v1.size(): " <<        v1.size() << endl;
			v1[gene].insert(
					make_pair(v.size() - 1, v.at(v.size() - 1)));
            //cout << "after insert" << endl;

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

				mPositionBaseComposition[key][gene] 
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

		auto g = mPositionBaseComposition;

		map<int, map<char, double>>::iterator it;

		for (int i = 0; i < rawLen; i++) {

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

		ofile.close();
	}

	void genData_qc_bqd() {

		long sum = 0;
		long left = 0;
		//map<int, long> perQualCounts;
		//map<int, long> mPerQualReadsCounts;
		vector<int> quals;
		//map<int, int> mQc_bqd_data;

		for(auto l : perQualCounts) {

			sum += l;
		}

		left = sum;

		int i = 0;
		for(auto l : perQualCounts) {

			mQc_bqd_data[i] = (double(left) / double(sum)) * 100;

			//cout << mQc_bqd_data[l.first] << endl;
			//cout << l.first << " fraq: " << double(left)/double(sum) << endl;
			left -= l;
			i++;
		}
	}

	void genFile_qc_bqd() {

		//map<int, int> mQc_bqd_data;

		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_bqd_data-" + name + ".txt");

		ofile 	<< "graph_title\taccumulated base quality over all bases\n"
			<< "allX\t" << mQc_bqd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tbase quality\n"
			<< "labelY\tpercentage\n"
			<< endl

			<< "Quality\t"
			<< "Count"
			<< endl;

		for (auto i : mQc_bqd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
		ofile.close();
	}

	void genData_qc_rqd() {
		mQc_rqd_data = mPerQualReadsCounts;
	}

	void genFile_qc_rqd() {
		
		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_rqd_data-" + name + ".txt");

		ofile 	<< "graph_title\tquality score distribution over all sequences\n"
			<< "allX\t" << mQc_rqd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tbase quality\n"
			<< "labelY\tpercentage\n"
			<< endl

			<< "Quality\t"
			<< "Count"
			<< endl;

		for (auto i : mQc_rqd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
		ofile.close();
	}

	void genData_qc_gcd() {

		mQc_gcd_data = mPerQualReadsCounts;
	}

	void genFile_qc_gcd() {
		
		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_gcd_data-" + name + ".txt");

		ofile 	<< "graph_title\tGC distribution over all sequences\n"
			<< "allX\t" << mQc_gcd_data.size() << "\n"
			<< "allY\t1\n"
			<< "labelX\tmean GC content (%)\n"
			<< "labelY\tGC count per read\n"
			<< endl

			<< "GC Counts\t"
			<< "Count"
			<< endl;

		for (auto i : mQc_gcd_data) {

			ofile 	<< i.first << "\t" << i.second << endl;
		}
		ofile.close();
	}

	void genData_qc_summary() {

		effectiveBases = effectiveReads * rawLen;
		effectiveRate  = effectiveBases / (double)rawBases;
		q30Rate = q30Bases / (double)rawBases;
		q20Rate = q20Bases / (double)rawBases;
		gcRate  = rawGcBases / (double)rawBases;

		meanDepth = rawLen * rawReads / (double)gnomeBaseTotal;
	}

	void genFile_qc_summary() {

		ofstream ofile;
		ofile.open (outputFolderPath + "/qc_ss1_stat-" + name + ".txt");

		ofile 	<< "\tsample1" << "\n"
			<< "qc_read_len\t" << rawLen << "\n"
			<< "qc_raw_reads\t" << rawReads << "\n"
			<< "qc_raw_bases\t" << rawBases << "\n"
			<< "qc_effictive_reads\t" << effectiveReads << "\n"
			<< "qc_effictive_bases\t" << effectiveBases << "\n"
			<< "qc_effective_rate\t" << effectiveRate << "\n"
			<< "qc_q30\t" << q30Rate << "\n"
			<< "qc_q20\t" << q20Rate << "\n"
			<< "qc_gc\t"  << gcRate  << "\n"
			<< "qc_mean_depth\t" << meanDepth  << "\n"
			<< "qc_gnome_base_total\t" << gnomeBaseTotal << endl;
	}

	void setPerPosAggVal(vector<AggVal> mat) {

		this->vPos2AggVal = mat;
	}

};
