#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <errno.h>  

int debug_log = 0, pading_size = 0;

int convert_dword_to_bit(unsigned int p, char * szline)
{
	int i = 0;
	for(i = 0; i< 32; i++) {
		if(((p >> (31 - i)) & 1) > 0)
			strcat(szline, "1");
		else
			strcat(szline, "0");		
	}
	if(debug_log > 0)
		sprintf(szline + strlen(szline), "==>%08x", p);

	strcat(szline, "\r\n");
	return 0;
}

int main(int argc, char** argv)
{
	FILE * fp_r, *fp_w;
	int file_size, i;
	unsigned int *p;
	unsigned char * rd_buf = NULL;
	char wname[128];
	char szline[128];

	debug_log = 0;
	if(argc >=5 )
		debug_log = 1;

	if(debug_log > 0) {
		printf("Start convert bootrom bin to txt file, %d, %s\n", argc, argv[1]);
		printf("Usage: \t0: in file\n \t1: out file \t2: output_size(kBytes)\n \t3: debug_log\n");
	}
	if(argc < 4) {
		printf("Need have >=4 parameters\n");
		return 0;
	}

	if(argc >=5 )
		debug_log = 1;

	pading_size = atoi(argv[3]);
	pading_size *= 1024;
	
	if(debug_log > 0)
		printf("pading_size =%d, %dkBytes\n", pading_size, pading_size/1024);	

	fp_r = fopen (argv[1], "rb");

	if (fp_r == NULL) {
		printf("Open %s Failed, return\n", argv[1]);
		return 1;
	}
	
	sprintf(wname, "%s", argv[2]);

	fp_w = fopen (wname, "w");
	if (fp_w == NULL) {
		printf("Open %s Failed, return\n", wname);
		return 1;
	}
	
	fseek(fp_r, 0L, SEEK_END);  
	file_size = ftell(fp_r);  
	fseek(fp_r, 0L, SEEK_SET);
  
	rd_buf = (char *) malloc (file_size);
	if(rd_buf == NULL)
		return 1;

	if (fread (rd_buf, 1, file_size, fp_r) != file_size)
		return 1;
	p = (unsigned int *)rd_buf;

	for(i = 0; i< file_size/4; i++) {
		szline[0] = 0;

		if(debug_log > 0)
			printf("0x%08x \n", *p);
		convert_dword_to_bit(*p, szline);
		fprintf(fp_w, "%s", szline);
		p ++;
	}
	if(pading_size > file_size) {
		for(i = 0; i< (pading_size - file_size) /4 ; i++) {
			szline[0] = 0;
			convert_dword_to_bit(0,szline);
			fprintf(fp_w, "%s", szline);
		}
	}
	free(rd_buf);
	fclose(fp_r);
	fclose(fp_w);
	return 0;
}
