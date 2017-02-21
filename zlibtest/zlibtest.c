/* zlib test */
/* test example from http://www.zlib.net/zlib_how.html */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

#define CHUNK 128000

int def(FILE *source, FILE *dest, int level)
{
	int ret, flush;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if(ret != Z_OK){
		return ret;
	}

	do{
		strm.avail_in = fread(in, 1, CHUNK, source);
		if(ferror(source)){
			(void)deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source)? Z_FINISH: Z_NO_FLUSH;
		strm.next_in = in;
		do{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);
			have = CHUNK - strm.avail_out;
			if(fwrite(out, 1, have, dest)!=have||ferror(dest)){
				(void)deflateEnd(&strm);
				return Z_ERRNO;
			}
		}while(strm.avail_out == 0);
	}while(flush != Z_FINISH);

	(void)deflateEnd(&strm);
	return Z_OK;
}

int def2(FILE *source, unsigned char *buffer, long *buffer_len, int level)
{
	int ret, flush;
	unsigned have;
	long len = 0, inlen = 0;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	ret = deflateInit(&strm, level);
	if(ret != Z_OK){
		return ret;
	}

	do{
		strm.avail_in = fread(in, 1, CHUNK, source);
		inlen += strm.avail_in;
		if(ferror(source)){
			(void)deflateEnd(&strm);
			return Z_ERRNO;
		}
		flush = feof(source)? Z_FINISH: Z_NO_FLUSH;
		strm.next_in = in;
		do{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = deflate(&strm, flush);
			have = CHUNK - strm.avail_out;
			memcpy(buffer+len,out,have);
			len += have;
		}while(strm.avail_out == 0);
	}while(flush != Z_FINISH);

	(void)deflateEnd(&strm);
	printf("compress rate:[%0.4lf]\n",((double)(inlen-len)/(double)inlen));
	*buffer_len = len;
	return Z_OK;
}

int inf(FILE *source, FILE *dest)
{
	int ret;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);
	if(ret!=Z_OK){
		return ret;
	}

	do{
		strm.avail_in = fread(in, 1, CHUNK, source);
		if(ferror(source)){
			(void)inflateEnd(&strm);
			return Z_ERRNO;
		}
		if(strm.avail_in==0)
			break;
		strm.next_in = in;
		do{
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			switch (ret){
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return ret;
			}
			have = CHUNK - strm.avail_out;
			if(fwrite(out, 1, have, dest)!=have||ferror(dest)){
				(void)inflateEnd(&strm);
				return Z_ERRNO;
			}
		}while(strm.avail_out==0);
	}while(ret != Z_STREAM_END);
	(void)inflateEnd(&strm);
	
	return ret==Z_STREAM_END? Z_OK:Z_DATA_ERROR;
}

void usage(){
	printf("usage:zlibtest -[i|d] infile outfile\n");
	exit(7);
}


int main(int argc, char **argv)
{
	if(argc!=4){
		usage();
	}
	long len;
	unsigned char buf[1024*1024*3]={0};
	FILE *infp;
	FILE *outfp;
	infp = fopen(argv[2],"r");
	outfp = fopen(argv[3],"w");
	if(strcmp(argv[1],"-d")==0){
		def2(infp,buf,&len,9);
		fwrite(buf,1,len,outfp);
	}
	else if(strcmp(argv[1],"-i")==0)
		inf(infp,outfp);
	else{
		usage();
	}

	fclose(infp);
	fclose(outfp);
	return 0;
}
