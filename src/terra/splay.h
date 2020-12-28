#ifndef __TERRA__SPLAY_H__
#define __TERRA__SPLAY_H__

void InitSplay(XStream& fo);
void  decompress_line1(int *decomp_tree, char** in, char** out);
void  decompress_line2(int *decomp_tree, char** in, char** out);
void ExpandBuffer(unsigned char* InBuffer,unsigned char* OutBuffer);

//#define BETTA_TESTING 
#endif