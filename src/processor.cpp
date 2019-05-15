#include <fstream>
#include <cstdint>
#include "processor.h"
#define buff_size 1 << 20

vector<string> v1;
vector<string> v2;

FILE *my_fopen(char *file_name, const char *mode)
{
	FILE *fp = fopen(file_name,mode);
	if(fp == NULL)
	{
		printf("error, failed to open file %s\n",file_name);
		exit(0);
	}

	return fp;
}


/**
void Processor::pro(char *input, char *output)
{
	ifstream fin;
	odstream fout;
	fin.seekg(0, is.end);
	fout.open(output,ios::out);
	fin.open(input,ios::out);

} **/


void Processor::read_src_list(char *src_fp)
{
	FILE *file = my_fopen(src_fp,"r");
	char file_name[256], temp[256];
	char *buff = new char[buff_size];
	string suffix = ".msh";
	ifstream fin;
	ofstream fout;

	while (fscanf(file,"%s", file_name) != EOF )
	{
		fin.open(file_name,ifstream::binary);
		fin.seekg(0, fin.end);

		if( fin.tellg() <= buff_size)
		{
			src_map.insert( pair<string,string>(string(file_name),string(file_name)) );		
			v1.push_back(string(file_name));
			v2.push_back(string(file_name));
		}
		else
		{
			fin.seekg(0, fin.beg);
			sprintf(temp,"tmp_%s",file_name);
			fin.read(buff, buff_size);
			fout.open(temp,ifstream::binary);
			fout.write(buff,buff_size);
			fout.close();
			src_map.insert( pair<string,string>(string(file_name),string(temp)) );
			v1.push_back(string(file_name));
			v2.push_back(string(temp));
		}
		fin.close();
	}
	printf("begin sketch\n");
	
/**	for(int i = 0; i < v1.size(); i++)
	{
		printf("%d\n",i);
		printf("%s\n",v1[i].c_str());
		v1.push_back(v1[i]);
		v2.push_back(v2[i]);
	}
**/
	//	for(map<string,string>::iterator it = src_map.begin(); it != src_map.end(); it++)
	//  cs->run(it->second);
	mash::CommandSketch *cs = new mash::CommandSketch();	
	for(int i = 0; i < v2.size(); i++)
		cs->run(v2[i]);

	fclose(file);
	delete[] buff;
	return;
}

vector<string> Processor::generate_list()
{
    return v1;
}

void normalize(vector<vector<double>> &vv)
{
	int i,j,n = vv.size();
	double min = vv[0][0],max = vv[0][0];
    for(i = 0; i < n; i++)
    {
    	for( j = 0; j < n; j++)
    	{
    		if(min > vv[i][j])
    			min = vv[i][j];
    		if(max < vv[i][j])
    			max = vv[i][j];
    	}
    }

    double interval = max - min;
    for(i = 0; i < n; i++)
    {
    	for( j = 0; j < n; j++)
    		vv[i][j] = min + (vv[i][j] - min)/interval;
    }
}


void Processor::row_computation(vector<vector<double>> &vv, int i)
{
	/**
	int j,n = vv.size();
	double dist;
	string suffix = ".msh";
	mash::CommandDistance *cd = new mash::CommandDistance();
	printf("new %dth row\n",i);	
	map<string,string>::iterator it = src_map.begin();
    for( j = i+1; j < n; j++)
    {
    printf("%s and %s\n",(next(it,i)->second).c_str(), (next(it,j)->second).c_str());
   	    printf("%d %d\n",i,j);		
   		cd->run(next(it,i)->second+suffix,next(it,j)->second+suffix,&dist);
	  	vv[i][j] = dist;
 	  	vv[j][i] = dist;	  		
    }
    delete cd;	
    **/
}


vector<vector<double>> Processor::generate_similarity_matrix()
{
	string suffix = ".msh";
	vector<vector<double>> vv;
	printf("number of files: %d\n", v1.size());
    int i, j, n = v2.size();
    for( i = 0; i < n; i++)
    	vv.push_back( vector<double>(n,0.0) );
    double dist;
 //   map<string,string>::iterator it = src_map.begin();

    vector<string> v;
    for( i = 0; i < n; i++)
    	v.push_back( v2[i] +suffix );

	mash::CommandDistance *cd = new mash::CommandDistance();
	cd->run(v, vv);
	delete cd;

    
    for(i = 0; i < n; i++) 
   	{
   		if(v1[i]!=v2[i])
			remove( v2[i].c_str() );
   		remove( (v2[i] +suffix).c_str() );
	} 
    normalize(vv);  
	return vv;
}


