#include "../global.h"

/**************************************************************************
 *                                                                        *
 *  HUFF1.C:    Huffman Compression Program.                              *
 *              14-August-1990    Bill Demas          Version 1.0         *
 *		               						  *
 *   This program compresses a file using the Huffman codes.              *
 *                                                                        *
 *           USAGE:   HUFF1 <input file> <output file>                    *
 *		               						  *
 *   (DISK to DISK:  Input direct from disk, output direct to disk)       *
 **************************************************************************/

#include "huff1.h"
																			  
/**************************************************************************

 COMPRESS_IMAGE ()

 This function performs the actual data compression.
 **************************************************************************/

DHuff::DHuff(void){
	code = new unsigned int[256];
	code_length = new unsigned char[256];

	father = new int[512];
	heap = new unsigned long[257];
	frequency_count = new long[512];
    decomp_tree = new int[512];

	memset( decomp_tree, 0, 512*sizeof( int ));
}

DHuff::~DHuff(void){
	delete code;
	delete code_length;

	delete father;
	delete heap;
	delete frequency_count;
	delete decomp_tree;
}

void DHuff::compress_line(unsigned char *in, char *out){
   unsigned char    thebyte = 0;
   short           loop1;
   unsigned int  loop;

   unsigned short   curbyte = 0;
   int           curbit = 7;

   for (loop = 0; loop < (1<<11); loop++) {
      unsigned char dvalue = *in++;
      unsigned int  current_code  = code[dvalue];
      int current_length = code_length[dvalue];

      for (loop1 = current_length-1; loop1 >= 0; --loop1) {
         if ((current_code >> loop1) & 1)
				thebyte |= (char) (1 << curbit);

         if (--curbit < 0) {
            *out++ = thebyte;
            thebyte = 0;
            curbyte++;
            curbit = 7;
         }
      }//  end for loop1
   }
   if ( curbit != 7){
	   *out++ = thebyte;
	   compress_charcount = ++curbyte;
   } else
	    compress_charcount = curbyte;
}

/**************************************************************************

 GENERATE_CODE_TABLE ()

 This function generates the compression code table.
 **************************************************************************/

unsigned short DHuff::generate_code_table (void){
   unsigned short  loop;
   unsigned char  current_length;
   unsigned int  current_bit;

   unsigned int  bitcode;
   int           parent;


   for (loop = 0; loop < 256; loop++)
      if (frequency_count[loop]) {
         current_length = bitcode = 0;
         current_bit = 1;
         parent = father[loop];

         while (parent) {
            if (parent < 0) {
               bitcode += current_bit;
               parent = -parent;
            }
            parent = father[parent];
            current_bit <<= 1;
            current_length++;
         }

         code[loop] = bitcode;

         if (current_length > 32)
            return (0);
         else
            code_length[loop] = (unsigned char) current_length;
      }
      else
         code[loop] = code_length[loop] = 0;

   return (1);
}


/**************************************************************************

 BUILD_CODE_TREE ()

 This function builds the compression code tree.
 **************************************************************************/

void DHuff::build_code_tree (void){

   unsigned short  findex;
   unsigned long   heap_value;


   while (heap_length != 1){
      heap_value = heap[1];
      heap[1]    = heap[heap_length--];

      reheap(1);
      findex = heap_length + 255;

      frequency_count[findex] = frequency_count[heap[1]] +  frequency_count[heap_value];
      father[heap_value] =  findex;
      father[heap[1]]    = -findex;
      heap[1]            =  findex;

      reheap (1);
   }

   father[256] = 0;
}


/**************************************************************************

 REHEAP ()

 This function creates a "legal" heap from the current heap tree structure.
 **************************************************************************/

void DHuff::reheap (unsigned short  heap_entry){
   unsigned short  index;
   unsigned short  flag = 1;

   unsigned long   heap_value;

   heap_value = heap[heap_entry];

   while ((heap_entry <= (heap_length >> 1)) && (flag)) {
      index = heap_entry << 1;

      if (index < heap_length)
         if (frequency_count[heap[index]] >= frequency_count[heap[index+1]])
            index++;

      if (frequency_count[heap_value] < frequency_count[heap[index]])
			flag--;
      else {
         heap[heap_entry] = heap[index];
         heap_entry       = index;
      }
   }//  end while

   heap[heap_entry] = heap_value;
}


/**************************************************************************

 BUILD_INITIAL_HEAP ()

 This function builds a heap from the initial frequency count data.
 **************************************************************************/

void DHuff::build_initial_heap (void){

   unsigned short  loop;


   heap_length = 0;

   for (loop = 0; loop < 256; loop++)
      if (frequency_count[loop])
         heap[++heap_length] = (unsigned long) loop;

   for (loop = heap_length; loop > 0; loop--)
      reheap (loop);
}

/**************************************************************************

 BUILD_DECOMP_TREE ()

 This function builds the decompression tree.
 **************************************************************************/

void  DHuff::build_decomp_tree (void){
   unsigned short  loop1;
   unsigned int  current_index;

   unsigned short  loop;
   unsigned short  current_node = 1;

   decomp_tree[1] = 1;

   for (loop = 0; loop < 256; loop++) {
      if (code_length[loop]){
		current_index = 1;

		for (loop1 = code_length[loop] - 1; loop1 > 0; loop1--){
			current_index = (decomp_tree[current_index] << 1) + ((code[loop] >> loop1) & 1);

			if (!(decomp_tree[current_index]))
				decomp_tree[current_index] = ++current_node;
		}

		decomp_tree[(decomp_tree[current_index] << 1) +  (code[loop] & 1)] = -loop;
      }
   }//  end for loop
}


/**************************************************************************

 DECOMPRESS_IMAGE ()

 This function decompresses the compressed image.
 **************************************************************************/

void  DHuff::decompress_line1 (char*& in, char*& out){
   unsigned short  cindex = 1;
   char            curchar;
   char            lastchar = 0;
   short           bitshift;

   unsigned long  charcount = 0L;

   while (charcount < (1<<11)){
      curchar = *in++;

     for (bitshift = 7; bitshift >= 0; --bitshift){
		cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

		if (decomp_tree[cindex] <= 0){
			*out = (unsigned char)(-decomp_tree[cindex]) + lastchar;
			lastchar = *out++;

			if ((++charcount) == (1<<11))
				bitshift = 0;
           else
               cindex = 1;
		} else
			cindex = decomp_tree[cindex];
     }//  end for bitshift
   }//  end while
}

void  DHuff::decompress_line2 (char*& in, char*& out){
   unsigned short  cindex = 1;
   char            curchar;
   char            lastchar = 0;
   short           bitshift;

   unsigned long  charcount = 0L;

   while (charcount < (1<<11)){
      curchar = *in++;

     for (bitshift = 7; bitshift >= 0; --bitshift){
		cindex = (cindex << 1) + ((curchar >> bitshift) & 1);

		if (decomp_tree[cindex] <= 0){
			*out = (unsigned char)(-decomp_tree[cindex]) ^ lastchar;
			lastchar = *out++;

			if ((++charcount) == (1<<11))
				bitshift = 0;
           else
               cindex = 1;
		} else
			cindex = decomp_tree[cindex];
     }//  end for bitshift
   }//  end while
}


