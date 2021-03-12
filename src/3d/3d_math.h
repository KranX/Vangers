#ifndef __3D__3D_MATH_H__
#define __3D__3D_MATH_H__
/******************************************************************************
			 3D MATHEMATICS FUNCTION.
			 KD_Lab::Alexander "Sicher" Kotliar (C) 1995 - 1997
******************************************************************************/


/******************************************************************************
	Scalar constant and function part
******************************************************************************/

#undef PI

#undef M_PI
#define M_PI		  3.14159265358979323846


#define DBL_EPS 1e-15
#define INT_INF 0x7FFFFFFF

#define SWAP(a,b)	{ *(int*)&a ^= int(b); *(int*)&b ^= int(a); *(int*)&a ^= int(b); }
#define GOOD_SHR(x,n)	((x) < 0 ? -((-(x) + (1 << ((n) - 1))) >> (n)) : (((x) + (1 << ((n) - 1))) >> (n)))
#define DIV16(a,b)	  (a = ((a) << 16)/(b))
#define DIV16T(x,y)	((int)div_table_short[((x) & 0xFF) | (((y) << 8) & 0xFF00)] << 8)
#define MUL_CHAR(x,y)	     (mul_table[((x) & 0xFF) | (((y) << 8) & 0xFF00)])

int _rand(void);
void _srand(unsigned int seed);

#ifdef _MT_
#undef random
#endif

#ifndef random
#define random(num) ((int)(((long)_rand()*(num)) >> 15))
#endif

#undef max
#undef min

inline double max(double x,double y){ return x > y ? x : y; }
inline double min(double x,double y){ return x < y ? x : y; }

inline int max(int x,int y){ return x > y ? x : y; }
inline int min(int x,int y){ return x < y ? x : y; }

inline int max(int x,int y,int z){ return max(max(x,y),z); }
inline int min(int x,int y,int z){ return min(min(x,y),z); }

inline int maxa(int x,int y){ return abs(x) > abs(y) ? x : y; }
inline int maxa(int x,int y,int z){ return maxa(maxa(x,y),z); }

inline int mina(int x,int y){ return abs(x) < abs(y) ? x : y; }
inline int mina(int x,int y,int z){ return mina(mina(x,y),z); }


inline double fmax(double x,double y){ return x > y ? x : y; }
inline double fmax(double x,double y,double z){ return fmax(fmax(x,y),z); }

inline double fmaxa(double x,double y){ return fabs(x) > fabs(y) ? x : y; }
inline double fmaxa(double x,double y,double z){ return fmaxa(fmaxa(x,y),z); }

inline double fmin(double x,double y){ return x < y ? x : y; }
inline double fmin(double x,double y,double z){ return fmin(fmin(x,y),z); }

inline double fmina(double x,double y){ return fabs(x) < fabs(y) ? x : y; }
inline double fmina(double x,double y,double z){ return fmina(fmina(x,y),z); }

/* Return a length of number (Bit Scan Revers). Validate for number < 0 */

/*
inline int BitSR(int x)
{
	_asm {
		mov	eax, x
		cdq
		xor	eax,edx
		sub     eax,edx
		bsr     eax,eax
		}
}
*/


//inline int round_sin(int x){ return x >> 15;}
inline int round_sin(int x){ return x >= 0 ? (x + (1 << 14)) >> 15 : -((-x + (1 << 14)) >> 15); }

/* Use instead of "x >> 15" for Round and correctly "-1 >> 15" */
inline int roundi(int x){ return x >= 0 ? (x + (1 << 14)) >> 15 : -((-x + (1 << 14)) >> 15); }

/*******************************************************************************
	Some table's functions and their constants
*******************************************************************************/

const int  M_1_ = 32768;

const int  Pi_len		= 11;
const int  Pi		= 1 << Pi_len;
const int  ANGLE_MASK	= 2*Pi - 1;

const int  ArcCosLen =    12;
const int  ArcCosMAX =    1 << ArcCosLen;
const int  ArcTanLen =    12;
const int  ArcTanMAX =    1 << ArcTanLen;
const int  SqrtLen =      12;
const int  SqrtMax =      1 << SqrtLen;

#define N_POWER_MAX	16

/*
		About ANGLES.
      Angles are integers, where Pi is multiple 2. Therefore only (Pi_len + 1)
low digits are significant and You can use ANGLE_MASK to extract them. Moreover,
it is supported that  Matrix(angle,any_axis)*Matrix(-angle,any_axis) = I.
Make sure using Pi (integer constant) and M_PI (3.1416...). Apply GTOR and RTOG
for converting.
*/

#define GTOR(x) (double(x)*(M_PI/double(Pi)))
#define RTOG(x) (round(x*(double(Pi)/M_PI)))

// Distance between two angles for integer (Pi) and real (PI) angles respectively
int DistPi(int s0,int s1);
double DistPIreal(double s0,double s1);

#ifdef _STATIC_TABLES_
extern float cos_table[2*Pi];
extern char mul_table[256*256];
extern short div_table_short[256*256];
extern char power_table[256*(N_POWER_MAX - 1)];
#else
extern float* cos_table;
extern char* mul_table;
extern short* div_table_short;
extern char* power_table;
#endif

void make_tables();
void write_tables();

inline float Sin(int angle){ return cos_table[(angle - Pi/2) & ANGLE_MASK]; }
inline float Cos(int angle){ return cos_table[angle & ANGLE_MASK]; }

int ArcCos(int Dx,int Abs);
int Atan2(int Dy,int Dx);
int Sqrt(int x);

// returns (x*y >> 7), where x - [-127,127], y - [-127,127]
#define MUL(x,y)	((int)mul_table[(x & 0xFF) | ((y << 8) & 0xFF00)])

//// returns ((y << 7)/x), where x - [-127..127], y - [0..256]
#define DIV_by_table(y,x)	 ((int)div_table[(x & 0xFF) | ((y << 8) & 0xFF00)])

// returns (x^n >> 7*(n - 1)), where x - [-127,127], n - [2,N_POWER_MAX]
#define POW(x,n)	((int)power_table[((n) - 2 << 8) + (x)])



/******************************************************************************
			     3D part
******************************************************************************/
/* Axises for DBM constructor */
const int  X_AXIS = 0;
const int  Y_AXIS = 1;
const int  Z_AXIS = 2;

/* Matrix & DBM constructor modes */

// Creation of diagonal matrix
#define DIAGONAL	1

// Rotation by Euler-angles: Rz(psi)*Ry(tetta)[*Rz(phi)]:
#define EULER		2

// Inversion of rotation by Euler-angles: [Rz(-phi)*]Ry(-tetta)*Rz(-psi):
#define EULER_INV	3

// Rotation around vector defined by spherical coord. (psi,tetta):
// Rz(psi)*Ry(tetta)*Rz(phi)*Ry(-tetta)*Rz(-psi):
#define AROUND_EULER	4

// Rotation around vector defined by descart coord. (x,y,z):
// Rz(psi)*Ry(tetta)*Rz(vabs())*Ry(-tetta)*Rz(psi):
#define AROUND_VECTOR	5

// Note: All of these operations are high optimized. Use them instead of
// another matrix constructor.


/* Spherical representation */
#define _psi_		x
#define _tetta_ 	y
#define _radius_	z

/* Pre-declaration */
struct Vector;
struct Matrix;
struct DBV;
struct DBM;

/******************************************************************************
			   INTEGER VECTOR
******************************************************************************/
struct Vector {
	int x,y,z;

		Vector(){}
		Vector(int u,int v,int t);
		Vector(const DBV& w);

	int operator [](unsigned int i) const { return ((int*)this)[i % 3]; }					      
	int& operator [](unsigned int i) { return ((int*)this)[i % 3]; }					      

	Vector& operator= (const DBV& w);

	Vector operator- () const;

		/* Logical operations */
	int operator () () const;
	int operator ! () const;
	int operator == (const Vector& v) const;
	int operator != (const Vector& v) const;

		/* Norm operations */
	int abs2() const;
	int vabs() const;
	int sum() const;
	Vector& norm(int r);
	Vector get_norm(int r) const;

		/* Vector - Vector operations */
	Vector& operator+= (const Vector& v);
	Vector& operator-= (const Vector& v);
	Vector& operator%= (const Vector& v); //      Cross product

	Vector operator+ (const Vector& v) const;
	Vector operator- (const Vector& v) const;
	Vector operator% (const Vector& v) const;   //	    Cross product
	int operator* (const Vector& v) const;		//	Dot product

	Vector operator/ (const Vector& v) const;   //	    Component-wise division
	Vector& operator *= (const Vector& v);	//	Component-wise multiplication
	Vector& operator /= (const Vector& v);	//	Component-wise division

		/* Vector - Matrix operations */
	Vector& operator*= (const DBM& m);
	Vector operator* (const DBM& m) const;
	friend Vector operator* (const DBM& v, const Vector& m);

		/* Scalar operations */
	Vector& operator*= (int w);
	Vector& operator/= (int w);
	Vector operator* (int w) const;
	Vector operator/ (int w) const;
	friend Vector operator* (int w,const Vector& v);
	
	Vector& operator*= (double w);
	Vector& operator/= (double w);
	Vector operator* (double w) const;
	Vector operator/ (double w) const;
	friend Vector operator* (double w,const Vector& v);


		/* Shifting operations */
	Vector& operator>>= (int n);
	Vector& operator<<= (int n);

	Vector operator>> (int n) const;
	Vector operator<< (int n) const;

		/* I/O operations */
	//friend XConsole& operator<= (XConsole& c,const Vector& v);
	void print();
	friend XStream& operator<= (XStream& s,const Vector& v);
	friend XStream& operator>= (XStream& s,Vector& v);
	friend XStream& operator< (XStream& s,Vector& v);
	friend XStream& operator> (XStream& s,Vector& v);

	friend XBuffer& operator<= (XBuffer& b,const Vector& v);
	friend XBuffer& operator>= (XBuffer& b,Vector& v);
	friend XBuffer& operator< (XBuffer& b,const Vector& v);
	friend XBuffer& operator> (XBuffer& b,Vector& v);

		/* Descart - spherical function */
	int psi() const;
	int tetta() const;
	Vector& spherical(int psi,int tetta,int radius);

		/* Convertions to & from angular coord systems*/
	Vector& spherical2descart();
	Vector& descart2spherical();
	Vector descart() const; 	      // Get descart from spherical
	Vector spherical() const;	      // Get spherical from descart

		/* Special function */
	int BitSR() const;
	Vector& rnd(int r);
	};

struct Matrix {
	int  a[9];

		Matrix(){}
		Matrix(const DBM& m);
	};


/******************************************************************************
			   DOUBLE VECTOR
******************************************************************************/
struct DBV {
	double x;
	double y;
	double z;

		DBV();
		DBV(double u,double v,double t);
		DBV(const DBV& v);
		DBV(const Vector& v);

	double operator [](unsigned int i) const { return ((double*)this)[i % 3]; }					   
	double& operator [](unsigned int i) { return ((double*)this)[i % 3]; }					      

	DBV& operator = (const Vector& v);

	DBV operator- ();

		/* Logical operations */
	int operator () () const;
	int operator ! () const;
	int operator == (const DBV& v) const;
	int operator != (const DBV& v) const;

		/* Norm operations */
	double abs2() const;
	double vabs() const;
	double sum() const;
	DBV& norm(double r);
	DBV get_norm(double r) const;

		/* DBV - DBV operations */
	DBV& operator+= (const DBV& v);
	DBV& operator-= (const DBV& v);
	DBV& operator%= (const DBV& v);       //      Cross product

	DBV operator+ (const DBV& v) const;
	DBV operator- (const DBV& v) const;
	DBV operator% (const DBV& v) const;   //      Cross product
	DBV operator/ (const DBV& v) const;   //      Component-wise division

	DBV& operator*= (const DBV& v);       //      Component-wise multiplication
	DBV& operator/= (const DBV& v);       //      Component-wise division
	double operator* (const DBV& v) const;//      Dot product

		/* DBV - DBM operations */
	DBV& operator*= (const DBM& m);
	DBV operator* (const DBM& m) const;
	friend DBV operator* (const DBM& v, const DBV& m);

		/* Scalar operations */
	DBV& operator*= (double w);
	DBV& operator/= (double w);
	DBV operator* (double w) const;
	DBV operator/ (double w) const;
	friend DBV operator* (double w,const DBV& v);

		/* Shifting operations */
	DBV& operator>>= (int n);
	DBV& operator<<= (int n);

	DBV operator>> (int n) const;
	DBV operator<< (int n) const;

		/* I/O operations */
	//friend XConsole& operator<= (XConsole& c,const DBV& v);
	void print();
	friend XStream& operator<= (XStream& s,const DBV& v);
	friend XStream& operator>= (XStream& s,DBV& v);
	friend XStream& operator< (XStream& s,DBV& v);
	friend XStream& operator> (XStream& s,DBV& v);

	friend XBuffer& operator<= (XBuffer& b,const DBV& v);
	friend XBuffer& operator>= (XBuffer& b,DBV& v);
	friend XBuffer& operator< (XBuffer& b,const DBV& v);
	friend XBuffer& operator> (XBuffer& b,DBV& v);

		/* Descart - spherical functions */
	double psi() const;
	double tetta() const;
	DBV& spherical(double psi,double tetta,double radius);

		/* Convertions to & from angular coord systems*/
	DBV& spherical2descart();
	DBV& descart2spherical();
	DBV descart() const;		   // Get descart from spherical
	DBV spherical() const;		   // Get spherical from descart

		/* Special functions */
	double angle(const DBV& v) const;
	double angle(const DBV& v1,const DBV& v2) const;
};
/******************************************************************************
			   DOUBLE MATRIX
******************************************************************************/
struct DBM {
	double a[9];

			// Creates I
		DBM();
		DBM(const DBM& m);
		DBM(double a0,double a1,double a2,
		    double a3,double a4,double a5,
		    double a6,double a7,double a8);

			// Rotation around 'axis' (for integer angles)
		DBM(int angle,int axis);
			// Rotation around 'axis' (for real angles)
		DBM(double angle,int axis);
			// Diagonal and Euler-angles-matrix (for real angles)
		DBM(double psi,double tetta,double phi,int mode);
			// Rotation around vector
		DBM(const DBV& v,double angle);
	
	//	Access to elements:
	//	0  1  2		1,1   1,2    1,3    	
	//	3  4  5		2,1   2,2    2,3 
	//	6  7  8		3,1   3,2    3,3 
	double operator [](unsigned int i) const { return a[i]; }					      
	double operator ()(int i,int j) const { i--; j--; return a[i + i + i + j]; }	       
	double& operator [](unsigned int i) { return a[i]; }					      
	double& operator ()(int i,int j){ i--; j--; return a[i + i + i + j]; }	       

	DBM& transpose();
	friend DBM transpose(const DBM& m);

	DBM& operator+= (const DBM& m);
	DBM& operator-= (const DBM& m);
	DBM& operator*= (const DBM& m);
	DBM& operator*= (double w);
	DBM& operator/= (double w);
	DBM& operator+= (double w);
	DBM& operator-= (double w);

	DBM operator+ (const DBM& m) const;
	DBM operator- (const DBM& m) const;
	DBM operator* (const DBM& m) const;
	DBM operator* (double w) const;
	DBM operator/ (double w) const;
	DBM operator+ (double w) const;
	DBM operator- (double w) const;


		/* I/O operations */
	//friend XConsole& operator<= (XConsole& c,const DBM& v);
	void print();
	friend XStream& operator<= (XStream& s,const DBM& v);
	friend XStream& operator>= (XStream& s,DBM& v);
	friend XStream& operator< (XStream& s,DBM& v);
	friend XStream& operator> (XStream& s,DBM& v);

	friend XBuffer& operator<= (XBuffer& b,const DBM& v);
	friend XBuffer& operator>= (XBuffer& b,DBM& v);
	friend XBuffer& operator< (XBuffer& b,const DBM& v);
	friend XBuffer& operator> (XBuffer& b,DBM& v);

	DBV rotation_angles() const;
	double rotation_energy() const;

	double det() const;
	DBM inverse() const;
};

/******************************************************************************
				Quaternion  Declaration
******************************************************************************/
struct Quaternion {
	double w;
	double x;
	double y;
	double z;

		Quaternion(){}
		Quaternion(double w,double x,double y,double z);	// Simple assignment 
		Quaternion(double theta,DBV u);					// Rotation by theta around u, where u needn't to be unit
		Quaternion(const DBM& m);					// Representation of matrix by quaternion, where ||m|| == 1
		operator DBM();								// Generate corresponding matrix 

	Quaternion operator ~() const;			// Invertion of imaginary parts (x,y,z)
	Quaternion inverse() const;			// q^-1 = ~q/(q*~q)	
	Quaternion operator -() const;		

	void rotation(double& theta,DBV& u) const;
	void Euler(double& phi,double& psi,double& theta) const;

		/* Norm operations */
	double norm2() const;
	double norm() const;
	Quaternion& norm(double s);
	Quaternion get_norm(double s) const;

		/* Quaternion - Quaternion operations */
	Quaternion& operator+= (const Quaternion& q);
	Quaternion& operator-= (const Quaternion& q);
	Quaternion operator+ (const Quaternion& q) const;
	Quaternion operator- (const Quaternion& q) const;
	
	Quaternion& operator%= (const Quaternion& q);       //      Cross product
	Quaternion operator% (const Quaternion& q) const;   //      Cross product
	
	double operator* (const Quaternion& q) const;		//      Dot product

		/* Scalar operations */
	Quaternion& operator*= (double s);
	Quaternion& operator/= (double s);
	Quaternion operator* (double s) const;
	Quaternion operator/ (double s) const;
	friend Quaternion operator* (double s,const Quaternion& q);

		/* I/O operations */
	//friend XConsole& operator<= (XConsole& c,const Quaternion& q);
	void print();

	friend XStream& operator<= (XStream& s,const Quaternion& q);
	friend XStream& operator>= (XStream& s,Quaternion& q);
	friend XStream& operator< (XStream& s,Quaternion& q);
	friend XStream& operator> (XStream& s,Quaternion& q);

	friend XBuffer& operator<= (XBuffer& b,const Quaternion& q);
	friend XBuffer& operator>= (XBuffer& b,Quaternion& q);
	friend XBuffer& operator< (XBuffer& b,const Quaternion& q);
	friend XBuffer& operator> (XBuffer& b,Quaternion& q);

	/* Special function */
	Quaternion& rotate(DBV v);
};
// Spherical linear interpolation
extern Quaternion Slerp(const Quaternion& A,const Quaternion& B,double t);


/******************************************************************************
	Decomposition of vector 'v' to tangent-component
	and normal-componet for axis 'axis'
******************************************************************************/
#define DECOMPOSITION(v_tangent,v_normal,v,axis)   { v_tangent = (axis*(axis*v))/axis.abs2(); v_normal	 = v - v_tangent; }


/******************************************************************************
		Inline Vector Funtion definition
******************************************************************************/
inline Vector::Vector(int u,int v,int t)
{
	x = u;
	y = v;
	z = t;
}
inline Vector::Vector(const DBV& w)
{
	x = round(w.x);
	y = round(w.y);
	z = round(w.z);
}
inline Vector& Vector::operator= (const DBV& w)
{
	x = round(w.x);
	y = round(w.y);
	z = round(w.z);
	return *this;
}
inline Vector Vector::operator- () const
{	    
	return Vector(-x,-y,-z);
}

	/* Logical operations */
inline int Vector::operator () () const
{
	return x || y || z;
}
inline int Vector::operator ! () const
{
	return !x && !y && !z;
}
inline int Vector::operator == (const Vector& v) const
{
	return x == v.x && y == v.y && z == v.z;
}
inline int Vector::operator != (const Vector& v) const
{
	return x != v.x || y != v.y || z == v.z;
}

	/* Norm operations */
inline int Vector::abs2() const
{
	return x*x + y*y + z*z;
}
inline int Vector::vabs() const
{
	return (int)sqrt((double)x*x + y*y + z*z);
}
inline int Vector::sum() const
{
	return abs(x) + abs(y) + abs(z);
}
inline Vector& Vector::norm(int r)
{
	int a = vabs();
	if(a){
		x = x*r/a;
		y = y*r/a;
		z = z*r/a;
		}
	return *this;
}
inline Vector Vector::get_norm(int r) const
{
	int a = vabs();
	if(a)
		return Vector(x*r/a, y*r/a, z*r/a);
	else
		return Vector(0,0,0);
}

	/* Vector - Vector operations */
inline Vector& Vector::operator+= (const Vector& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
inline Vector& Vector::operator-= (const Vector& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
inline Vector& Vector::operator%= (const Vector& v)
{
	int xx = y*v.z - z*v.y;
	int yy = z*v.x - x*v.z;
	z = x*v.y - y*v.x;
	x = xx;
	y = yy;
	return *this;
}

inline Vector Vector::operator+ (const Vector& v) const
{
	return Vector(x + v.x, y + v.y, z + v.z);
}
inline Vector Vector::operator- (const Vector& v) const
{
	return Vector(x - v.x, y - v.y, z - v.z);
}
inline Vector Vector::operator% (const Vector& v) const
{
	return Vector(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}
inline Vector Vector::operator/ (const Vector& v) const
{
	return Vector(x/v.x, y/v.y, z/v.z);
}
inline Vector& Vector::operator*= (const Vector& v)
{
	x *= v.x; y *= v.y; z *= v.z;
	return *this;
}
inline Vector& Vector::operator/= (const Vector& v)
{
	x /= v.x; y /= v.y; z /= v.z;
	return *this;
}
inline int Vector::operator* (const Vector& v) const
{
	return x*v.x + y*v.y + z*v.z;
}

	/* Scalar operations */
inline Vector& Vector::operator*= (int w)
{
	x *= w;
	y *= w;
	z *= w;
	return *this;
}
inline Vector& Vector::operator/= (int w)
{
	double d = 1/(double)w;
	x = round(x*d);
	y = round(y*d);
	z = round(z*d);
	return *this;
}
inline Vector Vector::operator* (int w) const
{
	return Vector(x*w, y*w, z*w);
}
inline Vector Vector::operator/ (int w) const
{
	double d = 1/(double)w;
	return Vector(round(x*d), round(y*d), round(z*d));
}
inline Vector operator* (int w,const Vector& v)
{
	return Vector(v.x*w, v.y*w, v.z*w);
}

inline Vector& Vector::operator*= (double w)
{
	x = round(x*w);
	y = round(y*w);
	z = round(z*w);
	return *this;
}
inline Vector& Vector::operator/= (double w)
{
	w = 1/w;
	x = round(x*w);
	y = round(y*w);
	z = round(z*w);
	return *this;
}
inline Vector Vector::operator* (double w) const
{
	return Vector(round(x*w), round(y*w), round(z*w));
}
inline Vector Vector::operator/ (double w) const
{
	w = 1/w;
	return Vector(round(x*w), round(y*w), round(z*w));
}
inline Vector operator* (double w,const Vector& v)
{
	return Vector(round(v.x*w), round(v.y*w), round(v.z*w));
}


	/* Shifting operations */
inline Vector& Vector::operator>>= (int n)
{
	x >>= n;
	y >>= n;
	z >>= n;
	return *this;
}
inline Vector& Vector::operator<<= (int n)
{
	x <<= n;
	y <<= n;
	z <<= n;
	return *this;
}
inline Vector Vector::operator>> (int n) const
{
	return Vector(x >> n, y >> n, z >> n);
}
inline Vector Vector::operator<< (int n) const
{
	return Vector(x << n, y << n, z << n);
}

	/* Descart - spherical function */
inline int Vector::psi() const
{
	return Atan2(y,x);
}
inline int Vector::tetta() const
{
	return ArcCos(z,vabs());
}
inline Vector& Vector::spherical(int psi,int tetta,int radius)
{
	double xy = radius*Sin(tetta);
	x = round(xy*Cos(psi));
	y = round(xy*Sin(psi));
	z = round(radius*Cos(tetta));
	return *this;
}
inline Vector& Vector::spherical2descart()
{
	double xy = _radius_*Sin(_tetta_);
	int yy = round(xy*Sin(_psi_));
	x = round(xy*Cos(_psi_));	// ! _psi_ is the same as x
	z = round(_radius_*Cos(_tetta_));
	y = yy;
	return *this;
}
inline Vector& Vector::descart2spherical()
{
	int p = Atan2(y,x);
	int r = vabs();
	int t = ArcCos(z,r);
	_psi_ = p;
	_tetta_ = t;
	_radius_ = r;
	return *this;
}
inline Vector Vector::descart() const
{
	double xy = _radius_*Sin(_tetta_);
	return Vector(
			round(xy*Cos(_psi_)),
			round(xy*Sin(_psi_)),
			round(_radius_*Cos(_tetta_))
			);
}
inline Vector Vector::spherical() const
{
	Vector v;
	v._psi_    = Atan2(y,x);
	v._radius_ = vabs();
	v._tetta_  = ArcCos(z,v._radius_);
	return v;
}

	/* Vector - Matrix operations */
inline Vector& Vector::operator*= (const DBM& m)
{
	int xx = round(m.a[0]*x + m.a[1]*y + m.a[2]*z);
	int yy = round(m.a[3]*x + m.a[4]*y + m.a[5]*z);
	z = round(m.a[6]*x + m.a[7]*y + m.a[8]*z);
	x = xx;
	y = yy;
	return *this;
}
inline Vector Vector::operator* (const DBM& m) const
{
	return Vector(
		round(m.a[0]*x + m.a[1]*y + m.a[2]*z),
		round(m.a[3]*x + m.a[4]*y + m.a[5]*z),
		round(m.a[6]*x + m.a[7]*y + m.a[8]*z));
}
inline Vector operator* (const DBM& m,const Vector& v)
{
	return Vector(
		round(m.a[0]*v.x + m.a[1]*v.y + m.a[2]*v.z),
		round(m.a[3]*v.x + m.a[4]*v.y + m.a[5]*v.z),
		round(m.a[6]*v.x + m.a[7]*v.y + m.a[8]*v.z));
}

	/* Special function */
inline int Vector::BitSR() const
{
	return max(::BitSR(x),::BitSR(y),::BitSR(z));
}
inline Vector& Vector::rnd(int r)
{
	x = random(r);
	y = random(r);
	z = random(r);
	return norm(r);
}

/******************************************************************************
			 DB_VECTOR Definition
******************************************************************************/
inline DBV::DBV()
{
	x = y = z = 0;
}
inline DBV::DBV(double u,double v,double t)
{
	x = u;
	y = v;
	z = t;
}
inline DBV::DBV(const DBV& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}
inline DBV::DBV(const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
}
inline DBV& DBV::operator = (const Vector& v)
{
	x = v.x;
	y = v.y;
	z = v.z;
	return	*this;
}

inline DBV DBV::operator- ()
{
	return DBV(-x,-y,-z);
}

	/* Logical operations */
inline int DBV::operator () () const
{
	return fabs(x) > DBL_EPS || fabs(y) > DBL_EPS || fabs(z) > DBL_EPS;
}
inline int DBV::operator ! () const
{
	return fabs(x) < DBL_EPS && fabs(y) < DBL_EPS && fabs(z) < DBL_EPS;
}
inline int DBV::operator == (const DBV& v) const
{
	return fabs(x - v.x) < DBL_EPS &&
	       fabs(y - v.y) < DBL_EPS &&
	       fabs(z - v.z) < DBL_EPS;
}
inline int DBV::operator != (const DBV& v) const
{
	return fabs(x - v.x) > DBL_EPS ||
	       fabs(y - v.y) > DBL_EPS ||
	       fabs(z - v.z) > DBL_EPS;
}

	/* Norm operations */
inline double DBV::abs2() const
{
	return x*x + y*y + z*z;
}
inline double DBV::vabs() const
{
	return sqrt(x*x + y*y + z*z);
}
inline double DBV::sum() const
{
	return fabs(x) + fabs(y) + fabs(z);
}
inline DBV& DBV::norm(double r)
{
	double a = vabs();
	if(a > DBL_EPS){
		r /= a;
		x = x*r;
		y = y*r;
		z = z*r;
		}
	return *this;
}
inline DBV DBV::get_norm(double r) const
{
	double a = vabs();
	if(a > DBL_EPS){
		r /= a;
		return DBV(x*r, y*r, z*r);
		}
	else
		return DBV(0,0,0);
}

	/* DBV - DBV operations */
inline DBV& DBV::operator+= (const DBV& v)
{
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}
inline DBV& DBV::operator-= (const DBV& v)
{
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}
inline DBV& DBV::operator%= (const DBV& v)
{
	double xx = y*v.z - z*v.y;
	double yy = z*v.x - x*v.z;
	z = x*v.y - y*v.x;
	x = xx;
	y = yy;
	return *this;
}

inline DBV DBV::operator+ (const DBV& v) const
{
	return DBV(x + v.x, y + v.y, z + v.z);
}
inline DBV DBV::operator- (const DBV& v) const
{
	return DBV(x - v.x, y - v.y, z - v.z);
}
inline DBV DBV::operator% (const DBV& v) const
{
	return DBV(y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
}
inline DBV DBV::operator/ (const DBV& v) const
{
	return DBV(x/v.x, y/v.y, z/v.z);
}
inline DBV& DBV::operator*= (const DBV& v)
{
	x *= v.x; y *= v.y; z *= v.z;
	return *this;
}
inline DBV& DBV::operator/= (const DBV& v)
{
	x /= v.x; y /= v.y; z /= v.z;
	return *this;
}
inline double DBV::operator* (const DBV& v) const
{
	return x*v.x + y*v.y + z*v.z;
}

	/* Scalar operations */
inline DBV& DBV::operator*= (double w)
{
	x *= w;
	y *= w;
	z *= w;
	return *this;
}
inline DBV& DBV::operator/= (double w)
{
	w = 1/w;
	x *= w;
	y *= w;
	z *= w;
	return *this;
}
inline DBV DBV::operator* (double w) const
{
	return DBV(x*w, y*w, z*w);
}
inline DBV DBV::operator/ (double w) const
{
	w = 1/w;
	return DBV(x*w, y*w, z*w);
}
inline DBV operator* (double w,const DBV& v)
{
	return DBV(v.x*w, v.y*w, v.z*w);
}

	/* DBV - Matrix operation */
inline DBV& DBV::operator*= (const DBM& m)
{
	double xx = m.a[0]*x + m.a[1]*y + m.a[2]*z;
	double yy = m.a[3]*x + m.a[4]*y + m.a[5]*z;
	z = m.a[6]*x + m.a[7]*y + m.a[8]*z;
	x = xx;
	y = yy;
	return *this;
}
inline DBV DBV::operator* (const DBM& m) const
{
	return DBV (m.a[0]*x + m.a[1]*y + m.a[2]*z,
		     m.a[3]*x + m.a[4]*y + m.a[5]*z,
		     m.a[6]*x + m.a[7]*y + m.a[8]*z);
}
inline DBV operator* (const DBM& m, const DBV& v)
{
	return DBV (m.a[0]*v.x + m.a[1]*v.y + m.a[2]*v.z,
		     m.a[3]*v.x + m.a[4]*v.y + m.a[5]*v.z,
		     m.a[6]*v.x + m.a[7]*v.y + m.a[8]*v.z);
}

	/* Shifting operations */
inline DBV& DBV::operator>>= (int n)
{
	double a = 1/(double)(1 << n);
	x *= a;
	y *= a;
	z *= a;
	return *this;
}
inline DBV& DBV::operator<<= (int n)
{
	double a = 1 << n;
	x *= a;
	y *= a;
	z *= a;
	return *this;
}
inline DBV DBV::operator>> (int n) const
{
	double a = 1/(double)(1 << n);
	return DBV(x*a, y*a, z*a);
}
inline DBV DBV::operator<< (int n) const
{
	double a = 1 << n;
	return DBV(x*a, y*a, z*a);
}

	/* Descart - spherical function */
inline double DBV::psi() const
{
	return atan2(y,x);
}
inline double DBV::tetta() const
{
	double a = vabs();
	if(fabs(z) >= a - DBL_EPS) {
		if(z >= 0) {
			return 0;
		} else {
			return M_PI;
		}
	}
	return acos(z/a);
}
inline DBV& DBV::spherical(double psi,double tetta,double radius)
{
	x = radius*sin(tetta);
	y = x*sin(psi);
	x = x*cos(psi);
	z = radius*cos(tetta);
	return *this;
}
inline DBV& DBV::spherical2descart()
{
	double xx = _radius_*sin(_tetta_);
	double yy = xx*sin(_psi_);
	xx *= cos(_psi_);
	z = _radius_*cos(_tetta_);
	x = xx;
	y = yy;
	return *this;
}
inline DBV& DBV::descart2spherical()
{
	double p = atan2(y,x);
	double r = vabs();
	double t = acos(z/r);
	_psi_ = p;
	_tetta_ = t;
	_radius_ = r;
	return *this;
}
inline DBV DBV::descart() const
{
	double xx = _radius_*sin(_tetta_);
	return DBV(
			xx*cos(_psi_),
			xx*sin(_psi_),
			_radius_*cos(_tetta_)
			);
}
inline DBV DBV::spherical() const
{
	DBV v;
	v._psi_    = atan2(y,x);
	v._radius_ = vabs();
	v._tetta_  = acos(z/v._radius_);
	return v;
}


/******************************************************************************
			 Double Matrix Definition
******************************************************************************/
inline DBM::DBM()
{
	a[0] = 1; a[1] = 0; a[2] = 0;
	a[3] = 0; a[4] = 1; a[5] = 0;
	a[6] = 0; a[7] = 0; a[8] = 1;
}
inline DBM::DBM(const DBM& m)
{
	a[0] = m[0]; a[1] = m[1]; a[2] = m[2];
	a[3] = m[3]; a[4] = m[4]; a[5] = m[5];
	a[6] = m[6]; a[7] = m[7]; a[8] = m[8];
}
inline DBM::DBM(double a0,double a1,double a2,
	 double a3,double a4,double a5,
	 double a6,double a7,double a8)
{
	a[0] = a0; a[1] = a1; a[2] = a2;
	a[3] = a3; a[4] = a4; a[5] = a5;
	a[6] = a6; a[7] = a7; a[8] = a8;
}
inline DBM::DBM(int angle,int axis)
{
//	------ Calculate Matrix for ROTATE point an angle ------
	double calpha = Cos(angle);
	double salpha = Sin(angle);
	switch(axis){
		case Z_AXIS:
			a[0]   =  calpha; a[1]	 = -salpha; a[2]   = 0;
			a[3]   =  salpha; a[4]	 = calpha;  a[5]   = 0;
			a[6]   = 0;	  a[7]	 = 0;	    a[8]   = 1;
			break;
		case X_AXIS:
			a[0]   =  1;	  a[1]	 =  0;	    a[2]   = 0;
			a[3]   =  0;	  a[4]	 =  calpha; a[5]   = -salpha;
			a[6]   =  0;	  a[7]	 =  salpha; a[8]   = calpha;
			break;
		case Y_AXIS:
			a[0]   = calpha;  a[1]	 =  0;	    a[2]   = salpha;
			a[3]   = 0;	  a[4]	 =  1;	    a[5]   = 0;
			a[6]   = -salpha; a[7]	 =  0;	    a[8]   = calpha;
			break;
		}
}
inline DBM::DBM(double angle,int axis)
{
//	------ Calculate Matrix for ROTATE point an angle ------
	double calpha = cos(angle);
	double salpha = sin(angle);
	switch(axis){
		case Z_AXIS:
			a[0]   =  calpha; a[1]	 = -salpha; a[2]   = 0;
			a[3]   =  salpha; a[4]	 = calpha;  a[5]   = 0;
			a[6]   = 0;	  a[7]	 = 0;	    a[8]   = 1;
			break;
		case X_AXIS:
			a[0]   =  1;	  a[1]	 =  0;	    a[2]   = 0;
			a[3]   =  0;	  a[4]	 =  calpha; a[5]   = -salpha;
			a[6]   =  0;	  a[7]	 =  salpha; a[8]   = calpha;
			break;
		case Y_AXIS:
			a[0]   = calpha;  a[1]	 =  0;	    a[2]   = salpha;
			a[3]   = 0;	  a[4]	 =  1;	    a[5]   = 0;
			a[6]   = -salpha; a[7]	 =  0;	    a[8]   = calpha;
			break;
		}
}

inline DBM& DBM::transpose()
{
	double t;
	t = a[1]; a[1] = a[3]; a[3] = t;
	t = a[2]; a[2] = a[6]; a[6] = t;
	t = a[5]; a[5] = a[7]; a[7] = t;
	return *this;
}
inline DBM transpose(const DBM& m)
{
	return DBM(m[0],m[3],m[6],
			    m[1],m[4],m[7],
			    m[2],m[5],m[8]);
}

inline DBM& DBM::operator+= (const DBM& m)
{
	for(int i = 0;i < 9;i++)
		a[i] += m.a[i];
	return *this;
}
inline DBM& DBM::operator-= (const DBM& m)
{
	for(int i = 0;i < 9;i++)
		a[i] -= m.a[i];
	return *this;
}
inline DBM& DBM::operator*= (const DBM& m)
{
	double t[9];
	t[0] = a[0]*m.a[0] + a[1]*m.a[3] + a[2]*m.a[6];
	t[1] = a[0]*m.a[1] + a[1]*m.a[4] + a[2]*m.a[7];
	t[2] = a[0]*m.a[2] + a[1]*m.a[5] + a[2]*m.a[8];
	t[3] = a[3]*m.a[0] + a[4]*m.a[3] + a[5]*m.a[6];
	t[4] = a[3]*m.a[1] + a[4]*m.a[4] + a[5]*m.a[7];
	t[5] = a[3]*m.a[2] + a[4]*m.a[5] + a[5]*m.a[8];
	t[6] = a[6]*m.a[0] + a[7]*m.a[3] + a[8]*m.a[6];
	t[7] = a[6]*m.a[1] + a[7]*m.a[4] + a[8]*m.a[7];
	t[8] = a[6]*m.a[2] + a[7]*m.a[5] + a[8]*m.a[8];
	memcpy(a,t,sizeof(double)*9);
	return *this;
}
inline DBM& DBM::operator*= (double w)
{
	for(int i = 0;i < 9;i++)
		a[i] *= w;
	return *this;
}
inline DBM& DBM::operator/= (double w)
{
	w = 1/w;
	for(int i = 0;i < 9;i++)
		a[i] *= w;
	return *this;
}
inline DBM& DBM::operator+= (double w)
{
	for(int i = 0;i < 9;i++)
		a[i] += w;
	return *this;
}
inline DBM& DBM::operator-= (double w)
{
	for(int i = 0;i < 9;i++)
		a[i] -= w;
	return *this;
}

inline DBM DBM::operator+ (const DBM& m) const
{
	DBM m1;
	for(int i = 0;i < 9;i++)
		m1.a[i] = a[i] + m.a[i];
	return m1;
}
inline DBM DBM::operator- (const DBM& m) const
{
	DBM m1;
	for(int i = 0;i < 9;i++)
		m1.a[i] = a[i] - m.a[i];
	return m1;
}
inline DBM DBM::operator* (const DBM& m) const
{
	DBM m1;
	m1.a[0] = a[0]*m.a[0] + a[1]*m.a[3] + a[2]*m.a[6];
	m1.a[1] = a[0]*m.a[1] + a[1]*m.a[4] + a[2]*m.a[7];
	m1.a[2] = a[0]*m.a[2] + a[1]*m.a[5] + a[2]*m.a[8];
	m1.a[3] = a[3]*m.a[0] + a[4]*m.a[3] + a[5]*m.a[6];
	m1.a[4] = a[3]*m.a[1] + a[4]*m.a[4] + a[5]*m.a[7];
	m1.a[5] = a[3]*m.a[2] + a[4]*m.a[5] + a[5]*m.a[8];
	m1.a[6] = a[6]*m.a[0] + a[7]*m.a[3] + a[8]*m.a[6];
	m1.a[7] = a[6]*m.a[1] + a[7]*m.a[4] + a[8]*m.a[7];
	m1.a[8] = a[6]*m.a[2] + a[7]*m.a[5] + a[8]*m.a[8];
	return m1;
}
inline DBM DBM::operator* (double w) const
{
	DBM m;
	for(int i = 0;i < 9;i++)
		m.a[i] = a[i]*w;
	return m;
}
inline DBM DBM::operator/ (double w) const
{
	DBM m;
	w = 1/w;
	for(int i = 0;i < 9;i++)
		m.a[i] = a[i]*w;
	return m;
}
inline DBM DBM::operator+ (double w) const
{
	DBM m;
	for(int i = 0;i < 9;i++)
		m.a[i] = a[i] + w;
	return m;
}
inline DBM DBM::operator- (double w) const
{
	DBM m;
	for(int i = 0;i < 9;i++)
		m.a[i] = a[i] - w;
	return m;
}

#endif