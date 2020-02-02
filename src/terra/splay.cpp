#include "../global.h"

#include "splay.h"

static int* decomp_tree1 = NULL;
static int* decomp_tree3 = NULL;

static unsigned char* decomp_tree_char1;
static unsigned char* decomp_tree_char3;


void InitSplay(XStream& fo){
	std::cout<<"Start load decomp trees"<<std::endl;
	if(!decomp_tree1) 
		decomp_tree1 = new int[512 + 512/sizeof(int)];
	if(!decomp_tree3)
		decomp_tree3 = new int[512 + 512/sizeof(int)];

	decomp_tree_char1 = (unsigned char *)(decomp_tree1 + 512);
	decomp_tree_char3 = (unsigned char *)(decomp_tree3 + 512);

	fo.read( decomp_tree1, sizeof( int )*512 );
	fo.read( decomp_tree3, sizeof( int )*512 );

	for( int i = 0; i < 512; i++) {
		decomp_tree_char1[i] = -decomp_tree1[i];
		decomp_tree_char3[i] = -decomp_tree3[i];
	}
}

void decompress_line1(int *decomp_tree, char** inBuf, char** outBuf){
	unsigned int	cindex = 1;
	char		curchar;
	char		lastchar = 0;
	int		bitshift;
	unsigned int charcount = (1<<11);

	char *in = *inBuf, *out = *outBuf;

	while(charcount){
		curchar = *in++;
		for (bitshift = 7; bitshift >= 0; --bitshift){
			cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

			if(decomp_tree[cindex] <= 0){
				lastchar += ((char *)(decomp_tree + 512))[cindex];
				*out++ = lastchar;

				if(--charcount) cindex = 1;
				else{
					*inBuf = in;
					*outBuf = out;
					return;
				}
			}else
				cindex = decomp_tree[cindex];
		}//  end for bitshift
	}//  end while


}

void decompress_line2(int *decomp_tree, char** inBuf, char** outBuf){
	unsigned int	cindex = 1;
	char		curchar;
	char		lastchar = 0;
	int		bitshift;
	unsigned int charcount = (1<<11);

	char *in = *inBuf, *out = *outBuf;

	while(charcount){
		curchar = *in++;
		for (bitshift = 7; bitshift >= 0; --bitshift){
			cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

			if(decomp_tree[cindex] <= 0){
				lastchar ^= ((char *)(decomp_tree + 512))[cindex];
				*out++ = lastchar;

				if(--charcount) cindex = 1;
				else{
					*inBuf = in;
					*outBuf = out;
					return;
				}
			}else
				cindex = decomp_tree[cindex];
		}//  end for bitshift
	}//  end while
}

void ExpandBuffer(unsigned char* InBuffer,unsigned char* OutBuffer){
	decompress_line1(decomp_tree1, (char**)&InBuffer, (char**)&OutBuffer);
	decompress_line2(decomp_tree3, (char**)&InBuffer, (char**)&OutBuffer);
}
