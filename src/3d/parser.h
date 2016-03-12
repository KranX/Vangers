struct Parser : XBuffer {

		Parser(const char* name, const char* del_chars = 0);

	void search_name(const char* name);
	int is_next_name(const char* name);
	int get_int(){ int t; *this >= t; return t; }
	double get_double(){ double t; *this >= t; return t; }
	double quick_get_double();
	char* get_name();
	char* get_string();
};


