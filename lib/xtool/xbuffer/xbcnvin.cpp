#include "xglobal.h"

char _ConvertBuffer[_CONV_BUFFER_LEN + 1];

XBuffer& XBuffer::operator<= (char var)
{
	char* s = port_itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned char var)
{
	char* s = port_itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (short var)
{
	char* s = port_itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned short var)
{
	char* s = port_ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (int var)
{
	char* s = port_itoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned var)
{
	char* s = port_ultoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long var)
{
	char* s = port_ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (long long int var)
{
	char* s = port_ltoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (unsigned long var)
{
	char* s = port_ultoa(var,_ConvertBuffer,radix);
	write(s,strlen(s),0);
	return *this;
}

XBuffer& XBuffer::operator<= (float var)
{
	int len = snprintf(_ConvertBuffer, _CONV_BUFFER_LEN, "%.*g", digits, var);
	write(_ConvertBuffer,len,0);
	return *this;
}

XBuffer& XBuffer::operator<= (double var)
{
	int len = snprintf(_ConvertBuffer, _CONV_BUFFER_LEN, "%.*g", digits, var);
	write(_ConvertBuffer,len,0);
	return *this;
}

XBuffer& XBuffer::operator<= (long double var)
{
	int len = snprintf(_ConvertBuffer, _CONV_BUFFER_LEN, "%.*Lg", digits, var);
	write(_ConvertBuffer,len,0);
	return *this;
}
