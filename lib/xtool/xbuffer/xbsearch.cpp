#include "xglobal.h"

int XBuffer::search(const char* what, int mode, int cs)
{
	register int i,j;
	unsigned int wlen = strlen(what) - 1;
	if(mode == XB_GLOBAL) { i = 0; mode = XB_FORWARD; } else i = offset;
	if(mode == XB_FORWARD){
		if(cs == XB_CASEON){
			while(buf[i]){
				if(buf[i] == *what){
					j = wlen;
					while(buf[i + j] == what[j] && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i++;
				}
			}
		else {
			while(buf[i]){
				if(toupper(buf[i]) == toupper(*what)){
					j = wlen;
					while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i++;
				}
			}
		}
	else {
		i-=wlen + 1;
		if(cs == XB_CASEON){
			while(i >= 0){
				if(buf[i] == *what){
					j = wlen;
					while(buf[i + j] == what[j] && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i--;
				}
			}
		else {
			while(i >= 0){
				if(toupper(buf[i]) == toupper(*what)){
					j = wlen;
					while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i--;
				}
			}
		}
	return 0;
}

int XBuffer::search(char* what, int mode, int cs)
{
	register int i,j;
	unsigned int wlen = strlen(what) - 1;
	if(mode == XB_GLOBAL) { i = 0; mode = XB_FORWARD; } else i = offset;
	if(mode == XB_FORWARD){
		if(cs == XB_CASEON){
			while(buf[i]){
				if(buf[i] == *what){
					j = wlen;
					while(buf[i + j] == what[j] && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i++;
				}
			}
		else {
			while(buf[i]){
				if(toupper(buf[i]) == toupper(*what)){
					j = wlen;
					while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i++;
				}
			}
		}
	else {
		i-=wlen + 1;
		if(cs == XB_CASEON){
			while(i >= 0){
				if(buf[i] == *what){
					j = wlen;
					while(buf[i + j] == what[j] && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i--;
				}
			}
		else {
			while(i >= 0){
				if(toupper(buf[i]) == toupper(*what)){
					j = wlen;
					while(toupper(buf[i + j]) == toupper(what[j]) && j) j--;
					if(j <= 0) { offset = i; return 1; };
					}
				i--;
				}
			}
		}
	return 0;
}

