#include "GZPair.h"

void getOpts(int argc, char** argv);
void checkOpts(char** argv);

string call_id;
string sample;

string gz1_path;
string gz2_path;
string output_path;
string log_path;
string sample_name;
int row_num = 0;
bool is_compress;

int main(int argc, char** argv) 
{
	getOpts(argc, argv);
	logMsg("Hello World!");
	
	// 1 gz or 2 gz
	if (gz2_path == "NONE")
	{
		logMsg("single gz");
		GZ gz(gz1_path, output_path);
		gz.testFunc(row_num, is_compress);
	}
	else 
	{
		logMsg("double gz");
		GZPair gzp(gz1_path, gz2_path, output_path);
		gzp.testFunc(row_num, is_compress);
	}

	return 0;
}

void getOpts(int argc, char** argv) 
{
	int c;

	while (1) 
	{
		int option_index = 0;
		static struct option long_options[] = {
			{"gz1",	 required_argument, 0,  0 },
			{"gz2",  required_argument, 0,  0 },
			{"output",  required_argument, 0,  0 },
			{"callId",  required_argument, 0,  0 },
			{"row", required_argument, 0,  0 },
			{"log", required_argument, 0,  0 },
			{0, 0, 0, 0 }
		};

		c = getopt_long(argc, argv, ":c",
				 long_options, &option_index);
		if (c == -1)
		{
			break;
		}

		switch (c) {
		case 0:
			printf("option %s", long_options[option_index].name);
			if (optarg)
				printf(" with arg %s", optarg);
			printf("\n");
			if (long_options[option_index].flag != 0) {
				cout << "break after flag test! \n";
				break;
			}
			if (strcmp(long_options[option_index].name,"gz1") == 0) {
				gz1_path = optarg;
				cout << gz1_path << endl;
			}
			if (strcmp(long_options[option_index].name,"gz2") == 0) {
				if(optarg)
				{
					gz2_path = optarg;
					cout << gz2_path << endl;
				}
			}
			if (strcmp(long_options[option_index].name,"output") == 0) {
				output_path = optarg;
				cout << output_path << endl;
			}
			if (strcmp(long_options[option_index].name,"row") == 0) {
				row_num = atoi(optarg);
				cout << row_num << endl;
			}
			if (strcmp(long_options[option_index].name,"sample") == 0) {
				sample_name = optarg;
				cout << sample_name << endl;
			}
			if (strcmp(long_options[option_index].name,"log") == 0) {
				log_path = optarg;
				cout << log_path << endl;
			}
			if (strcmp(long_options[option_index].name,"callId") == 0) {
				call_id = optarg;
				cout << call_id << endl;
			}
			break;

		case 'c':
			is_compress = true;
			printf("compress the result\n");
			break;


		default:
			printf("?? getopt returned character code 0%o ??\n", c);
		}
	}


	if (optind < argc) 
	{
		printf("non-option ARGV-elements: ");
		while (optind < argc)
			printf("%s ", argv[optind++]);
		printf("\n");
	}

	// check options
	checkOpts(argv);

	// init log path
	initLogPath();
}

void checkOpts(char** argv)
{
	if (gz1_path.size() == 0 || output_path.size() == 0 || row_num == 0) 
	{

		printf( "Usage: %s [-c] \\ \n"
			"	  --gz1 path1  [--gz2 path2] \\ \n"
			"	  --output out_folder_path \\ \n"
			"	  --row [0-9+] \\ \n"
			"	  --callId call_id \\ \n"
			"	  --sample sample_name \\ \n"
			"	  --log log_folder_path \n"
					, argv[0]);

		cerr << "MUST SPECIFY: gz1, output and row_num! \n";
		exit(EXIT_FAILURE);
	}
}
