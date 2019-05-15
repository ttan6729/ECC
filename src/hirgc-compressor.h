#ifndef ECC_COMPRESSOR_H_
#define ECC_COMPRESSOR_H_


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
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../libbsc/bsc.h"
#include "util.h"
#include "hirgc-p.h"


using namespace std;
class Compressor
{
	protected:
		vector<string> file_list;
		vector<int> cluster_index;
		vector<int> centroids;

	public:
		int compress_count = 0;		
		string result_name;
		string ref_file;
		int thread_number = 1;
		int final_id;
		struct stat info;	

		Compressor(string m_r, vector<string> m_file_list, vector<int> m_centroids, vector<int> m_cluster_index, int m_final_id = -1):
		result_name(m_r), file_list(m_file_list), centroids(m_centroids), cluster_index(m_cluster_index), final_id(m_final_id)
		{	}

		Compressor(string m_r, vector<string> m_file_list, string m_ref):
		result_name(m_r), file_list(m_file_list), ref_file(m_ref)
		{	}		
		void set_thread_number(int m_thread_number) { thread_number = m_thread_number;}
		virtual void compress();
		virtual void cluster_compress();
		virtual void ref_compress();
	    
		virtual ~Compressor() {}

};


#endif