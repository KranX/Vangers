const unsigned SS_WIDTH = 16;

const uchar SHADOW_MASK = 1 << 7;
const uchar DELTA_MASK = 1 | (1 << 1);
const uchar DOUBLE_LEVEL = 1 << 6;

const int TERRAIN_MAX = 16;
const uchar TERRAIN_OFFSET = 2;
const uchar TERRAIN_MASK = (1 << TERRAIN_OFFSET) | (1 << (TERRAIN_OFFSET + 1)) | (1 << (TERRAIN_OFFSET + 2)) | (1 << (TERRAIN_OFFSET + 3));

#define GET_DELTA(a)	((a) & DELTA_MASK)
#define SET_DELTA(a,b)	{ a &= ~DELTA_MASK, a += b; }
#define SET_TERRAIN(a,b)	{ a &= ~TERRAIN_MASK, a += b; }
#define IS_DOUBLE(a)	((a) & DOUBLE_LEVEL)
#define GET_TERRAIN_TYPE(a)	((a) & TERRAIN_MASK)
#define GET_TERRAIN(a)		(((a) & TERRAIN_MASK) >> TERRAIN_OFFSET)

const uchar DELTA_SHIFT = 3;
const uchar MAX_RDELTA = 16 << DELTA_SHIFT;
const uchar MIN_RDELTA = 1 << DELTA_SHIFT;

const int SHADOWDEEP	       =  384;
