// aggregate values
struct AggVal {

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
	AggVal() :qualVals(51,0){}

} ;
