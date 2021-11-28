#include "../global.h"

#include "../3d/3d_math.h"

#include "../sqexp.h"
#include "../backg.h"

#include "../terra/vmap.h"
#include "../terra/world.h"

#include "particle.h"
#include "partmap.h"

#undef random
#define random(a) BogusRND(a)

const int PART_H_SIZE = H_SIZE>>PARTICLE_SHIFT;

unsigned BogusRNDVAL = 83838383;

void ParticleProcess::quant1(){
	BackD.put(this);
	ListExhausted = 0;
	GetBackGround();
	express();
}

void ParticleProcess::set_hot_spot(int x,int y,int val,int lev){
	Particle* p;

	p = AddRandom((x&clip_mask_x)>>PARTICLE_SHIFT,(y&clip_mask_y)>>PARTICLE_SHIFT);
	if(p){
		p -> Value = val;
		p -> Level = lev;
	}
}

void ParticleProcess::quant2(){
	Particle* p;
	NewParticles = ActivList.prev;

	for( p = ActivList.next; p != NewParticles -> next; p = p -> next)
		p -> process(this);

	for( p = NewParticles -> next; p != &ActivList; p = p -> next)
		p -> delFromCol(this);

	for(p = ActivList.next;p != &ActivList; p = p -> next){
		p -> valueDecrease();
	}
}

void Particle::process(ParticleProcess* proc){
	NewValue = 0;
	Age++;
	if(LU) NewValue += LU -> Value;     else proc -> AddNear(X - 1,Y - 1,this,0);
	if(U ) NewValue += U  -> Value;     else proc -> AddNear(X    ,Y - 1,this,1);
	if(RU) NewValue += RU -> Value;     else proc -> AddNear(X + 1,Y - 1,this,2);
	if(L ) NewValue += L  -> Value;     else proc -> AddNear(X - 1,Y    ,this,3);

	if(R ) NewValue += R  -> Value;     else proc -> AddNear(X + 1,Y    ,this,4);
	if(LD) NewValue += LD -> Value;     else proc -> AddNear(X - 1,Y + 1,this,5);
	if(D ) NewValue += D  -> Value;     else proc -> AddNear(X    ,Y + 1,this,6);
	if(RD) NewValue += RD -> Value;     else proc -> AddNear(X + 1,Y + 1,this,7);
}

Particle* Particle::b_link(ParticleProcess* proc){
	Particle* np;
	int dX;
	Particle* after = 0;

	for(dX = -1;dX <= 1;dX++){
		int col = X + dX;
		if(col < 0) col = PART_H_SIZE - 1;
		else if(col >= PART_H_SIZE) col = 0;

		for(np = proc -> ParticlesCol[col]; np; np = np -> nextInCol){
			if(Y == np -> Y - 1 || Y == np -> Y || Y == np -> Y + 1){
				int dY = np -> Y - Y;
				int d = (dY + 1)*3 + (dX + 1);
				switch(d){
					case 0: LU = np;np -> RD = this; break;
					case 1:  U = np;np ->  D = this; break;
					case 2: RU = np;np -> LD = this; break;
					case 3: L  = np;np -> R  = this; break;
					case 4: break;
					case 5: R  = np;np -> L  = this; break;
					case 6: LD = np;np -> RU = this; break;
					case 7:  D = np;np ->  U = this; break;
					case 8: RD = np;np -> LU = this; break;
					default :
						ErrH.Abort("Wrong near code in ::link");
				}
			}
			if(!dX && np -> Y < Y)
				after = np;
			if(Y < np -> Y)
				break;
		}
	}
	return after;
}

void Particle::b_unlink(){
	if(LU){
		LU -> RD = NULL;
		LU = NULL;
	}
	if( U){ 
		U ->  D = NULL; 
		U = NULL;
	}
	if(RU){
		RU -> LD = NULL;
		RU = NULL;
	}
	if(L ){
		L  -> R	= NULL;
		L  = NULL;
	}
	if(R ){
		R  -> L	= NULL;
		R  = NULL;
	}
	if(LD){
		LD -> RU = NULL;
		LD = NULL;
	}
	if( D){ 
		D ->  U = NULL; 
		D = NULL;
	}
	if(RD){
		RD -> LU = NULL;
		RD = NULL;
	}
}

Particle* ParticleProcess::AddRandom(int x,int y){
	//x = (x + PART_H_SIZE)%PART_H_SIZE;
	Particle* p;
	for( p = ActivList.next; p != &ActivList; p = p -> next)
		if(p -> X == x && p -> Y == y){
			return p;
		}

	if(InActivList.next == &InActivList){
		ListExhausted = 1;
		return 0;
	}

	p = InActivList.next;
	p -> Age = 0;
	p -> X = x;
	p -> Y = y;
	Particle* np;

	for( np = ActivList.next; np != &ActivList; np = np -> next){
		int dX = np -> X - p -> X;
		int dY = np -> Y - p -> Y;
		if (!np -> X && p -> X == PART_H_SIZE - 1)
			dX = 1;
		if (!p -> X && np -> X == PART_H_SIZE - 1)
			dX = -1;
		if(abs(dX) <= 1 && abs(dY) <= 1){
			int d = (dY + 1)*3 + (dX + 1);
			switch(d){
				case 0: p -> LU = np; np -> RD = p; break;
				case 1: p ->  U = np; np ->  D = p; break;
				case 2: p -> RU = np; np -> LD = p; break;
				case 3: p -> L	= np; np -> R  = p; break;
				case 4: break;
				case 5: p -> R	= np; np -> L  = p; break;
				case 6: p -> LD = np; np -> RU = p; break;
				case 7: p ->  D = np; np ->  U = p; break;
				case 8: p -> RD = np; np -> LU = p; break;
				default :
					ErrH.Abort("Wrong near code in ::link");
			}
		}
	}
	p -> Protected = 1;

	p -> del();
	p -> ins(&ActivList);
	InActivN--;
	ActivN++;
	return p;
}

Particle* ParticleProcess::AddNear(int x,int y,Particle* cr,int cr_dir){
	if(x < 0) x = PART_H_SIZE - 1;
	else if(x >= PART_H_SIZE) x = 0;

	Particle* p;
	int exist = 0;
	for(p = ParticlesCol[x]; p ;p = p -> nextInCol){
		if(y <= p -> Y){
			if(y == p -> Y)
				exist = 1;
			break;
			}
	}

	if(!exist){
		if(InActivList.next == &InActivList){
			ListExhausted = 1;
			return 0;
		}
		p = InActivList.next;
		p -> Age = 0;
		p -> X = x;
		p -> Y = y;
		p -> NewValue = 0;
		p -> Protected = 0;
		p -> Level = cr -> Level;
		Particle* after = p -> b_link(this);
		p -> insInCol(after,this);
		p -> del();
		p -> ins(&ActivList);
		InActivN--;
		ActivN++;
	}

	if(!p -> Protected && cr_dir != 7)
			p -> NewValue += cr -> Value;

	switch(cr_dir){
		case 0: p -> RD = cr;cr -> LU = p;break;
		case 1: p ->  D = cr;cr ->  U = p;break;
		case 2: p -> LD = cr;cr -> RU = p;break;
		case 3: p -> R	= cr;cr -> L  = p;break;
		case 4: p -> L	= cr;cr -> R  = p;break;
		case 5: p -> RU = cr;cr -> LD = p;break;
		case 6: p ->  U = cr;cr ->  D=	p;break;
		case 7: p -> LU = cr;cr -> RD = p;break;
	}
	return p;
}

inline void Particle::delFromCol(ParticleProcess* proc){
	if(nextInCol)
		nextInCol -> prevInCol = prevInCol;
	if(prevInCol)
		prevInCol -> nextInCol = nextInCol;
	else
		proc -> ParticlesCol[X] = 0;
}

void Particle::insInCol(Particle* a,ParticleProcess* proc){
	if(a){
		if(a -> nextInCol){
			a -> nextInCol -> prevInCol = this;
			nextInCol = a -> nextInCol;
		}else
			nextInCol = 0;

		a -> nextInCol = this;
		prevInCol = a;
	}else{
		if(proc -> ParticlesCol[X])
			nextInCol = proc -> ParticlesCol[X];
		else
			nextInCol = 0;

		prevInCol = 0;
		proc -> ParticlesCol[X] = this;
	}
}

inline void Particle::ins(Particle* np){
	next = np;
	prev = np -> prev;
	np -> prev -> next = this;
	np -> prev = this;
}

inline void Particle::del(){
	next -> prev = prev;
	prev -> next = next;
}

void ParticleProcess::test(void){
	Particle* p;
	int i;

	p = &ActivList;
	for( i = 0; i < MaxParticle + 1; i++){
		if ( p == &ActivList) break;
		p = p -> next;
	}

	if (i == MaxParticle + 1)
		ErrH.Abort("Particle TEST - ActiveList");

	p = &InActivList;
	for( i = 0; i < MaxParticle+1; i++){
		if ( p == &InActivList) break;
		p = p -> next;
	}

	if (i == MaxParticle + 1)
		ErrH.Abort("Particle TEST - InActiveList");
}

void ParticleProcess::reset(){
	ActivN = 0;
	InActivN = MaxParticle;
	ActivList.next = &ActivList;
	ActivList.prev = &ActivList;
	InActivList.next = heap;
	InActivList.prev = heap + MaxParticle - 1;
	Particle* p = heap;

	for(int i = 0; i < MaxParticle; i++){
		p -> next = p + 1;
		p -> prev = p - 1;
		p -> LU = NULL;
		p ->  U = NULL;
		p -> RU = NULL;
		p -> L	= NULL;
		p -> R	= NULL;
		p -> LD = NULL;
		p ->  D = NULL;
		p -> RD = NULL;
		p++;
	}

	heap[0].prev = &InActivList;
	heap[MaxParticle - 1].next = &InActivList;
	memset(ParticlesCol,0,PART_H_SIZE*sizeof(Particle*));
}

int ParticleProcess::init(int m,int pal_col,int n_h_s = 1){
	MaxParticle = m;
	PaletteColor = pal_col;
	ParticlesCol = new Particle*[PART_H_SIZE];

	memset(ParticlesCol,0,(PART_H_SIZE)*sizeof(Particle*));
	heap = new Particle[MaxParticle];
	memset(heap,0,MaxParticle*sizeof(Particle));

	if(!heap){
		ErrH.Abort("Not enought memory for Particle heap");
	}

	heap_end = heap + MaxParticle;
	reset();
	NhotSpots = n_h_s;
	HotCenterX = 0;
	HotCenterY = 0;
	AverageSpeed = 14;
	XHotSpotArea = 0;
	YHotSpotArea = 20;
	HotCenterXdelta = 0x7FFFFFFF;
	HotCenterYdelta = 0x7FFFFFFF;

	return heap != 0;
}

void ParticleProcess::finit(){
	if(heap){
		delete[] heap;
		delete[] ParticlesCol;

	}else ErrH.Abort("Bad Heap Pointer");
}

void Particle::purge(ParticleProcess* proc){
	if(!Value){
		if(*BackGround){
			int _X = X<<PARTICLE_SHIFT;
			int _Y = Y<<PARTICLE_SHIFT;

		
			for(int j = 0; j < PARTICLE_SIZE; j++)
				if (vMap -> lineT[YCYCL(_Y + j)])
					memcpy(vMap -> lineTcolor[YCYCL(_Y + j)] + _X,BackGround + (j<<2),PARTICLE_SIZE);

			*BackGround = 0;
		}
		b_unlink();
		del();
		ins(&proc -> InActivList);
		proc -> InActivN++;
		proc -> ActivN--;
	}
}

void Particle::express(ParticleProcess* proc){
	uchar **lt = vMap -> lineT;
	uchar **ltc = vMap -> lineTcolor;
	uchar *ParticlePaletteTable;
	const int noise = 4;
	//const int noise = 2;

	switch(proc -> PaletteColor){
	case 0 :
		ParticlePaletteTable = ParticlePaletteTableDust;
		break;
	case 1:
		ParticlePaletteTable = ParticlePaletteTableSmog;
		break;
	default:
		ParticlePaletteTable = ParticlePaletteTableDust;
		break;
	};

	int val = Value;
	int _X = X<<PARTICLE_SHIFT;
	int _Y = YCYCL(Y<<PARTICLE_SHIFT);
	int i;

	for(i = 0; i < 4; i++)
		if (!lt[YCYCL(_Y + i)]) return;
	
	unsigned char fl = *(lt[_Y ] + H_SIZE + _X);
	if(fl & DOUBLE_LEVEL)
		if(!Level)
			return;

	uchar *Ground = BackGround;
	if(*BackGround){
		int lu = LU ? LU -> Value : 0;
		int u = U ? U  -> Value : 0;
		int ru = RU ? RU -> Value : 0;
		int l = L ? L  -> Value : 0;
		int r = R ? R  -> Value : 0;
		int ld = LD ? LD -> Value : 0;
		int d = D ? D  -> Value : 0;
		int rd = RD ? RD -> Value : 0;
		int val1;
		
		val1 = (val + l + lu + u) >> 2;
		unsigned char* mm,*mm1;
		
		mm = ltc[_Y] + _X;

		if ( val1){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
		} else
			mm++;

		val1 = (val + u) >> 1;

		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
		} else mm += 2;

		val1 = ((val + u + ru + r) >> 2);

		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
		} else
			mm++;

		mm = ltc[YCYCL(_Y + 1)] + _X;
		mm1 = ltc[YCYCL(_Y + 2)] + _X;

		val1 = (val + l) >> 1;

		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm1++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *(Ground+3));
		} else {
			mm++;
			mm1++;
		}


		if ( val ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm1++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *(Ground+2));
			*mm1++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *(Ground+3));
		} else {
			mm += 2;
			mm1 += 2;
		}

		val1 = (val + r) >> 1;
		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm1++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) +*(Ground+3));
		} else {
			mm++;
			mm1++;
		}

		mm = ltc[YCYCL(_Y + 3)] + _X;
		val1 = (val + l + ld + d) >> 2;

		Ground += 4;

		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
		} else 
			mm++;

		val1 = (val + d) >> 1;

		if ( val1 ){
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
			*mm++ = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground++);
		} else mm += 2;

		val1 = (val + r + rd + d) >> 2;
		
		if ( val1 ){
			*mm = *(ParticlePaletteTable + ((val1 + RND(noise)) << 8) + *Ground);
		}
	}
}

void ParticleProcess::purge(){
	Particle* p, *p_next;
	for(p = ActivList.next;p != &ActivList; p = p_next){
		p_next = p -> next;
		p -> purge(this);
	}
}

void ParticleProcess::express(){
	Particle* p;
	for(p = ActivList.next;p != &ActivList; p = p -> next)
		p -> express(this);
}

void ParticleProcess::GetBackGround(void){
	Particle* p;

	for(p = ActivList.next;p != &ActivList; p = p -> next)
		p -> GetBackGround();
}

void ParticleProcess::BackRestore(void){
	Particle* p;
	purge();
	for(p = ActivList.next;p != &ActivList; p = p -> next)
		p -> BackRestore();
}

void Particle::BackRestore(void){
	uchar **lt = vMap -> lineT;
	uchar **ltc = vMap -> lineTcolor;
	int y;

	if (*BackGround){
		int _X = X<<PARTICLE_SHIFT;
		int _Y = Y<<PARTICLE_SHIFT;
		for(int j = 0;j < PARTICLE_SIZE;j++){
			y = YCYCL(_Y + j);
			if( lt[y] )
				memcpy(ltc[y] + _X,BackGround + (j<<2),PARTICLE_SIZE);
		}
	}
}

void Particle::GetBackGround(void){
	uchar **lt = vMap -> lineT;
	uchar **ltc = vMap -> lineTcolor;
	int y;

	memset(BackGround, 0, 2<<PARTICLE_SHIFT);

	int _X = X<<PARTICLE_SHIFT;
	int _Y = Y<<PARTICLE_SHIFT;

	for(int j = 0;j < PARTICLE_SIZE;j++){
		y = YCYCL(_Y + j);
		if( lt[y] )
			memcpy(BackGround+(j<<2),ltc[y] + _X,PARTICLE_SIZE);
	}
}

void Particle::valueDecrease()
{
	Value = ((3*NewValue >> 3) + Value)>>2;

	switch(RND(13)){
		case 0: break;
		case 1:  if(U)  Value = ((3*U -> NewValue >>3) + Value)>>2; break;
		case 2: break;
		case 3: if(L)  Value = ((3*L -> NewValue >>3) + Value)>>2; break;
		case 4: break;
		case 5: if(R) Value = ((3*R -> NewValue >>3) + Value)>>2; break;
		case 6:  break;
		case 7:  if(D)  Value = ((3*D -> NewValue >>3) + Value)>>2; break;
		case 8: break;
	}

	Protected = 0;
}

uchar *ParticlePaletteTableDust;
uchar *ParticlePaletteTableSmog;

void ParticlePaletteTableInit( char unsigned *pal){
	int k, i, j;

	for( i = 0;i < 256;i++){
		int I = ((int)pal[3*i] + (int)pal[3*i + 1] + (int)pal[3*i + 2])/3;
//		int I = ((19595*(int)pal[3*i] + 38470*(int)pal[3*i + 1] + 7471*(int)pal[3*i + 2])>>16)/3;
		for(int j = 0; j < 256; j++){
			int P = I>>1;
			if(P >= PARTICLE_PROCESS_COLOR_MAX)
				P = PARTICLE_PROCESS_COLOR_MAX - 1;
			
			ParticlePaletteTableDust[i + (j << 8)] = P + 48;
			I++;
		}
	}

	k = 1;

	for( i = BEGCOLOR[k]; i <= ENDCOLOR[k]; i++){				
		for(int j = 0; j < 8; j++){
				ParticlePaletteTableDust[i + (j << 8)] = (i*(8-j) + (j*ParticlePaletteTableDust[i + (j << 8)]))>>3;
				//ParticlePaletteTable[i + (j << 8)] = 0;
		}
	}

	for( i = 0; i < TERRAIN_MAX; i++){
		for( j = 0; j < 256; j++){
			for( k = BEGCOLOR[i]; k <= ENDCOLOR[i]; k++){
				ParticlePaletteTableSmog[k + (j << 8)] = BEGCOLOR[i];
				if (j < 8) {
					ParticlePaletteTableSmog[k + (j << 8)] = (k*(8-j) + (j*ParticlePaletteTableSmog[k + (j << 8)]))>>3;
                }
			}//  end for k
		}//  end for j
	}//  end fro i
} 

/*void ParticlePaletteTableInit(  char unsigned *pal ){
	int i, j, k;

	for( i = 0; i < TERRAIN_MAX; i++){
		for( j = 0; j < 256; j++){
			int I = j;
			for( k = BEGCOLOR[i]; k <= ENDCOLOR[i]; k++){

				ParticlePaletteTable[k + (j << 8)] = BEGCOLOR[i];
				if ( j < 8) 
					//ParticlePaletteTable[k + (j << 8)] = ((((k - BEGCOLOR[i])>>1) + BEGCOLOR[i])*(8-j) + (j*ParticlePaletteTable[k + (j << 8)]))>>3;
					ParticlePaletteTableSmog[k + (j << 8)] = (k*(8-j) + (j*ParticlePaletteTableSmog[k + (j << 8)]))>>3;
								
				//if( ParticlePaletteTable[k + (j << 8)] > ENDCOLOR[i] ) ParticlePaletteTable[k + (j << 8)] = ENDCOLOR[i];
				//I++;
			}//  end for k
		}//  end for j
	}//  end fro i
} 
*/
