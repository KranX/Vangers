#ifndef __UNITS__COMPAS_H
#define __UNITS__COMPAS_H

const int CMP_TARGET_ESCAVE = 0;	//char*
const int CMP_TARGET_SPOT = 1;		//char*
const int CMP_TARGET_PASSAGE = 2;	//char*
const int CMP_TARGET_ITEM = 3;		//uvsTarget
const int CMP_TARGET_VANGER = 4;	//uvsTarget
const int CMP_TARGET_SENSOR = 5;	//char*

const int CMP_OBJECT_ESCAVE = 0;
const int CMP_OBJECT_SPOT = 1;
const int CMP_OBJECT_PASSAGE = 2;
const int CMP_OBJECT_ITEM = 3;
const int CMP_OBJECT_VANGER = 4;
const int CMP_OBJECT_SENSOR = 5;
const int CMP_OBJECT_PHANTOM = 6;
const int CMP_OBJECT_VECTOR = 7;

void AddTarget2Compas(int type,void* point, const char* head);
void SelectCompasTarget(const char * n);
char* GetCompasTarget(void);

void aciAdd2Targets(char* p);
void aciDeleteFromTargets(char* p);

extern int GlobalFuryLevel;
inline void IncFuryFactor(void)
{
	GlobalFuryLevel ++;
};

inline void DecFuryFactor(void)
{
	if(GlobalFuryLevel > 0) GlobalFuryLevel--;	
};

#endif