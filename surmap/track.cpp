#include "../src/global.h"

#include "sqint.h"

#include "../src/common.h"
#include "sqexp.h"
#include "tools.h"
#include "track.h"

#include "impass.h"
#include "../src/terra/vmap.h"
#include "../src/terra/world.h"
#include "../src/terra/render.h"

#include "port.h"
#define itoa port_itoa
#include "missed.h"

//XStream fout( "out.txt", XS_OUT );

extern int NodeHeight, NodeSlope;
extern iGameMap* curGMap;
extern vrtMap *vMap;
extern int TrackBuild, TerrainMode;
extern int ColorBase;
extern int RenderMode;
extern int MaterSection;
extern int WideSection;
extern int Redraw;
extern sqFont sysfont;

int ColorTable[7] = {COL1,63,127,139,152,175,180};
int WATERLINE = 16;
int ROAD_MATERIAL;
int ROAD_FORCING;

int sqTrackBuild;
int Track_show_all;

static int delta = 20;

int NoiseLevel,NoiseAmp,ShapeNumber,ShapeAmp;

int DeltamH = 0;
int DeltalH = 0;
int cPoint = 0;
int cwBranch = 0;
int cwPoint = 0;
int cBranch = 0;
int cNode = 0;

//int UcutLeft,UcutRight,VcutUp,VcutDown;

double f0( double t, double );
double f1( double t, double );
double f2( double t, double );
double f3( double t, double );
double f4( double t, double );
double f5( double t, double );

PF profils[] = {f0, f1, f2, f3, f4, f5};
eBranch* delBranch = 0;
eNode* delNode = 0;
void drawpoly(int* x, int* y, int n, int, int, double, int, int, int, int, int, int, double, int, int);
void calc_normal_vector( eSection* data, int pos, int& xv, int& yv );
void set_one_section(int,eBranch*&,int,int ,int,int,int,int,int,int);

unsigned realRND(unsigned m);

double f0( double t, double d ){
	return 0.0;
}

double f1( double t, double d ){
	if ( t < 1.0/6) return 0.0;
	else if ( t < 1.0/3 ) return -d/2;
	else if ( t < 2.0/3 ) return -d;
	else if ( t < 5.0/6 ) return -d/2;
	else return 0.0;
}

double f2( double t, double d ){
	if ( t < 0.1 ) return 4.0;
	else if ( t < 0.5 ) return d*2.5*( 0.1 - t );
	else if ( t < 0.9 ) return d*2.5*( t - 0.9 );
	else return 0.0;
}

double f3( double t, double d ){
	double z;

	if ( t < 1.0/3) z = sqrt( 0.17*0.17 - (t-0.17)*(t-0.17) );
	else if ( t < 2.0/3 ) z =-sqrt( 0.17*0.17 - (t-0.5)*(t-0.5) );
	else z = sqrt( 0.17*0.17 - (t-0.83)*(t-0.83) );
	return z*6*d;
}

double f4( double t, double d ){
	double z;

	if ( t < 0.1 ) z = 0.0;
	else if ( t < 0.9 ) z =-sqrt( 0.4*0.4 - (t-0.5)*(t-0.5) );
	else z = 0.0;
	return z*2.5*d;
}

double f5( double t, double d ){
	if ( t < 0.05 ) return t*20*d;
	if ( t < 0.15 ) return d + (0.05-t)*20*d;
	if ( t < 0.25 ) return d + (t-0.25)*20*d;
	if ( t < 0.35 ) return d + (0.25-t)*20*d;
	if ( t < 0.45 ) return d + (t-0.45)*20*d;
	if ( t < 0.55 ) return d + (0.45-t)*20*d;
	if ( t < 0.65 ) return d + (t-0.65)*20*d;
	if ( t < 0.75 ) return d + (0.65-t)*20*d;
	if ( t < 0.85 ) return d + (t-0.85)*20*d;
	if ( t < 0.95 ) return (1.0-t)*20*d;
	return 0.0;
}

void render_line( int xr, int yr, int xl, int yl, int hh, int what );
void lline(int x,int y,int len,int color);
void line( int, int, int, int, int );
void m_line( int, int, int, int, int );

void find_point_on_line( gLine, gLine, int&, int& );
void spline( eSection*& data_base, rSection* data_all, int i, int n_section, int& n_point );

void do_left_buttion_off( int mX, int mY){
      if( Track.check_mouse_work() ) return;

	switch( TrackBuild ){
		case MOVING_NODE:
			Track.set_node( cNode, 0, mX, mY );
			TrackBuild = WAIT;
		break;
		case MOVING_SECTION:
			Track.set_point_in_branch( cBranch, cPoint, mX, mY, -1 );
			TrackBuild = WAIT;
		break;
		case BUILDING:
			if ( Track.find_node( cNode, mX, mY ) ){
				cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
				Track.set_point_in_branch( cBranch, 0, mX, mY, -1 );
				Track.add_branch_to_node( cNode, cBranch );
				Track.build_all_node();
				cNode = 0;
				TrackBuild = WAIT;
			} else if ( Track.find_branch( cBranch, cwBranch, cwPoint, mX, mY ) ) {
//				  if ( cBranch != cwBranch ){
					if ( Track.decompose_branch( cwBranch, cwPoint, cNode ) ) {
						cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
						Track.set_point_in_branch( cBranch, 0, mX, mY, -1 );
						Track.add_branch_to_node( cNode, cBranch );
						Track.set_node( cNode, 1, mX, mY );
						Track.build_all_node();
					} else {
						cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
						Track.set_point_in_branch( cBranch, 0, mX, mY, -1 );
						Track.concate_branch( cwBranch, cBranch );
					}
					TrackBuild = WAIT;
//				  } else {
//					  cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
//				  }
			} else {
				cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
			}
		break;
		case WAIT    :
			if ( Track.find_node( cNode, mX, mY ) ){
				TrackBuild = MOVING_NODE;
			} else if ( Track.find_branch( -1, cBranch, cPoint, mX, mY ) ){
				TrackBuild = MOVING_SECTION;
			} else if ( Track.find_bound_point( cBranch, cPoint, mX, mY ) ){
				TrackBuild = SET_WIDE;
			} else if ( !Track.insert_point_in_branch( mX, mY ) ){
				cBranch = Track.add_branch( mX, mY, cPoint, MIN_WIDE);
				cPoint = 2;
				TrackBuild = BUILDING;
			}
		break;
		case SET_WIDE :
			TrackBuild = WAIT;
		break;
		case SET_HEIGHT :
			Track.get_section_parametr( cBranch, cPoint, NoiseLevel, ShapeNumber, NoiseAmp, ShapeAmp, NodeHeight, NodeSlope, TerrainMode, ColorBase, RenderMode, WideSection, MaterSection);
			cNode = 0;
			sqE -> put(E_TRACKFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			TrackBuild = WAIT;
		break;
		case SET_HEIGHT_NODE :
			ShapeNumber = 0;
			ShapeAmp = 0;
			NodeSlope = 0;
			WideSection = 0;
			RenderMode = 0;
			Track.get_node_parametr( cNode, NoiseLevel, NoiseAmp, NodeHeight, ColorBase, MaterSection, TerrainMode);
			cBranch = 0;
			sqE -> put(E_TRACKFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			TrackBuild = WAIT;
		break;

	}
}

void do_right_buttion_off( int mX, int mY){
      if( Track.check_mouse_work() ) return;

	switch( TrackBuild ){
		case BUILDING:
			Track.set_point_in_branch( cBranch, 0, mX, mY, -1 );
			TrackBuild = WAIT;
		break;
		case MOVING_SECTION:
			if ( Track.decompose_branch( cBranch, cPoint, cNode ) ) {
				cBranch = Track.add_branch( mX, mY, cPoint, MIN_WIDE );
				cPoint = 2;
				Track.add_branch_to_node( cNode, cBranch );
				Track.set_node( cNode, 1, mX, mY );
				Track.build_all_node();
				TrackBuild = BUILDING;
			} else {
				Track.prepare_branch_to_add_point( cBranch, cPoint );
				cPoint = Track.add_point_to_branch( cBranch, mX, mY, MIN_WIDE );
				TrackBuild = BUILDING;
			}
		break;
		case MOVING_NODE:
			cBranch = Track.add_branch( mX, mY, cPoint, MIN_WIDE );
			Track.add_branch_to_node( cNode, cBranch );
			cNode = 0;
			cPoint = 2;
			TrackBuild = BUILDING;
		break;
		case WAIT    :
			if ( Track.find_node( cNode, mX, mY ) ){
				if(!Track.delete_node( cNode ) ){
					ShapeNumber = 0;
					ShapeAmp = 0;
					NodeSlope = 0;
					WideSection = 0;
					RenderMode = 0;
					Track.get_node_parametr( cNode, NoiseLevel, NoiseAmp, NodeHeight, ColorBase, MaterSection, TerrainMode );
					Track.show_node_height( cNode );
					cBranch = 0;
					sqTrackBuild = SET_HEIGHT_NODE;
					sqE -> put(E_TRACKFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
				}
			} else if ( Track.find_branch( -1, cBranch, cPoint, mX, mY ) ){
				Track.delete_point_from_branch( cBranch, cPoint );
				Track.build_all_node();
			} else if ( Track.find_bound_point( cBranch, cPoint, mX, mY ) ){
				Track.get_section_parametr( cBranch, cPoint, NoiseLevel, ShapeNumber, NoiseAmp, ShapeAmp, NodeHeight, NodeSlope, TerrainMode, ColorBase, RenderMode, WideSection, MaterSection);
				cNode = 0;
				Track.show_branch_height( cBranch, cPoint);

				sqTrackBuild = SET_HEIGHT;
				sqE -> put(E_TRACKFORM,E_COMMON,XGR_MAXX/2,XGR_MAXY/2);
			} else {
				cBranch = Track.insert_point_in_branch( mX, mY );
				if ( cBranch )
					Track.delete_branch( cBranch );
				else
					Track.build_all_node();
//					  Track.build_all_spline();
					curGMap -> shift(mX - curGMap -> CX,mY - curGMap -> CY);
			}
		break;
		case SET_WIDE :
			TrackBuild = WAIT;
		break;
		case SET_HEIGHT :
			TrackBuild = WAIT;
		break;
		case SET_HEIGHT_NODE :
			TrackBuild = WAIT;
		break;
	}
}

void line( int x1, int y1, int x2, int y2, int color){
	int CX = curGMap -> CX;
	int CY = curGMap -> CY;
	int xc = curGMap -> xc;
	int yc = curGMap -> yc;

	int xx1, yy1, yy2, xx2;

	xx1 = ((x1&clip_mask_x) - CX);

	yy1 = y1 - CY;
	xx2 = ((x2&clip_mask_x) - CX);

	yy2 = y2 - CY;

	//int old_x1 = xx1;
	//int old_x2 = xx2;

	if( xx1 > ((int)clip_mask_x/2) ) xx1 = xx1 - clip_mask_x-1;
	if( xx1 < (-(int)clip_mask_x/2) ) xx1 = xx1 + clip_mask_x+1;

	if ( xx2 > ((int)clip_mask_x/2) ) xx2 = xx2 - clip_mask_x-1;
	if ( xx2 < (-(int)clip_mask_x/2) ) xx2 = xx2 + clip_mask_x+1;

	if ( abs( xx1 - xx2 ) < XGR_MAXX )
		m_line( xx1 + xc, yy1 + yc, xx2 + xc, yy2 + yc, color);
}

sTrack::sTrack(void){
	n_node = 0;
	n_branch = 0;

	node = 0;
	branch = 0;

	linkingLog = 0;
	nTails = new eNode*[PART_MAX];
	bTails = new eBranch*[PART_MAX];
}

eBranch::eBranch(void){
	index = 0;
	n_section = 0;
	n_point = 0;
	beg_node = 0;
	end_node = 0;
	all_on = 0;
	color = 0;
	material = 2;
	is_render = 1;

	xpb = new int[3];
	ypb = new int[3];
	xpe = new int[3];
	ype = new int[3];

	data_base = new eSection[MAX_BASE];
//	  data_all = new rSection[MAX_ALL];
	data_all = NULL;

	l = 0;
	r = 0;
}

eBranch::~eBranch(void){
	delete[] data_base;
	delete[] xpb;
	delete[] ypb;
	delete[] xpe;
	delete[] ype;
	if ( data_all != NULL ) delete[] data_all;
}


void eBranch::show(int all){
	int i = n_section;

	if ( beg_node != 0 ){
		line(xpb[1], ypb[1], xpb[0], ypb[0], ColorTable[ color ]);
		line(xpb[1], ypb[1], xpb[2], ypb[2], ColorTable[ color ]);
		line(xpb[2], ypb[2], xpb[0], ypb[0], ColorTable[ color ]);
	}

	if ( end_node != 0 ){
		i = n_section-1;
		line(xpe[1], ype[1], xpe[0], ype[0], ColorTable[ color ]);
		line(xpe[1], ype[1], xpe[2], ype[2], ColorTable[ color ]);
		line(xpe[2], ype[2], xpe[0], ype[0], ColorTable[ color ]);
	}

	if (all_on) {
		for( i = 0; i < n_point-1; i++ ){
			line(data_all[i].xl, data_all[i].yl, data_all[i+1].xl, data_all[i+1].yl, ColorTable[ color ]);
			line(data_all[i].xr, data_all[i].yr, data_all[i+1].xr, data_all[i+1].yr, ColorTable[ color ]);
		}

//		  for( i = 0; i < n_point; i++ ){
//			  line(data_all[i].x-1, data_all[i].y-1, data_all[i].x+1, data_all[i].y-1, COLOR_TRACK);
//			  line(data_all[i].x-1, data_all[i].y, data_all[i].x+1, data_all[i].y, COLOR_TRACK);
//			  line(data_all[i].x-1, data_all[i].y+1, data_all[i].x+1, data_all[i].y+1, COLOR_TRACK);
//		  }
	} else {
		for( i = 0; i < n_section-1; i++ ){
			line(data_base[i].x, data_base[i].y, data_base[i+1].x, data_base[i+1].y, ColorTable[ color ]);
			if ( (data_base[i].x != data_base[i+1].x) || (data_base[i].y != data_base[i+1].y) ){
				line(data_base[i].xl, data_base[i].yl, data_base[i+1].xl, data_base[i+1].yl, ColorTable[ color ]);
				line(data_base[i].xr, data_base[i].yr, data_base[i+1].xr, data_base[i+1].yr, ColorTable[ color ]);

				line(data_base[i].xr, data_base[i].yr, data_base[i].xl, data_base[i].yl, ColorTable[ color ]);
				line(data_base[i+1].xr, data_base[i+1].yr, data_base[i+1].xl, data_base[i+1].yl, ColorTable[ color ]);
			}
		}

		for( i = 0; i < n_section; i++ ){
			line(data_base[i].x-1, data_base[i].y-1, data_base[i].x+1, data_base[i].y-1, ColorTable[ color ]);
			line(data_base[i].x-1, data_base[i].y, data_base[i].x+1, data_base[i].y, ColorTable[ color ]);
			line(data_base[i].x-1, data_base[i].y+1, data_base[i].x+1, data_base[i].y+1, ColorTable[ color ]);
		}
	}
}

void eNode::show(void){
	line(x-2, y-2, x+2, y-2, ColorTable[ color ]);
	line(x-2, y-1, x+2, y-1, ColorTable[ color ]);
	line(x-2, y, x+2, y, ColorTable[ color ]);
	line(x-2, y+1, x+2, y+1, ColorTable[ color ]);
	line(x-2, y+2, x+2, y+2, ColorTable[ color ]);
}

void sTrack::show(void ){
	int i;
	eBranch* l_branch = branch;

	for( i = 0; i < n_branch; i++ ){
		l_branch -> show(0);
		l_branch = l_branch -> r;
	}

	eNode* l_node = node;

	for( i = 0; i < n_node; i++ ){
		l_node -> show();
		l_node = l_node -> r;
	}

	if ( Track_show_all) show_all_track();
/*
	if ( TrackBuild == SET_HEIGHT ){
		show_branch_hight( cBranch, cPoint);
	}
	if ( TrackBuild == SET_HEIGHT_NODE ){
		show_node_hight( cNode );
	}
*/
}

int sTrack::add_branch( int X, int Y, int &pos, int WIDE){
	eBranch* l_branch;

	if ( n_branch > 0 ){
		n_branch++;
		if ( delBranch != 0 ){
			l_branch = delBranch;
			delBranch = delBranch -> r;

			l_branch -> end_node = 0;
			l_branch -> beg_node = 0;
			l_branch -> n_section = 0;
			l_branch -> n_point = 0;
			l_branch -> color = 0;
			l_branch -> material = 2;
			l_branch -> is_render = 1;
		} else {
			l_branch = new eBranch;
			l_branch -> index = n_branch;
		}

		l_branch -> l = branch -> l;
		l_branch -> r = branch;
		l_branch -> l -> r = l_branch;
		branch -> l = l_branch;
		pos = l_branch -> add_section( X, Y, WIDE);
	} else {
		n_branch++;
		if ( delBranch != 0 ){
			branch = delBranch;
			delBranch = delBranch -> r;

			branch -> end_node = 0;
			branch -> beg_node = 0;
			branch -> n_section = 0;
			branch -> n_point = 0;
			branch -> color = 0;
			branch -> material = 2;
			branch -> is_render = 1;
		} else {
			branch = new eBranch;
			branch -> index = n_branch;
		}
		branch -> l = branch;
		branch -> r = branch;
		l_branch = branch;
		pos = branch -> add_section( X, Y, WIDE);
	}
	return l_branch -> index;
}

int sTrack::add_node( int X, int Y, int H){
	eNode* l_node;

	if ( n_node > 0 ){
		if ( delNode != 0 ){
			l_node = delNode;
			delNode = delNode -> r;
			l_node -> n_branch = 0;
			l_node -> color = 0;
			l_node -> material = 2;
			l_node -> mode = 0;

			l_node -> x = X;
			l_node -> y = Y;
			l_node -> h = H;

			l_node -> polygon_on = 0;
			l_node -> noise = 0;
			l_node -> dnoise = 0;
			n_node++;
		} else {
			n_node++;
			l_node = new eNode( X, Y, H );
			l_node -> index = n_node;
		}

		l_node -> l = node -> l;
		l_node -> r = node;
		l_node -> l -> r = l_node;
		node -> l = l_node;
	} else {
		n_node++;
		if ( delNode != 0 ){
			node = delNode;
			delNode = delNode -> r;
			node -> n_branch = 0;
			node -> color = 0;
			node -> material = 2;
			node -> mode = 0;
			node -> polygon_on = 0;
			node -> noise = 0;
			node -> dnoise = 0;

			node -> x = X;
			node -> y = Y;
			node -> h = H;
		} else {
			node = new eNode( X, Y, H );
			node -> index = n_node;
		}
		node -> l = node;
		node -> r = node;
		l_node = node;
	}
	return l_node -> index;
}

void sTrack::add_branch_to_node( int cNode, int cBranch ){
	eBranch* l_branch = branch;
	eNode* l_node = node;

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch ) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find Branch in add_branch_to_node" );
	i = 0;
	while( l_node -> index != cNode && i < n_node ) {
		l_node = l_node -> r;
		i++;
	}
	if( i == n_node ) ErrH.Abort( "Dont Find Node in add_branch_to_node" );

	if ( abs(l_branch -> data_base[0].x-l_node -> x) < delta && abs(l_branch -> data_base[0].y-l_node -> y) < delta ){
		l_branch -> beg_node = cNode;
		l_node -> add_branch( cBranch );
	} else {
		l_branch -> end_node = cNode;
		l_node -> add_branch( cBranch );
	}
}

int sTrack::add_point_to_branch(int pos, int X, int Y, int WIDE){
	eBranch* l_branch = branch;
	int pos_p;

	int i = 0;
	while( l_branch -> index != pos && i < n_branch ) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find Branch in add_point_to_" );

	pos_p = l_branch -> add_section( X, Y, WIDE );
	return pos_p;
}

void sTrack::set_point_in_branch( int pos1, int pos2, int X, int Y, int hh ){
	eBranch* l_branch = branch;

	int i = 0;
	while( l_branch -> index != pos1 && i < n_branch ) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find Branch in set_point_in" );

	if ( l_branch -> n_section == 2 && pos2 < 1 )
		delete_branch( pos1 );
	else
		l_branch -> set_section( pos2, X, Y, hh );
	build_all_node();
}

void sTrack::set_node( int cNode, int Mode, int X, int Y){
	int i;
	eNode* l_node = node;

	i = 0;
	while( l_node -> index != cNode && i < n_node ) {
		l_node = l_node -> r;
		i++;
	}
	if( i == n_node ) ErrH.Abort( "Dont Find Node in set_node" );

	l_node -> polygon_on = 0;

	if ( Mode )
		for( i = 0; i < l_node -> n_branch; i++ ){
			eBranch* l_branch = branch;

			while( l_branch -> index != l_node -> branches[i] ) l_branch = l_branch -> r;

			if ( l_branch -> beg_node == cNode ) l_branch -> set_section( 1, X, Y, l_node -> h );

			register int pos = l_branch -> n_section;

			if ( l_branch -> end_node == cNode ) l_branch -> set_section( pos, X, Y, l_node -> h );
		}
	else
		for( i = 0; i < l_node -> n_branch; i++ ){
			eBranch* l_branch = branch;

			while( l_branch -> index != l_node -> branches[i] ) l_branch = l_branch -> r;

			if ( l_branch -> beg_node == cNode ) l_branch -> set_section( 1, X, Y, -1 );

			register int pos = l_branch -> n_section;

			if ( l_branch -> end_node == cNode ) l_branch -> set_section( pos, X, Y, -1 );
		}


	l_node -> x = X;
	l_node -> y = Y;
	build_all_node();
}

void sTrack::curr_build(void){
	int mX;
	int mY;

	int xm = XGR_MouseObj.PosX;
	int ym = XGR_MouseObj.PosY;
	mX = (xm - curGMap -> xc + curGMap -> CX) & clip_mask_x;
	mY = (ym - curGMap -> yc + curGMap -> CY) & clip_mask_y;

//	  Redraw = 1;

	switch( TrackBuild ){
		case MOVING_NODE:
			set_node( cNode, 0, mX, mY );
		break;
		case MOVING_SECTION :
		case BUILDING:
			set_point_in_branch( cBranch, cPoint, mX, mY, -1 );
		break;
		case WAIT    :
		break;
		case SET_HEIGHT:
		break;
		case SET_WIDE:
			set_wide_in_branch( cBranch, cPoint, mX, mY );
		break;
	}
}

int sTrack::find_branch( int cBranch, int& cwBranch, int& cPoint, int& X, int& Y ){
	eBranch* l_branch = branch;
	int i;

	for( i = 0; i < n_branch; i++){
		if ( l_branch -> index != cBranch ){
			if ( l_branch -> find_point( cPoint, X, Y ) ){
				cwBranch = l_branch -> index;
				return 1;
			}
		}
		l_branch = l_branch -> r;
	}
	return 0;
}

int sTrack::find_node( int& cNode, int& X, int& Y ){
	eNode* l_node = node;
	int i;

	if ( n_node == 0 ) return 0;

	for( i = 0; i < n_node; i++){
		if ( l_node -> find_point( X, Y ) ){
			cNode = l_node -> index;
			return 1;
		}
		l_node = l_node -> r;
	}
	return 0;
}

void sTrack::delete_point_from_branch( int cBranch, int cPoint ){
	eBranch* l_branch = branch;
	int i;

	for( i = 0; i < n_branch; i++){
		if ( l_branch -> index == cBranch ){
			if ( !l_branch -> delete_point( cPoint ) )
				delete_branch( cBranch );
			return;
		}
		l_branch = l_branch -> r;
	}
	build_all_node();
}

int eBranch::add_section( int X, int Y, int WIDE){

	if (!n_section){
		data_base[n_section].x = X;
		data_base[n_section].y = Y;
		data_base[n_section].h = H_MAIN;
		data_base[n_section].hl = H_MAIN;
		data_base[n_section].hr = H_MAIN;
		data_base[n_section].wide = WIDE;
		data_base[n_section].noise = 0;
		data_base[n_section].dnoise = 0;
		data_base[n_section].profil = 0;
		data_base[n_section].dprofil = 0;
		data_base[n_section].mode = 0;
		data_base[n_section].point = 0;
		n_section+=1;
	}

	data_base[n_section].x = X;
	data_base[n_section].y = Y;
	data_base[n_section].h = H_MAIN;
	data_base[n_section].hl = H_MAIN;
	data_base[n_section].hr = H_MAIN;
	data_base[n_section].wide = WIDE;
	data_base[n_section].dnoise = 0;
	data_base[n_section].noise = 0;
	data_base[n_section].profil = 0;
	data_base[n_section].dprofil = 0;
	data_base[n_section].mode = 0;
	data_base[n_section].point = 0;

	n_section+=1;
	if ( n_section == MAX_BASE ) n_section--;
	return n_section;
}

void eBranch::set_section( int pos, int X, int Y, int hh){
	if ( pos == 0 ) {
		n_section--;
		set_wide( n_section );
	} else {
		pos--;
		data_base[pos].x = X;
		data_base[pos].y = Y;

		if ( hh > 0 ) {
			data_base[pos].hl += hh - data_base[pos].hl;
			data_base[pos].hr += hh - data_base[pos].hr;
			data_base[pos].h = hh;
		}

		set_wide( pos );
		set_wide( pos+1 );
		set_wide( pos+2 );
	}
}

int eBranch::find_point( int& cPoint, int& X, int& Y){
	int i;
	int _x = X;
	int _y = Y;

	for( i = 0; i < n_section; i++ ){
		int dop = 0;

		if ((data_base[i].x - X) > (int)clip_mask_x) dop = clip_mask_x+1;
		else if ((data_base[i].x - X) < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		_x += dop;

		dop = 0;
		if ((data_base[i].y - Y) > (int)clip_mask_y) dop = clip_mask_y+1;
		else if ((data_base[i].y - Y) < -(int)clip_mask_y) dop = -((int)clip_mask_y+1);

		_y += dop;

		if ( abs(data_base[i].x-_x) < delta && abs(data_base[i].y-_y) < delta ){
			X = data_base[i].x;
			Y = data_base[i].y;
			cPoint = i+1;
			return 1;
		}
	}
	return 0;
}

int eBranch::delete_point( int pos){
	int i;

	if ( n_section	< 3 ) return 0;

	for( i = pos-1; i < n_section-1; i++){
		data_base[i].x = data_base[i+1].x;
		data_base[i].y = data_base[i+1].y;
		data_base[i].h = data_base[i+1].h;
		data_base[i].hl = data_base[i+1].hl;
		data_base[i].hr = data_base[i+1].hr;
		data_base[i].wide = data_base[i+1].wide;
		data_base[i].noise = data_base[i+1].noise;
		data_base[i].dnoise = data_base[i+1].dnoise;
		data_base[i].profil = data_base[i+1].profil;
		data_base[i].dprofil = data_base[i+1].dprofil;
		data_base[i].mode = data_base[i+1].mode;
	}
	n_section--;
	rebuild_wide();

	return 1;
}

eNode::eNode(int X, int Y, int H){
	index = 0;
	n_branch = 0;
	color = 0;
	material = 2;
	mode = 0;

	x = X;
	y = Y;
	h = H;

	xp = new int[MAX_BRANCH];
	yp = new int[MAX_BRANCH];
	branches = new int[MAX_BRANCH];

	polygon_on = 0;
	noise = 0;
	dnoise = 0;
	l = 0;
	r = 0;
}

eNode::~eNode(void){
	delete[] xp;
	delete[] yp;
	delete[] branches;
}

int eNode::find_point( int& X, int& Y){
	if ( (abs(x-X) < delta) && (abs(y-Y) < delta) ){
		X = x;
		Y = y;
		return 1;
	}
	return 0;
}

void eNode::add_branch( int cBranch ){
	branches[n_branch] = cBranch;
	n_branch++;
}

void sTrack::concate_branch( int cwBranch, int cBranch ){
	eBranch* lw_branch = branch;
	eBranch* ls_branch = branch;
	int i;

	if ( cwBranch == cBranch ) return;

	i = 0;
	while( lw_branch -> index != cwBranch && i < n_branch ) {
		lw_branch = lw_branch -> r;
		i++;
	}
	if ( i == n_branch ) ErrH.Abort( "Dont Find cwBranch .." );

	i = 0;
	while( ls_branch -> index != cBranch && i < n_branch) {
		ls_branch = ls_branch -> r;
		i++;
	}
	if ( i == n_branch ) ErrH.Abort( "Dont Find cBranch .." );

	int pos = lw_branch -> n_section-1;
	int pos2 = ls_branch -> n_section-1;

	if ( abs(lw_branch -> data_base[0].x-ls_branch -> data_base[pos2].x) < 2*delta && abs(lw_branch -> data_base[0].y-ls_branch -> data_base[pos2].y) < 2*delta){
		for( i = 1; i < lw_branch -> n_section; i++ ){
			ls_branch -> data_base[i+pos2].x = lw_branch -> data_base[i].x;
			ls_branch -> data_base[i+pos2].y = lw_branch -> data_base[i].y;
			ls_branch -> data_base[i+pos2].h = lw_branch -> data_base[i].h;
			ls_branch -> data_base[i+pos2].hl = lw_branch -> data_base[i].hl;
			ls_branch -> data_base[i+pos2].hr = lw_branch -> data_base[i].hr;
			ls_branch -> data_base[i+pos2].wide = lw_branch -> data_base[i].wide;
			ls_branch -> data_base[i+pos2].noise = lw_branch -> data_base[i].noise;
			ls_branch -> data_base[i+pos2].dnoise = lw_branch -> data_base[i].dnoise;
			ls_branch -> data_base[i+pos2].profil = lw_branch -> data_base[i].profil;
			ls_branch -> data_base[i+pos2].dprofil = lw_branch -> data_base[i].dprofil;
			ls_branch -> data_base[i+pos2].mode = lw_branch -> data_base[i].mode;
		}
		ls_branch -> n_section += pos;

		if ( lw_branch -> end_node > 0 ) {
			delete_branch_from_node(lw_branch -> end_node, cwBranch);
			add_branch_to_node( lw_branch -> end_node, cBranch);
		}
		ls_branch -> end_node = lw_branch -> end_node;
		lw_branch -> end_node = 0;

		delete_branch( cwBranch );
	} else {
		for( i = 1; i <= pos; i++ ){
			ls_branch -> data_base[i+pos2].x = lw_branch -> data_base[pos-i].x;
			ls_branch -> data_base[i+pos2].y = lw_branch -> data_base[pos-i].y;
			ls_branch -> data_base[i+pos2].h = lw_branch -> data_base[pos-i].h;
			ls_branch -> data_base[i+pos2].hl = lw_branch -> data_base[pos-i].hl;
			ls_branch -> data_base[i+pos2].hr = lw_branch -> data_base[pos-i].hr;
			ls_branch -> data_base[i+pos2].wide = lw_branch -> data_base[pos-i].wide;
			ls_branch -> data_base[i+pos2].noise = lw_branch -> data_base[pos-i].noise;
			ls_branch -> data_base[i+pos2].dnoise = lw_branch -> data_base[pos-i].dnoise;
			ls_branch -> data_base[i+pos2].profil = lw_branch -> data_base[pos-i].profil;
			ls_branch -> data_base[i+pos2].dprofil = lw_branch -> data_base[pos-i].dprofil;
			ls_branch -> data_base[i+pos2].mode = lw_branch -> data_base[pos-i].mode;
		}
		ls_branch -> n_section += pos;

		if ( lw_branch -> beg_node > 0 ) {
			delete_branch_from_node(lw_branch -> beg_node, cwBranch);
			add_branch_to_node( lw_branch -> beg_node, cBranch);
		}
		ls_branch -> end_node = lw_branch -> beg_node;
		lw_branch -> beg_node = 0;

		delete_branch( cwBranch );
	}
	ls_branch -> rebuild_wide();
}

int sTrack::decompose_branch( int cwBranch, int cwPoint, int& cNode ){
	eBranch* l_branch = branch;
	int i;
	int pos, loc;

	i = 0;
	while( l_branch -> index != cwBranch && i < n_branch ) {
		l_branch = l_branch -> r;
		i++;
	}
	if ( i == n_branch ) ErrH.Abort( "Dont find branch in decompose_branch" );

	if ( (cwPoint == 1) || (cwPoint == l_branch -> n_section) ) return 0;

	loc = add_branch( l_branch -> data_base[cwPoint-1].x, l_branch -> data_base[cwPoint-1].y, pos, l_branch -> data_base[cwPoint-1].wide );

	eBranch* lw_branch = branch;
	i = 0;
	while( lw_branch -> index != loc && i < n_branch ) {
		lw_branch = lw_branch -> r;
		i++;
	}
	if ( i == n_branch ) ErrH.Abort( "Dont find newbranch in decompose_branch" );
	lw_branch -> color = l_branch -> color;

	pos = 0;
	for( i = cwPoint-1; i < l_branch -> n_section; i++, pos++ ){
		lw_branch -> data_base[pos].x = l_branch -> data_base[i].x;
		lw_branch -> data_base[pos].y = l_branch -> data_base[i].y;
		lw_branch -> data_base[pos].h = l_branch -> data_base[i].h;
		lw_branch -> data_base[pos].xr = l_branch -> data_base[i].xr;
		lw_branch -> data_base[pos].yr = l_branch -> data_base[i].yr;
		lw_branch -> data_base[pos].hr = l_branch -> data_base[i].hr;
		lw_branch -> data_base[pos].xl = l_branch -> data_base[i].xl;
		lw_branch -> data_base[pos].yl = l_branch -> data_base[i].yl;
		lw_branch -> data_base[pos].hl = l_branch -> data_base[i].hl;
		lw_branch -> data_base[pos].wide = l_branch -> data_base[i].wide;
		lw_branch -> data_base[pos].profil = l_branch -> data_base[i].profil;
		lw_branch -> data_base[pos].dprofil = l_branch -> data_base[i].dprofil;
		lw_branch -> data_base[pos].noise = l_branch -> data_base[i].noise;
		lw_branch -> data_base[pos].dnoise = l_branch -> data_base[i].dnoise;
		lw_branch -> data_base[pos].mode = l_branch -> data_base[i].mode;
	}

	lw_branch -> n_section = pos;
	l_branch -> n_section = cwPoint;

	cNode = add_node( l_branch -> data_base[cwPoint-1].x, l_branch -> data_base[cwPoint-1].y, l_branch -> data_base[cwPoint-1].h );

	if (l_branch -> end_node > 0){
		delete_branch_from_node(l_branch -> end_node, cwBranch);
		add_branch_to_node( l_branch -> end_node, loc);
		l_branch -> end_node = 0;
	}

	add_branch_to_node( cNode, loc );
	add_branch_to_node( cNode, cwBranch );

	return 1;
}

void sTrack::delete_branch_from_node( int cNode, int cBranch ){
	eNode* l_node = node;
	int i;

	if ( n_node == 0 ) return;

	if ( cNode <= 0 ) return;

	i = 0;
	while( l_node -> index != cNode && i < n_node) {
		l_node = l_node -> r;
		i++;
	}
	if( i == n_node ) ErrH.Abort( "Dont Find Node in delete branch.." );

	for( i = 0; l_node -> branches[i] != cBranch && i < l_node -> n_branch; i++ );

	if( i == l_node -> n_branch ) ErrH.Abort( "Dont Find Branch in Node" );

	l_node -> n_branch--;
	for(; i < l_node -> n_branch; i++ )
		l_node -> branches[i] = l_node -> branches[i+1];
	build_all_node();
}

void sTrack::delete_branch( int cwBranch ){
	eBranch* l_branch = branch;

	if ( cwBranch < 1 ) ErrH.Abort( "Error in struct Track" );

	int i = 0;
	while( l_branch -> index != cwBranch && i < n_branch) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find deleted Branch" );

	delete_branch_from_node( l_branch -> beg_node, cwBranch );
	l_branch -> beg_node = 0;
	delete_branch_from_node( l_branch -> end_node, cwBranch );
	l_branch -> end_node = 0;

	l_branch -> r -> l = l_branch -> l;
	l_branch -> l -> r = l_branch -> r;

	if (delBranch != 0){
		l_branch -> r = delBranch;
		delBranch = l_branch;
	} else {
		delBranch = l_branch;
		delBranch -> r = 0;
	}
	branch = l_branch -> l;
	n_branch--;
	if(n_branch == 0) branch = 0;
	build_all_node();
}

int sTrack::delete_node( int cNode ){
	eNode* l_node = node;
	eBranch* l_branch = branch;

	if ( cNode < 1 ) ErrH.Abort("Error in struct Node" );

	int i = 0;
	while( l_node -> index != cNode && i < n_node ) {
		l_node = l_node -> r;
		i++;
	}

	if ( i == n_node ) ErrH.Abort( " Dont Find Node" );

	if ( l_node -> n_branch > 2 )
		return 0;
	else if ( l_node -> n_branch == 2 ) {
		prepare_branch_concate( l_node -> branches[0], cNode );
		concate_branch( l_node -> branches[1], l_node -> branches[0] );
	} else if ( l_node -> n_branch == 1 ){

		i = 0;
		while( l_branch -> index != l_node -> branches[0] && i < n_branch ) {
			l_branch = l_branch -> r;
			i++;
		}

		if ( i == n_branch ) ErrH.Abort( " Dont Find branch for Node.." );

		if ( l_branch -> beg_node == cNode ) l_branch -> beg_node = 0;
		if ( l_branch -> end_node == cNode ) l_branch -> end_node = 0;
	}

	l_node -> r -> l = l_node -> l;
	l_node -> l -> r = l_node -> r;

	node = l_node -> l;

	if (delNode != 0){
		l_node -> r = delNode;
		delNode = l_node;
	} else {
		delNode = l_node;
		delNode -> r = 0;
	}

	n_node--;
	if ( n_node == 0 ) node = 0;
	build_all_node();
	return 1;
}

void sTrack::prepare_branch_to_add_point( int cBranch, int cPoint ){
	eBranch* l_branch = branch;
	int i;

	if ( cPoint != 1 ) return;

	while( l_branch -> index != cBranch ) l_branch = l_branch -> r;

	int pos = l_branch -> n_section-1;
	for( i = 0; i < l_branch -> n_section/2; i++ ){
		register int tmp = l_branch -> data_base[i].x;
		l_branch -> data_base[i].x = l_branch -> data_base[pos-i].x;
		l_branch -> data_base[pos-i].x = tmp;

		tmp = l_branch -> data_base[i].y;
		l_branch -> data_base[i].y = l_branch -> data_base[pos-i].y;
		l_branch -> data_base[pos-i].y = tmp;

		tmp = l_branch -> data_base[i].h;
		l_branch -> data_base[i].h = l_branch -> data_base[pos-i].h;
		l_branch -> data_base[pos-i].h = tmp;

		tmp = l_branch -> data_base[i].hl;
		l_branch -> data_base[i].hl = l_branch -> data_base[pos-i].hr;
		l_branch -> data_base[pos-i].hr = tmp;

		tmp = l_branch -> data_base[i].hr;
		l_branch -> data_base[i].hr = l_branch -> data_base[pos-i].hl;
		l_branch -> data_base[pos-i].hl = tmp;

		tmp = l_branch -> data_base[i].wide;
		l_branch -> data_base[i].wide = l_branch -> data_base[pos-i].wide;
		l_branch -> data_base[pos-i].wide = tmp;

		tmp = l_branch -> data_base[i].noise;
		l_branch -> data_base[i].noise = l_branch -> data_base[pos-i].noise;
		l_branch -> data_base[pos-i].noise = tmp;

		tmp = l_branch -> data_base[i].dnoise;
		l_branch -> data_base[i].dnoise = l_branch -> data_base[pos-i].dnoise;
		l_branch -> data_base[pos-i].dnoise = tmp;

		tmp = l_branch -> data_base[i].profil;
		l_branch -> data_base[i].profil = l_branch -> data_base[pos-i].profil;
		l_branch -> data_base[pos-i].profil = tmp;

		tmp = l_branch -> data_base[i].dprofil;
		l_branch -> data_base[i].dprofil = l_branch -> data_base[pos-i].dprofil;
		l_branch -> data_base[pos-i].dprofil = tmp;

		tmp = l_branch -> data_base[i].mode;
		l_branch -> data_base[i].mode = l_branch -> data_base[pos-i].mode;
		l_branch -> data_base[pos-i].mode = tmp;
	}
	register int tmp = l_branch -> end_node;
	l_branch -> end_node = l_branch -> beg_node;
	l_branch -> beg_node = tmp;

	l_branch -> rebuild_wide();
}

void sTrack::prepare_branch_concate( int cBranch, int cNode ){
	eBranch* l_branch = branch;
	int i;

	if ( cNode < 1 ) ErrH.Abort( "This Node dont exist" );

	i = 0;
	while( l_branch -> index != cBranch && i < n_branch ) {
		l_branch = l_branch -> r;
		i++;
	}

	if ( i == n_branch ) ErrH.Abort( "Dont Find Branch" );

	if ( cNode == l_branch -> end_node ) return;
	if ( cNode != l_branch -> beg_node ) ErrH.Abort( "Error in Struct" );

	int pos = l_branch -> n_section-1;
	for( i = 0; i < l_branch -> n_section/2; i++ ){
		int tmp = l_branch -> data_base[i].x;
		l_branch -> data_base[i].x = l_branch -> data_base[pos-i].x;
		l_branch -> data_base[pos-i].x = tmp;

		tmp = l_branch -> data_base[i].y;
		l_branch -> data_base[i].y = l_branch -> data_base[pos-i].y;
		l_branch -> data_base[pos-i].y = tmp;

		tmp = l_branch -> data_base[i].h;
		l_branch -> data_base[i].h = l_branch -> data_base[pos-i].h;
		l_branch -> data_base[pos-i].h = tmp;

		tmp = l_branch -> data_base[i].hl;
		l_branch -> data_base[i].hl = l_branch -> data_base[pos-i].hl;
		l_branch -> data_base[pos-i].hl = tmp;

		tmp = l_branch -> data_base[i].hr;
		l_branch -> data_base[i].hr = l_branch -> data_base[pos-i].hr;
		l_branch -> data_base[pos-i].hr = tmp;

		tmp = l_branch -> data_base[i].wide;
		l_branch -> data_base[i].wide = l_branch -> data_base[pos-i].wide;
		l_branch -> data_base[pos-i].wide = tmp;

		tmp = l_branch -> data_base[i].noise;
		l_branch -> data_base[i].noise = l_branch -> data_base[pos-i].noise;
		l_branch -> data_base[pos-i].noise = tmp;

		tmp = l_branch -> data_base[i].dnoise;
		l_branch -> data_base[i].dnoise = l_branch -> data_base[pos-i].dnoise;
		l_branch -> data_base[pos-i].dnoise = tmp;

		tmp = l_branch -> data_base[i].profil;
		l_branch -> data_base[i].profil = l_branch -> data_base[pos-i].profil;
		l_branch -> data_base[pos-i].profil = tmp;

		tmp = l_branch -> data_base[i].dprofil;
		l_branch -> data_base[i].dprofil = l_branch -> data_base[pos-i].dprofil;
		l_branch -> data_base[pos-i].dprofil = tmp;

		tmp = l_branch -> data_base[i].mode;
		l_branch -> data_base[i].mode = l_branch -> data_base[pos-i].mode;
		l_branch -> data_base[pos-i].mode = tmp;
	}
	int itmp = l_branch -> end_node;
	l_branch -> end_node = l_branch -> beg_node;
	l_branch -> beg_node = itmp;

	l_branch -> rebuild_wide();
}

int sTrack::insert_point_in_branch( int X, int Y ){
	eBranch* l_branch = branch;
	int i;

	for( i = 0; i < n_branch; i++){
		if ( l_branch -> insert_point( X, Y ) ){
			l_branch -> rebuild_wide();
			return l_branch -> index;
		}
		l_branch = l_branch -> r;
	}
	return 0;
}

int eBranch::insert_point( int X, int Y ){
	int i;

	for( i = 0; i < n_section-1; i++ ){
		int minx;
		int miny;
		int maxx;
		int maxy;

		int x1 = data_base[i].x;
		int x2 = data_base[i+1].x;

		int y1 = data_base[i].y;
		int y2 = data_base[i+1].y;

		int dop = 0;

		if (x1 - x2 > (int)clip_mask_x) dop = clip_mask_x+1;
		else if (x1 - x2 < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		x2 += dop;
		dop = 0;

		if (x1 - X  > (int)clip_mask_x) dop = clip_mask_x+1;
		else if (x1 - X  < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);
		X += dop;

		if ( x1 > x2 ){
			maxx = x1;
			minx = x2;
		} else {
			maxx = x2;
			minx = x1;
		}

		if ( y1 > y2 ){
			maxy = y1;
			miny = y2;
		} else {
			maxy = y2;
			miny = y1;
		}

		if ( (minx-delta) <= X && (maxx+delta) >= X && (miny-delta) <= Y && (maxy+delta) >= Y ){
			int yy = 0;

			if ( abs(x2-x1) > delta )
				yy = (int)(y1 + double((X - x1)*( y2 - y1))/( x2 - x1));
			else
				yy = Y;

			if ( abs(yy - Y) < delta ) {
				int j;
				for( j = n_section; j > i+1; j-- ){
					data_base[j].x = data_base[j-1].x;
					data_base[j].y = data_base[j-1].y;
					data_base[j].h = data_base[j-1].h;
					data_base[j].hl = data_base[j-1].hl;
					data_base[j].hr = data_base[j-1].hr;
					data_base[j].wide = data_base[j-1].wide;
					data_base[j].noise = data_base[j-1].noise;
					data_base[j].dnoise = data_base[j-1].dnoise;
					data_base[j].profil = data_base[j-1].profil;
					data_base[j].dprofil = data_base[j-1].dprofil;
					data_base[j].mode = data_base[j-1].mode;
				}
				data_base[i+1].x = X&clip_mask_x;
				data_base[i+1].y = Y;
				data_base[i+1].h = H_MAIN;
				data_base[i+1].hl = H_MAIN;
				data_base[i+1].hr = H_MAIN;
				data_base[i+1].wide = MIN_WIDE;
				data_base[i+1].noise = 0;
				data_base[i+1].dnoise = 0;
				data_base[i+1].profil = 0;
				data_base[i+1].dprofil = 0;
				data_base[i+1].mode = 0;
				n_section++;

				return 1;
			}
		}
	}  //  end for
	return 0;
}

void calc_normal_vector( eSection* data, int pos, int& xv, int& yv ){
       int dop = 0;

       if (data[pos-1].x - data[pos].x + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
       if (data[pos-1].x - data[pos].x - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

	xv = data[pos].y - data[pos-1].y;
	yv = data[pos-1].x - data[pos].x - dop;
}

void eBranch::set_wide( int pos ){
	int xv, yv;
	double len;

	if ( pos < 1 || pos > n_section ) return;

	if ( pos == n_section ) {
		pos--;
		calc_normal_vector( data_base, pos, xv, yv);

		len = sqrt((double)yv*yv + xv*xv);
		if ( len == 0 ) return;

		xv = (int)(double(xv*data_base[pos].wide)/len);
		yv = (int)(double(yv*data_base[pos].wide)/len);

		data_base[pos].xr = data_base[pos].x + xv;
		data_base[pos].yr = data_base[pos].y + yv;

		data_base[pos].xl = data_base[pos].x - xv;
		data_base[pos].yl = data_base[pos].y - yv;
	} else if ( pos == 1 ) {
		pos--;
		calc_normal_vector( data_base, pos+1, xv, yv);

		len = sqrt((double)yv*yv + xv*xv);
		if ( len == 0 ) return;

		xv = (int)(double(xv*data_base[pos].wide)/len);
		yv = (int)(double(yv*data_base[pos].wide)/len);

		data_base[pos].xr = data_base[pos].x + xv;
		data_base[pos].yr = data_base[pos].y + yv;

		data_base[pos].xl = data_base[pos].x - xv;
		data_base[pos].yl = data_base[pos].y - yv;
	} else {
		int xx, yy;
		pos--;
		calc_normal_vector( data_base, pos, xv, yv);

		calc_normal_vector( data_base, pos+1, xx, yy);
		xv += xx;
		yv += yy;

		len = sqrt((double)yv*yv + xv*xv);
		if ( len == 0 ) return;

		xv = (int)(double(xv*data_base[pos].wide)/len);
		yv = (int)(double(yv*data_base[pos].wide)/len);

		data_base[pos].xr = data_base[pos].x + xv;
		data_base[pos].yr = data_base[pos].y + yv;

		data_base[pos].xl = data_base[pos].x - xv;
		data_base[pos].yl = data_base[pos].y - yv;
	}
}

void eBranch::rebuild_wide( void ){
	int i;

	for( i = 1; i <= n_section; i++ ) set_wide( i );
}

void sTrack::build_all_node( void ){
	int i;
	eNode* l_node = node;
	eBranch* l_branch = branch;
	int* s;

	s = new int[MAX_BRANCH];

	for( i = 0; i < n_branch; i++ ){
		l_branch -> rebuild_wide();
		l_branch = l_branch -> r;
	}

	for( i = 0; i < n_node; i++ ){
		l_node -> build_polygon( branch, s, 0 );
		l_node = l_node -> r;
	}
	delete[] s;
	build_all_branch();
}

void eNode::build_polygon( eBranch*& branch, int*& s, int what ){
	int i, j;
	double	a[MAX_BRANCH];

	if ( n_branch < 3 ) return;

	polygon_on = 1;

	for( i = 0; i < n_branch; i++ ){
		a[i] = get_alpha( branch, branches[i] );
		s[i] = branches[i];
	}

	//  Построение обхода путей s[i]

	for( i = 0; i < n_branch-1; i++ ){
		for( j = i+1; j < n_branch; j++ )
		if ( a[j] < a[i] ) {
			double tmp = a[i];
			a[i] = a[j];
			a[j] = tmp;

			register int t = s[i];
			s[i] = s[j];
			s[j] = t;
		}  //  end if
	}  //  end for

	if ( what ) return;

	for( i = 0; i < n_branch-1; i++ ){
		find_cross_point( branch, s[i], s[i+1], i );
	}
	find_cross_point( branch, s[i], s[0], i );
}

double eNode::get_alpha( eBranch *branch, int cBranch ){
	int xx = 0, yy = 0;
	int dop = 0;

	while( branch -> index != cBranch ) branch = branch -> r;

	if ( branch -> beg_node == index ){
		dop = 0;

		if (x - branch -> data_base[1].x + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (x - branch -> data_base[1].x - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		xx = branch -> data_base[1].x - x + dop;
		yy = branch -> data_base[1].y - y;
	} else if ( branch -> end_node == index ){
		register int pos = branch -> n_section-2;
		dop = 0;

		if (x - branch -> data_base[pos].x + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (x - branch -> data_base[pos].x - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		xx = branch -> data_base[pos].x - x + dop;
		yy = branch -> data_base[pos].y - y;
	} else ErrH.Abort( "Error in node." );

	return atan2((double)yy,(double)xx);
}

void eNode::find_cross_point( eBranch*& branch, int ls, int rs, int pos ){
	eBranch* l_br = branch;
	eBranch* r_br = branch;
	gLine ll, rl;
	int xx, yy;
	int dop;

	while( l_br -> index != ls ) l_br = l_br -> r;
	while( r_br -> index != rs ) r_br = r_br -> r;

	if ( l_br -> beg_node == index ){
		calc_normal_vector( l_br -> data_base, 1, xx, yy);

		ll.x = l_br -> data_base[0].xl;
		ll.y = l_br -> data_base[0].yl;
	} else {
		register int p = l_br -> n_section-1;

		calc_normal_vector( l_br -> data_base, p, xx, yy);

		ll.x = l_br -> data_base[p].xr;
		ll.y = l_br -> data_base[p].yr;
	}
	dop = 0;

	if (x - ll.x + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
	if (x - ll.x - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

	ll.x += dop;

	ll.a = -yy;
	ll.b = xx;

	if ( r_br -> beg_node == index ){
		calc_normal_vector( r_br -> data_base, 1, xx, yy);

		rl.x = r_br -> data_base[0].xr;
		rl.y = r_br -> data_base[0].yr;
	} else {
		register int p = r_br -> n_section-1;
		calc_normal_vector( r_br -> data_base, p, xx, yy);

		rl.x = r_br -> data_base[p].xl;
		rl.y = r_br -> data_base[p].yl;
	}
	dop = 0;

	if (x - rl.x + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
	if (x - rl.x - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

	rl.x += dop;

	rl.a = -yy;
	rl.b = xx;

	find_point_on_line( ll, rl, xx, yy );

	if ( l_br -> beg_node == index ){
		dop = 0;

		if (xx - l_br -> data_base[0].xl + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (xx - l_br -> data_base[0].xl - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		l_br -> data_base[0].xl = xx - dop;
		l_br -> data_base[0].yl = yy;
	} else {
		register int p = l_br -> n_section-1;
		dop = 0;

		if (xx - l_br -> data_base[p].xr + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (xx - l_br -> data_base[p].xr - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		l_br -> data_base[p].xr = xx - dop;
		l_br -> data_base[p].yr = yy;
	}

	if ( r_br -> beg_node == index ){
		dop = 0;

		if (xx - r_br -> data_base[0].xr + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (xx - r_br -> data_base[0].xr - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		r_br -> data_base[0].xr = xx - dop;
		r_br -> data_base[0].yr = yy;
	} else {
		register int p = r_br -> n_section-1;
		dop = 0;

		if (xx - r_br -> data_base[p].xl + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
		if (xx - r_br -> data_base[p].xl - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		r_br -> data_base[p].xl = xx - dop;
		r_br -> data_base[p].yl = yy;
	}

	xp[pos] = xx;
	yp[pos] = yy;
}

void sTrack::render( int what )
{
	set_height_from_ground(what);

	build_all_spline();
//	  save(0);
	  saveKron(0);

	if(linkingLog){
		int i,j = ((curGMap -> CY - curGMap -> yside) & clip_mask_y) >> WPART_POWER;
		int m = ((((curGMap -> CY + curGMap -> yside) & clip_mask_y) >> WPART_POWER) + 1) & (PART_MAX - 1);
		eNode* l_node = node;
		eBranch* l_branch = branch;
		for( i = 0; i < n_branch; i++ ){
			l_branch -> builded = 0;
			l_branch = l_branch -> r;
			}

		for( i = 0; i < n_node; i++ ){
			l_node -> builded = 0;
			l_node = l_node -> r;
			}
		for(;j != m;j = (j + 1) & (PART_MAX - 1)){
			l_branch = bTails[j];
			while(l_branch){
				if(!l_branch -> builded){
					l_branch -> render(what);
					l_branch -> builded = 1;
					}
				l_branch = l_branch -> next;
				}
			l_node = nTails[j];
			while(l_node){
				if(!l_node -> builded){
					l_node -> render(what);
					l_node -> builded = 1;
					}
				l_node = l_node -> next;
				}
			}
		}
	else {
		int i;
		eNode* l_node = node;
		eBranch* l_branch = branch;

		for( i = 0; i < n_branch; i++ ){
			l_branch -> render( what );
			l_branch = l_branch -> r;
			}

		for( i = 0; i < n_node; i++ ){
			l_node -> render( what );
			l_node = l_node -> r;
			}
		}
}

void eNode::render( int what ){
	int is_render = 0;
	int i;

	eBranch *l_branch = Track.branch;

	for ( i = 0; i	< n_branch; i++){

		while ( l_branch -> index != branches[i] ) l_branch = l_branch -> r;
		is_render |= l_branch -> is_render;
	}

	if ( !is_render ) return;

	if ( polygon_on )
		drawpoly( xp, yp, n_branch, h, h, 0.0, 0, 0, noise, dnoise, 0, what, 0.0, material, mode);
}

void eBranch::render( int what ){
	int i,j;
	int k;
	int n;
	int noise;
	int dnoise;
	int Amp;
	int x[4];
	int y[4];
	int lhl,lhr;
	double d_mode,t,t1;
	int mode1,mode2;

	if ( !is_render ) return;

	i = 0;
	rSection* ps = data_all;
	eSection* pe = data_base;
	eSection* pe1 = data_base + 1;
	for(j = 0;j < n_section - 1;j++,pe++,pe1++){
		n = pe -> point;
		for(k = 0;k < n;i++,k++){
			if ( i >= n_point ) ErrH.Abort( " Out n_point" );

			lhl = ps -> hl;
			lhr = ps -> hr;

			x[0] = ps -> xr;
			y[0] = ps -> yr;
			x[3] = ps -> xl;
			y[3] = ps -> yl;
			ps++;
			x[1] = ps -> xr;
			y[1] = ps -> yr;
			x[2] = ps -> xl;
			y[2] = ps -> yl;

			t = (double)k/n;
			t1 = 1.0 - t;

			mode1 = pe -> mode%2;
			mode2 = pe1 -> mode%2;

			if(mode1 != mode2) d_mode = t*mode2 + t1*mode1;
			else d_mode = mode1;

			noise = round((double)pe -> noise*t1 + (double)pe1 -> noise*t);
			dnoise = round(pe -> dnoise*t1 + pe1 -> dnoise*t);
			Amp = round(pe -> dprofil*t1 + pe1 -> dprofil*t);

			drawpoly(x,y,4,lhl,lhr,t,pe -> profil,pe1 -> profil,noise,dnoise,Amp,what,d_mode, material, pe -> mode);
			}
		}

	eNode *l_node = Track.node;

	if(beg_node){
		while( l_node -> index != beg_node ) l_node = l_node -> r;

//		  drawpoly(xpb,ypb,3,data_base[0].h,data_base[0].h,0.0,0,0,data_base[0].noise,data_base[0].dnoise,0,what,0.0);
		drawpoly(xpb,ypb,3,l_node -> h,l_node -> h,0.0,0,0,l_node -> noise,l_node -> dnoise,0,what,0.0, material, data_base[0].mode);
	}

	if(end_node){
		int pos = n_section-1;
		while( l_node -> index != end_node ) l_node = l_node -> r;

//		  drawpoly(xpe,ype,3,data_base[pos].h,data_base[pos].h,0.0,0,0,data_base[pos].noise,data_base[pos].dnoise,0,what,0.0);
		drawpoly(xpe,ype,3,l_node -> h, l_node -> h,0.0,0,0,l_node -> noise,l_node -> dnoise,0,what,0.0, material, data_base[pos].mode);
		}
}

void  find_point_on_line( gLine ll, gLine rl, int& xx, int& yy ){

	if ( ll.a*rl.b == ll.b*rl.a ){
		xx =  ll.x;
		yy =  ll.y;
	} else {
		double t = double((rl.x - ll.x)*rl.b-(rl.y-ll.y)*rl.a)/(ll.a*rl.b-rl.a*ll.b);
		xx = (int)(ll.x + t*ll.a);
		yy = (int)(ll.y + t*ll.b);
	}
}

#define DIVISION_PLUS(a,b,c) if(c) a = (b << 16)/c; else a = (b << 16);
#define DIVISION_MINUS(a,b,c) if(c) a = -(b << 16)/c; else a = -(b << 16);

void lline(int x,int y,int len,int color)
{
//	  if(y < VcutUp || y >= VcutDown) return;
//	  int xx = x + len - 1;
//	  if(xx < UcutLeft || x >= UcutRight) return;
//	  if(x < UcutLeft){
//		  len -= UcutLeft - x;
//		  x = UcutLeft;
//		  }
//	  if(xx >= UcutRight)
//		  len -= xx - UcutRight + 1;
	int CX = curGMap -> CX;
	int CY = curGMap -> CY;
	int xc = curGMap -> xc;
	int yc = curGMap -> yc;

	XGR_LineTo(x+xc-CX, y+yc-CY, len+1, 2, color);
}

void drawpoly(int* x, int* y, int n, int hl, int hr, double t, int f1, int f2, int noise, int dnoise, int Amp, int what, double mode, int material, int ter_mode ){
	int xl,xr;
	int kl,kr;
	int al,bl,ar,br;
	int d;
	int where;
	int Y;

	int minX = map_size_x,maxX = 0;
	int minY = map_size_y,maxY = 0;
	int* p = y;
	register int i,j;
	for(i = 0;i < n;i++){
		j = *p++;
		if(j > maxY) maxY = j;
		if(j < minY) minY = j;
		}
	p = x;
	for(i = 0;i < n;i++){
		j = *p++;
		if(j > maxX) maxX = j;
		if(j < minX) minX = j;
		}
	uchar** lt = vMap -> lineT;
	if((!lt[minY] || !lt[maxY]) && what) return;
	if((maxX < curGMap -> CX - curGMap -> xside || minX > curGMap -> CX + curGMap -> xside) && what) return;

	if(!what)
		if(!lt[minY] || !lt[maxY])
			vMap -> change(minY,maxY);

	if(hl <= 1) hl = 2;
	else if(hl >= 254) hl = 253;

	if(hr <= 1) hr = 2;
	else if(hr >= 254) hr = 253;

	int lfrom  = 0;
	while(y[(lfrom + 1)%n] < y[lfrom])
		lfrom = (lfrom + 1)%n;
	while(y[(lfrom - 1 + n)%n] < y[lfrom])
		lfrom = (lfrom - 1 + n)%n;
	int rfrom = lfrom;
	int lto = (lfrom - 1 + n)%n;
	int rto = (rfrom + 1)%n;
	int rfv = lfrom;

	Y = y[lfrom];

	xl = x[lfrom];
	al = x[lto] - xl + 1;
	bl = y[lto] - Y + 1;
	ar = x[rto] - xl + 1;
	br = y[rto] - Y + 1;
	xl = (xl << 16) + (1 << 15);
	xr = xl;
	if(al < 0){
		al = -al;
		DIVISION_MINUS(kl,al,bl)
		}
	else
		DIVISION_PLUS(kl,al,bl)
	if(ar > 0)
		DIVISION_PLUS(kr,ar,br)
	else {
		ar = -ar;
		DIVISION_MINUS(kr,ar,br)
		}

	int xx = 0,height,max,cx,h;
	uchar fld;
	double tt = 0.0;
	int xll,tmp_h,ii,jj,hh;
	double tmp_t,tmp1,tmp2;
	double t1 = 1.0 - t;
	int cmode = (mode != 0.0) ? 1 : 0;
	int xmode = (hl != hr || f1 || f2) ? 1 : 0;
	uchar* pa,*pf,*pa0;
	uchar* ch = vMap -> changedT + Y;

	while(1){
		if(bl > br){
			d = br;
			where = 0;
			}
		else {
			d = bl;
			where = 1;
			}
		while(d > 0){
			d--;
			max = ((xr >> 16) - (xl >> 16)) + 1;
			xll = xl >> 16;
			pa0 = lt[Y];
			fld = FloodLvl[Y >> WPART_POWER];

			for(j = 0,xx = xll;j < max;j++,xx++){
				if(xmode){
					if(n > 3){
						tmp1 = sqrt((double)(x[0] - xx)*(x[0] - xx) + (y[0] - Y)*(y[0] - Y));
						tmp2 = sqrt((double)(x[3] - xx)*(x[3] - xx) + (y[3] - Y)*(y[3] - Y));
						tt = tmp1/(tmp1 + tmp2);
						}
					height = round(hr*tt + hl*(1.0 - tt));
					height += round(profils[f1](tt,Amp)*t1 + profils[f2](tt,Amp)*t);

					if(cmode){
						tmp_t = (2.0*tt - 1.0);
						tmp_t *= tmp_t;

						tmp_h = 0;
//						  for(ii = -2; ii <= 2; ii++)
//							  for(jj = -2; jj <= 2; jj++)

						for(ii = 0; ii < 5; ii++)
							for(jj = 0; jj < 5; jj++)
								tmp_h += lt[Y + ii][(xx + jj) & clip_mask_x];
						tmp_h /= 25;
						hh = height;
						height = round(height*(1.0 - tmp_t) + tmp_h*tmp_t);
						height = round(hh*(1.0 - mode) + height*mode);
						}
					}
				else
					height = hl;
				if(noise)
					if((int)realRND(100) < noise) height += dnoise - realRND((dnoise << 1) + 1);

				pa = pa0 + (cx = XCYCL(xx));
				pf = pa + H_SIZE;
				if(!(*pf & DOUBLE_LEVEL) || (cx & 1)){
					h = GET_UP_ALT(pf,*pa,pa0,cx);
#ifndef TERRAIN16
					if(GET_TERRAIN_TYPE(*pf) != WATER_TERRAIN){
#endif
						if(h + ROAD_FORCING <= height){
							pixSet(cx,Y,height - *pa);
//							*pa = height;
							SET_REAL_TERRAIN(pf,material << TERRAIN_OFFSET,cx);
							}
#ifndef TERRAIN16
						}
					else 
						if(fld - h < WATERLINE){
							pixSet(cx,Y,height - *pa);
//							*pa = height;
							SET_REAL_TERRAIN(pf,material << TERRAIN_OFFSET,cx);
							}
#endif
					}
			}
			xl += kl;
			xr += kr;
			Y++; *ch++ = 1;
			}
		if(where){
			if(lto == rto) return;
			lfrom = lto;
			lto = (lto - 1 + n)%n;

			br -= bl;
			xl = x[lfrom];
			al = x[lto] - xl + 1;
			bl = y[lto] - Y + 1;
			xl = (xl << 16) + (1 << 15);
			if(al < 0){
				al = -al;
				DIVISION_MINUS(kl,al,bl)
				}
			else
				DIVISION_PLUS(kl,al,bl)
			}
		else {
			if(rto == lto) return;
			rfv = rto;
			rto = (rto + 1)%n;

			bl -= br;
			xr = x[rfv];
			ar = x[rto] - xr + 1;
			br = y[rto] - Y + 1;
			xr = (xr << 16) + (1 << 15);
			if(ar > 0)
				DIVISION_PLUS(kr,ar,br)
			else {
				ar = -ar;
				DIVISION_MINUS(kr,ar,br)
				}
			}
		}
}

void m_line( int x1, int y1, int x2, int y2, int color ){

	if (( x1 > UcutLeft)&&( x1 < UcutRight)&&
	    ( x2 > UcutLeft)&&( x2 < UcutRight)&&
	    ( y1 > VcutUp)&&( y1 < VcutDown)&&
	    ( y2 > VcutUp)&&( y2 < VcutDown))
		XGR_Line(x1, y1, x2, y2, color );
	else {
		double tminu, tmaxu, tminv, tmaxv;
		int a1, a2, b1, b2;
		int u1, u2, v1, v2;

		a1 = UcutLeft - x1;
		a2 = UcutRight - x1 - 1;
		b1 = x2 - x1;

		if ( b1 > 0.0 ){
			tminu = double(a1)/b1;
			tmaxu = double(a2)/b1;
		} else if ( b1 < 0.0 ){
			tminu = double(a2)/b1;
			tmaxu = double(a1)/b1;
		} else if (( x1 > UcutLeft ) && ( x1 < UcutRight )){
			tminu = 0.0;
			tmaxu = 1.0;
		} else return;

		a1 = VcutUp - y1;
		a2 = VcutDown - y1 - 1;
		b2 = y2 - y1;

		if ( b2 > 0.0 ){
			tminv = double(a1)/b2;
			tmaxv = double(a2)/b2;
		} else if ( b2 < 0.0 ){
			tminv = double(a2)/b2;
			tmaxv = double(a1)/b2;
		} else if (( y1 > VcutUp ) && ( y1 < VcutDown )){
			tminv = 0.0;
			tmaxv = 1.0;
		} else return;

		if ( tminu < tminv ) tminu = tminv;
		if ( tmaxu > tmaxv ) tmaxu = tmaxv;

		if ( tminu < 0.0 ) tminu = 0.0;
		if ( tmaxu > 1.0 ) tmaxu = 1.0;
		if (( tminu >= tmaxu ) || ( tminu >= 1.0 ) || ( tmaxu <= 0.0 )) return;

		u1 = (int)(x1 + tminu*b1);
		v1 = (int)(y1 + tminu*b2);

		u2 = (int)(x1 + tmaxu*b1);
		v2 = (int)(y1 + tmaxu*b2);

		XGR_Line(u1, v1, u2, v2, color);
	}
}

void sTrack::set_height_from_ground(int what){
	int i;
	eBranch* l_branch = branch;

	for( int j = 0; j < n_branch; j++ ){
		for( i = 1; i < l_branch -> n_section-1; i++ )
			set_height_in_branch_from_ground( l_branch, i, what);

		if ( !l_branch -> beg_node )
			set_height_in_branch_from_ground( l_branch, 0, what);

		if ( !l_branch -> end_node )
			set_height_in_branch_from_ground( l_branch, l_branch -> n_section-1, what);

		l_branch = l_branch -> r;
	}
}

//@caiiiycuk: memory leak at the end (not worth to fix)
char* trackName = strdup("track0.trk");

void sTrack::save(int n){
	int i;
	eNode *l_node = node;
	eBranch *l_branch = branch;

	trackName[5] += n;
	XStream fout(GetTargetName(trackName), XS_OUT );
	trackName[5] -= n;

	fout < n_node < n_branch;

	i = 0;
	l_node = delNode;
	while( l_node != 0 ) {
		i++;
		l_node = l_node -> r;
	}
	fout < i;

	i = 0;
	l_branch = delBranch;
	while( l_branch != 0 ) {
		i++;
		l_branch = l_branch -> r;
	}
	fout < i;

	l_node = node;
	l_branch = branch;

	for( i = 0; i < n_node; i++ ){
		l_node -> save( fout );
		l_node = l_node -> r;
	}

	for( i = 0; i < n_branch; i++ ){
		l_branch -> save( fout );
		l_branch = l_branch -> r;
	}

	l_node = delNode;
	while( l_node != 0 ) {
		fout < l_node -> index;
		l_node = l_node -> r;
	}

	l_branch = delBranch;
	while( l_branch != 0 ) {
		fout < l_branch -> index;
		l_branch = l_branch -> r;
	}
	fout.close();
}

void eNode::save(XStream& fout){
	int i;
	fout < index;
	fout < x < y < h;

	fout < noise;
	fout < dnoise;
	fout < n_branch;
	fout < color;
	fout < mode;
	fout < material;

	for( i = 0; i < n_branch; i++ )
		fout < branches[i];
}

void eBranch::save(XStream& fout){
	int i;
	fout < index;
	fout < n_section;
	fout < beg_node;
	fout < end_node;
	fout < color;
	fout < material;
	fout < is_render;

	for( i = 0; i < n_section; i++ ){
		fout < data_base[i].x;
		fout < data_base[i].y;
		fout < data_base[i].h;
		fout < data_base[i].hr;
		fout < data_base[i].hl;
		fout < data_base[i].wide;
		fout < data_base[i].noise;
		fout < data_base[i].dnoise;
		fout < data_base[i].profil;
		fout < data_base[i].dprofil;
		fout < data_base[i].mode;

//		  fout < data_base[i].xl;
//		  fout < data_base[i].yl;
//		  fout < data_base[i].hl;
//
//		  fout < data_base[i].xr;
//		  fout < data_base[i].yr;
//		  fout < data_base[i].hr;
	}
}

void sTrack::load(int n){
	int i;
	int ln_node, ln_branch, dn_node, dn_branch;

	XStream fin(0);

	trackName[5] += n;
	if(!fin.open(GetTargetName(trackName),XS_IN)){
		trackName[5] -= n;
		return;
	}
	trackName[5] -= n;

	eNode *l_node = node;
	eBranch *l_branch = branch;

	if ( n_branch > 0 ){
		eBranch *tmp;

		for( i = 0; i < n_branch; i++ ){
			tmp = l_branch;
			l_branch = l_branch -> r;
			delete tmp;
		}
		n_branch = 0;
		branch = 0;
	}

	while( delBranch != 0 ){
		eBranch *tmp = delBranch;
		delBranch = delBranch -> r;
		delete tmp;
	}

	if ( n_node > 0 ){
		eNode *tmp;

		for( i = 0; i < n_node; i++ ){
			tmp = l_node;
			l_node = l_node -> r;
			delete tmp;
		}
		n_node = 0;
		node = 0;
	}

	while( delNode != 0 ){
		eNode *tmp = delNode;
		delNode = delNode -> r;
		delete tmp;
	}

	fin > ln_node > ln_branch > dn_node > dn_branch;

	for( i = 0; i < ln_node; i++ ){
		add_node(0, 0, H_MAIN);
	}

	int tmp;
	for( i = 0; i < ln_branch; i++ ){
		add_branch(0, 0, tmp, MIN_WIDE);
	}

	l_node = node;
	l_branch = branch;

	for( i = 0; i < n_node; i++ ){
		l_node -> load( fin );
		l_node = l_node -> r;
	}

	for( i = 0; i < n_branch; i++ ){
		l_branch -> load( fin );
		l_branch = l_branch -> r;
	}

	delNode = 0;
	for( i = 0; i < dn_node; i++){
		l_node = new eNode(0, 0, 0);
		fin > l_node -> index;

		l_node -> r = delNode;
		delNode = l_node;
	}

	delBranch = 0;
	for( i = 0; i < dn_branch; i++){
		l_branch = new eBranch;
		fin > l_branch -> index;

		l_branch -> r = delBranch;
		delBranch = l_branch;
	}

	fin.close();

	build_all_node();
//	  build_all_spline();
}

void eNode::load(XStream& fin){
	int i;
	fin > index;
	fin > x > y > h;

	fin > noise;
	fin > dnoise;
	fin > n_branch;
	fin > color;
	fin > mode;
	fin > material;

	for( i = 0; i < n_branch; i++ )
		fin > branches[i];
}

void eBranch::load(XStream& fin){
	int i;
	fin > index;
	fin > n_section;
	fin > beg_node;
	fin > end_node;
	fin > color;
	fin > material;
	fin > is_render;

	for( i = 0; i < n_section; i++ ){
		fin > data_base[i].x;
		fin > data_base[i].y;
		fin > data_base[i].h;
		fin > data_base[i].hr;
		fin > data_base[i].hl;
		fin > data_base[i].wide;
		fin > data_base[i].noise;
		fin > data_base[i].dnoise;
		fin > data_base[i].profil;
		fin > data_base[i].dprofil;
		fin > data_base[i].mode;
	}
}

void sTrack::build_all_spline(void){
	int i;
	eBranch* l_branch = branch;

	build_all_node();
	for( i = 0; i < n_branch; i++ ){
		l_branch -> build_spline();
		l_branch = l_branch -> r;
	}
}

static rSection l_all[MAX_ALL];

void eBranch::build_spline(void){
	int i;

	if ( all_on ) {
		all_on = 0;
		return;
	}

	n_point = 0;

	if ( data_all != NULL ) delete[] data_all;

	if ( n_section == 2 ){
		data_all = new rSection[2];
		data_all[0].xl = data_base[0].xl;
		data_all[0].yl = data_base[0].yl;
		data_all[0].hl = data_base[0].hl;

		data_all[1].xl = data_base[1].xl;
		data_all[1].yl = data_base[1].yl;
		data_all[1].hl = data_base[1].hl;

		data_all[0].xr = data_base[0].xr;
		data_all[0].yr = data_base[0].yr;
		data_all[0].hr = data_base[0].hr;

		data_all[1].xr = data_base[1].xr;
		data_all[1].yr = data_base[1].yr;
		data_all[1].hr = data_base[1].hr;

		data_base[0].point = 1;
		n_point = 2;
		all_on = 1;
	} else {
		//rSection *l_all;
		//l_all = new rSection[MAX_ALL];

		all_on = 1;
		for( i = 0; i < n_section-1; i++ ){
			spline( data_base, l_all, i, n_section-1, n_point );
//			  data_base[i].point = n_point;
		}

		data_all = new rSection[n_point];
		for( i = 0; i < n_point; i++ ){
			data_all[i].xl = l_all[i].xl;
			data_all[i].yl = l_all[i].yl;
			data_all[i].hl = l_all[i].hl;
			data_all[i].xr = l_all[i].xr;
			data_all[i].yr = l_all[i].yr;
			data_all[i].hr = l_all[i].hr;
		}
		//delete  l_all;
	}
}

int min_len = 40;

void set_point_from_base( eSection* base, rSection& P, int pos, int pos0){
	int dop = 0;

	if (base[pos0].xl - base[pos].xl + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
	if (base[pos0].xl - base[pos].xl - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

	P.xl = base[pos].xl + dop;
	P.yl = base[pos].yl;
	P.hl = base[pos].hl;

	dop = 0;
	if (base[pos0].xl - base[pos].xl + XGR_MAXX > (int)clip_mask_x) dop = clip_mask_x+1;
	if (base[pos0].xl - base[pos].xl - XGR_MAXX < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

	P.xr = base[pos].xr + dop;
	P.yr = base[pos].yr;
	P.hr = base[pos].hr;
}

void spline( eSection*& base, rSection* all, int k, int n_section, int& n_point ){
	int i;
	rSection P1, P2, P3, P4;
	Vec al, bl, cl, dl;
	Vec ar, br, cr, dr;

	if ( k == 0 ){
		set_point_from_base( base, P1, k, k);
		set_point_from_base( base, P4, k+2, k);
	} else if ( k == n_section-1) {
		set_point_from_base( base, P1, k-1, k);
		set_point_from_base( base, P4, k+1, k);
	} else {
		set_point_from_base( base, P1, k-1, k);
		set_point_from_base( base, P4, k+2, k);
	}
	set_point_from_base( base, P2, k, k);
	set_point_from_base( base, P3, k+1, k);

	al.x = 0.5*(3.0*P2.xl - 3.0*P3.xl - P1.xl + P4.xl );
	al.y = 0.5*(3.0*P2.yl - 3.0*P3.yl - P1.yl + P4.yl );
	al.h = 0.5*(3.0*P2.hl - 3.0*P3.hl - P1.hl + P4.hl );

	bl.x = 0.5*(4.0*P3.xl + 2.0*P1.xl - P4.xl - 5.0*P2.xl);
	bl.y = 0.5*(4.0*P3.yl + 2.0*P1.yl - P4.yl - 5.0*P2.yl);
	bl.h = 0.5*(4.0*P3.hl + 2.0*P1.hl - P4.hl - 5.0*P2.hl);

	cl.x = 0.5*(P3.xl - P1.xl );
	cl.y = 0.5*(P3.yl - P1.yl );
	cl.h = 0.5*(P3.hl - P1.hl );

	dl.x = P2.xl;
	dl.y = P2.yl;
	dl.h = P2.hl;
	/*-------------------------------------------*/
	ar.x = 0.5*(3.0*P2.xr - 3.0*P3.xr - P1.xr + P4.xr );
	ar.y = 0.5*(3.0*P2.yr - 3.0*P3.yr - P1.yr + P4.yr );
	ar.h = 0.5*(3.0*P2.hr - 3.0*P3.hr - P1.hr + P4.hr );

	br.x = 0.5*(4.0*P3.xr + 2.0*P1.xr - P4.xr - 5.0*P2.xr);
	br.y = 0.5*(4.0*P3.yr + 2.0*P1.yr - P4.yr - 5.0*P2.yr);
	br.h = 0.5*(4.0*P3.hr + 2.0*P1.hr - P4.hr - 5.0*P2.hr);

	cr.x = 0.5*(P3.xr - P1.xr );
	cr.y = 0.5*(P3.yr - P1.yr );
	cr.h = 0.5*(P3.hr - P1.hr );

	dr.x = P2.xr;
	dr.y = P2.yr;
	dr.h = P2.hr;

	double lenl = sqrt((double)(P2.xl-P3.xl)*(P2.xl-P3.xl) + (P2.yl-P3.yl)*(P2.yl-P3.yl) );
	double lenr = sqrt((double)(P2.xr-P3.xr)*(P2.xr-P3.xr) + (P2.yr-P3.yr)*(P2.yr-P3.yr) );
	double len;

	if ( lenl > lenr ) len = lenl; else len = lenr;

	int n = round(len) + 1;
	double delta = 1.0/n;
	double t = 0.0;

	if ( n_point + n > MAX_ALL ) ErrH.Abort( "Out of all section" );

	for( i = n_point; i < n_point+n; i++ ){
		double t2, t3;

		t2 = t*t;
		t3 = t2*t;

		all[i].xl = round(al.x*t3 + bl.x*t2 + cl.x*t + dl.x);
		all[i].yl = round(al.y*t3 + bl.y*t2 + cl.y*t + dl.y);
		all[i].hl = round(al.h*t3 + bl.h*t2 + cl.h*t + dl.h);

		all[i].xr = round(ar.x*t3 + br.x*t2 + cr.x*t + dr.x);
		all[i].yr = round(ar.y*t3 + br.y*t2 + cr.y*t + dr.y);
		all[i].hr = round(ar.h*t3 + br.h*t2 + cr.h*t + dr.h);

		t += delta;
	}
	n_point += n;
	base[k].point = n;

	if ( k == n_section-1 ) {
		all[i].xl = base[k+1].xl;
		all[i].yl = base[k+1].yl;
		all[i].hl = base[k+1].hl;

		all[i].xr = base[k+1].xr;
		all[i].yr = base[k+1].yr;
		all[i].hr = base[k+1].hr;

		n_point++;
	}
}

int sTrack::find_bound_point( int& cBranch, int& cPoint, int X, int Y ){
	eBranch* l_branch = branch;
	int i;

	for( i = 0; i < n_branch; i++){
		if ( l_branch -> find_bound_point( cPoint, X, Y ) ){
			cBranch = l_branch -> index;
			return 1;
		}
		l_branch = l_branch -> r;
	}
	return 0;
}

void sTrack::set_wide_in_branch( int cBranch, int cPoint, int X, int Y ){
	eBranch* l_branch = branch;

	while( l_branch -> index != cBranch ) l_branch = l_branch -> r;

	l_branch -> set_wide_in_section( cPoint, X, Y );
}

void eBranch::set_wide_in_section( int pos, int X, int Y ){
	int len;

	pos--;

	len = (int)sqrt((double)(data_base[pos].x-X)*(data_base[pos].x - X)+(data_base[pos].y-Y)*(data_base[pos].y-Y));
	data_base[pos].wide = len;

	set_wide( pos );
	set_wide( pos+1 );
	set_wide( pos+2 );
}

int eBranch::find_bound_point( int& cPoint, int X, int Y ){
	int i;
	int Xl;

	for( i = 0; i < n_section; i++ ){
		int dop = 0;

		if ((data_base[i].xr - X)  > (int)clip_mask_x) dop = clip_mask_x+1;
		else if ((data_base[i].xr - X) < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		Xl = X + dop;

		if ( abs(data_base[i].xr - Xl) < delta && abs(data_base[i].yr-Y) < delta ) {
			cPoint = i+1;
			return 1;
		}

		dop = 0;
		if ((data_base[i].xl - X)  > (int)clip_mask_x) dop = clip_mask_x+1;
		else if ((data_base[i].xl - X)  < -(int)clip_mask_x) dop = -((int)clip_mask_x+1);

		Xl = X + dop;

		if ( abs(data_base[i].xl - Xl) < delta && abs(data_base[i].yl-Y) < delta ) {
			cPoint = i+1;
			return 1;
		}
	}
	return 0;
}

const int COLOR_FON = 32;

void sTrack::show_branch_height(int cBranch, int cPoint){
	int CX = curGMap -> CX;
	int CY = curGMap -> CY;
	int xc = curGMap -> xc;
	int yc = curGMap -> yc;
	eBranch* l_branch = branch;
	char str[10];

	int i = 0;
	while( l_branch -> index != cBranch ) l_branch = l_branch -> r;

	cPoint--;

	l_branch -> data_base[cPoint].h += DeltamH;
	l_branch -> data_base[cPoint].hl += DeltalH+DeltamH;
	l_branch -> data_base[cPoint].hr -= DeltalH-DeltamH;

	if (l_branch -> data_base[cPoint].h < 0 ) l_branch -> data_base[cPoint].h -= DeltamH;
	if (l_branch -> data_base[cPoint].h > 255 ) l_branch -> data_base[cPoint].h -= DeltamH;

	if (l_branch -> data_base[cPoint].hl < 0 ) {
		l_branch -> data_base[cPoint].h -= DeltamH;
		l_branch -> data_base[cPoint].hl -= DeltalH+DeltamH;
		l_branch -> data_base[cPoint].hr += DeltalH-DeltamH;
	}
	if (l_branch -> data_base[cPoint].hl > 255 ) {
		l_branch -> data_base[cPoint].hl -= DeltalH+DeltamH;
		l_branch -> data_base[cPoint].hr += DeltalH-DeltamH;
		l_branch -> data_base[cPoint].h -= DeltamH;
	}

	if (l_branch -> data_base[cPoint].hr < 0 ) {
		l_branch -> data_base[cPoint].hl -= DeltalH+DeltamH;
		l_branch -> data_base[cPoint].hr += DeltalH-DeltamH;
		l_branch -> data_base[cPoint].h -= DeltamH;
	}
	if (l_branch -> data_base[cPoint].hr > 255 ) {
		l_branch -> data_base[cPoint].hr += DeltalH-DeltamH;
		l_branch -> data_base[cPoint].hl -= DeltalH+DeltamH;
		l_branch -> data_base[cPoint].h -= DeltamH;
	}

//	  int u1 = l_branch -> data_base[cPoint].x;
//	  int v1 = l_branch -> data_base[cPoint].y;
	int u1 = 20+CX-xc;
	int v1 = 60+CY-yc;
	int u2 = u1+150, v2 = v1+70;

	for( i = v1; i < v2; i++)
		line( u1, i, u2, i, COLOR_FON);

	line( u1, v1, u1, v2, COLOR_TRACK);
	line( u1, v1, u2, v1, COLOR_TRACK);
	line( u2, v1, u2, v2, COLOR_TRACK);
	line( u1, v2, u2, v2, COLOR_TRACK);

	itoa( l_branch -> data_base[cPoint].h, str, 10 );
	sysfont.draw( u1+xc-CX+2, v1+yc-CY+2, (uchar*)str, COLOR_TRACK, -1);
	sysfont.draw( u1+xc-CX+26, v1+yc-CY+2, (uchar*)",", COLOR_TRACK, -1);

	itoa( l_branch -> data_base[cPoint].hl, str, 10 );
	sysfont.draw( u1+xc-CX+32, v1+yc-CY+2, (uchar*)str, COLOR_TRACK, -1);

	line( (u1+u2)/2-20, v2-2, (u2+u1)/2-20, v2 - (l_branch -> data_base[cPoint].hl/4), COLOR_TRACK);
	line( (u1+u2)/2+20, v2-2, (u2+u1)/2+20, v2 - (l_branch -> data_base[cPoint].hr/4), COLOR_TRACK);
	line((u2+u1)/2-20, v2 - (l_branch -> data_base[cPoint].hl/4),(u2+u1)/2, v2 - (l_branch -> data_base[cPoint].h/4), COLOR_TRACK);
	line((u2+u1)/2+20, v2 - (l_branch -> data_base[cPoint].hr/4),(u2+u1)/2, v2 - (l_branch -> data_base[cPoint].h/4), COLOR_TRACK);

	line( (u1+u2)/2, v2-2, (u2+u1)/2, v2 - (l_branch -> data_base[cPoint].h/4), COLOR_TRACK);

	if (cPoint > 0 ){
		line( u1+2, v2-2, u1+2, v2 - (l_branch -> data_base[cPoint-1].h/4), COLOR_TRACK);
		line( u1+2, v2 - (l_branch -> data_base[cPoint-1].h/4), (u2+u1)/2, v2 - (l_branch -> data_base[cPoint].h/4), COLOR_TRACK );
	}

	if ( cPoint+1 < l_branch -> n_section ){
		line( u2-2, v2-2, u2-2, v2 - (l_branch -> data_base[cPoint+1].h/4), COLOR_TRACK);
		line( u2-2, v2 - (l_branch -> data_base[cPoint+1].h/4), (u2+u1)/2, v2 - (l_branch -> data_base[cPoint].h/4), COLOR_TRACK );
	}
	DeltamH = 0;
	DeltalH = 0;

	XGR_Flush(20,60,150,70);
}

void sTrack::show_node_height(int cNode ){
	int CX = curGMap -> CX;
	int CY = curGMap -> CY;
	int xc = curGMap -> xc;
	int yc = curGMap -> yc;
	eNode* l_node = node;
	char str[10];

	int i = 0;
	while( l_node -> index != cNode ) l_node = l_node -> r;

	l_node -> h += DeltamH;

	if (l_node -> h < 0 ) l_node -> h = 0;
	if (l_node -> h > 255 ) l_node -> h = 255;

	register int H = l_node -> h;

/*	for( i = 0; i < l_node -> n_branch; i++ ){
		eBranch* l_branch = branch;

		while( l_branch -> index != l_node -> branches[i] ) l_branch = l_branch -> r;

		if ( l_branch -> beg_node == cNode ) {
			l_branch -> data_base[0].h = H;
			l_branch -> data_base[0].hl = H;
			l_branch -> data_base[0].hr = H;
		}

		register int pos = l_branch -> n_section-1;

		if ( l_branch -> end_node == cNode ) {
			l_branch -> data_base[pos].h = H;
			l_branch -> data_base[pos].hr = H;
			l_branch -> data_base[pos].hl = H;
		}
	}
	*/

//	  int u1 = l_node -> x;
//	  int v1 = l_node -> y;
	int u1 = 20+CX-xc;
	int v1 = 60+CY-yc;

	int u2 = u1+150, v2 = v1+70;

	for( i = v1; i < v2; i++)
		line( u1, i, u2, i, COLOR_FON);

	line( u1, v1, u1, v2, COLOR_TRACK);
	line( u1, v1, u2, v1, COLOR_TRACK);
	line( u2, v1, u2, v2, COLOR_TRACK);
	line( u1, v2, u2, v2, COLOR_TRACK);

	itoa( l_node -> h, str, 10 );
	sysfont.draw( u1+xc-CX+2, v1+yc-CY+2, (uchar*)str, COLOR_TRACK, -1);

	line( (u1+u2)/2, v2-2, (u2+u1)/2, v2 - (H/4), COLOR_TRACK);
	DeltamH = 0;

	XGR_Flush(20,60,150,70);
}

void sTrack::build_all_branch(void){
	int i;
	eBranch* l_branch = branch;

	for( i = 0; i < n_branch; i++ ){
		l_branch -> build_section();
		l_branch = l_branch -> r;
	}
}

void eBranch::build_section(void){
	int xv, yv;
	int xr, yr;
	gLine ll, lr;

	if ( beg_node != 0 ){
		calc_normal_vector( data_base, 1, xv, yv);

		xr = data_base[0].xr - data_base[0].xl;
		yr = data_base[0].yr - data_base[0].yl;

		ll.a = xv;
		ll.b = yv;
		lr.a = -yv;
		lr.b = xv;

		if ( (-yv*xr + xv*yr) > 0  ){
			ll.x = data_base[0].xr;
			ll.y = data_base[0].yr;

			lr.x = data_base[0].xl;
			lr.y = data_base[0].yl;

			find_point_on_line( ll, lr, xpb[0], ypb[0] );

			xpb[2] = data_base[0].xr;
			ypb[2] = data_base[0].yr;

			xpb[1] = data_base[0].xl;
			ypb[1] = data_base[0].yl;

			data_base[0].xl = xpb[0];
			data_base[0].yl = ypb[0];
		} else {
			ll.x = data_base[0].xl;
			ll.y = data_base[0].yl;

			lr.x = data_base[0].xr;
			lr.y = data_base[0].yr;

			find_point_on_line( ll, lr, xpb[0], ypb[0] );

			xpb[2] = data_base[0].xr;
			ypb[2] = data_base[0].yr;

			xpb[1] = data_base[0].xl;
			ypb[1] = data_base[0].yl;

			data_base[0].xr = xpb[0];
			data_base[0].yr = ypb[0];
		}
	}

	int pos = n_section-1;
	if ( end_node != 0 ){
		calc_normal_vector( data_base, pos, xv, yv);

		xv = -xv;
		yv = -yv;

		xr = data_base[pos].xr - data_base[pos].xl;
		yr = data_base[pos].yr - data_base[pos].yl;

		ll.a = xv;
		ll.b = yv;
		lr.a = -yv;
		lr.b = xv;

		if ( (-yv*xr + xv*yr) > 0 ){
			ll.x = data_base[pos].xr;
			ll.y = data_base[pos].yr;

			lr.x = data_base[pos].xl;
			lr.y = data_base[pos].yl;

			find_point_on_line( ll, lr, xpe[0], ype[0] );

			xpe[1] = data_base[pos].xr;
			ype[1] = data_base[pos].yr;

			xpe[2] = data_base[pos].xl;
			ype[2] = data_base[pos].yl;

			data_base[pos].xl = xpe[0];
			data_base[pos].yl = ype[0];
		} else {
			ll.x = data_base[pos].xl;
			ll.y = data_base[pos].yl;

			lr.x = data_base[pos].xr;
			lr.y = data_base[pos].yr;

			find_point_on_line( ll, lr, xpe[0], ype[0] );

			xpe[1] = data_base[pos].xr;
			ype[1] = data_base[pos].yr;

			xpe[2] = data_base[pos].xl;
			ype[2] = data_base[pos].yl;

			data_base[pos].xr = xpe[0];
			data_base[pos].yr = ype[0];
		}
	}
}

void sTrack::get_section_parametr( int cBranch, int cPoint, int& NoiseLevel, int& profil, int& NoiseAmp, int& ShapeAmp, int& NodeHeight, int& NodeSlope, int& TerrainMode, int& ColorBase, int& RenderMode, int& WideSection, int& MaterSection ){
	eBranch* l_branch = branch;

	if ( cBranch < 1 ) ErrH.Abort( "Error in struct Track" );

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find Branchs parametor" );

	NodeHeight = l_branch -> data_base[cPoint-1].h;
	NodeSlope = l_branch -> data_base[cPoint-1].hl - NodeHeight;
	NoiseLevel = l_branch -> data_base[cPoint-1].noise;
	profil = l_branch -> data_base[cPoint-1].profil;
	NoiseAmp = l_branch -> data_base[cPoint-1].dnoise;
	ShapeAmp = l_branch -> data_base[cPoint-1].dprofil;
	TerrainMode = l_branch -> data_base[cPoint-1].mode;
	WideSection = l_branch -> data_base[cPoint-1].wide;
	ColorBase = l_branch -> color;
	RenderMode = l_branch -> is_render;
	MaterSection = l_branch -> material;
}

void set_one_section(int i, eBranch*& l_branch, int NoiseLevel,int profil,int NoiseAmp,int ShapeAmp,int TerrainMode,int NodeHeight,int NodeSlope,int WideSection){
	l_branch -> data_base[i].noise = NoiseLevel;
	l_branch -> data_base[i].profil = profil;
	l_branch -> data_base[i].dnoise = NoiseAmp;
	l_branch -> data_base[i].dprofil = ShapeAmp;
	l_branch -> data_base[i].wide = WideSection;

	if ( TerrainMode < 2 ) {
		l_branch -> data_base[i].h = NodeHeight;
		l_branch -> data_base[i].hl = NodeHeight + NodeSlope;
		l_branch -> data_base[i].hr = NodeHeight - NodeSlope;

		l_branch -> data_base[i].mode = TerrainMode;
	} else {
		Track.set_height_in_branch_from_ground( l_branch, i, 1);

		l_branch -> data_base[i].mode = TerrainMode;
//		  l_branch -> data_base[i].mode = TerrainMode - 2;
	}
}

void sTrack::set_section_parametr( int cBranch, int cPoint, int NoiseLevel, int profil, int NoiseAmp, int ShapeAmp, int GroupToggle, int NodeHeight, int NodeSlope, int TerrainMode, int ColorBase, int RenderMode, int WideSection, int MaterSection){
	eBranch* l_branch = branch;

	if ( cBranch < 1 ) ErrH.Abort( "Error in struct Track" );

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch) {
		l_branch = l_branch -> r;
		i++;
	}
	if( i == n_branch ) ErrH.Abort( "Dont Find Branchs parametor" );

	l_branch -> color = ColorBase;
	l_branch -> material = MaterSection;
	l_branch -> is_render = RenderMode;

	if ( GroupToggle ) {
		for( i = 1; i < l_branch -> n_section-1; i++ ){
			set_one_section(i,l_branch,NoiseLevel,profil,NoiseAmp,ShapeAmp,TerrainMode,NodeHeight,NodeSlope,WideSection);
		}

		if ( !l_branch -> beg_node )
			set_one_section(0,l_branch,NoiseLevel,profil,NoiseAmp,ShapeAmp,TerrainMode,NodeHeight,NodeSlope, WideSection);

		if ( !l_branch -> end_node )
			set_one_section(l_branch -> n_section-1,l_branch,NoiseLevel,profil,NoiseAmp,ShapeAmp,TerrainMode,NodeHeight,NodeSlope, WideSection);
	} else {
		i = cPoint-1;
		set_one_section(i,l_branch,NoiseLevel,profil,NoiseAmp,ShapeAmp,TerrainMode,NodeHeight,NodeSlope, WideSection);
	}
	build_all_node();
}

void sTrack::get_node_parametr( int cNode, int& NoiseLevel, int& NoiseAmp, int& NodeHeight, int& ColorBase, int& MaterSection, int& TerrainMode ){
	eNode* l_node = node;

	if ( cNode < 1 ) ErrH.Abort( "Error in struct Track" );

	int i = 0;
	while( l_node -> index != cNode && i < n_node) {
		l_node = l_node -> r;
		i++;
	}
	if( i == n_node ) ErrH.Abort( "Dont Find nodes parametor" );

	NoiseLevel = l_node -> noise;
	NoiseAmp = l_node -> dnoise;
	NodeHeight = l_node -> h;
	ColorBase = l_node -> color;
	MaterSection = l_node -> material;
	TerrainMode = l_node -> mode;
}

void sTrack::set_node_parametr( int cNode, int CommonDoing, int NoiseLevel, int NoiseAmp, int NodeHeight, int ColorBase, int MaterSection, int TerrainMode ){
	eNode* l_node = node;
	int i;

	if ( cNode < 1 ) ErrH.Abort( "Error in struct Track" );

	i = 0;
	while( l_node -> index != cNode && i < n_node) {
		l_node = l_node -> r;
		i++;
	}
	if( i == n_node ) ErrH.Abort( "Dont Find nodes parametor" );

	if ( CommonDoing ){
		set_node_height( cNode, NodeHeight);
		for( i = 0; i < l_node -> n_branch; i++ )
			set_noise_in_branch( cNode, l_node -> branches[i], NoiseLevel, NoiseAmp);
	} else {
		l_node -> h = NodeHeight;
	}

	l_node -> noise = NoiseLevel;
	l_node -> dnoise = NoiseAmp;
	l_node -> color = ColorBase;
	l_node -> material = MaterSection;
	l_node -> mode = TerrainMode;
}

void sTrack::set_noise_in_branch( int cNode, int cBranch, int NoiseLevel, int NoiseAmp){
	eBranch* l_branch = branch;

	while( l_branch -> index != cBranch ) l_branch = l_branch -> r;

	if ( l_branch -> beg_node == cNode ) {
		l_branch -> data_base[0].noise = NoiseLevel;
		l_branch -> data_base[0].dnoise = NoiseAmp;
	}

	int pos = l_branch -> n_section-1;
	if ( l_branch -> end_node == cNode ) {
		l_branch -> data_base[pos].noise = NoiseLevel;
		l_branch -> data_base[pos].dnoise = NoiseAmp;
	}
}

/*
void sTrack::save(int n){
	int i;
	eNode *l_node = node;
	eBranch *l_branch = branch;

	trackName[5] += n;
	XStream fout( trackName, XS_OUT );
	trackName[5] -= n;

	fout <= n_node < "\t" <= n_branch < "\n";

	i = 0;
	l_node = delNode;
	while( l_node != 0 ) {
		i++;
		l_node = l_node -> r;
	}
	fout <= i < "delNode\t";

	i = 0;
	l_branch = delBranch;
	while( l_branch != 0 ) {
		i++;
		l_branch = l_branch -> r;
	}
	fout <= i < "delBranch\n";

	l_node = node;
	l_branch = branch;

	for( i = 0; i < n_node; i++ ){
		l_node -> save( fout );
		l_node = l_node -> r;
	}

	for( i = 0; i < n_branch; i++ ){
		l_branch -> save( fout );
		l_branch = l_branch -> r;
	}

	l_node = delNode;
	while( l_node != 0 ) {
		fout <= l_node -> index < "\t";
		l_node = l_node -> r;
	}
	fout < "\n";

	l_branch = delBranch;
	while( l_branch != 0 ) {
		fout <= l_branch -> index < "\t";
		l_branch = l_branch -> r;
	}
	fout.close();
}

void eNode::save(XStream& fout){
	int i;
	fout <= index < " - node\n";
	fout <= x < "\t" <= y < "\t" <= h < "\n";

//	  fout < noise;
	fout <= n_branch < "\n";

	for( i = 0; i < n_branch; i++ )
		fout <= branches[i] < "\t";
	fout < "\n";
}

void eBranch::save(XStream& fout){
	int i;
	fout <= index < "- branch\t";
	fout <= n_section < "\t";
	fout <= beg_node < "\t";
	fout <= end_node < "\n";

	for( i = 0; i < n_section; i++ ){
		fout <= data_base[i].x < "\t";
		fout <= data_base[i].y < "\n";

//		  fout < data_base[i].xl;
//		  fout < data_base[i].yl;
//		  fout < data_base[i].hl;
//
//		  fout < data_base[i].xr;
//		  fout < data_base[i].yr;
//		  fout < data_base[i].hr;
	}
}
*/

void sTrack::saveKron(int n){
	int i;
	eNode *l_node = node;
	eBranch *l_branch = branch;

	XBuffer fname;
	fname.init();
	fname < "track" <= n < ".krn";
	XStream fout(GetTargetName(fname.GetBuf()), XS_OUT );

	fout < n_node < n_branch;

	l_node = node;
	l_branch = branch;

	for( i = 0; i < n_node; i++ ){
		l_node -> saveKron( branch, fout );
		l_node = l_node -> r;
	}

	for( i = 0; i < n_branch; i++ ){
		l_branch -> saveKron( fout );
		l_branch = l_branch -> r;
	}

	fout.close();
}

void eNode::saveKron( eBranch*& branch, XStream& fout)
{
	int* s = new int[MAX_BRANCH];

	build_polygon( branch, s, 1 );
	int i;
	fout < index;
	fout < x < y < h;
	fout < n_branch;
	for( i = 0; i < n_branch; i++ ){
		fout < s[n_branch-i-1];
		fout < xp[i];
		fout < yp[i];
	};

	delete[] s;
}

void eBranch::saveKron(XStream& fout)
{
	int i;

	fout < index;

	fout < beg_node;
	fout < end_node;

	i = n_point/min_len+1;
	if ( beg_node > 0 ) i++;
	if ( end_node > 0 ) i++;

	fout < i;
	int max_noise = 0;

	for( i = 0; i < n_section; i++ )
	if ( data_base[i].dnoise > max_noise ) max_noise = data_base[i].dnoise;

//	  fout < max_noise;

	if ( beg_node > 0 ){
		fout < (data_base[0].xl & clip_mask_x);
		fout < data_base[0].yl;
		fout < data_base[0].hl;

		fout < (data_base[0].xr & clip_mask_x);
		fout < data_base[0].yr;
		fout < data_base[0].hr;

	}

	for( i = 0; i < n_point; i++ )
	if ( i%min_len == 0 ){
		fout < data_all[i].xl;
		fout < data_all[i].yl;
		fout < data_all[i].hl;

		fout < data_all[i].xr;
		fout < data_all[i].yr;
		fout < data_all[i].hr;
	};

	i = n_point-1;
	if ( n_point%min_len == 0 ){
		fout < data_all[i].xl;
		fout < data_all[i].yl;
		fout < data_all[i].hl;

		fout < data_all[i].xr;
		fout < data_all[i].yr;
		fout < data_all[i].hr;
	};

	if ( end_node > 0 ){
		fout < data_base[n_section-1].xl;
		fout < data_base[n_section-1].yl;
		fout < data_base[n_section-1].hl;

		fout < data_base[n_section-1].xr;
		fout < data_base[n_section-1].yr;
		fout < data_base[n_section-1].hr;
	}

}

void sTrack::set_node_height(int cNode, int Height){

	eNode* l_node = node;

	int i = 0;
	while( l_node -> index != cNode ) l_node = l_node -> r;

	l_node -> h = Height;

	if (l_node -> h < 0 ) l_node -> h = 0;
	if (l_node -> h > 255 ) l_node -> h = 255;

	register int H = l_node -> h;

	for( i = 0; i < l_node -> n_branch; i++ ){
		eBranch* l_branch = branch;

		while( l_branch -> index != l_node -> branches[i] ) l_branch = l_branch -> r;

		if ( l_branch -> beg_node == cNode ) {
			l_branch -> data_base[0].h = H;
			l_branch -> data_base[0].hl = H;
			l_branch -> data_base[0].hr = H;
		}

		register int pos = l_branch -> n_section-1;

		if ( l_branch -> end_node == cNode ) {
			l_branch -> data_base[pos].h = H;
			l_branch -> data_base[pos].hr = H;
			l_branch -> data_base[pos].hl = H;
		}
	}
}

void sTrack::set_height_in_branch_from_ground(eBranch* l_branch, int cSection, int what){
	if ( l_branch -> data_base[cSection].mode < 2 ) return;

	int xxl = l_branch -> data_base[cSection].xl;
	int yyl = l_branch -> data_base[cSection].yl;

	int xxr = l_branch -> data_base[cSection].xr;
	int yyr = l_branch -> data_base[cSection].yr;

	if((!vMap -> lineT[yyr] || !vMap -> lineT[yyl]) && what) return;
	if(!what && (!vMap -> lineT[yyr] || !vMap -> lineT[yyl]))
		vMap -> change(MIN(yyr,yyl),MAX(yyr,yyl));

	l_branch -> data_base[cSection].hl = vMap -> lineT[yyl][xxl&clip_mask_x];
	l_branch -> data_base[cSection].hr = vMap -> lineT[yyr][xxr&clip_mask_x];
	l_branch -> data_base[cSection].h = (l_branch -> data_base[cSection].hr+l_branch -> data_base[cSection].hl)>>1;
}

void eNode::show_on_all(int UpX, int UpY, int x_size, int y_size){
	int x1 = UpX + ((x*x_size)>>MAP_POWER_X);
	int y1 = UpY + ((y*y_size)>>MAP_POWER_Y);

	XGR_Line(x1, y1, x1, y1, ColorTable[ color ]);
}

void eBranch::show_on_all(int UpX, int UpY, int x_size, int y_size){
	int i;
	int x1,x2,y1,y2;

	for( i = 0; i < n_section-1; i++ ){
		x1 = UpX + ((data_base[i].x*x_size)>>MAP_POWER_X);
		x2 = UpX + ((data_base[i+1].x*x_size)>>MAP_POWER_X);
		y1 = UpY + ((data_base[i].y*y_size)>>MAP_POWER_Y);
		y2 = UpY + ((data_base[i+1].y*y_size)>>MAP_POWER_Y);

		if ( abs(data_base[i].x - data_base[i+1].x) < (int)clip_mask_x-XGR_MAXX ){
			XGR_Line(x1, y1, x2, y2, ColorTable[ color ]);
		}
	}
}
int UpX = 5;
int UpY = 25;

void sTrack::show_all_track(void){
	int color_boder = 255;
	int color_back = 45;
	int x_size;
	int y_size;
	int i;
	int mX;
	int mY;

	if (!Track_show_all) return;

	int xm = XGR_MouseObj.PosX;
	int ym = XGR_MouseObj.PosY;
	mX = (xm - curGMap -> xc + curGMap -> CX) & clip_mask_x;
	mY = (ym - curGMap -> yc + curGMap -> CY) & clip_mask_y;

//	  UpY = 25;
	y_size = XGR_MAXY - 50;
	x_size = y_size>>(MAP_POWER_Y - MAP_POWER_X);

//	  UpX = (XGR_MAXX-x_size)>>1;
//	  UpX = 5;

	XGR_Line(UpX-1, UpY-1, UpX+x_size, UpY-1, color_boder);
	XGR_Line(UpX-1, UpY+y_size+1, UpX+x_size+1, UpY+y_size+1, color_boder);
	XGR_Line(UpX+x_size+1, UpY-1, UpX+x_size+1, UpY+y_size+1, color_boder);
	XGR_Line(UpX-1, UpY-1, UpX-1, UpY+y_size+1, color_boder);

	for( i = 0; i < y_size; i++ ){
		XGR_Line(UpX, UpY+i, UpX+x_size, UpY+i, color_back);
	}

	eBranch* l_branch = branch;

	for( i = 0; i < n_branch; i++ ){
		l_branch -> show_on_all(UpX, UpY, x_size, y_size);
		l_branch = l_branch -> r;
	}

	eNode* l_node = node;

	for( i = 0; i < n_node; i++ ){
		l_node -> show_on_all(UpX, UpY, x_size, y_size);
		l_node = l_node -> r;
	}
	int x1 = UpX + ((mX*x_size)>>MAP_POWER_X);
	int y1 = UpY + ((mY*y_size)>>MAP_POWER_Y);

	int color_mouse = 142;
	XGR_Line(x1-1, y1-1, x1+1, y1-1, color_mouse);
	XGR_Line(x1-1, y1, x1+1, y1, color_mouse);
	XGR_Line(x1-1, y1+1, x1+1, y1+1, color_mouse);
}

void sTrack::lift_branch(int cBranch ){
	eBranch* l_branch = branch;

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch){
		l_branch = l_branch -> r;
		i++;
	}

	if ( i == n_branch ) ErrH.Abort("Dont Find Branch in lift.");

	if(branch == l_branch)
		branch = l_branch -> r;
	else {
		l_branch->l->r = l_branch->r;
		l_branch->r->l = l_branch->l;

		l_branch->r = branch;
		l_branch->l = branch->l;

		branch->l = l_branch;
		l_branch->l->r = l_branch;
		}
}

void sTrack::recalc_height_branch(int cBranch ){
	eBranch* l_branch = branch;
	int hle, hbc, hlb;
	int hre, hec, hrb;

	int dhr, dhc, dhl;

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch){
		l_branch = l_branch -> r;
		i++;
	}

	if ( i == n_branch ) ErrH.Abort("Dont Find Branch in recalc_heght.");

	hlb = l_branch -> data_base[0].hl;
	hbc = l_branch -> data_base[0].h;
	hrb = l_branch -> data_base[0].hr;

	i = l_branch -> n_section-1;

	hle = l_branch -> data_base[i].hl;
	hec = l_branch -> data_base[i].h;
	hre = l_branch -> data_base[i].hr;

	dhl = (hle-hlb)/i;
	dhr = (hre-hrb)/i;
	dhc = (hec-hbc)/i;

	for ( int j = 1; j < i; j++){
		hlb += dhl;
		hbc += dhr;
		hrb += dhc;

		l_branch -> data_base[j].hl = hlb;
		l_branch -> data_base[j].h  = hbc;
		l_branch -> data_base[j].hr = hrb;
	}
}

int sTrack::check_mouse_work(void){
	int xc, yc;

	if (!Track_show_all) return( 0 );

	int mX = XGR_MouseObj.PosX;
	int mY = XGR_MouseObj.PosY;

	int y_size = XGR_MAXY - 50;
	int x_size = y_size>>(MAP_POWER_Y - MAP_POWER_X);

	if ( UpY < mY && UpY + y_size > mY && UpX < mX && UpX + x_size > mX) {
		xc = ((mX - UpX)<<MAP_POWER_X)/x_size;
		yc = ((mY - UpY)<<MAP_POWER_Y)/y_size;
		curGMap -> gotoxy( xc, yc );
		return( 1 );
	}

	return( 0 );
}

void sTrack::recalc_wide_branch(int cBranch, int new_wide ){
	eBranch* l_branch = branch;

	int i = 0;
	while( l_branch -> index != cBranch && i < n_branch){
		l_branch = l_branch -> r;
		i++;
	}

	if ( i == n_branch ) ErrH.Abort("Dont Find Branch in recalc new_wide.");

	for( i = 0; i < l_branch -> n_section; i++){
		l_branch -> data_base[i].wide = new_wide;
	}

	build_all_node();
}

/*void eBranch::saveKron(XStream& fout)
{
	int i;
	int n;

	fout < index;

	fout < beg_node;
	fout < end_node;

	for( i = 0, n = 0; i < n_point; i++, n++ ){
		int i1, i2;

		i1 = find_next_point_in_spline(i,0);
		i2 = find_next_point_in_spline(i,1);

		if ( i1 < i2 ) i = i1; else i = i2;
	};

	if ( beg_node > 0 ) n++;
	if ( end_node > 0 ) n++;

	fout < n;
	int max_noise = 0;

	for( i = 0; i < n_section; i++ )
	if ( data_base[i].dnoise > max_noise ) max_noise = data_base[i].dnoise;

//	  fout < max_noise;

	if ( beg_node > 0 ){
		fout < (data_base[0].xl & clip_mask_x);
		fout < data_base[0].yl;
		fout < data_base[0].hl;

		fout < (data_base[0].xr & clip_mask_x);
		fout < data_base[0].yr;
		fout < data_base[0].hr;

	}

	for( i = 0; i < n_point; i++ ){
		int i1, i2;

		i1 = find_next_point_in_spline(i,0);
		i2 = find_next_point_in_spline(i,1);

		if ( i1 < i2 ) i = i1; else i = i2;

		fout < data_all[i].xl;
		fout < data_all[i].yl;
		fout < data_all[i].hl;

		fout < data_all[i].xr;
		fout < data_all[i].yr;
		fout < data_all[i].hr;
	};

	if ( end_node > 0 ){
		fout < data_base[n_section-1].xl;
		fout < data_base[n_section-1].yl;
		fout < data_base[n_section-1].hl;

		fout < data_base[n_section-1].xr;
		fout < data_base[n_section-1].yr;
		fout < data_base[n_section-1].hr;
	}

}*/

int eBranch::find_next_point_in_spline(int n, int w){
	double sum = 0.0;
	int i, j;
	int a,b;
	double DD = 1;
	double t;

	for( i = n+2;( (i < n_point)&&(sum/(i-n+1) < DD) ); i++){
		if ( w == 0 ){
			a = data_all[i].xl - data_all[n].xl;
			b = data_all[i].yl - data_all[n].yl;

			sum = 0.0;
			for( j = n+1; j < i; j++ ){
				 t = double(data_all[j].yl*b+data_all[j].xl*a-data_all[n].yl*b-data_all[n].xl*a)/(a*a+b*b);

				 sum += sqrt(sqr(data_all[n].xl+a*t-data_all[j].xl)+sqr(data_all[n].yl+b*t-data_all[j].yl));
			}
		} else {
			a = data_all[i].xr - data_all[n].xr;
			b = data_all[i].yr - data_all[n].yr;

			sum = 0.0;
			for( j = n+1; j < i; j++ ){
				 t = double(data_all[j].yr*b+data_all[j].xr*a-data_all[n].yr*b-data_all[n].xr*a)/(a*a+b*b);

				 sum += sqrt(sqr(data_all[n].xr+a*t-data_all[j].xr)+sqr(data_all[n].yr+b*t-data_all[j].yr));
			}
		}  //  end if
	}  // end for i

	if ( i >= n_point ) return n_point-1; else return i;
}

