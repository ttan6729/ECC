#ifndef ECC_PROCESSOR_H_
#define ECC_PROCESSOR_H_

#include <string>
#include <omp.h>
#include <vector>
#include <map>

#include "util.h"


#include "../Mash/src/mash/CommandBounds.h"
#include "../Mash/src/mash/CommandList.h"
#include "../Mash/src/mash/CommandSketch.h"
#include "../Mash/src/mash/CommandFind.h"
#include "../Mash/src/mash/CommandDistance.h"
#include "../Mash/src/mash/CommandScreen.h"
#include "../Mash/src/mash/CommandTriangle.h"
#include "../Mash/src/mash/CommandContain.h"
#include "../Mash/src/mash/CommandInfo.h"
#include "../Mash/src/mash/CommandPaste.h"

#include "../Mash/src/mash/Sketch.h"
#include <unistd.h>
#include <zlib.h>
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
#include <map>
#include "../Mash/src/mash/kseq.h"
#include "../Mash/src/mash/MurmurHash3.h"
#include <assert.h>
#include <queue>
#include <deque>
#include <set>
#include "../Mash/src/mash/Command.h" // TEMP for column printing
#include <sys/stat.h>
#include <capnp/message.h>
#include <capnp/serialize.h>
#include <sys/mman.h>
#include <math.h>
#include <list>
#include <string.h>


/**
Used for rading a list sequences and compute the similairty matrix based on Minhash
**/
class Processor 
{
public:
	//first is the orginal src file, second is corresponding file for distance calcultation, can be identical
	map<string, string> src_map;
//	vector<string> file_list;
//	vector<string> tmp_list;

	Processor(char *src_fp)
	{
		read_src_list(src_fp);
	}
	~Processor() {}
	vector<vector<double>> generate_similarity_matrix();
	vector<string> generate_list();
private:
	void read_src_list(char *src_file);
	void row_computation(vector<vector<double>> &vv, int i);
};




#endif