#ifndef ECC_HIRGC_H_
#define ECC_HIEGC_H_


# include <cstdlib>
# include <iostream>
# include <fstream>
# include <ctime>
# include <sys/time.h>
# include <cmath>
# include <vector>
# include <algorithm>
# include <map>
# include <climits>
# include <cstdint>
# include <cstring>
# include <string>
# include <utility>
# include <thread>
# include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "util.h"
using namespace std;

//const int sub_str_num = 20; //hash function; the modulus s

//struct POSITION_RANGE { //an interval
//	int begin, length;//
//};

//struct POSITION_OTHER_CHAR {//for other characters
//	int pos, ch;
//};

// this is set to compress the given range of tasks of a file list
class hirgc
{

	public:	
		char *meta_data; // identifier
		POSITION_RANGE *pos_vec, *n_vec; //pos_ver is intervals of low-case letters; n_vec for the letter 'N'
		POSITION_OTHER_CHAR *other_char_vec; // for other characters
		int *line_break_vec;//EOL character; equivalent length of short sequence
		int *tar_seq_code, *ref_seq_code; // target and reference sequence after encoding
		int other_char_len, str_len, tar_seq_len, ref_seq_len, pos_vec_len, n_vec_len, line_break_len; //lengths related to their arrays
		int *loc; //an array of header points
		int *point; // an array of entries
		char *dismatched_str; //mismatched subsequence

	vector<string> genome_list;
	vector<int> cluster_list;
	vector<int> centroid_list;
	int start,end,tid,final_id;

	struct  timeval t_start;
	struct  timeval t_end;
	unsigned long timer;	

	hirgc(vector<string> m_genome_list,int m_start, int m_end,int m_tid,int m_final_id=-1,vector<int>m_cluster_list={},
		vector<int> m_centroid_list={}): 
	genome_list(m_genome_list),cluster_list(m_cluster_list), centroid_list(m_centroid_list), start(m_start), end(m_end), tid(m_tid),final_id(m_final_id)
	{

	}

	void initial();


void clear() { // free
//	printf("clear 1\n");
	delete[] meta_data;
//	printf("clear 2\n");	
	delete[] pos_vec;
//	printf("clear 3\n");	
	delete[] line_break_vec;
//	printf("clear 4\n");	
	delete[] n_vec;
//	printf("clear 5\n");	
	delete[] other_char_vec;
//	printf("clear 6\n");	
	delete[] tar_seq_code;
//	printf("clear 7\n");	
	delete[] ref_seq_code;
//	printf("clear 8\n");	
	delete[] loc;
//	printf("clear 9\n");	
	delete[] point;
	printf("clear finish\n");	
//	delete[] dismatched_str;
}

	virtual void readRefFile(char *refFile);
	virtual void readTarFile(char *tarFile);
	virtual void writeRunLengthCoding(FILE *fp, int vec_len, int *vec);
	virtual void saveOtherData(FILE *fp);
	virtual void preProcessRef();
	virtual void searchMatch(char *refFile);
	virtual void hirgc_cluster_compress(char *res_folder);
	virtual void hirgc_single_ref_compress(char *res_folder, char *ref);
	virtual void write_time_record(char *res_folder);


};

string strip_string(string s);

#endif


