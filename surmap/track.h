
const int MOVING_SECTION = 2;
const int MOVING_NODE = 3;
const int SET_WIDE = 4;
const int SET_HEIGHT = 5;
const int SET_HEIGHT_NODE = 6;
const int BUILDING = 1;
const int WAIT = 0;

const int MAX_BRANCH = 8;
const int MAX_BASE = 64;
#ifdef __ZORTECHC__
const int MAX_ALL  = 1024;
#else
const int MAX_ALL  = 32*1024;
#endif

extern int min_len;
extern int MIN_WIDE;
extern int H_MAIN;
extern int WATERLINE;
extern int ROAD_MATERIAL;
extern int ROAD_FORCING;

const int COLOR_TRACK = 255;
const int COLOR_NODE = 100;

struct eNode;

struct Vec {
	double x, y, h;
	};

struct VectorT {
	int x, y, h;
	};

struct gLine {
	int a, b;
	int x, y;
	};

struct eSection {
	int x, y, h;
	int xl, yl, hl;
	int xr, yr, hr;
	int wide;
	int noise;
	int dnoise;
	int profil;
	int dprofil;
	int point;
	int mode;
	};

struct rSection {
	int xl, yl, hl;
	int xr, yr, hr;
	};

struct eBranch{
	int index;
	int n_section;
	int n_point;
	int *xpb, *ypb;
	int beg_node;

	int *xpe, *ype;
	int end_node;
	int all_on;
	int color;
	int material;
	int is_render;

	eSection* data_base;
	rSection* data_all;

	eBranch* l;
	eBranch* r;
	eBranch* next;
	int builded;

	eBranch(void);
	~eBranch(void);

	void show(int);
	void show_on_all(int,int,int,int);
	int add_section( int, int, int );
	int delete_point( int );
	int find_point( int&, int&, int& );
	int find_bound_point( int&, int, int );
	int find_next_point_in_spline(int, int);

	int insert_point( int, int );

	void build_section(void);
	void build_spline(void);
	void set_section( int, int, int, int );
	void set_wide_in_section( int , int, int );

	void set_wide( int );
	void rebuild_wide( void );
	void render( int );
	void render_full_section( int, int );

	void save(XStream&);
	void saveKron(XStream&);
	void load(XStream&);

	int linking(int stage);
	};

struct eNode {
	int index;
	int x, y, h;
	int *xp, *yp;
	int polygon_on;
	int noise;
	int dnoise;
	int color;
	int material;
	int mode;

	int n_branch;
	int* branches;

	eNode* l;
	eNode* r;
	eNode* next;
	int builded;

	eNode(int, int, int);
	~eNode(void);
	int find_point( int&, int& );

	void find_cross_point( eBranch*& ,int, int, int );
	void add_branch( int );
	void build_polygon( eBranch*&, int*&, int );
	double get_alpha(eBranch*, int );

	void show(void);
	void show_on_all(int,int,int,int);
	void render(int);

	void save(XStream&);
	void saveKron(eBranch*& branch, XStream&);
	void load(XStream&);

	int linking(int stage);
	};

struct sTrack {
	int n_node;
	int n_branch;

	eNode* node;
	eBranch* branch;

	eNode** nTails;
	eBranch** bTails;
	int linkingLog;

	sTrack(void);

	void curr_build(void);
	void show(void);
	void show_all_track(void);
	void show_branch_height(int , int);
	void show_node_height(int);

	void build_all_branch(void);
	void build_all_node(void);
	void build_all_spline(void);
	void render(int);

	void set_noise_in_branch( int, int, int, int );
	void set_wide_in_branch( int , int , int , int );
	void set_point_in_branch( int, int, int, int, int );
	void set_node( int, int, int, int );
	void set_node_height( int, int );
	void set_height_from_ground(int what);
	void set_height_in_branch_from_ground( eBranch*, int cSection, int what );
	void set_section_parametr( int , int , int , int, int, int, int, int, int, int, int, int, int, int );
	void set_node_parametr( int, int, int, int, int, int, int, int);
	void get_node_parametr( int, int&, int&, int&, int&, int&, int&);
	void get_section_parametr( int, int, int&, int&, int&, int&, int&, int&, int&, int&, int&, int&, int&);

	void delete_point_from_branch( int, int );
	void delete_branch_from_node( int, int );
	void delete_branch( int );
	int delete_node( int );

	int add_branch(int, int, int&, int);
	int add_node(int, int, int);
	int add_point_to_branch(int, int, int, int);
	void add_branch_to_node( int, int );
	int insert_point_in_branch( int, int );

	int find_branch( int, int&, int&, int&, int& );
	int find_node( int&, int&, int& );
	int find_bound_point( int&, int&, int, int);

	void concate_branch( int cwBranch, int cBranch );
	int decompose_branch( int cwBranch, int cwPoint, int& cNode );
	void prepare_branch_to_add_point( int cBranch, int cPoint );
	void prepare_branch_concate( int cBranch, int cNode );

	void lift_branch(int cBranch );
	void recalc_height_branch(int cBranch );
	void recalc_wide_branch(int cBranch, int new_wide );
	int  check_mouse_work(void);

	void save(int);
	void saveKron(int);
	void load(int);
	void linking(void);
};

typedef double (*PF)(double,double);
extern PF profils[6];

extern sTrack Track;

extern int cPoint;
extern int cBranch;
extern int cwPoint;
extern int cwBranch;
extern int cNode;

extern int UcutLeft,UcutRight,VcutUp,VcutDown;

