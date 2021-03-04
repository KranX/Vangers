#ifndef __UNITS__TRACK_H
#define __UNITS__TRACK_H

const int BRANCH_STATUS_HI = 180;
const int BRANCH_STATUS_LOW = 80;
const int BRANCH_STATUS_WATER = 10;

struct NodeType;
struct TrackLinkType;

struct LinkType
{
	int x,y,z;
	int xr,yr,zr;
	int xl,yl,zl;
	int Len;
	int Wide;
	int Noise;
	int Status;

	char Level;
	int Speed;
	int Time;
	int TrackCount;
};

struct BranchType
{
	int index;
	NodeType* pBeg;
	NodeType* pEnd;
	int NumLink;

	int Len;
	LinkType* Link;
	int Top,Bottom;

	int Status;
	int Time;

	int NoWay;

	void Open(XStream& fin,NodeType* node,int ind);
	void Save(XStream& fout);
	void Close(void);
};

struct NodeType
{
	int index;
	int x,y,z;

	int *BorderX;
	int *BorderY;

	int NumBranch;
	int* Branches;
	char* Status;

	BranchType** pBranches;
	char Level;
	int TrackCount;

	int Top,Bottom;

	int NumWayNode;
	int* WayBranchIndex;
	int* WayNodeIndex;

	void Open(XStream& fin,BranchType* branch,int ind);
	void Save(XStream& fout);
	void Close(void);
};

/*struct WayNodeType
{
	NodeType* Link;
//	int WayDist;
	int* Index;
	char NumBranch;
	int* LenBranch;
	BranchType** BranchData;
	WayNodeType** NodeData;
};*/

struct TrackType
{
	int NumNode;
	int NumBranch;

	NodeType* node;
	BranchType* branch;

	void Open(char* filename);
	void Save(char* filename);
	void Close(void);
	void GetPosition(int x,int y,LinkType*& ln,BranchType*& bn);
	void GetPosition(int x,int y,LinkType*& ln,BranchType*& bn,NodeType*& n);
	void GetPosition(TrackLinkType* p);
	void GetFirstPosition(int x,int y,LinkType*& ln,BranchType*& bn);
	void GetInsidePosition(TrackLinkType* p);
};

struct TrackLinkType
{
	char PointStatus;
	Vector vPoint;
	LinkType* pNextLink;
	LinkType* pPrevLink;
	BranchType* pBranch;
	NodeType* pNode;

	TrackLinkType(void);
	void ChangeLink(void);
	void HideChangeLink(char dir);
	void AddLink(char s);
	void DecLink(char s);
};

extern TrackType HideTrack;

#endif