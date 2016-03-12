
extern int aciCurCredits00;
extern int aciCurCredits01;
extern int aciCurCredits02;
extern int aciCurCredits03;
extern int aciCurCredits04;
extern int aciCurCredits05;
extern int aciCurCredits06;
extern int aciCurCredits07;


inline int aciGetCurCredits(void)
{
	return	 aciCurCredits00	|
		(aciCurCredits01 << 4)	|
		(aciCurCredits02 << 8)	|
		(aciCurCredits03 << 12) |
		(aciCurCredits04 << 16) |
		(aciCurCredits05 << 20) |
		(aciCurCredits06 << 24) |
		(aciCurCredits07 << 28);
};

inline void aciUpdateCurCredits(int val)
{
	aciCurCredits00 = val & 0x0F;
	aciCurCredits01 = (val >> 4) & 0x0F;
	aciCurCredits02 = (val >> 8) & 0x0F;
	aciCurCredits03 = (val >> 12) & 0x0F;
	aciCurCredits04 = (val >> 16) & 0x0F;
	aciCurCredits05 = (val >> 20) & 0x0F;
	aciCurCredits06 = (val >> 24) & 0x0F;
	aciCurCredits07 = (val >> 28) & 0x0F;
};

