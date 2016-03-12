#ifndef __XCPUID_H
#define __XCPUID_H

#define CPU_INTEL		0x00000001

// Intel features IDs

#define INTEL_MMX		0x00800000

// Intel Family IDs
#define CPU_INTEL_386		3
#define CPU_INTEL_486		4
#define CPU_INTEL_PENTIUM	5
#define CPU_INTEL_PENTIUM_PRO	6

//Intel Pentium model's
#define PENTIUM_P5A		0
#define PENTIUM_P5		1
#define PENTIUM_P54C		2
#define PENTIUM_P54T		3
#define PENTIUM_MMX		4
#define PENTIUM_DX4		5
#define PENTIUM_P5OVER		6
#define PENTIUM_P54CA		7

// Intel Pentium Pro model's
#define PENTIUM_PROA		0
#define PENTIUM_PRO		1
#define PENTIUM_PRO2		3
#define PENTIUM_PRO55CT 	4
#define PENTIUM_PROF		5

extern unsigned int xt_processor_id;
extern unsigned int xt_processor_type;
extern unsigned int xt_processor_family;
extern unsigned int xt_processor_model;
extern unsigned int xt_processor_revision;
extern unsigned int xt_processor_features;

unsigned int xt_get_cpuid(void);
char* xt_getMMXstatus(void);

extern int xt_mmxUse;

#endif // __XCPUID_H

