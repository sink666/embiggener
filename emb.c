/* png embiggener */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/* copied from rosettacode */
uint32_t
rc_crc32(uint32_t crc, const char *buf, size_t len)
{
	static uint32_t table[256];
	static int have_table = 0;
	uint32_t rem;
	uint8_t octet;
	int i, j;
	const char *p, *q;

	/* This check is not thread safe; there is no mutex. */
	if (have_table == 0) {
		/* Calculate CRC table. */
		for (i = 0; i < 256; i++) {
			rem = i;  /* remainder from polynomial division */
			for (j = 0; j < 8; j++) {
				if (rem & 1) {
					rem >>= 1;
					rem ^= 0xedb88320;
				} else
					rem >>= 1;
			}
			table[i] = rem;
		}
		have_table = 1;
	}

	crc = ~crc;
	q = buf + len;
	for (p = buf; p < q; p++) {
		octet = *p;  /* Cast to unsigned octet. */
		crc = (crc >> 8) ^ table[(crc & 0xff) ^ octet];
	}
	return ~crc;
}
/* end of copying from rosettacode */

struct chunkdata{
	unsigned char name[4];
	size_t length;
	unsigned char *data;
	unsigned char crc[4];
};

void
biggen_text(struct chunkdata *memberptr)
{
	unsigned char name[4]="tEXt";
	memcpy(&memberptr->name[0], &name, sizeof(name)*1);

        unsigned char keytoadd[80]=
	"69696969696969696969696969696969696969696969696969696969696969696969:";
	unsigned char valtoadd[]=
	"What the fuck did you just fucking say about me, you little bitch?";

	memberptr->length=sizeof(keytoadd)+sizeof(valtoadd)-1;
	memberptr->data=malloc(memberptr->length);

	memcpy(&memberptr->data[0], &keytoadd, sizeof(keytoadd)*1);
	memcpy(&memberptr->data[81], &valtoadd, sizeof(valtoadd)*1);

	unsigned char arr[4];
	uint32_t crc=rc_crc32(0, memberptr->data, memberptr->length);

	arr[0]=(0xff<<24&crc)>>24;
	arr[1]=(0xff<<16&crc)>>16;
	arr[2]=(0xff<<8 &crc)>> 8;
	arr[3]=(0xff<<0 &crc)>> 0;

	memcpy(&memberptr->crc[0], &arr, sizeof(arr)*1);
}

/* struct chunkadder{ */
/* 	char const *name; */
/* 	void (*func)(struct chunkdata *memberptr); */
/* }; */

/* struct chunkadder const chunkadderlist[]={ */
/* 	{"tEXt", biggen_text}, */
/* }; */

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

	unsigned char tempdata[4];
	struct chunkdata *cdataarray=NULL;
	size_t cdatalen=0;
	for(;;){
		fread(tempdata, 1, 4, fp);

		if(feof(fp))
			break;

		cdatalen++;
		cdataarray=realloc(cdataarray, cdatalen*sizeof(struct chunkdata));

		struct chunkdata *cmember=&cdataarray[cdatalen-1];

		/* transform chunk length from bytes to size_t */
		cmember->length=((size_t)tempdata[0]<<24)+
			((size_t)tempdata[1]<<16)+
			((size_t)tempdata[2]<<8)+
			((size_t)tempdata[3]);

		/* read chunk name */
		fread(cmember->name, 1, 4, fp);

		/* allocate and read chunk data to structure */
		cmember->data=malloc(cmember->length);
		fread(cmember->data, 1, cmember->length, fp);
		fread(cmember->crc, 1, 4, fp);

	}

	/* add chunks to file */
	for(;;)
	{
		cdatalen++;
		cdataarray=realloc(cdataarray, sizeof(struct chunkdata)*cdatalen);
		memmove(&cdataarray[1], &cdataarray[0], sizeof(struct chunkdata)*(cdatalen-1));
		biggen_text(&cdataarray[1]);
		break;
	}

	fwrite("\x89PNG\r\n\x1a\n", 1, 8, stdout);

	for(size_t i=0; i<cdatalen; i++){
		unsigned char temp[4];
		temp[0]=(0xff<<24&cdataarray[i].length)>>24;
		temp[1]=(0xff<<16&cdataarray[i].length)>>16;
		temp[2]=(0xff<<8 &cdataarray[i].length)>> 8;
		temp[3]=(0xff<<0 &cdataarray[i].length)>> 0;
		fwrite(&temp[0], 1, 4, stdout);
		fwrite(cdataarray[i].name, 1, 4, stdout);
		fwrite(cdataarray[i].data, 1,
		       cdataarray[i].length, stdout);
		fwrite(cdataarray[i].crc, 1, 4, stdout);
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
