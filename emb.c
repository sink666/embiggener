/* png embiggener */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
biggen_idat(unsigned char **dataptr, size_t *sizeptr)
{

}

void
biggen_iccp(unsigned char **dataptr, size_t *sizeptr)
{

}

struct chunk{
	char const *name;
	void (*func)(unsigned char **dataptr, size_t *sizeptr);
};

struct chunk const chunktypelist[]={
	{"IDAT", biggen_idat},
	{"iCCP", biggen_iccp},
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
	fread(header, 1, 8, fp);
	if(memcmp(header, "\x89PNG\r\n\x1a\n", 8)!=0){
		perror("png header damaged or not present");
		return 1;
	}

	puts("header read successfully");

        /* REPL: read, embiggen, print, loop */
	unsigned char chunkheader[4];
	unsigned char *chunkdata=NULL;
	for(;;){
		fread(chunkheader, 1, 4, fp);

		if(feof(fp))
			break;

		size_t chunksize;
		chunksize=((size_t)chunkheader[0]<<24)+
			((size_t)chunkheader[1]<<16)+
			((size_t)chunkheader[2]<<8)+
			((size_t)chunkheader[3]);

		fread(chunkheader, 1, 4, fp);



		chunkdata=realloc(chunkdata, chunksize);
		fread(chunkdata, 1, chunksize, fp);
		fread(chunkheader, 1, 4, fp);

		for(size_t b=0; b<chunksize; b++)
			putchar(chunkdata[b]);

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
