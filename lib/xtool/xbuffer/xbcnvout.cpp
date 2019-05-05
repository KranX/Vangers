#include "xglobal.h"

XBuffer& XBuffer::operator>= (char& var)
{
	char* p = buf + offset;
	var = (char)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned char& var)
{
	char* p = buf + offset;
	var = (unsigned char)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (short& var)
{
	char* p = buf + offset;
	var = (short)strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned short& var)
{
	char* p = buf + offset;
	var = (unsigned short)strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (int& var)
{
	char* p = buf + offset;
	var = strtol(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned& var)
{
	char* p = buf + offset;
	var = strtoul(p,&p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (long& var)
{
	char* p = buf + offset;
	var = strtol(p, &p, 0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (unsigned long& var)
{
	char* p = buf + offset;
	var = strtoul(p, &p,0);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (double& var)
{
	char* p = buf + offset;
	var = strtod(p, &p);
	offset += p - (buf + offset);

	return *this;
}

XBuffer& XBuffer::operator>= (long double& var)
{
	char* p = buf + offset;
	var = strtod(p, &p);
	offset += p - (buf + offset);
	return *this;
}

XBuffer& XBuffer::operator>= (float& var)
{
	char* p = buf + offset;
	var = (float)strtod(p, &p);
	offset += p - (buf + offset);
	return *this;
}


