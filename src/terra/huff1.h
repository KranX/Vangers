#ifndef __TERRA__HUFF1_H__
#define __TERRA__HUFF1_H__


struct DHuff{
	unsigned int  *code;
	unsigned char   *code_length;
	int *          decomp_tree;

	int           *father;
	unsigned short  heap_length;
	unsigned long   *heap;
	unsigned short  compress_charcount;
	long            *frequency_count;

	DHuff(void);
	~DHuff(void);

	void reheap (unsigned short  heap_entry);
	void compress_line (unsigned char *in, char *out);
	unsigned short  generate_code_table (void);
	void build_code_tree (void);
	void build_initial_heap (void);

	void  build_decomp_tree (void);
	void  decompress_line1 (char*& in, char*& out);
	void  decompress_line2 (char*& in, char*& out);
};

#endif