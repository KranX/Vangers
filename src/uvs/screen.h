struct WorldScreen;

struct Place : listElem {
	int x,y;

		Place(WorldScreen* w,int _x,int _y){ x = _x; y = _y; }

	void link(Place*& tail);
	};

struct WorldScreen {
	int x,y;
	int sx,sy;
	int fx,fy;
	int x_size,y_size;	

	Place* placeTail;

	XBuffer info;

	uvsWorld* Pworld;

		WorldScreen(int _y,int _sy,uvsWorld* _Pworld);
	
	int GetX(int xx){ return xx*sx/y_size; }
	int GetY(int yy){ return yy*sy/x_size; }
	void Quant(void);

	void DollyQuant(uvsDolly* pd);
	void VangerQuant(uvsVanger* pv);
	};

void ScreenQuant(void);
void StandScreenPrepare(void);
