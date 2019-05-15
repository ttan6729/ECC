#include "hirgc-compressor.h"
#include "../libbsc/bsc.h"
void Compressor::compress()
{
	char cmd[1024];
	sprintf(cmd,"rm -rf %s",result_name.c_str() );
	system(cmd);
	sprintf(cmd,"mkdir -p %s",result_name.c_str());
	system(cmd);
	

	if( cluster_index.size() > 0 )
		cluster_compress();

	else if ( ref_file != "" )
	{
		printf("compress with first file as reference\n");
		ref_compress();
	}

}


void Compressor::cluster_compress()
{
	printf("number of files: %d\n",file_list.size());

	char res_folder[1024];
	sprintf(res_folder,"%s",result_name.c_str());

	system(("rm -rf "+ result_name).c_str());
	thread *threads[thread_number];
	hirgc *hirgcs[thread_number];
	int start = 0, task_number = file_list.size();
	int end = start;
	int avg_task = task_number/thread_number;
	int moduler = task_number%thread_number;

	for (int i = 0; i < thread_number; i++)
	{
		end +=  i < moduler ? avg_task+1 : avg_task;
		hirgcs[i] = new hirgc(file_list,start,end,i,final_id,cluster_index,centroids);
		threads[i] = new thread(&hirgc::hirgc_cluster_compress,hirgcs[i],res_folder);
		start = end;
	}

	for (int i = 0; i < thread_number; i++)
	{
		threads[i]->join();
		delete hirgcs[i];
		delete threads[i];
	}

	string ref_name;
	char cmd[1024];
	char current_directory[1024];
	char folder_buffer[1024];
	getcwd(current_directory, 1024);

	spring::bsc::BSC_compress( file_list[final_id].c_str(),(result_name+"_ref").c_str());

	chdir(res_folder);		
	for (int i = 0; i < centroids.size(); i++)
	{
		ref_name = strip_string(file_list[centroids[i]]);		
		//create new folder
		sprintf(folder_buffer,"%s_ref",ref_name.c_str());
		mkdir(folder_buffer,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		//move files to new folder
		sprintf(cmd,"mv *_ref_%s %s  2>/dev/null",ref_name.c_str(),folder_buffer);
		system(cmd);
		sprintf(cmd,"tar -cf %s.tar -C %s .",folder_buffer,folder_buffer);
		system(cmd);		
		//bsc compression
		spring::bsc::BSC_compress( (string(folder_buffer)+".tar").c_str(), (string(folder_buffer)+".tar.bsc").c_str() );

		/**
		sprintf(folder_buffer,"%s_ref",ref_name.c_str());
		sprintf(cmd,"mkdir %s",folder_buffer);
		system(cmd);                
		sprintf(cmd,"mv *_ref_%s %s",ref_name.c_str(),folder_buffer);
		system(cmd);
		sprintf(cmd,"tar -cf %s.tar -C %s .",folder_buffer,folder_buffer);
		system(cmd);
		sprintf(cmd,"%s/bsc e %s.tar %s.tar.bsc -b64p",current_directory,folder_buffer,folder_buffer);
		printf("bsc command is %s\n",cmd);
		system(cmd); **/		
	}



	mkdir("bsc_files",S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	sprintf(cmd,"mv *.bsc bsc_files");
	system(cmd);
	sprintf(cmd,"tar -cf %s.tar -C bsc_files .",res_folder);
	system(cmd);
	chdir(current_directory);
	sprintf(cmd,"mv %s/%s.tar ./",res_folder,res_folder);
	system(cmd);
}

void Compressor::ref_compress()
{
	printf("number of files: %d\n",file_list.size());
	char cmd[1024];;
	char ref[1024];
	sprintf(ref,"%s",ref_file.c_str());

	char res_folder[1024];
	sprintf(res_folder,"%s_ref",ref);

	sprintf(cmd,"mkdir  %s",res_folder);
	system(cmd);
	
	thread *threads[thread_number];
	hirgc *hirgcs[thread_number];
	int start = 0, task_number = file_list.size();
	int end = start;
	int avg_task = task_number/thread_number;
	int moduler = task_number%thread_number;

	for (int i = 0; i < thread_number; i++)
	{
		end +=  i < moduler ? avg_task+1 : avg_task;
		hirgcs[i] = new hirgc(file_list,start,end,i);
		threads[i] = new thread(&hirgc::hirgc_single_ref_compress,hirgcs[i],res_folder,ref);
		start = end;
	}

	for (int i = 0; i < thread_number; i++)
	{
		threads[i]->join();
		delete hirgcs[i];
		delete threads[i];
	}


	char folder_buffer[1024];

	sprintf(cmd,"tar -cf %s.tar -C %s .",res_folder,res_folder);
	system(cmd);

	sprintf(cmd,"./bsc e %s.tar %s.tar.bsc -b64p",res_folder,res_folder);
	system(cmd);

	sprintf(cmd,"mkdir %s",result_name.c_str());
	system(cmd);
	sprintf(cmd,"mv %s.tar.bsc %s ",res_folder,result_name.c_str());
	system(cmd);
	sprintf(cmd,"tar -cf %s.tar -C %s .",result_name.c_str(),result_name.c_str());		
	system(cmd);
	return;
} 