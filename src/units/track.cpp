#include "../global.h"

#include "../3d/3d_math.h"
#include "../3d/3dgraph.h"
#include "../3d/3dobject.h"
#include "../3d/parser.h"

#include "../common.h"
#include "../sqexp.h"
#include "../backg.h"
#include "../network.h"

#include "../terra/vmap.h"
#include "../terra/world.h"
#include "../terra/render.h"

#include "../actint/item_api.h"
#include "uvsapi.h"

#include "../particle/particle.h"
#include "../particle/partmap.h"
#include "../particle/df.h"
#include "../particle/light.h"

#include "../uvs/univang.h"

#include "../dast/poly3d.h"

#include "track.h"
#include "hobj.h"
#include "effect.h"
#include "moveland.h"
#include "items.h"
#include "sensor.h"
#include "mechos.h"


//#define SAVE_TRACK_STATUS

void TrackType::Open(char* filename)
{
	int i;
	
	XStream fin(GetTargetName(filename), XS_IN);	
	fin > NumNode > NumBranch;

	node = new NodeType[NumNode];
	branch = new BranchType[NumBranch];

	for(i = 0;i < NumNode;i++ ) node[i].Open(fin,branch,i);
	for(i = 0;i < NumBranch;i++) branch[i].Open(fin,node,i);
	fin.close();

	for(i = 0;i < NumNode;i++){
		for(int j = 0;j < node[i].NumBranch;j++){
			if(node[i].Status[j] && node[i].pBranches[j]->pBeg != &node[i]) ErrH.Abort("Bad Status Node");
			if(!(node[i].Status[j]) && node[i].pBranches[j]->pEnd != &node[i]) ErrH.Abort("Bad Status Node");
		};
	};
};

void TrackType::Save(char* filename)
{
	int i;
	XStream flib(GetTargetName(filename), XS_OUT);	
	flib < NumNode < NumBranch;

	for(i = 0;i < NumNode;i++ ) node[i].Save(flib);
	for(i = 0;i < NumBranch;i++) branch[i].Save(flib);
	flib.close();
};

void TrackType::Close(void)
{
	int i;
	for(i = 0;i < NumBranch;i++) branch[i].Close();
	for(i = 0;i < NumNode;i++) node[i].Close();
	delete[] node;
	delete[] branch;
};

void NodeType::Open(XStream& fin,BranchType* branch,int ind)
{
	int nBranch,i;

	index = ind;

	fin > x;
	fin > y;
	fin > z;
	fin > NumBranch;

	fin > Level;

	fin > Top;
	fin > Bottom;	

	fin > TrackCount;

	fin > NumWayNode;

	Branches = new int[NumBranch];
	Status = new char[NumBranch];
	pBranches = new BranchType*[NumBranch];

	BorderX = new int[NumBranch];
	BorderY = new int[NumBranch];

	WayBranchIndex = new int[NumWayNode*2];
	WayNodeIndex = WayBranchIndex + NumWayNode;

	for(i = 0;i < NumWayNode;i++){
		fin > WayBranchIndex[i];
		fin > WayNodeIndex[i];
	};

	for(i = 0;i < NumBranch;i++){
		fin > nBranch;
		fin > Status[i];

		fin > BorderX[i];
		fin > BorderY[i];

		if(nBranch >= 0) pBranches[i] = &branch[nBranch];
		else ErrH.Abort("Node Linking Error");
	};
};

void NodeType::Save(XStream& flib)
{
	int i;

	flib < x;
	flib < y;
	flib < z;
	flib < NumBranch;

	flib < Level;

	flib < Top;
	flib < Bottom;

	flib < TrackCount;

	flib < NumWayNode;

	for(i = 0;i < NumWayNode;i++){
		flib < WayBranchIndex[i];
		flib < WayNodeIndex[i];
	};

	for(i = 0;i < NumBranch;i++){
		flib < pBranches[i]->index;
		flib < Status[i];

		flib < BorderX[i];
		flib < BorderY[i];
	};
};

void NodeType::Close(void)
{
	delete[] BorderX;
	delete[] BorderY;
	delete[] pBranches;
	delete[] Status;
	delete[] Branches;
	delete[] WayBranchIndex;
};

void BranchType::Open(XStream& fin,NodeType* node,int ind)
{
	int beg,end;

	index = ind;

	fin > beg;
	fin > end;

	if(beg != -1) pBeg = &node[beg];
	else pBeg = NULL;
	if(end != -1) pEnd = &node[end];
	else pEnd = NULL;

	fin > NumLink;

	fin > Len;
	fin > Status;

	fin > Time; //!!!!!!!!!!!!

	fin > Top;
	fin > Bottom;

	Link = new LinkType[NumLink];

	for(int i = 0;i < NumLink;i++){
		fin > Link[i].xl;
		fin > Link[i].yl;
		fin > Link[i].zl;

		fin > Link[i].xr;
		fin > Link[i].yr;
		fin > Link[i].zr;

		fin > Link[i].x;
		fin > Link[i].y;
		fin > Link[i].z;

		fin > Link[i].Len;
		fin > Link[i].Wide;
		fin > Link[i].Noise;
		fin > Link[i].Status;
		fin > Link[i].Level; // !!!!!!!!!!!!!!!!

		fin > Link[i].TrackCount;
		fin > Link[i].Speed;
		fin > Link[i].Time;
	};
};


void BranchType::Save(XStream& flib)
{
	int i;

	if(pBeg == NULL) flib < (int)(-1);
	else flib < (pBeg->index);

	if(pEnd == NULL) flib < (int)(-1);
	else flib < (pEnd->index);

	flib < NumLink;

	flib < Len;
	flib < Status;

	flib < Time;

	flib < Top;
	flib < Bottom;

	for(i = 0;i < NumLink;i++){
		if(i == 0) Link[i].Noise = 0;
		else Link[i].Noise = Link[i].z - Link[i - 1].z;

		flib < Link[i].xl;
		flib < Link[i].yl;
		flib < Link[i].zl;

		flib < Link[i].xr;
		flib < Link[i].yr;
		flib < Link[i].zr;

		flib < Link[i].x;
		flib < Link[i].y;
		flib < Link[i].z;

		flib < Link[i].Len;
		flib < Link[i].Wide;
		flib < Link[i].Noise;
		flib < Link[i].Status;
		flib < Link[i].Level; // !!!!!!!!!!!!!!!!

		flib < Link[i].TrackCount;
		flib < Link[i].Speed;
		flib < Link[i].Time;
	};
};

void BranchType::Close(void)
{
	delete[] Link;
};

void OutCheckNode(Vector& v,NodeType* n,XStream& ff)
{
	char i;
	char nn;
	int bx,by,pbx,pby;
	int* dx;
	int* dy;

	nn = n->NumBranch;
	dx = n->BorderX;
	dy = n->BorderY;
	pbx = *dx;
	pby = *dy;
	ff < "\n\nNode:" <= n->index;
	ff < "\n" <= dx[nn - 1] < "," <= dy[nn - 1] < "," <= pbx < "," <= pby;
	ff < ";" <= GetAngle(v.x,v.y,dx[nn - 1],dy[nn - 1],pbx,pby);
	for(i = 1;i < nn;i++){
		dx++;
		dy++;
		bx = *dx;
		by = *dy;
		ff < "\n" <= pbx < "," <= pby < "," <= bx < "," <= by;
		ff < ";" <= GetAngle(v.x,v.y,pbx,pby,bx,by);
		pbx = bx;
		pby = by;
	};
};

void TrackType::GetFirstPosition(int x,int y,LinkType*& ln,BranchType*& bn)
{
	int i,j;
	int d1,d2,d3,md,d;
	LinkType* l;
	BranchType* b;

	LinkType* ml;
	BranchType* mb;

	md = 0xffffff;
	b = branch;
	ml = NULL;
	mb = NULL;
	for(i = 0;i < NumBranch;i++,b++){
		l = b->Link;
		for(j = 0;j < b->NumLink;j++,l++){
			d1 = abs(getDistX(l->x,x)) + abs(getDistY(l->y,y));
			d2 = abs(getDistX(l->xr,x)) + abs(getDistY(l->yr,y));
			d3 = abs(getDistX(l->xl,x)) + abs(getDistY(l->yl,y));

			d = MIN(MIN(d1,d2),d3);

			if(d < md){
				md = d;
				ml = l;
				mb = b;
			};
		};
	};

	if(ml){
		ln = ml;
		bn = mb;
	};
};

void TrackType::GetPosition(int x,int y,LinkType*& ln,BranchType*& bn)
{
	int i,j;
	int d,md;
	LinkType* l;
	BranchType* b;

	LinkType* ml;
	BranchType* mb;

	md = 0xffffff;
	b = branch;
	ml = NULL;
	mb = NULL;
	for(i = 0;i < NumBranch;i++,b++){
		if(y > b->Top && y < b->Bottom){
			l = b->Link;
			for(j = 0;j < b->NumLink;j++,l++){
				d = abs(getDistX(l->x,x)) + abs(getDistY(l->y,y));
				if(d < md){
					md = d;
					ml = l;
					mb = b;
				};
			};
		};
	};

	if(ml){
		ln = ml;
		bn = mb;
	};
};


void TrackType::GetPosition(int x,int y,LinkType*& ln,BranchType*& bn,NodeType*& n)
{
	int i,j;
	int d,md1,md2;
	LinkType* l;
	BranchType* b;
	NodeType* nn;

	LinkType* ml;
	BranchType* mb;
	NodeType* mn;

	md1 = 0xffffff;
	b = branch;
	ml = NULL;
	mb = NULL;
	for(i = 0;i < NumBranch;i++,b++){
		if(y > b->Top && y < b->Bottom){
			l = b->Link;
			for(j = 0;j < b->NumLink;j++,l++){
				d = abs(getDistX(l->x,x)) + abs(getDistY(l->y,y));
				if(d < md1){
					md1 = d;
					ml = l;
					mb = b;
				};
			};
		};
	};

	md2 = 0xffffff;
	nn = node;
	mn = NULL;
	for(i = 0;i < NumNode;i++,nn++){
		d = abs(getDistX(nn->x,x)) + abs(getDistY(nn->y,y));
		if(d < md2){
			mn = nn;
			md2 = d;
		};
	};

	if(!mn) ErrH.Abort("Bad find near node");

	if(ml){
		if(md1 < md2){
			n = NULL;
			ln = ml;
			bn = mb;
		}else{
			ln = NULL;
			bn = NULL;
			n = mn;
		};
	}else{
		ln = NULL;
		bn = NULL;
		n = mn;
	};
};

void TrackType::GetPosition(TrackLinkType* p)
{
	int x,y;

	int i,j;
	int d,md1,md2;

	LinkType* l;
	LinkType* ml;

	BranchType* b;
	BranchType* mb;

	NodeType* nn;
	NodeType* mn;

	md1 = 0xffffff;
	b = branch;
	mb = NULL;

	x = p->vPoint.x;
	y = p->vPoint.y;

	for(i = 0;i < NumBranch;i++,b++){
		if(y > b->Top && y < b->Bottom){
			l = b->Link;
			for(j = 0;j < b->NumLink;j++,l++){
				d = abs(getDistX(l->x,x)) + abs(getDistY(l->y,y));
				if(d < md1){
					md1 = d;
					ml = l;
					mb = b;
				};
			};
		};
	};

	md2 = 0xffffff;
	nn = node;
	mn = NULL;
	for(i = 0;i < NumNode;i++,nn++){
		d = abs(getDistX(nn->x,x)) + abs(getDistY(nn->y,y));
		if(d < md2){
			mn = nn;
			md2 = d;
		};
	};

	if(!mn) ErrH.Abort("Bad find near node");

	if(mb && md1 < md2){
		p->pNextLink = ml;
		p->pBranch = mb;

		if(ml == mb->Link){
			p->pPrevLink = ml;
			p->pNextLink++;
		}else p->pPrevLink = p->pNextLink - 1;

		if(CheckInBranch(p->vPoint,p->pPrevLink,p->pNextLink,p->pBranch)) p->PointStatus = TRK_IN_BRANCH;
		else p->PointStatus = TRK_OUT_BRANCH;
	}else{
		p->pNode = mn;
		if(CheckInNode(p->vPoint,p->pNode)) p->PointStatus = TRK_IN_NODE;
		else p->PointStatus = TRK_OUT_NODE;
	};
};

void TrackType::GetInsidePosition(TrackLinkType* p)
{
	int i,j;
	Vector vTrack;

	LinkType* ml;
	LinkType* pml;
	BranchType* mb;
	NodeType* nn;

//	Vector vCheck = R_curr;
	Vector vCheck;
	vCheck = p->vPoint;

	mb = branch;
	for(i = 0;i < NumBranch;i++,mb++){
		if(vCheck.y > mb->Top && vCheck.y < mb->Bottom){
			pml = mb->Link;
			ml = pml + 1;
			for(j = 1;j < mb->NumLink;j++){
				if(CheckInLink(vCheck,pml,ml)){
					p->pBranch = mb;
					p->pNextLink = ml;
					p->pPrevLink = pml;
					p->PointStatus = TRK_IN_BRANCH;
					return;
				};
				pml = ml;
				ml++;
			};
		};
	};

	nn = HideTrack.node;
	for(i = 0;i < HideTrack.NumNode;i++,nn++){
		if(vCheck.y > nn->Top && vCheck.y < nn->Bottom){
			if(CheckInNode(vCheck,nn) && nn->z < vCheck.z){
				p->pNode = nn;
				p->PointStatus = TRK_IN_NODE;
				return;
			};
		};
	};
	p->PointStatus = TRK_OUT_BRANCH;
};
