#include "util.h"

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