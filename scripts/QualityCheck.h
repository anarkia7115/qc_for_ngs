class QualityCheck {

	private:

	double rawLen 	= 0;
	long rawReads 	= 0;
	long rawBases 	= 0;
	long rawGcBases	= 0;

	long effectiveReads = 0;
	long effectiveBases = 0;

	long lineGcBases	= 0;
	long lineNBases  	= 0;
	long lineBases		= 0;

	bool isEffective 	= false;
	long excessNReads 	= 0;
	long passedReads 	= 0;
	double effectiveRate 	= 0;

	double Q30 = 0;
	double Q20 = 0;
	double GC  = 0;
	double genomeCoverage = 0;
	long meanDepth = 0;
	double mappedRate = 0;

	map<char, vector<int>> perPosCounts;
	map<char, vector<int>::iterator> perPosCountsIter;
	map<int, int> perGcReadsCounts;
	map<int, int> perNsReadsCounts;
	map<int, int> perBaseReadsCounts;

	vector<char> geneType = {'G', 'C', 'A', 'T', 'N'};

	// init iterators
	void initPerPosCountsIter() {
		char key;
		for(	auto iter = geneType.begin();
			iter != geneType.end(); iter++) {

			key = *iter;

			perPosCounts[key].push_back(0);
			perPosCountsIter[key] = perPosCounts[key].begin();
		}
	}

	void iterPlusPerPosCountsIter() {

		char key;
		for(	auto geneIter = perPosCountsIter.begin();
			geneIter != perPosCountsIter.end(); geneIter++) {

			key = geneIter->first;
			cout << "key1: " << key << endl;
			cout << "cur pos value: " << *geneIter->second << endl;
			
			// if empty, init as 0
			if (geneIter->second + 1 == perPosCounts[key].end()) {
				
				cout << "inited to zero" << endl;
				perPosCounts[key].push_back(0);
			}
			geneIter->second++;
		}
	}

	void resetPosPerPosCountsIter() {

		char key;
		for(	auto geneIter = perPosCountsIter.begin();
			geneIter != perPosCountsIter.end(); geneIter++) {

			key = geneIter->first;
			// pos ++
			geneIter->second = perPosCounts[key].begin();
		}
	}

	void addPerGcReadsCounts() {
		if(perGcReadsCounts.find(lineGcBases) != perGcReadsCounts.end()) {

			// add to orig record
			perGcReadsCounts[lineGcBases] += lineGcBases;
		}
		else {

			// assign to the record directly
			perGcReadsCounts[lineGcBases] = lineGcBases;
		}
	}

	void addPerNsReadsCounts() {
		if(perNsReadsCounts.find(lineNBases) != perNsReadsCounts.end()) {

			// add to orig record
			perNsReadsCounts[lineNBases] += lineNBases;
		}
		else {

			// assign to the record directly
			perNsReadsCounts[lineNBases] = lineNBases;
		}
	}

	void addPerBaseReadsCounts() {
		if(perBaseReadsCounts.find(lineBases) != perBaseReadsCounts.end()) {

			// add to orig record
			perBaseReadsCounts[lineBases] += lineBases;
		}
		else {

			// assign to the record directly
			perBaseReadsCounts[lineBases] = lineBases;
		}
	}

	void checkNStatus() {

		if (lineNBases > 3) {

			// too many N
			isEffective = false;
			excessNReads++;
		}
		else {
			isEffective = true;
			passedReads++;
		}
	}

	public:

	// Constructor
	QualityCheck() {

		initPerPosCountsIter();
	}

	void parseGene(char gene) {
		rawBases++;

		switch(gene){
			case 'G':
				lineGcBases++;
				// count++
				(*perPosCountsIter['G'])++;
				break;
			case 'C':
				lineGcBases++;
				(*perPosCountsIter['C'])++;
				break;
			case 'A':
				(*perPosCountsIter['A'])++;
				break;
			case 'T':
				(*perPosCountsIter['T'])++;
				break;
			case 'N':
				lineNBases++;
				(*perPosCountsIter['N'])++;
				break;
			default:
				cerr << "Warning:\n\t" << "\tunknown base?" << endl;
				break;	
		}

	}

	void parseReadsLine(string line) {

		// init line metrics
		lineGcBases = 0;
		lineNBases  = 0;
		lineBases = 0;

		// init pos
		resetPosPerPosCountsIter();

		// first iter
		auto siter = line.begin();
		parseGene(*siter);

		// parse gene by gene
		for(siter++; siter!= line.end(); siter++) {

			// shift pos
			iterPlusPerPosCountsIter();
			cout << "current gene before parse: " << *siter << endl;
			parseGene(*siter);
		}

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

	void sumUp() {
		rawLen = rawBases / rawReads;
	}
};


