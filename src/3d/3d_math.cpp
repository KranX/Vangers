#include "../global.h"
#include "general.h"

/******************************************************************************
		Creating tables, Sqrt(), Atan2(), ArcCos()
******************************************************************************/
#ifndef _STATIC_TABLES_
short unsigned int* ArcCosTable;
short unsigned int* ArcTanTable;
short unsigned int* SqrtTable;

float* cos_table;
char* mul_table;
short* div_table_short;
char* power_table;
#else
short unsigned int ArcCosTable[ArcCosMAX + 1];
short unsigned int ArcTanTable[ArcTanMAX + 1];
short unsigned int SqrtTable[SqrtMax];

float cos_table[2*Pi];
char mul_table[256*256];
short div_table_short[256*256];
char power_table[256*(N_POWER_MAX - 1)];
#endif

int tables_are_made = 0;
void make_tables()
{
	int i;
	if(tables_are_made)
		return;
	tables_are_made = 1;

#ifndef _STATIC_TABLES_
	int size = 2*Pi*sizeof(float) + ((ArcCosMAX + 1) + (ArcTanMAX + 1) + SqrtMax)*sizeof(short int);

	cos_table = ALLOC(size,float);
	ArcCosTable = (short unsigned int*)(cos_table + 2*Pi);
	ArcTanTable = ArcCosTable + ArcCosMAX + 1;
	SqrtTable = ArcTanTable + ArcTanMAX + 1;

	mul_table = new char[256*256]; 
	memset(mul_table, 0, 256*256);
	div_table_short = ALLOC(256*256,short);  // TODO uninitialised value(s)
	power_table = ALLOC(256*(N_POWER_MAX - 1),char);
#endif


	for(i = 0;i < 2*Pi;i++)
		cos_table[i] = (float)cos(GTOR(i));

	for(i = 0;i <= ArcCosMAX;i++)
		ArcCosTable[i] = (int)(double(Pi)*acos(((double)i)/ArcCosMAX)/M_PI);
	for(i = 0;i <= ArcTanMAX;i++)
		ArcTanTable[i] = (int)(double(Pi)*atan(((double)i)/ArcTanMAX)/M_PI);
	for(i = 0;i < SqrtMax;i++)
		SqrtTable[i] = (int)(sqrt((double)i) + .5);

	int x,y;
	for(x = -127;x <= 127;x++){
		for(y = -127;y <= 127;y++)
			mul_table[(x & 0xFF) | ((y << 8) & 0xFF00)] = GOOD_SHR(x*y,7);
		for(y = 0;y <= 255;y++){
			div_table_short[(x & 0xFF) | ((y << 8) & 0xFF00)] = y ? (x << 8)/y : 0;
			}
		}

	int n;
	for(n = 2;n <= N_POWER_MAX;n++){
		for(x = -127;x <= 127;x++){
			y = x;
			for(i = 1;i < n;i++)
				y = GOOD_SHR(y*x,7);
			power_table[((n - 2) << 8) + (x & 0xFF)] = y;
			}
		}
}
int ArcCos(int Dx,int Abs)
{
	if(!Abs)
		return 0;
	if(Dx >= 0){
		if(Dx >= Abs)
			return 0;
		return ArcCosTable[(((unsigned int )Dx) << ArcCosLen)/Abs];
		}
	else{
		if(-Dx >= Abs)
			return Pi;
		return Pi - ArcCosTable[(((unsigned int )-Dx) << ArcCosLen)/Abs];
		}
}
int Atan2(int Dy,int Dx)
{
	if(!Dx && !Dy)
		return 0;

	int Q = 0,S = 0;
	if(Dy >= 0){
		if(Dx < 0){
			Dx = -Dx;
			Q = Pi;
			S = 1;
			}
		}
	else
		if(Dx < 0){
			Dy = -Dy;
			Dx = -Dx;
			Q = Pi;
			}
		else{
			Dy = -Dy;
			Q = 2*Pi;
			S = 1;
			}

	unsigned int Ind;
	int Val;
	if(Dx >= Dy){
		Ind = Dy;
		Val = ArcTanTable[(Ind << ArcTanLen)/Dx];
		}
	else{
		Ind = Dx;
		Val = Pi/2 - ArcTanTable[(Ind << ArcTanLen)/Dy];
		}
	return Q + (S ? -Val : Val);
}

int Sqrt(int x)
{
	if(x < 0)
		x = 0;
	int len = BitSR(x);
	if(len >= SqrtLen){
		int shift = len - SqrtLen + 1;
		if(shift & 1)
			shift++;
		return SqrtTable[x >> shift] << (shift >> 1);
		}
	return SqrtTable[x];
}
int DistPi(int s0,int s1)
{
	s0 &= ANGLE_MASK;
	s1 &= ANGLE_MASK;
	int d0 = s0 - s1;
	int ad0 = abs(d0);
	int d1 = 2*Pi - ad0;
	if(ad0 < d1) return d0;
	return s0 > s1 ? -(2*Pi - s0 + s1) : 2*Pi - s1 + s0;
}
double DistPI(double s0,double s1)
{
	s0 = asin(sin(s0));
	s1 = asin(sin(s1));
	double d0 = s0 - s1;
	double ad0 = fabs(d0);
	double d1 = 2*M_PI - ad0;
	if(ad0 < d1) return d0;
	return s0 > s1 ? -(2*M_PI - s0 + s1) : 2*M_PI - s1 + s0;
}

/******************************************************************************
			RANDOM	FUNCTION
******************************************************************************/
static unsigned long int next = 1;
int _rand(void)
{
	next = next * 1103515245 + 12345;
	return ((unsigned int) (next >> 16) & 0x7FFF);
}
void _srand(unsigned int seed)
{
	next = seed;
}

/******************************************************************************
		    VECTOR Definition
******************************************************************************/
	/* I/O operations */
/*XConsole& operator<= (XConsole& c,const Vector& v)
{
	c <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return c;
}*/
void Vector::print() {
	std::cout<<x<<" \t"<<y<<" \t"<<z<<"\n";
}

XStream& operator<= (XStream& s,const Vector& v)
{
	s <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return s;
}
XStream& operator>= (XStream& s,Vector& v)
{
	s >= v.x >= v.y >= v.z;
	return s;
}
XStream& operator< (XStream& s,Vector& v)
{
	s.write(&v,sizeof(Vector));
	return s;
}
XStream& operator> (XStream& s,Vector& v)
{
	s.read(&v,sizeof(Vector));
	return s;
}

XBuffer& operator<= (XBuffer& b,const Vector& v)
{
	b <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return b;
}
XBuffer& operator>= (XBuffer& b,Vector& v)
{
	b >= v.x >= v.y >= v.z;
	return b;
}
XBuffer& operator< (XBuffer& b,const Vector& v)
{
	b < v.x < v.y < v.z;
	return b;
}
XBuffer& operator> (XBuffer& b,Vector& v)
{
	b > v.x > v.y > v.z;
	return b;
}

/******************************************************************************
		  MATRIX Definition
******************************************************************************/
Matrix::Matrix(const DBM& m)
{
	for(int i = 0;i < 9;i++)
		a[i] = round(m.a[i]);
}
/******************************************************************************
			 DB_VECTOR Definition
******************************************************************************/
	/* I/O operations */
/*XConsole& operator<= (XConsole& c,const DBV& v)
{
	c <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return c;
}*/
void DBV::print() {
	std::cout<<x<<" \t"<<y<<" \t"<<z<<"\n";
}

XStream& operator<= (XStream& s,const DBV& v)
{
	s <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return s;
}
XStream& operator>= (XStream& s,DBV& v)
{
	s >= v.x >= v.y >= v.z;
	return s;
}
XStream& operator< (XStream& s,DBV& v)
{
	s.write(&v,sizeof(DBV));
	return s;
}
XStream& operator> (XStream& s,DBV& v)
{
	s.read(&v,sizeof(DBV));
	return s;
}

XBuffer& operator<= (XBuffer& b,const DBV& v)
{
	b <= v.x < " \t" <= v.y < " \t" <= v.z < "\n";
	return b;
}
XBuffer& operator>= (XBuffer& b,DBV& v)
{
	b >= v.x >= v.y >= v.z;
	return b;
}
XBuffer& operator< (XBuffer& b,const DBV& v)
{
	b < v.x < v.y < v.z;
	return b;
}
XBuffer& operator> (XBuffer& b,DBV& v)
{
	b > v.x > v.y > v.z;
	return b;
}

	/* Special 3D functions */
double DBV::angle(const DBV& w) const
{
	double a = vabs()*w.vabs();
	double b = (*this)*w;
	double c = b/a;
	if(fabs(c) >= 1) {
		if(b >= 0) {
			return 0;
		} else {
			return M_PI;
        }
    }
	return acos(c);
}
double DBV::angle(const DBV& v1,const DBV& v2) const
{
	 DBV v = v1 % v2;
	 double val = v1.angle(v2);
	 if((*this)*v < -1e-10)
		val = 2*M_PI - val;
	return val;
}

/******************************************************************************
			DB_MATRIX Definition
******************************************************************************/
DBM::DBM(double psi,double tetta,double phi,int mode)
{
	switch(mode){
		case DIAGONAL:
			memset(a,0,sizeof(double)*9);
			a[0] = psi;
			a[4] = tetta;
			a[8] = phi;
			break;

		case EULER:
			if(fabs(phi) < DBL_EPS){
				double t1 = cos(psi);
				double t2 = cos(tetta);
				double t4 = sin(psi);
				double t5 = sin(tetta);
				a[0] = t1*t2;  a[1] = -t4;  a[2] = t1*t5;
				a[3] = t4*t2;  a[4] = t1;   a[5] = t4*t5;
				a[6] = -t5;    a[7] = 0.0;  a[8] = t2;
				}
			else{
				double t1 = cos(psi);
				double t2 = cos(tetta);
				double t3 = t1*t2;
				double t4 = cos(phi);
				double t6 = sin(psi);
				double t7 = sin(phi);
				double t13 = sin(tetta);
				double t15 = t6*t2;
				a[0] = t3*t4-t6*t7;  a[1] = -t3*t7-t4*t6;  a[2] = t1*t13;
				a[3] = t15*t4+t1*t7; a[4] = -t15*t7+t1*t4; a[5] = t6*t13;
				a[6] = -t13*t4;      a[7] = t13*t7;	   a[8] = t2;
				}
			break;

		case EULER_INV:
			if(fabs(phi) < DBL_EPS){
				double t1 = cos(psi);
				double t2 = cos(tetta);
				double t4 = sin(psi);
				double t6 = sin(tetta);
				a[0]   = t1*t2;   a[1]	 =  t4*t2;  a[2]   = -t6;
				a[3]   = -t4;	  a[4]	 =  t1;     a[5]   = 0;
				a[6]   = t1*t6;   a[7]	 =  t4*t6;  a[8]   = t2;
				}
			else{
				double t1 = cos(psi);
				double t2 = cos(tetta);
				double t3 = t1*t2;
				double t4 = cos(phi);
				double t6 = sin(psi);
				double t7 = sin(phi);
				double t10 = t6*t2;
				double t14 = sin(tetta);
				a[0] = t3*t4-t6*t7;  a[1] = t10*t4+t1*t7;  a[2] = -t14*t4;
				a[3] = -t3*t7-t4*t6; a[4] = -t10*t7+t1*t4; a[5] = t14*t7;
				a[6] = t1*t14;	     a[7] = t6*t14;	   a[8] = t2;
				}
			break;
		case AROUND_EULER: {
			double t1 = cos(psi);
			double t2 = t1*t1;
			double t3 = cos(tetta);
			double t4 = t3*t3;
			double t6 = cos(phi);
			double t8 = sin(psi);
			double t9 = t8*t8;
			double t11 = sin(tetta);
			double t12 = t11*t11;
			double t17 = t8*t4*t1*t6;
			double t19 = sin(phi);
			double t20 = t9*t3*t19;
			double t22 = t2*t3*t19;
			double t24 = t1*t8*t6;
			double t26 = t1*t12*t8;
			double t28 = t11*t1;
			double t29 = t3*t6;
			double t30 = t28*t29;
			double t31 = t11*t8;
			double t32 = t31*t19;
			double t33 = t28*t3;
			double t41 = t31*t29;
			double t42 = t28*t19;
			double t43 = t31*t3;
			a[0] = t2*t4*t6+t9*t6+t2*t12;
			a[1] = t17-t20-t22-t24+t26;
			a[2] = -t30+t32+t33;
			a[3] = t17+t22+t20-t24+t26;
			a[4] = t9*t4*t6+t2*t6+t9*t12;
			a[5] = -t41-t42+t43;
			a[6] = -t30-t32+t33;
			a[7] = -t41+t42+t43;
			a[8] = t12*t6+t4;
			break;
			}

		case AROUND_VECTOR: {
			DBV v(psi,tetta,phi);
			(*this) = DBM(v.psi(),v.tetta(),v.vabs(),AROUND_EULER);
			break;
			}
		}
}
DBM::DBM(const DBV& v,double angle)
{
	(*this) = DBM(v.psi(),v.tetta(),angle,AROUND_EULER);
}

	/* I/O operations */
/*XConsole& operator<= (XConsole& c,const DBM& m)
{
	c <= m.a[0] < " \t" <= m.a[1] < " \t" <= m.a[2] < "\n";
	c <= m.a[3] < " \t" <= m.a[4] < " \t" <= m.a[5] < "\n";
	c <= m.a[6] < " \t" <= m.a[7] < " \t" <= m.a[8] < "\n";
	return c;
}*/

void DBM::print() {
	std::cout<<a[0]<<" \t"<<a[1]<<" \t"<<a[2]<<"\n";
	std::cout<<a[3]<<" \t"<<a[4]<<" \t"<<a[5]<<"\n";
	std::cout<<a[6]<<" \t"<<a[7]<<" \t"<<a[8]<<"\n";
}


XStream& operator<= (XStream& s,const DBM& m)
{
	s <= m.a[0] < " \t" <= m.a[1] < " \t" <= m.a[2] < "\n";
	s <= m.a[3] < " \t" <= m.a[4] < " \t" <= m.a[5] < "\n";
	s <= m.a[6] < " \t" <= m.a[7] < " \t" <= m.a[8] < "\n";
	return s;
}
XStream& operator>= (XStream& s,DBM& m)
{
	s >= m.a[0] >= m.a[1] >= m.a[2];
	s >= m.a[3] >= m.a[4] >= m.a[5];
	s >= m.a[6] >= m.a[7] >= m.a[8];
	return s;
}
XStream& operator< (XStream& s,DBM& m)
{
	s.write(&m,sizeof(DBM));
	return s;
}
XStream& operator> (XStream& s,DBM& m)
{
	s.read(&m,sizeof(DBM));
	return s;
}

XBuffer& operator<= (XBuffer& b,const DBM& m)
{
	b <= m.a[0] < " \t" <= m.a[1] < " \t" <= m.a[2] < "\n";
	b <= m.a[3] < " \t" <= m.a[4] < " \t" <= m.a[5] < "\n";
	b <= m.a[6] < " \t" <= m.a[7] < " \t" <= m.a[8] < "\n";
	return b;
}
XBuffer& operator>= (XBuffer& b,DBM& m)
{
	b >= m.a[0] >= m.a[1] >= m.a[2];
	b >= m.a[3] >= m.a[4] >= m.a[5];
	b >= m.a[6] >= m.a[7] >= m.a[8];
	return b;
}
XBuffer& operator< (XBuffer& b,const DBM& m)
{
	b < m.a[0] < m.a[1] < m.a[2];
	b < m.a[3] < m.a[4] < m.a[5];
	b < m.a[6] < m.a[7] < m.a[8];
	return b;
}
XBuffer& operator> (XBuffer& b,DBM& m)
{
	b > m.a[0] > m.a[1] > m.a[2];
	b > m.a[3] > m.a[4] > m.a[5];
	b > m.a[6] > m.a[7] > m.a[8];
	return b;
}
double DBM::det() const
{
	return 	a[0]*a[4]*a[8] + a[1]*a[6]*a[5] + a[3]*a[7]*a[2] - a[6]*a[4]*a[2] - a[3]*a[1]*a[8] - a[0]*a[5]*a[7];
}

DBM DBM::inverse() const
{
      double t4 = a[0]*a[4];
      double t6 = a[0]*a[5];
      double t8 = a[1]*a[3];
      double t10 = a[2]*a[3];
      double t12 = a[1]*a[6];
      double t14 = a[2]*a[6];
      double t17 = 1/(t4*a[8]-t6*a[7]-t8*a[8]+t10*a[7]+t12*a[5]-t14*a[4]);

      DBM inv;
      inv.a[0] = (a[4]*a[8]-a[5]*a[7])*t17;
      inv.a[1] = -(a[1]*a[8]-a[2]*a[7])*t17;
      inv.a[2] = -(-a[1]*a[5]+a[2]*a[4])*t17;
      inv.a[3] = -(a[3]*a[8]-a[5]*a[6])*t17;
      inv.a[4] = (a[0]*a[8]-t14)*t17;
      inv.a[5] = -(t6-t10)*t17;
      inv.a[6] = (a[3]*a[7]-a[4]*a[6])*t17;
      inv.a[7] = -(a[0]*a[7]-t12)*t17;
      inv.a[8] = (t4-t8)*t17;
      return inv;
}

DBV DBM::rotation_angles() const
{
	return DBV(
		acos(a[0]/sqrt(sqr(a[0]) + sqr(a[3]) + sqr(a[6]) + DBL_EPS)),
		acos(a[4]/sqrt(sqr(a[1]) + sqr(a[4]) + sqr(a[7]) + DBL_EPS)),
		acos(a[8]/sqrt(sqr(a[2]) + sqr(a[5]) + sqr(a[8]) + DBL_EPS))
		);
}
double DBM::rotation_energy() const
{
	return rotation_angles().vabs();
}


/******************************************************************************
				Quaternion  Definition
******************************************************************************/
Quaternion::Quaternion(double ww,double xx,double yy,double zz)
{
	w = ww;
	x = xx;
	y = yy;
	z = zz;
}
Quaternion::Quaternion(double theta,DBV u)
{
	theta /= 2;
	w = cos(theta);
	u.norm(sin(theta));
	x = u.x;
	y = u.y;
	z = u.z;
}
Quaternion::Quaternion(const DBM& m)
{
	double a1 = m[0];
	double a2 = m[4];
	double a3 = m[8];
	x = 1 + a1 - a2 - a3;
	y = 1 - a1 + a2 - a3;
	z = 1 - a1 - a2 + a3;
	w = 1 + a1 + a2 + a3;
	if(x < 0)
		x = 0;
	if(y < 0)
		y = 0;
	if(z < 0)
		z = 0;
	if(w < 0)
		w = 0;
	x = sqrt(x)/2;
	y = sqrt(y)/2;
	z = sqrt(z)/2;
	w = sqrt(w)/2;
	if(SIGN(w*x) != SIGN(m[7] - m[5]))
		x = -x;
	if(SIGN(w*y) != SIGN(m[2] - m[6]))
		y = -y;
	if(SIGN(w*z) != SIGN(m[3] - m[1]))
		z = -z;
}
Quaternion::operator DBM()
{
	double t1 = w*w;
	double t2 = z*z;
	double t3 = y*y;
	double t4 = x*x;
	double t6 = w*z;
	double t7 = y*x;
	double t9 = w*y;
	double t10 = z*x;
	double t14 = z*y;
	double t15 = w*x;
	return DBM( t1-t2-t3+t4,	-2.0*t6+2.0*t7,			2.0*t9+2.0*t10,
			2.0*t6+2.0*t7,		-t2+t1-t4+t3,			2.0*t14-2.0*t15,
			-2.0*t9+2.0*t10,	2.0*t14+2.0*t15,	-t3-t4+t1+t2);
}

Quaternion Quaternion::operator ~() const
{
	return Quaternion(w,-x,-y,-z);
}
Quaternion Quaternion::inverse() const
{
	return ~*this/norm2();
}
Quaternion Quaternion::operator -() const
{
	return Quaternion(-w,-x,-y,-z);
}

void Quaternion::rotation(double& theta,DBV& u) const
{
	double w2 = sqr(w);
	u = DBV(x,y,z)/sqrt(1 - w2);
	theta = acos(2*w2 - 1);
}
void Quaternion::Euler(double& phi,double& psi,double& theta) const
{
	double a = sqrt(1 - sqr(w));
	if(a < DBL_EPS){
		phi = psi = theta = 0;
		return;
		}
	theta = acos(z/a);
	phi = 2*acos(w);
	psi = atan2(y,x);
}

	/* Norm operations */
double Quaternion::norm2() const
{
	return w*w + x*x + y*y + z*z;
}
double Quaternion::norm() const
{
	return sqrt(w*w + x*x + y*y + z*z);
}

Quaternion& Quaternion::norm(double s)
{
	double k = s/norm();
	w *= k;
	x *= k;
	y *= k;
	z *= k;
	return *this;
}
Quaternion Quaternion::get_norm(double s) const
{
	double k = s/norm();
	return Quaternion(w* k,x*k,y*k,z*k);
}
	/* Quaternion - Quaternion operations */
Quaternion& Quaternion::operator+= (const Quaternion& q)
{
	w += q.w;
	x += q.x;
	y += q.y;
	z += q.z;
	return *this;
}
Quaternion& Quaternion::operator-= (const Quaternion& q)
{
	w -= q.w;
	x -= q.x;
	y -= q.y;
	z -= q.z;
	return *this;
}
Quaternion Quaternion::operator+ (const Quaternion& q) const
{
	return Quaternion(w + q.w,x + q.x,y + q.y,z + q.z);
}
Quaternion Quaternion::operator- (const Quaternion& q) const
{
	return Quaternion(w - q.w,x - q.x,y - q.y,z - q.z);
}

Quaternion& Quaternion::operator%= (const Quaternion& q)
{
	*this = *this % q;
	return *this;
}	
Quaternion Quaternion::operator% (const Quaternion& q) const
{
	return Quaternion(
			w*q.w-x*q.x-y*q.y-z*q.z,
			w*q.x+q.w*x+y*q.z-z*q.y,
			w*q.y+q.w*y+z*q.x-x*q.z,
			w*q.z+q.w*z+x*q.y-y*q.x);
}


double Quaternion::operator* (const Quaternion& q) const
{
	return w*q.w + x*q.x + y*q.y + z*q.z;
}

	/* Scalar operations */
Quaternion& Quaternion::operator*= (double s)
{
	w *= s;
	x *= s;
	y *= s;
	z *= s;
	return *this;
}
Quaternion& Quaternion::operator/= (double s)
{
	s = 1/s;
	w *= s;
	x *= s;
	y *= s;
	z *= s;
	return *this;
}
Quaternion Quaternion::operator* (double s) const
{
	return Quaternion(w*s,x*s,y*s,z*s);
}
Quaternion Quaternion::operator/ (double s) const
{
	s = 1/s;
	return Quaternion(w*s,x*s,y*s,z*s);
}
Quaternion operator* (double s,const Quaternion& q)
{
	return Quaternion(q.w*s,q.x*s,q.y*s,q.z*s);
}

Quaternion Slerp(const Quaternion& A,const Quaternion& B,double t)
{
	double pre_theta = A*B;
	if (fabs(pre_theta) > 1.0) {
		return A;
	}

	double theta = acos(pre_theta);
	double sin_theta = sin(theta);
	if(fabs(sin_theta) < DBL_EPS) {
		return A;
	}

	sin_theta = 1/sin_theta;
	return A*(sin(theta*(1 - t))*sin_theta) + B*(sin(theta* t)*sin_theta);
}

	/* I/O operations */
/*XConsole& operator<= (XConsole& c,const Quaternion& q)
{
	c <= q.w < " \t\t"  <= q.x < " \t" <= q.y < " \t" <= q.z < "\n";
	return c;
}*/

void Quaternion::print() {
	std::cout<<w<<" \t\t"<<x<<" \t"<<y<<" \t"<<z<<"\n";
}

XStream& operator<= (XStream& s,const Quaternion& q)
{
	s <= q.w < " \t\t"  <= q.x < " \t" <= q.y < " \t" <= q.z < "\n";
	return s;
}
XStream& operator>= (XStream& s,Quaternion& q)
{
	s >= q.w >= q.x >= q.y >= q.z;
	return s;
}
XStream& operator< (XStream& s,Quaternion& q)
{
	s.write(&q,sizeof(Quaternion));
	return s;
}
XStream& operator> (XStream& s,Quaternion& q)
{
	s.read(&q,sizeof(Quaternion));
	return s;
}

XBuffer& operator<= (XBuffer& b,const Quaternion& q)
{
	b <= q.w < " \t\t"  <= q.x < " \t" <= q.y < " \t" <= q.z < "\n";
	return b;
}
XBuffer& operator>= (XBuffer& b,Quaternion& q)
{
	b >= q.w >= q.x >= q.y >= q.z;
	return b;
}
XBuffer& operator< (XBuffer& b,const Quaternion& q)
{
	b < q.w < q.x < q.y < q.z;
	return b;
}
XBuffer& operator> (XBuffer& b,Quaternion& q)
{
	b > q.w > q.x > q.y > q.z;
	return b;
}

Quaternion& Quaternion::rotate(DBV v)
{
	v *= 0.5;
        w += -x*v.x-y*v.y-z*v.z;
	x += w*v.x-z*v.y+y*v.z;
	y += w*v.y-x*v.z+z*v.x;
	z += w*v.z-y*v.x+x*v.y;
	norm(1);
	return *this;
}

/******************************************************************************
				Utils
******************************************************************************/
#ifdef __HIGHC__
_find_t find_buffer;
char* win32_findfirst(const char* mask)
{
	if(!_dos_findfirst(mask,_A_NORMAL,&find_buffer))
		return find_buffer.name;
	else
		return 0;
}
char* win32_findnext()
{
	if(!_dos_findnext(&find_buffer))
		return find_buffer.name;
	else
		return 0;
}
#endif
