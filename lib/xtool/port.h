#ifndef __PORT_H
#define __PORT_H

/**
	
 * C++ version char* style "itoa":
	
 */
	
char* port_itoa( int value, char* result, int base );

char* port_ltoa( long value, char* result, int base );

char* port_ultoa( unsigned long value, char* result, int base );

#if defined(__unix__) || defined(__APPLE__)
char *strupr(char *string);
#endif

#endif
