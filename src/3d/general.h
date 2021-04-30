#ifndef __3D__GENERAL_H__
#define __3D__GENERAL_H__

#include "parser.h"

#define _video_page	VS(_video)->_video

#define ALLOC(b,c)	new c[b]
#define FREE(p) 	delete[] p;

#include "3d_math.h"
#include "3dgraph.h"
#include "3dobject.h"
#include "heap.h"


extern MemoryHeap load_heap;
#define HEAP_BEGIN(sz)    load_heap.alloc(static_cast<int>(sz))
#define HEAP_ALLOC(b, c)  (static_cast<c*>(load_heap.get(static_cast<int>((b)*static_cast<int>(sizeof(c))))))
#define HEAP_ALLOC1(c)    (static_cast<c*>(load_heap.get(static_cast<int>(sizeof(c)))))
#define HEAP_END		  load_heap.end_alloc()


#ifdef _DEBUG
#define _FOUT_
extern XStream fout;
#endif
extern XBuffer msg_buf;

#endif