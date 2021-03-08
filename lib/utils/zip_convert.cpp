#include <zlib.h>
#include <iostream>
#include <fstream>
#include "xzip/xzip.h"
#include "../xtool/xglobal.h"
//#include "../xtool/zip_resource.h"

inline unsigned crt(unsigned& VAL)
{
	VAL ^= VAL >> 3;
	VAL ^= VAL << 28;
	VAL &= 0x7FFFFFFF;

	return VAL;
}

int xtInitApplication() {
	return 1;
}

void xtDoneApplication() {
	
}

/* For simple zlib files (like credits.txt) can use:
 * dd if=./credits.txt bs=1c skip=6 | zlib-flate -uncompress | iconv -f ibm866 -t utf-8
 */

int main(int argc, char** argv) {
//int SDL_main(int argc, char *argv[]) {
	if (argc<3) {
		VNG_DEBUG()<<"need 2 files"<<std::endl;
		return 0;
	}

	
	int i;
	char c, *buf, *out_buf;
	unsigned _time_;
	XStream ff(argv[1], XS_IN);
	std::fstream fh;
	int len = ff.size();
	ff > c;
	if(c) {
		VNG_DEBUG()<<"Not compress file"<<std::endl;
		ff.seek(0,XS_BEG);
		buf = new char[len+1];
		ff.read(buf,len);
		buf[len] = 0;
		ff.close();
	} else {
		VNG_DEBUG()<<"Compress file"<<std::endl;
		ff > _time_;
		_time_ *= 6386891;
		_time_ |= 1;

		int compressed_size = len - 5;
		char* compressed_buff = new char[compressed_size];
		ff.read(compressed_buff,compressed_size);
		ff.close();
		VNG_DEBUG()<<"Read compress file - done."<<std::endl;
		
		for(i = 0;i < compressed_size;i++)
			compressed_buff[i] ^= crt(_time_);

		VNG_DEBUG()<<"Decrypt data - done."<<std::endl;
		
		int decompressed_size = ZIP_GetExpandedSize(compressed_buff);
		out_buf = new char[decompressed_size+1];
		VNG_DEBUG()<<"Uncompress data size:"<<decompressed_size<<std::endl;
		ZIP_expand(out_buf,decompressed_size,compressed_buff,compressed_size);
		//delete compressed_buff;
		//buf[decompressed_size] = 0;
		VNG_DEBUG()<<"Uncompress data - done."<<std::endl;
		fh.open(argv[2], std::fstream::out |  std::fstream::binary);
		fh.write(out_buf, decompressed_size);
		fh.close();
	}
// 	VNG_DEBUG()<<out_buf<<std::endl;
	
	
	/*long sz,sz1;
	char* p,*p1;
	std::fstream fh;
	fh.open(argv[1], std::fstream::in | std::fstream::binary);
	fh.seekg (0, fh.end);
	sz = fh.tellg();
	fh.seekg (0, fh.beg);
	
	p = new char[sz];
	fh.read(p, sz);
	fh.close();

	for (int i=0; i<20; i++) {
		VNG_DEBUG()<<"A:"<<*(unsigned int *)(p+i)<<std::endl;
	}
	sz1 = ZIP_GetExpandedSize(p);
	p1 = new char[sz1];
	ZIP_expand(p1,sz1,p,sz);
	sz = sz1+20;
	p = new char[sz];

	VNG_DEBUG()<<"p1:"<<p1<<" sz1:"<<sz1<<std::endl;
	p[0] = (char)(8 & 0xFF); //8 it is DEFLATE method
	p[1] = (char)((8 >> 8) & 0xFF);
	*(unsigned int*)(p + 2) = (unsigned int)sz1;
	sz-=2+4;
	int stat = compress((Bytef*)(p+2+4),(uLongf*)&(sz),(Bytef*)p1,sz1);
	sz+=2+4;
	switch(stat){
		case Z_OK: VNG_DEBUG()<<"Compress ok."<<std::endl; break;
		case Z_MEM_ERROR: VNG_DEBUG()<<"not enough memory."<<std::endl; break;
		case Z_BUF_ERROR: VNG_DEBUG()<<"not enough room in the output buffer."<<std::endl; break;
	};
	fh.open(argv[2], std::fstream::out |  std::fstream::binary);
	fh.write(p, sz);
	fh.close();*/
	return 0;
}