#include <zlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <unistd.h>

using namespace std;

#define GZ_BUF_SIZE 1048576

void qc_us(){
	cerr << endl;
	cerr <<"================================================================================"  << endl;
	cerr << "Func:  tools for fq qc" << endl;
	cerr << "Version: 1.0" << endl;
	cerr << "Author:  zbxie" << endl;
	cerr << "Usage:   qc [options]" << endl;
	cerr << "Options: " << endl;
	cerr << "        -f FILE   forward fastq" << endl;
	cerr << "        -r FILE   reverse fastq" << endl;
	cerr << "        -t        trim" << endl;
	cerr << "        -p        output fq" << endl;
	cerr << "        -o DIR    dir to output.[default:./]" << endl;
	cerr << "===============================================================================" << endl;
	cerr << endl;
	exit(1);
}

void check_dir(string &dir){
        if(dir.substr(dir.size()-1,1) != "/")
                dir+="/";
}

bool check_gz(string &file){
	if(file.substr(file.size()-3)==".gz"){
		return true;
	}else{
		return false;
	}
}

string get_sample_name(const string &file){
	size_t start,end;
	if(file.find(".fq.") || file.find(".fastq.")){
        	start=file.rfind("/");//find last '/'
        	end=file.find("_R",start+1);
	}else{
		start=file.rfind("/");
		end=file.find(".",start+1);
	}

        string sample;
        if(start != string::npos){
                sample=file.substr(start+1,end-(start+1));
        }else{
                sample=file.substr(0,end);
        }
        return sample;
}


class Some_data{
	public:
		long raw_reads=0,raw_bases=0,passed_reads=0,passed_quals=0,effective_reads=0,effective_bases=0;
        	long gc_bases=0,q4_bases,q10_bases,q20_bases=0,q30_bases=0;
        	long excess_N_reads=0,low_qual_reads=0,excess_N_and_low_qual_reads=0;
        	map<size_t,long> per_pos_N_count;
        	map<size_t,long> per_pos_total_qual;
       		map<int,long> per_qual_bases_count;
		map<int,long> per_qual_reads_count;
		map<int,long> per_gc_reads_count;
		map<int,long> per_readLen_reads_count;
		map<char,long> per_Ns_reads_count;
		per_Ns_reads_counts['c'] = 2;
		
		map<size_t,long> per_pos_A_counts;
		map<size_t,long> per_pos_T_counts;
		map<size_t,long> per_pos_G_counts;
		map<size_t,long> per_pos_C_counts;
};

void print(const string &out_dir,const Some_data &total){
		
		string file=out_dir+"stats.txt";
		ofstream out(file);
		out << "raw_reads\t" << total.raw_reads << "\n";
		out << "raw_bases\t" << total.raw_bases << "\n";
		out << "passed_reads\t" << total.passed_reads << "\n";
		out << "passed_quals\t" << total.passed_quals << "\n";
		out << "effective_reads\t" << total.effective_reads << "\n";
		out << "effective_bases\t" << total.effective_bases << "\n";
		out << "gc_bases\t" << total.gc_bases << "\n";
		out << "q4_bases\t" << total.q4_bases << "\n";
		out << "q10_bases\t" << total.q10_bases << "\n";
		out << "q30_bases\t" << total.q30_bases << "\n";
		out << "q20_bases\t" << total.q20_bases << "\n";
		out << "excess_N_reads\t" << total.excess_N_reads << "\n";
		out << "low_qual_reads\t" << total.low_qual_reads << "\n";
		out << "excess_N_and_low_qual_reads\t" << total.excess_N_and_low_qual_reads << "\n";
		out.close();

		string file1=out_dir+"per_pos_N_counts.txt";
		ofstream ppnc(file1);
		ppnc << "#per_pos_N_count:\n";
		size_t len_A=total.per_pos_A_counts.size();
		size_t len_T=total.per_pos_T_counts.size();
		size_t len_G=total.per_pos_G_counts.size();
		size_t len_C=total.per_pos_C_counts.size();
		vector<size_t> leng;
		size_t max=len_A;
		for(auto &i:leng){
			if(max < i) max=i;
		}
		ppnc << "pos\tA\tT\tG\tC\n";
		for(auto i=0;i < max;i++){
			ppnc << i;

			if(total.per_pos_A_counts.count(i) !=0){
				ppnc << "\t" << total.per_pos_A_counts.at(i);
			}else{
				ppnc << "\t" << "0";
			}
			
			if(total.per_pos_T_counts.count(i) !=0){
                                ppnc << "\t" << total.per_pos_T_counts.at(i);
                        }else{
                                ppnc << "\t" << "0";
                        }

			if(total.per_pos_G_counts.count(i) !=0){
                                ppnc << "\t" << total.per_pos_G_counts.at(i);
                        }else{
                                ppnc << "\t" << "0";
                        }

			if(total.per_pos_C_counts.count(i) !=0){
                                ppnc << "\t" << total.per_pos_C_counts.at(i) ;
                        }else{
                                ppnc << "\t" << "0";
                        }
			ppnc << endl;
		}				
		ppnc.close();

		string file2=out_dir+"per_pos_mean_qual.txt";
		ofstream ppmq("file2");
		ppmq << "#per_pos_mean_qual:\n";
		auto site=total.per_pos_total_qual.begin();
		while(site != total.per_pos_total_qual.end()){
			ppmq << site->first+1 << "\t" << (site->second)/total.raw_reads << "\n";
			++site;
		}
		ppmq.close();

		string file3=out_dir+"per_qual_bases_counts.txt";
		ofstream ppbc(file3);
		ppbc << "#per_qual_bases_count:\n";
		auto qual=total.per_qual_bases_count.begin();
		while(qual != total.per_qual_bases_count.end()){
			ppbc << qual->first << "\t" << qual->second << "\n";
			++qual;
		}
		ppbc.close(); 

		string file4=out_dir+"per_qual_reads_count.txt";
		ofstream pqrc(file4);
		pqrc << "#per_qual_reads_count:\n";
		auto mean=total.per_qual_reads_count.begin();
		while(mean != total.per_qual_reads_count.end()){
			pqrc << mean->first << "\t" << mean->second << "\n";
			++mean;
		}
		pqrc.close();		

		string file5=out_dir+"per_gc_reads_counts.txt";
		ofstream pgrc(file5);
		pgrc << "#per_gc_reads_count:\n";
		auto gc=total.per_gc_reads_count.begin();
		while(gc != total.per_gc_reads_count.end()){
			pgrc << gc->first << "\t" << gc->second << "\n";
			++gc;
		}
		pgrc.close();

		string file6=out_dir+"per_readLen_reads_counts.txt";
		ofstream prrc(file6);
		prrc << "#per_readLen_reads_count:\n";
		auto len=total.per_readLen_reads_count.begin();
		while(len != total.per_readLen_reads_count.end()){
			prrc << len->first << "\t" << len->second << "\n";
			++len;
		}
		prrc.close();

		string file7=out_dir+"per_Ns_reads_counts.txt";
		ofstream pnrc(file7);
		pnrc << "#per_Ns_reads_count:\n";
		auto num=total.per_Ns_reads_count.begin();
		while(num != total.per_Ns_reads_count.end()){
			pnrc<< num->first << "\t" << num->second << "\n";
			++num;
		}
		pnrc.close();
	
		out.close();
}

bool count_read(string readLine,Some_data &total){
	int n_counts=0;
	int gc_counts=0;
	size_t len=readLine.size()-1;// "\n" exist
	bool ok=true;
	for(size_t i=0;i<len;i++){
		switch(readLine[i]){
			case 'G':
				gc_counts++;
				total.per_pos_G_counts[i]++;
				break;
			case 'C':
				gc_counts++;
				total.per_pos_C_counts[i]++;
				break;
			case 'A':
				total.per_pos_A_counts[i]++;
				break;
			case 'T':
				total.per_pos_T_counts[i]++;
				break;
			case 'N':
				n_counts++;
				total.per_pos_N_count[i+1]++;
				break;
			default:
				cerr << "Warning:\n\t" << i << "\tbase?" << endl;
				break;	
		}
	}
	total.raw_reads ++ ;
	total.raw_bases += len;
	total.gc_bases += gc_counts;
	total.per_gc_reads_count[gc_counts] ++;
	total.per_Ns_reads_count[n_counts] ++;
	total.per_readLen_reads_count[len] ++;

	if(n_counts > 3){
		ok=false;
		total.excess_N_reads ++ ;
	}else{
		total.passed_reads ++;
	}
	return ok;
}

bool count_qual(string qualLine,Some_data &total){
	int sum=0;
	size_t len=qualLine.size()-1;
	int mean_qual=0;
	int qual=0;
	int low_qual=0;
	float discard=0.5;
	bool ok=true;
	for(size_t i=0;i<len;i++){
		qual=qualLine[i]-33;
		total.per_pos_total_qual[i+1] += qual;
		total.per_qual_bases_count[qual] ++ ;
		sum+=qual;
		if(qual >= 30){
			total.q30_bases++;
			total.q20_bases++;
			total.q10_bases++;
			total.q4_bases++;
		}else if(qual >= 20){
			total.q20_bases++;
                        total.q10_bases++;
                        total.q4_bases++;
		}else if(qual >= 10){
			total.q10_bases++;
                        total.q4_bases++;
			low_qual++;
		}else if(qual >= 4) {
			total.q4_bases++;
			low_qual++;
		}else{
			low_qual++;
		}
	}
	mean_qual=sum/len;
	total.per_qual_reads_count[mean_qual] ++ ;
	float part=float(low_qual) / float(len);
	if(part > discard){
		ok=false;
		total.low_qual_reads ++;
	}else{
		total.passed_quals ++;
	}
	return ok;
}

void read_gz(string &forward,string &reverse, string &out_dir,bool &flag_trim,bool &flag_fq){
	
	string sample_name=get_sample_name(forward);
	
	string out_f=out_dir+sample_name+"_f.fq.gz";
	string out_r=out_dir+sample_name+"_r.fq.gz";
	
	string data_dir=out_dir+sample_name+"/";
	string cmd_mkdir="mkdir -p ";
	cmd_mkdir += data_dir;
	system(cmd_mkdir.c_str());
		
	gzFile new_f = gzopen(out_f.c_str(),"w");
	gzFile new_r = gzopen(out_r.c_str(),"w");

	if(flag_fq==false){
		string cmd="rm -rf ";
		string cmd_f=cmd+out_f;
		string cmd_r=cmd+out_r;

		gzclose(new_f);
		gzclose(new_r);

		system(cmd_f.c_str());
		system(cmd_r.c_str());
	}
	gzFile fq_f = gzopen(forward.c_str(),"r");
	gzFile fq_r = gzopen(reverse.c_str(),"r");


	vector<string> unit_f;
	vector<string> unit_r;

	char buf_f[GZ_BUF_SIZE];
        char buf_r[GZ_BUF_SIZE];

	Some_data total_f;
	Some_data total_r;

	while(gzgets(fq_f,buf_f,GZ_BUF_SIZE)){
		gzgets(fq_r,buf_r,GZ_BUF_SIZE);
		unit_f.push_back(buf_f);
		unit_r.push_back(buf_r);
	
		if(unit_f.size() == 4){
			if(unit_r.size() !=4){
				cerr << "\n\tfq_f,fq_r discordant!" << endl;
				exit(0);
			}

			bool read_ok_f=count_read(unit_f[1],total_f);
			bool read_ok_r=count_read(unit_r[1],total_r);
			bool qual_ok_f=count_qual(unit_f[3],total_f);
			bool qual_ok_r=count_qual(unit_r[3],total_r);

			if(read_ok_f && qual_ok_f && read_ok_r && qual_ok_r){
				total_f.effective_reads ++ ;
				total_r.effective_reads ++ ;
				total_f.effective_bases += unit_f[1].size() - 1;//"\n"
				total_r.effective_bases += unit_r[1].size() - 1;//"\n"

				if(flag_fq == true){
					string f_4=unit_f[0]+unit_f[1]+unit_f[2]+unit_f[3];
					string r_4=unit_r[0]+unit_r[1]+unit_r[2]+unit_r[3];
					gzputs(new_f,f_4.c_str());
					gzputs(new_r,r_4.c_str());
				}	
			}else{
				if(read_ok_f == false && qual_ok_f == false){
					total_f.excess_N_and_low_qual_reads ++ ;
				} 
				
				if(read_ok_r == false && qual_ok_r == false){
                                        total_r.excess_N_and_low_qual_reads ++ ;
                                }

				if(read_ok_f == false) total_f.excess_N_reads++;
				if(read_ok_r == false) total_r.excess_N_reads++;
				if(qual_ok_f == false) total_f.low_qual_reads++; 
				if(qual_ok_r == false) total_r.low_qual_reads++;
			}
		
			unit_f.clear();
			unit_r.clear();
		}
	}

	print(data_dir,total_f);
	print(data_dir,total_r);

	gzclose(fq_f);
	gzclose(fq_r);
}

void read_common(string &forward,string &reverse, string &out_dir,bool &flag_trim,bool &flag_fq){
}

int main(int argc,char** argv){
        if(argc < 2) qc_us();

        string out_dir="./";
        string fq_f;
        string fq_r;
        bool flag_trim=false;
	bool flag_fq=false;
	int c=0;
	opterr=0;
	while((c=getopt(argc,argv,"pf:r:o:t"))!=-1){
		switch(c){
			case 'f':
				fq_f=optarg;
				break;
			case 'r':
				fq_r=optarg;
				break;
			case 'o':
				out_dir=optarg;
				break;
			case 't':
				flag_trim=true;
				break;
			case 'p':
				flag_fq=true;
				break;
			default:
				qc_us();
				break;
		}
	}
	if(argc != optind) qc_us();
	if(fq_f.empty()) qc_us();
	if(fq_r.empty()) qc_us();
	check_dir(out_dir);
	bool flag_gz=check_gz(fq_f);
	
	if(flag_gz){
		read_gz(fq_f,fq_r,out_dir,flag_trim,flag_fq);
	}else{
		read_common(fq_f,fq_r,out_dir,flag_trim,flag_fq);
	}
}
	
	
