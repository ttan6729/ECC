#ifndef ECC_DECOMPRESSOR_H_
#define ECC_DECOMPRESSOR_H_



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



class deCompressor
{
public:
	char *meta_data;
	POSITION_RANGE *pos_vec, *n_vec;
	POSITION_OTHER_CHAR *other_char_vec;
	int *line_break_vec;
	char *ref_seq;
	char *tar_seq;
	int other_char_len, str_len, tar_seq_len, ref_seq_len, pos_vec_len, n_vec_len, line_break_len;


	string ref_name;
	deCompressor(string m_ref_name)
	{
		ref_name = m_ref_name;
		initial();
		char buffer[1024];
		sprintf(buffer,"%s",ref_name.c_str());
		readRefFile(buffer);
	}

	void decompress(char *new_ref, char *target)
	{
		if ( ref_name != string(new_ref)  )
		{
			readRefFile(new_ref);
			ref_name = string(new_ref);
		}

		char src[1024];
		sprintf(src,"%s_ref_%s",target,new_ref);
		printf("%s\n",target);
		readCompressedFile(src);
	// cerr << "after readCompressedFile()...\n";
		saveDecompressedData(target);
		char cmd[1024];
		sprintf(cmd,"rm %s",src);
		system(cmd);
		return;
	}
	~deCompressor() { clear(); }

	void initial();
	void clear();
	void readRefFile(char *refFile);
	void readCompressedFile(char *tarFile);
	void saveDecompressedData(char *resultFile);
	void readRunLengthCoding(FILE *fp, int &vec_len, int **vec);
	void readOtherData(FILE *fp); 
	bool exitSpace(char *str);
	void decompressFile(char *refFile, char *tarFile, char *resultFile);
	void decompressFile(char *refFile, char *tarFile);
	bool getList(char *list_file, vector<string> &name_list);	


};


#endif