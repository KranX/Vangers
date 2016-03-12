
//typedef unsigned long ulong;

unsigned int ZIP_compress(char* trg,unsigned int trgsize,char* src,unsigned int srcsize);
unsigned int ZIP_GetExpandedSize(char* p);
void ZIP_expand(char* trg,unsigned int trgsize,char* src,unsigned int srcsize);

