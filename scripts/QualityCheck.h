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

	long lineQualSum 	= 0;

	long q30Bases	= 0;
	long q20Bases	= 0;
	long q10Bases 	= 0;
	long q4Bases	= 0;
	long lowQualBases = 0;

	double genomeCoverage = 0;
	long meanDepth = 0;
	double mappedRate = 0;

	map<char, vector<int>> 	perPosCounts;
	map<char, vector<bool>>	tmp_perPosCounts;
	vector<int> 		perPosTotQual;

	map<int, int>	perGcReadsCounts;
	map<int, int>	perNsReadsCounts;
	map<int, int>	perBaseReadsCounts;
	map<int, int>	perQualBasesCounts;

	vector<char> geneType = {'G', 'C', 'A', 'T', 'N'};

	void addTmpPos(char targetKey) {

		char key;

		//cout << "perPosCounts size: " << perPosCounts.size() << endl;
		//cout << "key: " << endl;
		for(	auto k : geneType ) {

			key = k;

			// pos ++
			if(key == targetKey){

				tmp_perPosCounts[key].push_back(true);
			}
			else{

				tmp_perPosCounts[key].push_back(false);
			}
		}

	}

	void flushTmp() {
		//int lenTot = 0;
		//int lenTmp = 0;

		char key;

		for(	auto k : geneType) {

			key = k;

			// can deleted
			//int lenTot = perPosCounts->size();
			//int lenTmp = tmp_perPosCounts->size();

			auto totIter = perPosCounts[key].begin();
			auto tmpIter = tmp_perPosCounts[key].begin();

			for (   ; totIter != perPosCounts[key].end() 
				&& tmpIter != tmp_perPosCounts[key].end()
				; totIter++) {

				// if gene is on this pos
				if(*tmpIter){
					(*totIter)++;
				}

				tmpIter++;
			}

			// push back new to total, if tmp is not over
			for (; tmpIter != tmp_perPosCounts[key].end(); tmpIter++) {
				if(*tmpIter){
					perPosCounts[key].push_back(1);
				}
				else {
					perPosCounts[key].push_back(0);
				}
			}

		}

		//cout << "tmp size: " << tmp_perPosCounts.size() << endl;
		tmp_perPosCounts.clear();
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

		//cout << "perPosCounts[key] length: " << perPosCounts[key].size() << endl;
		for (auto i : perPosCounts[key]) {
			cout << i << endl;
		}

	}

	void parseGene(char gene) {
		lineBases++;

		switch(gene){
			case 'G':
				lineGcBases++;
				// count++
				addTmpPos('G');
				break;
			case 'C':
				lineGcBases++;
				addTmpPos('C');
				break;
			case 'A':
				addTmpPos('A');
				break;
			case 'T':
				addTmpPos('T');
				break;
			case 'N':
				lineNBases++;
				addTmpPos('N');
				break;
			default:
				cerr << "Warning:\n\t" << "\tunknown base?" << endl;
				break;	
		}
	}

	void parseQual(char qualCh) {

		// char to int
		int qual = qualCh - 33;

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
			lowQualBases++;
		}
		else if(qual >= 4) {
			q4Bases++;
			lowQualBases++;
		}
		else {
			lowQualBases++;
		}

	}

	void parseReadsLine(string line) {

		// init line metrics
		lineGcBases = 0;
		lineNBases  = 0;
		lineBases = 0;

		// first iter
		auto siter = line.begin();
		parseGene(*siter);

		// parse gene by gene
		for(siter++; siter!= line.end(); siter++) {

			// shift pos
			//cout << "current gene before parse: " << *siter << endl;
			parseGene(*siter);
		}

		// flush tmp vector
		flushTmp();

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


