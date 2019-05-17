#include <stdio.h>
#include <stdlib.h>
#include "processor.h"
#include "clustering.h"
#include "hirgc-compressor.h"
#include "hirgc-decompressor.h"
#include "../libbsc/bsc.h"


int generate_src_file(char *result,std::vector<string> file_list);
int generate_src_file(char *result, std::vector<int> centroid_index, std::vector<int> cluster_result, std::vector<string> file_list);
void decompress_mode(int argc, char *argv[]);
void compress_mode(int argc, char *argv[], bool compress=true);

static void show_usage()
{
	printf("v2.0 \n"
		   "./ECC p -r result_name -s src_list -t thread_number for reference-target pair selectoin\n"
		   "./ECC c -r result_name -s src_list -t thread_number for reference-target pair selectoin and compress data via hirgc\n"
		   "./ECC d src_file for decompression\n"
		   "Examples:\n"
		   "./ECC p -r my_archive -s file_list.txt -t 4\n"
		   "./ECC p -r my_archive -s file_list.txt -t 4\n"
		   "./ECC d my_archive\n"
		    );
}


int main(int argc, char **argv)
{

	char *mode = argv[1];
	if(argc < 3)
	{
		show_usage();
		return 0;
	}


	if ( strcmp(mode,"p")==0 )
		compress_mode(argc,argv,false);

	else if ( strcmp(mode,"c")==0 )
		compress_mode(argc,argv); 
	
	else if ( strcmp(mode,"d")==0 )
		decompress_mode(argc,argv);
	return 0;	
}

void compress_mode(int argc, char *argv[], bool compress)
{

	struct  timeval  start;
	struct  timeval  end;
	unsigned long timer;
	gettimeofday(&start,NULL);


	if(argc < 3)
	{
		printf("insufficent argument\n");
		return;
	}

	char *result, *buffer, *src;
	int n_count = 1, thread_number = 4;
	bool single_ref = false;
	while( n_count < argc )
	{
		buffer = argv[n_count];
		if( strcmp(buffer,"-s" )== 0 )
			src = n_count < (argc-1) ? argv[++n_count] : NULL;

		else if( strcmp(buffer,"-r") == 0)
			result = n_count < (argc-1) ? argv[++n_count] : NULL;
		
		else if( strcmp(buffer, "-ref") == 0 )
			single_ref = true;  

		else if( strcmp(buffer,"-t") == 0)
		{
			buffer = n_count < (argc-1) ? argv[++n_count] : NULL;			
			thread_number = atoi(buffer);
		}	
		n_count++;
	}	
	printf("read data\n");
	Processor processor(src);
	printf("calculate distance\n");
	vector<vector<double>> smatrix = processor.generate_similarity_matrix();
	vector<string> list = processor.generate_list();
	SubtracitveCluster sc;
	printf("clustering\n");
	vector<int> centroids = sc.fit(smatrix);
	printf("number of centroids: %d\n", centroids.size());
	KMedoids km(smatrix, centroids, list);
	km.run();
	string result_name = "ECC_" + string(result);
	printf("result file path %s_cluster.txt\n",result_name.c_str()); 
	printf("result file path %s_pair.txt\n",result_name.c_str()); 
	printf("result file path %s_seq.txt\n",result_name.c_str()); 	
	km.cluster_result(result_name);   //generate the clusteirng result
	km.sequential_result(result_name);  //generate the result for compressing with one refernce
	km.pair_result(result_name);   //generate each ref-target pair

	if(compress)
	{
		Compressor compressor(result_name.c_str(), km.list, km.centroid_ids, km.cluster_ids, km.final_id);	
		compressor.set_thread_number(thread_number);
		compressor.compress();			
	}
	char cmd[1024];
	sprintf(cmd,"rm -rf %s",result);
	system(cmd);
	gettimeofday(&end,NULL);
	timer = 1000000 * (end.tv_sec - start.tv_sec) + end.tv_usec - start.tv_usec;
	printf("compression takes = %lf ms; %lf min, thread_number = %d\n", timer/1000.0, timer/1000.0/1000.0/60.0,thread_number);		

}


int generate_src_file(char *result,std::vector<string> file_list)
{
	char file_name[1024];
	sprintf(file_name,"%s_src.txt", result);
	FILE *fp = fopen(file_name,"w");
	for(int i = 1; i < file_list.size(); i++)
	{
		fprintf(fp, "%s %s\n", file_list[0].c_str(), file_list[i].c_str());
	}
	fclose(fp);
	return 0;
}


int generate_src_file(char *result, std::vector<int> centroid_index, std::vector<int> cluster_result, std::vector<string> file_list)
{
	char file_name[1024];
	printf("number of cluster: %d\n", centroid_index.size());
	sprintf(file_name,"%s_src.txt", result);
	FILE *fp = fopen(file_name,"w");	
	int count, max_count = -1, final_ref_id = -1;
    for(int i = 0; i < centroid_index.size(); i++)
	{
		if ( centroid_index[i] == -1)
			continue;

		count = 0;
		for(int j = 0; j < cluster_result.size(); j++)
		{
			if(cluster_result[j]==i && j != centroid_index[i])
				count++;
		}

		if(count > max_count)
		{
			max_count = count;
			final_ref_id = centroid_index[i];  
		}			
	}
	for(int i = 0; i < centroid_index.size(); i++)
	{
		if ( centroid_index[i] == -1)
			continue;
					
		int tar_id = centroid_index[i];
		if( tar_id != final_ref_id)
			fprintf(fp,"%s %s\n", file_list[ final_ref_id ].c_str(), file_list[ tar_id ].c_str() );
	}	

    for(int i = 0; i < centroid_index.size(); i++)
	{
		if ( centroid_index[i] == -1)
			continue;

		count = 0;
		for(int j = 0; j < cluster_result.size(); j++)
		{
			if(cluster_result[j]==i && j != centroid_index[i])
				fprintf(fp,"%s %s\n",file_list[ centroid_index[i] ].c_str(), file_list[j].c_str() );
		}
	}	
	fclose(fp);
	return final_ref_id;
}




void decompress_mode(int argc, char *argv[])
{
	printf("decompress file\n");
	string src_name = "ECC_"+string(argv[2]);
	mkdir(src_name.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	printf("archive name: %s\n",src_name.c_str());
	char cmd[1024];
	sprintf(cmd, "tar -C %s -xvf %s.tar",src_name.c_str(),src_name.c_str());
	system(cmd);
	FILE *fp = fopen( (src_name+"_pair.txt").c_str(),"r");
	if( fp == NULL)
	{
		printf("failed to open file %s\n",(src_name+"_pair.txt").c_str());
		exit(0);
	}

	std::vector<string> ref_list;
	std::vector<string> tar_list;
	std::vector<string> refs;
	struct stat stat_buffer; 
	if ( stat ((src_name+"_ref").c_str(), &stat_buffer) != 0)
	{
		printf("error, %s not found \n", (src_name+"_ref").c_str() );
		exit(0);
	}


	char temp_ch[1024];
	string temp_ref=" ";
	printf("begin to read src\n");
	while(fscanf(fp,"%s",temp_ch) != EOF)
	{
		ref_list.push_back( string(temp_ch) );
		if ( temp_ref != string(temp_ch))
		{
			temp_ref = string(temp_ch);
			refs.push_back(temp_ref);
			printf("add ref %s\n",temp_ref.c_str());
		}
		fscanf(fp,"%s",temp_ch);
		tar_list.push_back( string(temp_ch) );
	}


	spring::bsc::BSC_decompress( (src_name+"_ref").c_str(), (src_name+"/"+ref_list[0]).c_str() );
	chdir(src_name.c_str());
	for (int i = 0; i < refs.size(); i++)
	{
		spring::bsc::BSC_decompress( (refs[i]+"_ref.tar.bsc").c_str(), (refs[i]+".tar").c_str() );
		remove((refs[i]+"_ref.tar.bsc").c_str());
		sprintf(cmd,"tar -xvf %s.tar",refs[i].c_str());
		system(cmd);
		sprintf(cmd,"rm -r %s.tar",refs[i].c_str());
		system(cmd);
	}
//	sprintf(cmd,"./bsc d %s_ref %s",src_name.c_str(),ref_list[0].c_str());
	deCompressor decomp(ref_list[0]);
	char buffer1[1024];
	char buffer2[1024];
	for(int i  = 0; i < ref_list.size(); i++)
	{
		sprintf(buffer1,"%s",ref_list[i].c_str());
		sprintf(buffer2,"%s",tar_list[i].c_str());

		decomp.decompress(buffer1, buffer2 );
	}

	return;
}
