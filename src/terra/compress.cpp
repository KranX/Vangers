#include "../global.h"

#include "huff1.h"

void make_encode_data(DHuff&);
void CompressMap( char* name, int X_SHIFT, int Y_SHIFT );

void make_stat( unsigned char* in, long* stat1, long* stat3 ){
	int i;
	int i1;

	for( i = 1; i < (1<<11); i++){
		stat1[ (unsigned char)(in[i]) ]++;
	}
	i++;
	for( i1 = 1; i1 < (1<<11); i1++,i++){
		stat3[ (unsigned char)(in[i]) ]++;
	}
}

void make_encode_data(DHuff& h){
     h.build_initial_heap();
     h.build_code_tree();
     if (!h.generate_code_table ())
	ErrH.Abort("ERROR!  Code Value Out of Range. Cannot Compress.\n");
}

void CompressMap( char* name, int X_SHIFT, int Y_SHIFT ){
	int i;
	long* stat1 = new long[256];
	long* stat3 = new long[256];
	unsigned char *Buffer;
	char Name[128];
	DHuff h1,  h3;

	Buffer = new unsigned char[2*(1<<X_SHIFT)];
	int *LinePos = new int[(1<<Y_SHIFT)];
	short *LineSize = new short[1<<Y_SHIFT];
	char *Out = new char[2*(1<<X_SHIFT)];

	std::cout<<"\n\nThe first start of game detected, please wait for some data preparing...";

	XStream a;
	XStream fo;

	memset( stat1, 0, sizeof( long )*256 );
	memset( stat3, 0, sizeof( long )*256 );

	a.open( name, XS_IN);

	for ( i = 0; i < (1<<Y_SHIFT); i++ ){
		a.read(Buffer, 2*(1<<X_SHIFT) );
		make_stat( Buffer, stat1, stat3 );
	}
	a.close();

	memcpy(  h1.frequency_count, stat1, sizeof(long)*256);
	memcpy( h3.frequency_count, stat3, sizeof(long)*256);

	strcpy( Name, name );
	Name[ strlen( Name )-1 ] = 'c';

	a.open( name, XS_IN);
	fo.open( Name, XS_OUT);

	for ( i = 0; i < (1<<Y_SHIFT); i++ ){
		fo < LinePos[0];
		fo < LineSize[0];
	}

	make_encode_data(h1);
	make_encode_data(h3);

	h1.build_decomp_tree ();
	h3.build_decomp_tree ();

	fo.write( h1.decomp_tree, sizeof(int)*512 );
	fo.write( h3.decomp_tree, sizeof(int)*512 );

	int CurrentPos = fo.tell();
	int LenAll2 = 0;

	std::cout<<"\nProgress: ";
	for ( i = 0; i < (1<<Y_SHIFT); i++ ){
		int LenLine = 0;
		if(!(i%256)) std::cout<<"+";

		a.read(Buffer, 2*(1<<X_SHIFT) );

		h1.compress_line( Buffer, Out );
		fo.write( Out, h1.compress_charcount );
		LenLine += h1.compress_charcount;

		h3.compress_line( Buffer+(1<<X_SHIFT), Out );
		fo.write( Out, h3.compress_charcount );
		LenLine += h3.compress_charcount;

		LinePos[i] = CurrentPos;
		LineSize[i] = LenLine;
		LenAll2 += LenLine;
		CurrentPos += LenLine;
	}
	fo.seek( 0, XS_BEG );

	for ( i = 0; i < (1<<Y_SHIFT); i++ ){
		fo < LinePos[i];
		fo < LineSize[i];
	}

	std::cout<<"\nPreparing completed...";

	fo.close();
	a.close();

	delete[] Buffer;
	delete[] Out;
	delete[] LinePos;
	delete[] LineSize;
}
