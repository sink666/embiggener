/* png embiggener */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
biggen_itxt(unsigned char **dataptr, size_t *sizeptr)
{
	stringtoadd[]="What the fuck did you just fucking say about me, you little bitch?";



}

struct chunkadder{
	char const *name;
	void (*func)(unsigned char **dataptr, size_t *sizeptr);
};

struct chunkadder const chunkadderlist[]={
	{"iTXt", biggen_itxt},
};

struct chunkdata{
	unsigned char name[4];
	size_t length;
	unsigned char *data;
	unsigned char crc[4];
};

int
main(int argc, char *argv[])
{
	if(argc!=2){
		puts("incorrect number of arguments");
		return 1;
	}

	char *fn=argv[1];
	FILE *fp=fopen(fn, "r");
	if(!fp){
		perror("could not open file");
		return 1;
	}

	char header[8];
b	fread(header, 1, 8, fp);
	if(memcmp(header, "\x89PNG\r\n\x1a\n", 8)!=0){
		perror("png header damaged or not present");
		return 1;
	}

	puts("header read successfully");

	unsigned char tempdata[4];
	unsigned char *chunkdata=NULL;
	struct chunkdata *cdataarray=NULL;
	size_t cdatalength=0;
	for(;;){
		fread(tempdata, 1, 4, fp);

		if(feof(fp))
			break;

		cdatalength++;
		cdataarray=realloc(cdatalength*sizeof(struct chunkdata));

		struct chunkdata *cmember=&cdataarray[cdatalength-1];

		/* transform chunk length from bytes to size_t */
		cmember->length=((size_t)tempdata[0]<<24)+
			((size_t)tempdata[1]<<16)+
			((size_t)tempdata[2]<<8)+
			((size_t)tempdata[3]);

		/* read chunk name */
		fread(cmemeber->name, 1, 4, fp);

		/* allocate and read chunk data to structure */
		cmember->data=malloc(cmember->length);
		fread(cmember->data, 1, cmember->length, fp);
		fread(cmember->crc, 1, 4, fp);

	}

	fflush(stdout);
	putchar('\n');

	if(ferror(fp))
		perror("io error when reading");
	if(feof(fp))
		perror("eof reached");

	fclose(fp);

	return 0;
}

/* eof */
