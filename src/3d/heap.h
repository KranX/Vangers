
struct MemoryHeap {
	int offset,size;
	char* heap;
	const char* overflow_message;

	MemoryHeap(const char* message);
	~MemoryHeap() {
		if(size) {
			free();
		}
	}

	void alloc(int sz);
	void free() {
		delete[] heap;
		size = 0;
	}

	void init() {
		offset = 0;
	}
	void* get(int sz) {
		char* p = heap + offset;
		offset += sz;
		if(offset > size) {
			ErrH.Abort(overflow_message, XERR_USER, sz);
		}
		return p;
	}
	void back(int sz){
		if((offset -= sz) < 0) {
			ErrH.Abort(overflow_message, XERR_USER, sz);
		}
	}
	void end_alloc();
};
