#ifdef GAME
#include "names.h"
#endif

#ifndef GAME
#include <stdio.h>
#include <stdlib.h>
#include <d3d8.h>
#else
#include "fault.h"
#include <xtl.h>
#endif
#include <string.h>


#include "floattype.h"
#include "moremath.h"

// please note that this file is shared with mkdata, thus the use
// of floatType vs. float or double

typedef floatType floatType2[2];
typedef floatType floatType3[3];
typedef floatType floatType4[4];

union FloatAndULong
{
	floatType f;
#if FLOATTYPE_BITS==32
	unsigned long      l;
#else
	unsigned long long l;
#endif
};


//--------------------------------------------------------------
// table-based sin/cos
//--------------------------------------------------------------

#if FLOATTYPE_BITS==32
#define RIGHT_JUSTIFIER_POS    (floatType)(1<<23)
#define RIGHT_JUSTIFIER_POSNEG (floatType)((1<<23)+(1<<22))
#define TABLE_ENTRY_SIZE_BITS 2
#else
#define RIGHT_JUSTIFIER_POS    (floatType)(1LL<<52)
#define RIGHT_JUSTIFIER_POSNEG (floatType)((1LL<<52)+(1LL<<51))
#define TABLE_ENTRY_SIZE_BITS 3
#endif
#define TABLE_ENTRY_SIZE (1<<TABLE_ENTRY_SIZE_BITS)

#define SIN_TABLE_BITS 10
#define SIN_TABLE_SIZE (1<<SIN_TABLE_BITS)
#define SIN_TABLE_MASK (SIN_TABLE_SIZE-1)
#define SIN_TABLE_SIZE_FLOATSIZE (SIN_TABLE_SIZE*TABLE_ENTRY_SIZE)
#define SIN_TABLE_MASK_FLOATSIZE (SIN_TABLE_MASK*TABLE_ENTRY_SIZE)
#define RADIANS_TO_KDIANS(r) ((r)*(floatType)(SIN_TABLE_SIZE/(GAME_PI*2.0)))
#define RADIANS_TO_KDIANS_FLOATSIZE(r) ((r)*(floatType)(SIN_TABLE_SIZE_FLOATSIZE/(GAME_PI*2.0)))
#define KDIANS_TO_RADIANS(k) ((k)*(floatType)((GAME_PI*2.0)/SIN_TABLE_SIZE))
#define KDIAN_COS_OFFSET (SIN_TABLE_SIZE>>2)
#define KDIAN_COS_OFFSET_FLOATSIZE (KDIAN_COS_OFFSET*TABLE_ENTRY_SIZE)
	
#define SIN_TABLE_90DEG_MASK (SIN_TABLE_MASK>>2)
#define SIN_TABLE_180DEG_MASK (SIN_TABLE_MASK>>1)
#define SIN_TABLE_90DEG_SIZE (SIN_TABLE_SIZE>>2)
#define SIN_TABLE_180DEG_SIZE (SIN_TABLE_SIZE>>1)
	
#define SIN_TABLE_90DEG_MASK_FLOATSIZE  (SIN_TABLE_90DEG_MASK *TABLE_ENTRY_SIZE)
#define SIN_TABLE_180DEG_MASK_FLOATSIZE (SIN_TABLE_180DEG_MASK*TABLE_ENTRY_SIZE)
#define SIN_TABLE_90DEG_SIZE_FLOATSIZE  (SIN_TABLE_90DEG_SIZE *TABLE_ENTRY_SIZE)
#define SIN_TABLE_180DEG_SIZE_FLOATSIZE (SIN_TABLE_180DEG_SIZE*TABLE_ENTRY_SIZE)
	
floatType sinTable[SIN_TABLE_90DEG_SIZE+1];

// note that when we scale to convert from radians to kdians,
// we also essentially shift the result left to create an offset in
// bytes so the C compiler doesn't have to shift left when it loads
// the float (usually floatTable[z] converts to floatTable+(z<<2))

#ifndef MATH_USE_ASM
floatType table_sinf(register floatType theta)
{
	FloatAndULong fl;

	fl.f = RADIANS_TO_KDIANS_FLOATSIZE(theta)+RIGHT_JUSTIFIER_POSNEG;
	register unsigned long index = fl.l;
	register unsigned long in90  = index&SIN_TABLE_90DEG_SIZE_FLOATSIZE;
	register unsigned long in180 = index&SIN_TABLE_180DEG_SIZE_FLOATSIZE;
	
	index&=SIN_TABLE_90DEG_MASK_FLOATSIZE;

	if(in90)
	{
		index=in90-index;
	}
	
	register floatType v = *(floatType *)((char *)sinTable+(index));
	
	if(in180)
	{
		v=-v;
	}
	
	return(v);
}

floatType table_cosf(floatType theta)
{
	register FloatAndULong fl;
	
	fl.f = RADIANS_TO_KDIANS_FLOATSIZE(theta)+(RIGHT_JUSTIFIER_POSNEG+KDIAN_COS_OFFSET_FLOATSIZE);
	
	register unsigned long index = fl.l;
	register unsigned long in90  = index&SIN_TABLE_90DEG_SIZE_FLOATSIZE;
	register unsigned long in180 = index&SIN_TABLE_180DEG_SIZE_FLOATSIZE;
	
	index&=SIN_TABLE_90DEG_MASK_FLOATSIZE;

	if(in90)
	{
		index=in90-index;
	}
	
	register floatType v = *(floatType *)((char *)sinTable+(index));
	
	if(in180)
	{
		v=-v;
	}
	
	return(v);
}
#endif

void InitSinTable(void)
{
	register int i;
	for(i=0;i<SIN_TABLE_SIZE/4;i++)
	{
		sinTable[i] = SIN(KDIANS_TO_RADIANS(i));
	}
	// make damned sure of the important ones
	sinTable[0] = 0.0f;
	sinTable[SIN_TABLE_SIZE/4] = 1.0f;
	table_sinf(0);
}



#if 1
float mypowf(float x,float y)
{
    float r1=1.0f,c,r2;
    float i;
#if 0
    float pf,mpf;
    pf=powf(x,y);
    printf("%.9f ",pf);
#endif
    // stupid iterative solution to get the int part of the power computed
    while(y>=1.0f)
    {
        r1*=x;
        y-=1.0f;
    }
    // covers both no-fract and zero exponent cases
    if(y==0.0f)
    {
        return(r1);
    }
    // now compute x^fract
    // first, x must be x-1.0 so that we're doing (1+x)^y -> (1+(x-1))^y -> x^y
    x-=1.0f;
    c=y*x;
    r2=1.0f+c;
    for(i=1.0f; i<30.0f; )
    {
        c*=(y-i)*x;
        i+=1.0f;
        c/=i;
        r2+=c;
    }
#if 0
    mpf=r1*r2;
    printf("%.9f ",mpf);
    for(;i<30.0f;)
    {
        c*=(y-i)*x;
        i+=1.0f;
        c/=i;
        r2+=c;
    }
    mpf=r1*r2;
    printf("%.9f\n",mpf);
#endif
    return(r1*r2);
}
#endif

#ifndef INLINE_VECTOR_SET
#ifndef MATH_USE_ASM
void V4Set(register floatType dest[3],register floatType x,register floatType y,register floatType z,register floatType w)
{
	dest[X]=x;
	dest[Y]=y;
	dest[Z]=z;
	dest[W]=w;
}

void V3Set(register floatType dest[3],register floatType x,register floatType y,register floatType z)
{
	dest[X]=x;
	dest[Y]=y;
	dest[Z]=z;
}

void V2Set(register floatType dest[2],register floatType x,register floatType y)
{
	dest[X]=x;
	dest[Y]=y;
}

void V4Set(register floatType dest[4],register floatType src[4])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
	dest[Z]=src[Z];
	dest[W]=src[W];
}

void V3Set(register floatType dest[3],register floatType src[3])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
	dest[Z]=src[Z];
}

void V2Set(register floatType dest[2],register floatType src[2])
{
	dest[X]=src[X];
	dest[Y]=src[Y];
}
#endif
#endif

void V3Set(register floatType dest[3],register short src[3])
{
	dest[0] = (floatType)src[0];
	dest[1] = (floatType)src[1];
	dest[2] = (floatType)src[2];
}

// this does V3Cross(dest,(p1-p0),(p2-p0))
void V3Cross(register floatType dest[3],register floatType p0[3],register floatType p1[3],register floatType p2[3])
{
	register floatType src1[3],src2[3];

	V3Sub(src1,p1,p0);
	V3Sub(src2,p2,p0);
	V3Cross(dest,src1,src2);
}



// does the above AND makes a unit normal out of it
void V3Norm(register floatType dest[3],register floatType p0[3],register floatType p1[3],register floatType p2[3])
{
	register floatType src1[3],src2[3];

	V3Sub(src1,p1,p0);
	V3Sub(src2,p2,p0);
	V3Cross(dest,src1,src2);
	V3Norm(dest);
}



#ifndef MATH_USE_ASM
// handy for getting 2D normals
void V2Rot90(register floatType dest[2],register floatType src[2])
{
	register floatType d1;
    d1      =  src[0];
    dest[0] = -src[1];
	dest[1] =  d1;
}
void V2RotNeg90(register floatType dest[2],register floatType src[2])
{
	register floatType d1;
    d1      = -src[0];
    dest[0] =  src[1];
	dest[1] =  d1;
}



void V3Cross(register floatType dest[3],register floatType src1[3],register floatType src2[3])
{
	register floatType d2,d1;
    d2      = src1[0]*src2[1]-src1[1]*src2[0];
    d1      = src1[2]*src2[0]-src1[0]*src2[2];
    dest[0] = src1[1]*src2[2]-src1[2]*src2[1];
	dest[1] = d1;
	dest[2] = d2;
}



// this because the cross product of two 2D values is always 0,0,something
// and so it's pretty dumb to set a 2D result (0,0)
floatType V2CrossProductZVal(register floatType src1[2],register floatType src2[2])
{
	return(src1[0]*src2[1]-src1[1]*src2[0]);
}



floatType V3Dot(register floatType v1[3],register floatType v2[3]) 
{
    return(v1[X]*v2[X] + v1[Y]*v2[Y] + v1[Z]*v2[Z]);
}


floatType V2Dot(register floatType v1[2],register floatType v2[2]) 
{
    return(v1[X]*v2[X] + v1[Y]*v2[Y]);
}


void V3Neg(register floatType dest[3],register floatType src[3]) 
{
	dest[X] = -src[X];
	dest[Y] = -src[Y];
	dest[Z] = -src[Z];
}


void V3Neg(register floatType dest[3]) 
{
	dest[X] = -dest[X];
	dest[Y] = -dest[Y];
	dest[Z] = -dest[Z];
}


void V2Neg(register floatType dest[2],register floatType src[2]) 
{
	dest[X] = -src[X];
	dest[Y] = -src[Y];
}


void V2Neg(register floatType dest[2]) 
{
	dest[X] = -dest[X];
	dest[Y] = -dest[Y];
}


void V2Mul(register floatType dest[2],register floatType src1[2],register floatType src2[2])
{
    dest[0]=src1[0]*src2[0];
    dest[1]=src1[1]*src2[1];
}



void V2Mul(register floatType dest[2],register floatType src[2],register floatType mult)
{
    dest[0]=mult*src[0];
    dest[1]=mult*src[1];
}



void V2Mul(register floatType dest[2],register floatType mult)
{
    dest[0]*=mult;
    dest[1]*=mult;
}



void V4Mul(register floatType dest[4],register floatType src1[4],register floatType src2[4])
{
    dest[0] = src1[0] * src2[0];
    dest[1] = src1[1] * src2[1];
    dest[2] = src1[2] * src2[2];
    dest[3] = src1[3] * src2[3];
}



void V4Mul(register floatType dest[4],register floatType src[4])
{
    dest[0] *= src[0];
    dest[1] *= src[1];
    dest[2] *= src[2];
    dest[3] *= src[3];
}



void V4Mul(register floatType dest[4],register floatType src[4],register floatType mult)
{
    dest[0] = mult * src[0];
    dest[1] = mult * src[1];
    dest[2] = mult * src[2];
    dest[3] = mult * src[3];
}



void V4Mul(register floatType dest[4],register floatType mult)
{
    dest[0] *= mult;
    dest[1] *= mult;
    dest[2] *= mult;
    dest[3] *= mult;
}


void V3Mul(register floatType dest[3],register floatType src1[3],register floatType src2[3])
{
    dest[0] = src1[0] * src2[0];
    dest[1] = src1[1] * src2[1];
    dest[2] = src1[2] * src2[2];
}



void V3Mul(register floatType dest[3],register floatType src[3])
{
    dest[0] *= src[0];
    dest[1] *= src[1];
    dest[2] *= src[2];
}



void V3Mul(register floatType dest[3],register floatType src[3],register floatType mult)
{
    dest[0] = mult * src[0];
    dest[1] = mult * src[1];
    dest[2] = mult * src[2];
}



void V3Mul(register floatType dest[3],register floatType mult)
{
    dest[0] *= mult;
    dest[1] *= mult;
    dest[2] *= mult;
}


void V3Div(register floatType dest[3],register floatType src[3],register floatType div)
{
	div=1/div;
    dest[0]=src[0]*div;
    dest[1]=src[1]*div;
    dest[2]=src[2]*div;
}



void V3Div(register floatType dest[3],register floatType div)
{
	div=1/div;
    dest[0]*=div;
    dest[1]*=div;
    dest[2]*=div;
}



void V2Div(register floatType dest[2],register floatType div)
{
	div=1/div;
    dest[0]*=div;
    dest[1]*=div;
}



void V3Norm(register floatType dest[3],register floatType from[3],register floatType to[3])
{
	register floatType x,y,z;
    register floatType len;
	x=to[0]-from[0];
	y=to[1]-from[1];
	z=to[2]-from[2];
    if(len=SQRT(x*x+y*y+z*z))
    {
		len=(floatType)1.0/len;
        dest[0]=x*len;
        dest[1]=y*len;
        dest[2]=z*len;
    }
    else
    {
        dest[0]=(floatType)0.0;
        dest[1]=(floatType)0.0;
        dest[2]=(floatType)1.0;
    }
}




void V3Norm(register floatType v[3])
{
    register floatType len;
    if(len=SQRT(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]))
    {
		len=(floatType)1.0/len;
        v[0]*=len;
        v[1]*=len;
        v[2]*=len;
    }
	else
    {
        v[0]=(floatType)0.0;
        v[1]=(floatType)0.0;
        v[2]=(floatType)1.0;
    }
}


void V3Norm(register floatType dest[3],register floatType src[3])
{
	register floatType len;
	if(len=SQRT(src[0]*src[0]+src[1]*src[1]+src[2]*src[2]))
	{
		len=(floatType)1.0/len;
		dest[0]=src[0]*len;
		dest[1]=src[1]*len;
		dest[2]=src[2]*len;
	}
	else
	{
		dest[0]=(floatType)0.0;
		dest[1]=(floatType)0.0;
		dest[2]=(floatType)1.0;
	}
}




void V2Norm(register floatType dest[2],register floatType from[2],register floatType to[2])
{
	register floatType x,y;
    register floatType len;
	x=to[0]-from[0];
	y=to[1]-from[1];
    if(len=SQRT(x*x+y*y))
    {
		len=(floatType)1.0/len;
        dest[0]=x*len;
        dest[1]=y*len;
    }
    else
    {
        dest[0]=(floatType)0.0;
        dest[1]=(floatType)0.0;
    }
}




void V2Norm(register floatType dest[2],register floatType src[2])
{
	register floatType len;
	if(len=SQRT(src[0]*src[0]+src[1]*src[1]))
	{
		len=(floatType)1.0/len;
		dest[0]=src[0]*len;
		dest[1]=src[1]*len;
	}
	else
	{
		dest[0]=(floatType)0.0;
		dest[1]=(floatType)1.0;
	}
}


void V2Norm(register floatType v[2])
{
    register floatType len;
    if(len=SQRT(v[0]*v[0]+v[1]*v[1]))
    {
		len=(floatType)1.0/len;
        v[0]*=len;
        v[1]*=len;
    }
	else
    {
        v[0]=(floatType)0.0;
        v[1]=(floatType)1.0;
    }
}



void V3Sub(register floatType dest[3],register floatType src1[3],register floatType src2[3])
{
    dest[0]=src1[0]-src2[0];
    dest[1]=src1[1]-src2[1];
    dest[2]=src1[2]-src2[2];
}



void V3Sub(register floatType dest[3],register floatType src[3])
{
    dest[0]-=src[0];
    dest[1]-=src[1];
    dest[2]-=src[2];
}



void V2Sub(register floatType dest[2],register floatType src1[2],register floatType src2[2])
{
    dest[0]=src1[0]-src2[0];
    dest[1]=src1[1]-src2[1];
}



void V2Sub(register floatType dest[3],register floatType src[3])
{
    dest[0]-=src[0];
    dest[1]-=src[1];
}



void V3Add(register floatType dest[3],register floatType src1[3],register floatType src2[3])
{
    dest[0]=src1[0]+src2[0];
    dest[1]=src1[1]+src2[1];
    dest[2]=src1[2]+src2[2];
}
void V3Add(register floatType dest[3],register floatType src[3])
{
    dest[0]+=src[0];
    dest[1]+=src[1];
    dest[2]+=src[2];
}

void V2Add(register floatType dest[2],register floatType src1[2],register floatType src2[2])
{
    dest[0]=src1[0]+src2[0];
    dest[1]=src1[1]+src2[1];
}
void V2Add(register floatType dest[2],register floatType src[2])
{
    dest[0]+=src[0];
    dest[1]+=src[1];
}




void V4Lerp(register floatType dest[4],register floatType src1[4],register floatType src2[4],register floatType l)
{
    dest[0]=(src1[0]-src2[0])*l+src2[0];
    dest[1]=(src1[1]-src2[1])*l+src2[1];
    dest[2]=(src1[2]-src2[2])*l+src2[2];
    dest[3]=(src1[3]-src2[3])*l+src2[3];
}

void V3Lerp(register floatType dest[3],register floatType src1[3],register floatType src2[3],register floatType l)
{
    dest[0]=(src1[0]-src2[0])*l+src2[0];
    dest[1]=(src1[1]-src2[1])*l+src2[1];
    dest[2]=(src1[2]-src2[2])*l+src2[2];
}

void V2Lerp(register floatType dest[2],register floatType src1[2],register floatType src2[2],register floatType l)
{
    dest[0]=(src1[0]-src2[0])*l+src2[0];
    dest[1]=(src1[1]-src2[1])*l+src2[1];
}



void V3Mac(register floatType dest[3],floatType src1[3],floatType src2[3],register floatType s)
{
    dest[0]=src1[0]+src2[0]*s;
    dest[1]=src1[1]+src2[1]*s;
    dest[2]=src1[2]+src2[2]*s;
}



void V3Mac(register floatType dest[3],register floatType src[3],register floatType s)
{
    dest[0]+=src[0]*s;
    dest[1]+=src[1]*s;
    dest[2]+=src[2]*s;
}



void V2Mac(register floatType dest[2],register floatType src1[2],register floatType src2[2],register floatType s)
{
    dest[0]=src1[0]+src2[0]*s;
    dest[1]=src1[1]+src2[1]*s;
}



void V2Mac(register floatType dest[2],register floatType src[2],register floatType s)
{
    dest[0]+=src[0]*s;
    dest[1]+=src[1]*s;
}






void V3Avg(register floatType dest[3],register floatType src1[3],register floatType src2[3])
{
    dest[0]=(src1[0]+src2[0])*(floatType)0.5;
    dest[1]=(src1[1]+src2[1])*(floatType)0.5;
    dest[2]=(src1[2]+src2[2])*(floatType)0.5;
}



void V2Avg(register floatType dest[2],register floatType src1[2],register floatType src2[2])
{
    dest[0]=(src1[0]+src2[0])*(floatType)0.5;
    dest[1]=(src1[1]+src2[1])*(floatType)0.5;
}



// this is now doneinline
// void VectorZero(register floatType dest[3])
// {
//     dest[0]=dest[1]=dest[2]=(floatType)0.0;
// }




floatType V3Dist(register floatType v1[3],register floatType v2[3])
{
    register floatType x,y,z;
    x=v1[0]-v2[0];
    y=v1[1]-v2[1];
    z=v1[2]-v2[2];
    return(SQRT(x*x+y*y+z*z));
}

floatType V2Dist(register floatType v1[2],register floatType v2[2])
{
	register floatType x,y;
	x=v1[0]-v2[0];
	y=v1[1]-v2[1];
	return(SQRT(x*x+y*y));
}



floatType V3DistSq(register floatType v1[3],register floatType v2[3])
{
    register floatType x,y,z;
    x=v1[0]-v2[0];
    y=v1[1]-v2[1];
    z=v1[2]-v2[2];
    return(x*x+y*y+z*z);
}


floatType V2DistSq(register floatType v1[2],register floatType v2[2])
{
    register floatType x,y;
    x=v1[0]-v2[0];
    y=v1[1]-v2[1];
    return(x*x+y*y);
}



floatType V3Len(register floatType v[3])
{
    return(SQRT(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]));
}

floatType V2Len(register floatType v[3])
{
    return(SQRT(v[0]*v[0]+v[1]*v[1]));
}

floatType V3LenSq(register floatType v[3])
{
    return(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

floatType V2LenSq(register floatType v[3])
{
    return(v[0]*v[0]+v[1]*v[1]);
}

#endif






void V3Rot(register floatType dest[3],floatType angle,register floatType axis[3])
{
    register floatType s = SIN(angle);
    register floatType c = COS(angle);
    register floatType t = (floatType)1.0-c;

	register floatType ax = axis[X];
	register floatType ay = axis[Y];
	register floatType az = axis[Z];
	
	register floatType taxay  = t*ax;
	register floatType taxaz  = taxay*az;
	taxay *= ay;
	register floatType tayaz  = t*ay*az;

	register floatType sax = s*ax;
	register floatType say = s*ay;
	register floatType saz = s*az;
	
	register floatType x = dest[X];
	register floatType y = dest[Y];
	register floatType z = dest[Z];
	
    dest[X] =
        x*(t*ax*ax + c  ) +
        y*(taxay   + saz) +
        z*(taxaz   - say);
    
    dest[Y] =
        x*(taxay   - saz) +
        y*(t*ay*ay + c  ) +
        z*(tayaz   + sax);
    
    dest[Z] =
        x*(taxaz   + say) +
        y*(tayaz   - sax) +
        z*(t*az*az + c  );
}


void V2Rot(register floatType dest[2],floatType angle)
{
    register floatType s,c,x,y;
	
    s = SIN(angle);
    c = COS(angle);

	x = dest[X];
	y = dest[Y];
	
    dest[X] = x*c + y*s;
    dest[Y] = y*c - x*s;
}


// some matrix stuff
void V3Mul4x4(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x,y,z,w;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	w = (floatType)1.0 / (x*src2[0][3] + y*src2[1][3] + z*src2[2][3] + src2[3][3]);
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0]) * w;
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1]) * w;
	dest[2] = (x*src2[0][2] + y*src2[1][2] + z*src2[2][2] + src2[3][2]) * w;
}



void V3Mul4x4_XYZW(register floatType dest[4],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x,y,z;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0]);
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1]);
	dest[2] = (x*src2[0][2] + y*src2[1][2] + z*src2[2][2] + src2[3][2]);
	dest[3] = (x*src2[0][3] + y*src2[1][3] + z*src2[2][3] + src2[3][3]);
}



void VS3Mul4x4_XYZW(register floatType dest[4],register short src1[3],register floatType src2[4][4])
{
	register floatType x,y,z;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0]);
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1]);
	dest[2] = (x*src2[0][2] + y*src2[1][2] + z*src2[2][2] + src2[3][2]);
	dest[3] = (x*src2[0][3] + y*src2[1][3] + z*src2[2][3] + src2[3][3]);
}



void V3Mul4x4_XY(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x,y,z,w;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	
	if(w = (x*src2[0][3] + y*src2[1][3] + z*src2[2][3] + src2[3][3]))
	{
		w = (floatType)1.0 / w;
	}
	// else leave it 0
	
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0]) * w;
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1]) * w;
}



void V3Mul4x3_XY(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x,y,z;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0]);
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1]);
}



void V3Mul3x4(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x,y,z,w;
	x = src1[0];
	y = src1[1];
	z = src1[2];
	w = (floatType)1.0 / (x*src2[0][3] + y*src2[1][3] + z*src2[2][3] + src2[3][3]);
	dest[0] = (x*src2[0][0] + y*src2[1][0] + z*src2[2][0]) * w;
	dest[1] = (x*src2[0][1] + y*src2[1][1] + z*src2[2][1]) * w;
	dest[2] = (x*src2[0][2] + y*src2[1][2] + z*src2[2][2]) * w;
}



void V3Mul4x3(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x=src1[0],y=src1[1],z=src1[2];
	dest[0] = x*src2[0][0] + y*src2[1][0] + z*src2[2][0] + src2[3][0];
	dest[1] = x*src2[0][1] + y*src2[1][1] + z*src2[2][1] + src2[3][1];
	dest[2] = x*src2[0][2] + y*src2[1][2] + z*src2[2][2] + src2[3][2];
}



// 3x3: get into world orientation from local orientation
void V3Mul3x3(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x=src1[0],y=src1[1],z=src1[2];
	dest[0] = x*src2[0][0] + y*src2[1][0] + z*src2[2][0];
	dest[1] = x*src2[0][1] + y*src2[1][1] + z*src2[2][1];
	dest[2] = x*src2[0][2] + y*src2[1][2] + z*src2[2][2];
}



// 3x3 transposed: good for getting into local orientation,
// as long as mtx is orthonormal
void V3Mul3x3T(register floatType dest[3],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x=src1[0],y=src1[1],z=src1[2];
	dest[0] = x*src2[0][0] + y*src2[0][1] + z*src2[0][2];
	dest[1] = x*src2[1][0] + y*src2[1][1] + z*src2[1][2];
	dest[2] = x*src2[2][0] + y*src2[2][1] + z*src2[2][2];
}



void V4Plane(floatType *dest, floatType *src1, floatType *src2, floatType *src3)
{
	V3Cross(dest, src1, src2, src3);
	V3Norm(dest);
	dest[3] = V3Dot(dest, src1);
}



void V3Mul3x3_XY(register floatType dest[2],register floatType src1[3],register floatType src2[4][4])
{
	register floatType x=src1[0],y=src1[1],z=src1[2];
	dest[0] = x*src2[0][0] + y*src2[1][0] + z*src2[2][0];
	dest[1] = x*src2[0][1] + y*src2[1][1] + z*src2[2][1];
}



// TODO - REWRITE THIS FUCKING THING
void M4Mul(register floatType dest[4][4],register floatType src1[4][4],register floatType src2[4][4])
{
	register int i,j;
	if(dest!=src1 && dest!=src2)
	{
		for(i=0; i<4 ;i++)
		{
			for(j=0; j<4; j++)
			{
				dest[i][j]  = src1[i][0]*src2[0][j]
				            + src1[i][1]*src2[1][j]
				            + src1[i][2]*src2[2][j]
				            + src1[i][3]*src2[3][j];
			}
		}
	}
	else
	{
		register floatType m[4][4];
		for(i=0; i<4; i++)
		{
			for(j=0; j<4; j++)
			{
				m[i][j]  = src1[i][0]*src2[0][j]
				         + src1[i][1]*src2[1][j]
				         + src1[i][2]*src2[2][j]
				         + src1[i][3]*src2[3][j];
			}
		}
		M4Set(dest, m);
	}
}



void M4Mul4x3(register floatType dest[4][4],register floatType src1[4][4],register floatType src2[4][4])
{
	if(dest==src2)
	{
		M4PreMul4x3(dest, src1);
	}
	else
	{
		register int i,j;
		register floatType m[4][4];
		for(i=0; i<4; i++)
		{
			for(j=0; j<3; j++)
			{
				m[i][j]  = src1[i][0]*src2[0][j]
				         + src1[i][1]*src2[1][j]
				         + src1[i][2]*src2[2][j]
				         + src1[i][3]*src2[3][j];
			}
		}
		M4Set(dest, m);
	}
}



void M4Ident(register floatType dest[4][4])
{
	dest[0][0] = 1.0f;
	dest[0][1] = 0.0f;
	dest[0][2] = 0.0f;
	dest[0][3] = 0.0f;

	dest[1][0] = 0.0f;
	dest[1][1] = 1.0f;
	dest[1][2] = 0.0f;
	dest[1][3] = 0.0f;

	dest[2][0] = 0.0f;
	dest[2][1] = 0.0f;
	dest[2][2] = 1.0f;
	dest[2][3] = 0.0f;

	dest[3][0] = 0.0f;
	dest[3][1] = 0.0f;
	dest[3][2] = 0.0f;
	dest[3][3] = 1.0f;
}



void M4Mul(register floatType dest[4][4],register floatType src[4][4])
{
	register floatType x,y,z,w;
	register floatType4 *destRowThree = (floatType4 *)dest + 3;
	
	for(;;)
	{
		x=dest[0][0];
		y=dest[0][1];
		z=dest[0][2];
		w=dest[0][3];
		dest[0][0] = x*src[0][0] + y*src[1][0] + z*src[2][0] + w*src[3][0];
		dest[0][1] = x*src[0][1] + y*src[1][1] + z*src[2][1] + w*src[3][1];
		dest[0][2] = x*src[0][2] + y*src[1][2] + z*src[2][2] + w*src[3][2];
		dest[0][3] = x*src[0][3] + y*src[1][3] + z*src[2][3] + w*src[3][3];
		if(dest==destRowThree)
		{
			return;
		}
		dest = (floatType4 *)dest + 1;
	}
}



void M4Mul4x3(register floatType dest[4][4],register floatType src[4][4])
{
	register floatType x,y,z,xo,yo,zo;
	register floatType4 *destRowThree = (floatType4 *)dest + 3;

	for(;;)
	{
		x=dest[0][0];
		y=dest[0][1];
		z=dest[0][2];
		
		xo = x*src[0][0] + y*src[1][0] + z*src[2][0];
		yo = x*src[0][1] + y*src[1][1] + z*src[2][1];
		zo = x*src[0][2] + y*src[1][2] + z*src[2][2];
		
		if(dest==destRowThree)
		{
			dest[0][0] = xo + src[3][0];
			dest[0][1] = yo + src[3][1];
			dest[0][2] = zo + src[3][2];
			return;
		}
		
		dest[0][0] = xo;
		dest[0][1] = yo;
		dest[0][2] = zo;
		
		dest = (floatType4 *)dest+1;
	}
}



void M4PreMul4x3(register floatType dest[4][4],register floatType src1[4][4])
{
	// src2 and dest are the same, ignore src2 from now on
	register floatType4 *destColTwo = (floatType4 *)((float *)dest + 2);
	for(;;)
	{
		floatType x = dest[0][0];
		floatType y = dest[1][0];
		floatType z = dest[2][0];

		dest[0][0]  = x*src1[0][0] + y*src1[0][1] + z*src1[0][2];
		dest[1][0]  = x*src1[1][0] + y*src1[1][1] + z*src1[1][2];
		dest[2][0]  = x*src1[2][0] + y*src1[2][1] + z*src1[2][2];
		dest[3][0] += x*src1[3][0] + y*src1[3][1] + z*src1[3][2];  // = xx + yy + zz + w;

		if(dest==destColTwo)
		{
			return;
		}

		// move to next column
		dest = (floatType4 *)((floatType *)dest + 1);
	}
}



void M4Mul3x3(register floatType dest[4][4],register floatType src1[4][4],register floatType src2[4][4])
{
	// not the world's most efficient func
	register floatType m[4][4];
	register int i,j;
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			m[i][j]  = src1[i][0]*src2[0][j];
			m[i][j] += src1[i][1]*src2[1][j];
			m[i][j] += src1[i][2]*src2[2][j];
		}
	}
	for(i=0;i<3;i++)
	{
		for(j=0;j<3;j++)
		{
			dest[i][j] = m[i][j];
		}
	}
}



#ifndef MATH_USE_ASM
void M4Scale3x3(register floatType dest[4][4],register floatType scale)
{
	dest[0][0]*=scale;
	dest[0][1]*=scale;
	dest[0][2]*=scale;
	dest[1][0]*=scale;
	dest[1][1]*=scale;
	dest[1][2]*=scale;
	dest[2][0]*=scale;
	dest[2][1]*=scale;
	dest[2][2]*=scale;
}



void M4Transpose(register floatType dest[4][4],register floatType src[4][4])
{
	// note that we interleave here so that we can follow dest=t with t=src
	// and not get a MIPS pipeline stall.
	register floatType t1,t2;
	
	#define MSWAP1(i,j) t1=src[i][j]; dest[i][j]=src[j][i]; dest[j][i]=t1;
	#define MSWAP2(i,j) t2=src[i][j]; dest[i][j]=src[j][i]; dest[j][i]=t2;
	
	MSWAP1(0,1);
	MSWAP2(0,2);
	MSWAP1(0,3);
	MSWAP2(1,2);
	MSWAP1(1,3);
	MSWAP2(2,3);

	if(src!=dest)
	{
		// copy the rest
		dest[0][0] = src[0][0];
		dest[1][1] = src[1][1];
		dest[2][2] = src[2][2];
		dest[3][3] = src[3][3];
	}
}



void M4Transpose3x3(register floatType dest[4][4],register floatType src[4][4])
{
	// note that we interleave here so that we can follow dest=t with t=src
	// and not get a MIPS pipeline stall.
	register floatType t1,t2;
	
	#define MSWAP1(i,j) t1=src[i][j]; dest[i][j]=src[j][i]; dest[j][i]=t1;
	#define MSWAP2(i,j) t2=src[i][j]; dest[i][j]=src[j][i]; dest[j][i]=t2;
	
	MSWAP1(0,1);
	MSWAP2(0,2);
	MSWAP1(1,2);

	if(src!=dest)
	{
		// copy the rest
		dest[0][0] = src[0][0];
		dest[0][3] = src[0][3];
		dest[1][1] = src[1][1];
		dest[1][3] = src[1][3];
		dest[2][2] = src[2][2];
		dest[2][3] = src[2][3];
		dest[3][0] = src[3][0];
		dest[3][1] = src[3][1];
		dest[3][2] = src[3][2];
		dest[3][3] = src[3][3];
	}
}



void M4Set(register floatType dest[4][4],register floatType src[4][4])
{
	memcpy(dest,src,sizeof(dest[0][0])*4*4);
}

void M4ViewFromOriginCamera(register floatType vm[4][4],register floatType cm[4][4])
{
	// we can convert camera to viewing matrix without using guLookAt
	// because the guLookAt just returns a rearranged version of the
	// camera matrix anyway.

	vm[0][0] = -cm[1][0];
	vm[0][1] =  cm[2][0];
	vm[0][2] = -cm[0][0];
	vm[0][3] =    0.0f;
	vm[1][0] = -cm[1][1];
	vm[1][1] =  cm[2][1];
	vm[1][2] = -cm[0][1];
	vm[1][3] =    0.0f;
	vm[2][0] = -cm[1][2];
	vm[2][1] =  cm[2][2];
	vm[2][2] = -cm[0][2];
	vm[2][3] =    0.0f;
#ifdef ADJUST_FOR_CAMERA
	vm[3][0] =    0.0f;
	vm[3][1] =    0.0f;
	vm[3][2] =    0.0f;
#else
	vm[3][0] =  V3Dot(cm[1],cm[3]);
	vm[3][1] = -V3Dot(cm[2],cm[3]);
	vm[3][2] =  V3Dot(cm[0],cm[3]);
#endif
	vm[3][3] =    1.0f;
}
#endif


void M4FromXYZRot(register floatType dest[4][4],register floatType rx,register floatType ry,register floatType rz)
{
	register floatType crx, cry, crz;
	register floatType srx, sry, srz;
	register floatType crxsrz, crxcrz;
	register floatType srxsrz, srxcrz;

	// I wonder if doing srx = SQRT(1.0-crx) would be faster here...
	if(rx)
	{
		crx = COS(rx);
		srx = SIN(rx);
	}
	else
	{
		crx = 1.0f;
		srx = 0.0f;
	}
	if(ry)
	{
		cry = COS(ry);
		sry = SIN(ry);
	}
	else
	{
		cry = 1.0f;
		sry = 0.0f;
	}
	if(rz)
	{
		crz = COS(rz);
		srz = SIN(rz);
	}
	else
	{
		crz = 1.0f;
		srz = 0.0f;
	}

	dest[0][0] = cry*crz;
	dest[0][1] = cry*srz;
	dest[0][2] = -sry;
	dest[0][3] = (floatType)0.0;
	
	crxsrz = crx*srz;
	crxcrz = crx*crz;
	srxsrz = srx*srz;
	srxcrz = srx*crz;

	dest[1][0] = srxcrz*sry - crxsrz;
	dest[1][1] = srxsrz*sry + crxcrz;
	dest[1][2] = srx*cry;
	dest[1][3] = (floatType)0.0;

	dest[2][0] = crxcrz*sry + srxsrz;
	dest[2][1] = crxsrz*sry - srxcrz;
	dest[2][2] = crx*cry;
	dest[2][3] = (floatType)0.0;
	
	dest[3][0] = (floatType)0.0;
	dest[3][1] = (floatType)0.0;
	dest[3][2] = (floatType)0.0;
	dest[3][3] = (floatType)1.0;
}



void M4FromYZXRot(register floatType dest[4][4],register floatType rx,register floatType ry,register floatType rz)
{
#if 1
	// TODO - make fast n stuff
	floatType tmp[4][4];	
	M4FromXYZRot(dest, 0.0f, ry,   rz);
	M4FromXYZRot(tmp,  rx,   0.0f, 0.0f);
	M4Mul4x3(dest, tmp);
#else
	register floatType crx, cry, crz;
	register floatType srx, sry, srz;
	register floatType crxsrz, crxcrz;
	register floatType srxsrz, srxcrz;

	// I wonder if doing srx = SQRT(1.0-crx) would be faster here...
	if(rx)
	{
		crx = COS(rx);
		srx = SIN(rx);
	}
	else
	{
		crx = 1.0f;
		srx = 0.0f;
	}
	if(ry)
	{
		cry = COS(ry);
		sry = SIN(ry);
	}
	else
	{
		cry = 1.0f;
		sry = 0.0f;
	}
	if(rz)
	{
		crz = COS(rz);
		srz = SIN(rz);
	}
	else
	{
		crz = 1.0f;
		srz = 0.0f;
	}

	dest[0][0] = cry*crz;
	dest[0][1] = cry*srz;
	dest[0][2] = -sry;
	dest[0][3] = (floatType)0.0;
	
	crxsrz = crx*srz;
	crxcrz = crx*crz;
	srxsrz = srx*srz;
	srxcrz = srx*crz;

	dest[1][0] = srxcrz*sry - crxsrz;
	dest[1][1] = srxsrz*sry + crxcrz;
	dest[1][2] = srx*cry;
	dest[1][3] = (floatType)0.0;

	dest[2][0] = crxcrz*sry + srxsrz;
	dest[2][1] = crxsrz*sry - srxcrz;
	dest[2][2] = crx*cry;
	dest[2][3] = (floatType)0.0;
	
	dest[3][0] = (floatType)0.0;
	dest[3][1] = (floatType)0.0;
	dest[3][2] = (floatType)0.0;
	dest[3][3] = (floatType)1.0;
#endif
}



#if 0
// TODO - delete this code, it's specific to Alias
void M4MakeJoint(register floatType m[4][4],
				 register floatType scale,
				 floatType translateIn[3],
				 register floatType rotate[3],
				 floatType translateOut[3])
{
	// hack scale,translateIn for now

// 	scale=0.821184f;
// 	translateIn[X] = -5.835590f ;
// 	translateIn[Y] = -8.090000f;
// 	translateIn[Z] = -0.506314f;
// 	rotate[X] = -119.800000f*GAME_PI/180.0f;
// 	rotate[Y] =   89.700000f*GAME_PI/180.0f;
// 	rotate[Z] =  -83.600000f*GAME_PI/180.0f;
// 	translateOut[X] = -0.598111f;
// 	translateOut[Y] =  5.198980f;
// 	translateOut[Z] =  3.841100f;
	
	m[0][0] = scale; m[0][1] = 0.0f;  m[0][2] = 0.0f;  m[0][3] = 0.0f;
	m[1][0] = 0.0f;  m[1][1] = scale; m[1][2] = 0.0f;  m[1][3] = 0.0f;
	m[2][0] = 0.0f;  m[2][1] = 0.0f;  m[2][2] = scale; m[2][3] = 0.0f;
	m[3][0] = translateIn[X];
	m[3][1] = translateIn[Y];
	m[3][2] = translateIn[Z];                          m[3][3] = 1.0f;
	
	// do rotate
	floatType mrt[4][4];
	if(rotate[X] || rotate[Y] || rotate[Z])
	{
		M4FromXYZRot(mrt,rotate[X],rotate[Y],rotate[Z]);
//	printf("rotate %.3f %.3f %.3f\n",rotate[X],rotate[Y],rotate[Z]);
// 	// redo rotate
// 	floatType rx[4][4];
// 	floatType ry[4][4];
// 	floatType rz[4][4];
// 	guRotateF(rx,ry,rz)
	
		// do translateOut
		mrt[POSITION][X]=translateOut[X];
		mrt[POSITION][Y]=translateOut[Y];
		mrt[POSITION][Z]=translateOut[Z];
	
//[0.821184 0.821184 0.821184] [-5.83559 -8.09 -0.506314] [-119.8 89.7 -83.6] [-0.598111 5.19898 3.8411]
// ---------------------------------------
// flag
// sp: -0.598112 5.198979 3.841095
// rp: -0.598111 5.198979 3.841096

// si: -7.106315 -9.851623 -0.616566
// s:  0.821184 0.821184 0.821184
// so: 7.106315 9.851623 0.616566
// ri: -7.106315 -9.851623 -0.616566
// r:  -119.800000 89.700000 -83.600000
// ro: 7.106315 9.851623 0.616566
// t:  -7.704427 -4.652643 3.224530

// 0:  0.000479 -0.004273 -0.821173
// 1:  -0.484995 0.662653 -0.003731
// 2:  0.662664 0.484990 -0.002137
// 3:  3.767894 -1.597895 9.714684

// 0:  0.000479 -0.004273 -0.821173
// 1:  -0.484995 0.662653 -0.003731
// 2:  0.662664 0.484990 -0.002137
// 3:  3.767894 -1.597895 9.714684

//	scale
// 0:  0.821184 0.000000 0.000000
// 1:  0.000000 0.821184 0.000000
// 2:  0.000000 0.000000 0.821184
// 3:  -5.835594 -8.089997 -0.506314

//  rotate
// 0:  0.000584 -0.005203 -0.999986
// 1:  -0.590604 0.806948 -0.004544
// 2:  0.806961 0.590599 -0.002602
// 3:  -0.598111 5.198980 3.841096
// ---------------------------------------
// 	static int printit=1;
// 			if(printit)
// 			{
// 				printf("-----------------------------\n");
// 				printf("[%f %f %f] [%f %f %f] [%f %f %f] [%f %f %f]\n",
// 					   scale,scale,scale,
// 					   translateIn[X],translateIn[Y],translateIn[Z],
// 					   rotate[X],rotate[Y],rotate[Z],
// 					   translateOut[X],translateOut[Y],translateOut[Z]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[0][0],m[0][1],m[0][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[1][0],m[1][1],m[1][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[2][0],m[2][1],m[2][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[3][0],m[3][1],m[3][2]);
// 				printf("\n");
// 				printf("%.6f %.6f %.6f %.6f\n",mrt[0][0],mrt[0][1],mrt[0][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",mrt[1][0],mrt[1][1],mrt[1][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",mrt[2][0],mrt[2][1],mrt[2][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",mrt[3][0],mrt[3][1],mrt[3][2]);
// 			}
				
	M4Mul(m,mrt);
	}
	else
	{
		// do translateOut
		m[POSITION][X]+=translateOut[X];
		m[POSITION][Y]+=translateOut[Y];
		m[POSITION][Z]+=translateOut[Z];
	}
// 			if(printit)
// 			{
// 				printf("\n");
// 				printf("%.6f %.6f %.6f %.6f\n",m[0][0],m[0][1],m[0][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[1][0],m[1][1],m[1][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[2][0],m[2][1],m[2][2]);
// 				printf("%.6f %.6f %.6f %.6f\n",m[3][0],m[3][1],m[3][2]);
// 				printit=0;
// 			}
}

void M4MakeJointWeighted(floatType m[4][4],
							  floatType scale,
							  floatType translateIn[3],
							  floatType rotate[3],
							  floatType translateOut[3],
							  floatType weight)
{
	if(weight==(floatType)1.0)
	{
		M4MakeJoint(m,scale,translateIn,rotate,translateOut);
	}
	else
	{
		floatType newTranslateIn[3];
		floatType newRotate[3];
		floatType newTranslateOut[3];
		scale*=weight;
		V3Mul(newTranslateIn,translateIn,weight);
		V3Mul(newRotate,rotate,weight);
		V3Mul(newTranslateOut,translateOut,weight);
		M4MakeJoint(m,scale,newTranslateIn,newRotate,newTranslateOut);
	}
}
#endif



/****
 *
 * affine_matrix4_inverse
 *
 * Computes the inverse of a 3D affine matrix; i.e. a matrix with a dimen-
 * sionality of 4 where the right column has the entries (0, 0, 0, 1).
 *
 * This procedure treats the 4 by 4 matrix as a block matrix and
 * calculates the inverse of one submatrix for a significant perform-
 * ance improvement over a general procedure that can invert any non-
 * singular matrix:
 *          --        --          --          --
 *          |          | -1       |    -1      |
 *          | A      0 |          |   A      0 |
 *    -1    |          |          |            |
 *   M   =  |          |     =    |     -1     |
 *          | C      1 |          | -C A     1 |
 *          |          |          |            |
 *          --        --          --          --
 *
 *  where     M is a 4 by 4 matrix,
 *            A is the 3 by 3 upper left submatrix of M,
 *            C is the 1 by 3 lower left submatrix of M.
 *
 * Input:
 *   in   - 3D affine matrix
 *
 * Output:
 *   out  - inverse of 3D affine matrix
 *
 * Returned value:
 *   TRUE   if input matrix is nonsingular
 *   FALSE  otherwise
 *
 ***/

int M4Inv(floatType out[4][4],floatType in[4][4])
{
    register  floatType    det_1;
              floatType    pos, neg, temp, absDet;

#define ACCUMULATE    \
    if (temp >= (floatType)0.0)  \
        pos += temp;  \
    else              \
        neg += temp;

#define PRECISION_LIMIT (floatType)(1.0e-15)

    /*
     * Calculate the determinant of submatrix A and determine if the
     * the matrix is singular as limited by the double precision
     * floating-point data representation.
     */
    pos = neg = (floatType)0.0;
    temp =  in[0][0] * in[1][1] * in[2][2];
    ACCUMULATE
    temp =  in[0][1] * in[1][2] * in[2][0];
    ACCUMULATE
    temp =  in[0][2] * in[1][0] * in[2][1];
    ACCUMULATE
    temp = -in[0][2] * in[1][1] * in[2][0];
    ACCUMULATE
    temp = -in[0][1] * in[1][0] * in[2][2];
    ACCUMULATE
    temp = -in[0][0] * in[1][2] * in[2][1];
    ACCUMULATE
    det_1 = pos + neg;

    /* Is the submatrix A singular? */
	if((absDet = (det_1 / (pos - neg))) < (floatType)0.0)
	{
		absDet=-absDet;
	}
	
    if ((det_1 == (floatType)0.0) || (absDet < PRECISION_LIMIT)) {

        /* Matrix M has no inverse */
#ifdef GAME
		PrintCustomFault("M4Inverse: singular matrix");
        return(0);
#else
        printf("M4Inverse: singular matrix\n");
		for(;;);
#endif
    }
    else {

        /* Calculate inverse(A) = adj(A) / det(A) */
        det_1 = (floatType)1.0 / det_1;
        out[0][0] =   ( in[1][1] * in[2][2] -
						in[1][2] * in[2][1] ) * det_1;
        out[1][0] = - ( in[1][0] * in[2][2] -
						in[1][2] * in[2][0] ) * det_1;
        out[2][0] =   ( in[1][0] * in[2][1] -
						in[1][1] * in[2][0] ) * det_1;
        out[0][1] = - ( in[0][1] * in[2][2] -
						in[0][2] * in[2][1] ) * det_1;
        out[1][1] =   ( in[0][0] * in[2][2] -
						in[0][2] * in[2][0] ) * det_1;
        out[2][1] = - ( in[0][0] * in[2][1] -
						in[0][1] * in[2][0] ) * det_1;
        out[0][2] =   ( in[0][1] * in[1][2] -
						in[0][2] * in[1][1] ) * det_1;
        out[1][2] = - ( in[0][0] * in[1][2] -
						in[0][2] * in[1][0] ) * det_1;
        out[2][2] =   ( in[0][0] * in[1][1] -
						in[0][1] * in[1][0] ) * det_1;
		
        /* Calculate -C * inverse(A) */
        out[3][0] = - ( in[3][0] * out[0][0] +
						in[3][1] * out[1][0] +
						in[3][2] * out[2][0] );
        out[3][1] = - ( in[3][0] * out[0][1] +
						in[3][1] * out[1][1] +
						in[3][2] * out[2][1] );
        out[3][2] = - ( in[3][0] * out[0][2] +
						in[3][1] * out[1][2] +
						in[3][2] * out[2][2] );

        /* Fill in last column */
        out[0][3] = out[1][3] = out[2][3] = (floatType)0.0;
        out[3][3] = (floatType)1.0;

        return(1);
    }
}


void M4OrthoInv(floatType out[4][4],floatType in[4][4])
{
	M4Transpose3x3(out,in);
	V3Neg(out[POSITION]);
	V3Mul3x3(out[POSITION],out[POSITION],out);
}


floatType zeroVector[4] =
{
	(floatType)0.0,
	(floatType)0.0,
	(floatType)0.0,
	(floatType)0.0
};

#ifndef MATH_USE_ASM
void M4Align(register floatType mf[4][4],register floatType up[3],register floatType forward[3])
{
	M4Align(mf,up,forward,zeroVector);
}

void M4Align(register floatType mf[4][4],register floatType up[3],register floatType target[3],register floatType pos[3])
{
	mf[FORWARD] [3] = (floatType)0.0;
	mf[SIDE]    [3] = (floatType)0.0;
	mf[UP]      [0] = up[X];
	mf[UP]      [1] = up[Y];
	mf[UP]      [2] = up[Z];
	mf[UP]      [3] = (floatType)0.0;
	mf[POSITION][0] = pos[X];
	mf[POSITION][1] = pos[Y];
	mf[POSITION][2] = pos[Z];
	mf[POSITION][3] = (floatType)1.0;

	V3Norm(mf[FORWARD],pos,target);
	V3Cross(mf[SIDE],mf[UP],mf[FORWARD]);
	
	register floatType len = V3Len(mf[SIDE]);
	if(!len)
	{
		// yucky code to get a vector that isn't colinear
		register floatType pt5 = (floatType)0.5;
		register floatType one = (floatType)1.0;
		V3Zero(mf[FORWARD]);
		if     (mf[UP][X] < pt5)
		{
			mf[FORWARD][X] = one;
		}
		else if(mf[UP][Y] < pt5)
		{
			mf[FORWARD][Y] = one;
		}
		else
		{
			mf[FORWARD][Z] = one;
		}
		V3Cross(mf[SIDE],mf[UP],mf[FORWARD]);
		len = V3Len(mf[SIDE]);
	}
	// if len somehow still==0.0, we will end up with a yucky matrix,
	// but that's better than something with an inf in it.
	if(len)
	{
		V3Div(mf[SIDE],len);
	}
	// we assume unit up vector, and we just made side a unit, so we can also
	// assume that forward out of this orthogonal cross product will be a unit.
	V3Cross(mf[FORWARD],mf[SIDE],mf[UP]);
}
#endif



floatType CoarseAngle(register floatType x,register floatType y)
{
	register floatType s,position,step,angle=(floatType)0.0,temp;
	register int i,add=1;
	
	// down to half the world
	if(y<0)
	{
		angle+=(floatType)GAME_PI;
		x=-x;
		y=-y;
	}
	
	// down to a quarter of the world
	if(x<0)
	{
		angle+=(floatType)(GAME_PI/2.0);
		temp=x;
		x=y;
		y=-temp;
	}

	// down to an octant
	if(x<y)
	{
		angle+=(floatType)(GAME_PI/4.0);
		
		temp=x;
		x=y;
		y=temp;
		
		add=0;
	}

	// normalize so that y = sin(angle)
	// note that we sacrifice x here to be a temp var.  this
	// saves on register vars.
	
	if(!(x = SQRT(y*y+x*x)))
	{
		return((floatType)0.0);
	}
	y/=x;

	// a little binary search to find theta such that y ~= sin(theta).
	// we start in the middle of the octant (22.5 degrees)
	// and step by half of that.
	
	position = (floatType)(GAME_PI/8.0);
	step = position*(floatType)0.5;

	// do up to 16 iterations
	for(i=0;i<16;i++)
	{
		// it's coarse anyway, might as well be fast too
		s=table_sinf(position);
		
		if(s < y)
		{
			// close enough
			if(y-s < (floatType)0.0025)
			{
//				printf("@ %d\n",i);
				break;
			}
			
			position+=step;
		}
		else if(s > y)
		{
			// close enough
			if(s-y < (floatType)0.0025)
			{
//				printf("@ %d\n",i);
				break;
			}
			
			position-=step;
		}
		else
		{
//			printf("@ %d\n",i);
			break;
		}
		
		step *= (floatType)0.5;
	}

	if(add)
	{
		return(angle + position);
	}
	else
	{
		return(angle + ((floatType)(GAME_PI/4.0)-position));
	}
}



int pointInsideTriangle(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2, register floatType *n)
{
	register floatType vp0[3], vp1[3];//, vp2[3];
	register floatType v10[3], v21[3], v02[3];
	register floatType np[3];
	
	/* normal for first side with point p */
	V3Sub(v10, v1, v0);
	V3Sub(vp1, p, v1);
	V3Cross(np, v10, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < 0.0f)
	{
		return(0);
	}
	
	/* normal for second side with point p */
	V3Sub(v21, v2, v1);
//	V3Sub(vp2, p, v2);
//	V3Cross(np, v21, vp2);
	// v1 is on this segment too, might as well use what we've made already
	V3Cross(np, v21, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < 0.0f)
	{
		return(0);
	}
	
	/* normal for third side with point p */
	V3Sub(v02, v0, v2);
	V3Sub(vp0, p, v0);
	V3Cross(np, v02, vp0);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < 0.0f)
	{
		return(0);
	}
	
	/* if all 3 inside-outside tests passed */
	/* return inside                        */
	return(1);
}
/* End of pointInsideTriangle( float *p, float *v0, float *v1, float *v2 ) */



// negative tolerance makes the triangle bigger, positive smaller
int pointInsideTriangle(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2, register floatType *n,register floatType tolerance)
{
	register floatType vp0[3], vp1[3];//, vp2[3];
	register floatType v10[3], v21[3], v02[3];
	register floatType np[3];
	
	/* normal for first side with point p */
	V3Sub(v10, v1, v0);
	V3Sub(vp1, p, v1);
	V3Cross(np, v10, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(0);
	}
	
	/* normal for second side with point p */
	V3Sub(v21, v2, v1);
//	V3Sub(vp2, p, v2);
//	V3Cross(np, v21, vp2);
	// v1 is on this segment too, might as well use what we've made already
	V3Cross(np, v21, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(0);
	}
	
	/* normal for third side with point p */
	V3Sub(v02, v0, v2);
	V3Sub(vp0, p, v0);
	V3Cross(np, v02, vp0);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(0);
	}
	
	/* if all 3 inside-outside tests passed */
	/* return inside                        */
	return(1);
}
/* End of pointInsideTriangle( float *p, float *v0, float *v1, float *v2 ) */



// assumes counterclockwise winding!  polygon normal must be 0,0,1
bool V2InsideTriangle(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2)
{
	register floatType vp0[2], vp1[2];
	register floatType v10[2], v21[2], v02[2];
//	register floatType np[2];
	
	// normal for first side with point p
	V2Sub(v10, v1, v0);
	V2Sub(vp1, p, v1);
	
	// if this normal points opposite direction of triangle normal
	// then return "outside" (assume normal = 0,0,1)
	if(V2CrossProductZVal(v10, vp1) < 0.0f)
	{
		return(false);
	}
	
	// normal for second side with point p
	V2Sub(v21, v2, v1);
	
	// if this normal points opposite direction of triangle normal
	// then return "outside" (assume normal = 0,0,1)
	if(V2CrossProductZVal(v21, vp1) < 0.0f)
	{
		return(false);
	}
	
	// normal for third side with point p
	V2Sub(v02, v0, v2);
	V2Sub(vp0, p, v0);
	
	// if this normal points opposite direction of triangle normal
	// then return "outside" (assume normal = 0,0,1)
	if(V2CrossProductZVal(v02, vp0) < 0.0f)
	{
		return(false);
	}
	
	// if all 3 inside-outside tests passed
	// return "inside"
	return(true);
}


// TODO - why does this still use v3's?
// negative tolerance makes the triangle bigger, positive smaller

bool V2InTri(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2, register floatType *n,register floatType tolerance)
{
	register floatType vp0[3], vp1[3];//, vp2[3];
	register floatType v10[3], v21[3], v02[3];
	register floatType np[3];
	
	/* normal for first side with point p */
	V3Sub(v10, v1, v0);
	V3Sub(vp1, p, v1);
	V3Cross(np, v10, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(false);
	}
	
	/* normal for second side with point p */
	V3Sub(v21, v2, v1);
//	V3Sub(vp2, p, v2);
//	V3Cross(np, v21, vp2);
	// v1 is on this segment too, might as well use what we've made already
	V3Cross(np, v21, vp1);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(false);
	}
	
	/* normal for third side with point p */
	V3Sub(v02, v0, v2);
	V3Sub(vp0, p, v0);
	V3Cross(np, v02, vp0);
	
	/* if this normal points opposite direction of triangle normal */
	/* then return outside                                        */
	if(V3Dot(np,n) < tolerance)
	{
		return(false);
	}
	
	/* if all 3 inside-outside tests passed */
	/* return inside                        */
	return(true);
}
/* End of pointInsideTriangle( float *p, float *v0, float *v1, float *v2 ) */



// assumes tri points up
// all math is in 2d
// NOTE!!! I AM USING THIS TO TEST FOR BEING *INSIDE*, NOT ON!
bool V2InTri(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2)
{
	register floatType n[3];
	register floatType tmp[3];

	V2Sub(tmp, p, v1);
	V2Sub(n, v1, v0);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) < 0)
	{
		return(false);
	}

	V2Sub(tmp, p, v2);
	V2Sub(n, v2, v1);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) < 0)
	{
		return(false);
	}

//	V2Sub(tmp, p, v0);  // v2 will do since it's on the third segment too
	V2Sub(n, v0, v2);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) < 0)
	{
		return(false);
	}
	
	// inside all three side, must be inside the triangle
	return(true);
}
bool V2OnTri(register floatType *p, register floatType *v0, register floatType *v1, register floatType *v2)
{
	register floatType n[3];
	register floatType tmp[3];

	V2Sub(tmp, p, v1);
	V2Sub(n, v1, v0);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) <= 0)
	{
		return(false);
	}

	V2Sub(tmp, p, v2);
	V2Sub(n, v2, v1);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) <= 0)
	{
		return(false);
	}

//	V2Sub(tmp, p, v0);  // v2 will do since it's on the third segment too
	V2Sub(n, v0, v2);
	V2Rot90(n, n);
	if(V2Dot(tmp, n) <= 0)
	{
		return(false);
	}
	
	// inside all three side, must be inside the triangle
	return(true);
}




// basically swiped from GGIII

int SegIntersect(register floatType *l1p1,register floatType *l1p2,register floatType *l2p1,register floatType *l2p2)
{
    register floatType max,min;
    register floatType a,b,c,d;

    // bounding box check
    max = (l1p1[X]>l1p2[X])? l1p1[X] : l1p2[X];
    min = (l2p1[X]<l2p2[X])? l2p1[X] : l2p2[X];
    if(max<min)
    {
        return(0);
    }
    max = (l2p1[X]>l2p2[X])? l2p1[X] : l2p2[X];
    min = (l1p1[X]<l1p2[X])? l1p1[X] : l1p2[X];
    if(max<min)
    {
        return(0);
    }
    max = (l1p1[Y]>l1p2[Y])? l1p1[Y] : l1p2[Y];
    min = (l2p1[Y]<l2p2[Y])? l2p1[Y] : l2p2[Y];
    if(max<min)
    {
        return(0);
    }
    max = (l2p1[Y]>l2p2[Y])? l2p1[Y] : l2p2[Y];
    min = (l1p1[Y]<l1p2[Y])? l1p1[Y] : l1p2[Y];
    if(max<min)
    {
        return(0);
    }

    // see if the endpoints of the second segment lie on opposite
    // sides of the first
    a = (l2p1[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p1[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);
    b = (l2p2[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p2[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);

    if(a!=0.0f && b!=0.0f && a*b>0.0f)
    {
        return(0);
    }
    
    // see if the endpoints of the first segment lie on opposite
    // sides of the second
    c = (l1p1[X]-l2p1[X]) * (l2p2[Y]-l2p1[Y]) -
        (l1p1[Y]-l2p1[Y]) * (l2p2[X]-l2p1[X]);
    d = (l1p2[X]-l2p1[X]) * (l2p2[Y]-l2p1[Y]) -
        (l1p2[Y]-l2p1[Y]) * (l2p2[X]-l2p1[X]);

    if(c!=0.0f && d!=0.0f && c*d>0.0f)
    {
        return(0);
    }

    // are they colinear?
    if(a-b==0.0f)
    {
        return(2);
    }

    // skip the actual determination of the point and just return

    return(1);
}



int SegIntersect(register floatType *l1p1,register floatType *l1p2,register floatType *l2p1,register floatType *l2p2,floatType *percent)
{
    register floatType max,min;
    register floatType a,b,c,d;

    // bounding box check
    max = (l1p1[X]>l1p2[X])? l1p1[X] : l1p2[X];
    min = (l2p1[X]<l2p2[X])? l2p1[X] : l2p2[X];
    if(max<min)
    {
        return(0);
    }
    max = (l2p1[X]>l2p2[X])? l2p1[X] : l2p2[X];
    min = (l1p1[X]<l1p2[X])? l1p1[X] : l1p2[X];
    if(max<min)
    {
        return(0);
    }
    max = (l1p1[Y]>l1p2[Y])? l1p1[Y] : l1p2[Y];
    min = (l2p1[Y]<l2p2[Y])? l2p1[Y] : l2p2[Y];
    if(max<min)
    {
        return(0);
    }
    max = (l2p1[Y]>l2p2[Y])? l2p1[Y] : l2p2[Y];
    min = (l1p1[Y]<l1p2[Y])? l1p1[Y] : l1p2[Y];
    if(max<min)
    {
        return(0);
    }

    // see if the endpoints of the second segment lie on opposite
    // sides of the first
    a = (l2p1[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p1[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);
    b = (l2p2[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p2[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);

    if(a!=0.0f && b!=0.0f && a*b>0.0f)
    {
        return(0);
    }
    
    // see if the endpoints of the first segment lie on opposite
    // sides of the second
    c = (l1p1[X]-l2p1[X]) * (l2p2[Y]-l2p1[Y]) -
        (l1p1[Y]-l2p1[Y]) * (l2p2[X]-l2p1[X]);
    d = (l1p2[X]-l2p1[X]) * (l2p2[Y]-l2p1[Y]) -
        (l1p2[Y]-l2p1[Y]) * (l2p2[X]-l2p1[X]);

    if(c!=0.0f && d!=0.0f && c*d>0.0f)
    {
        return(0);
    }

	floatType det = a-b;
	
    // are they colinear?
    if(det==0.0f)
    {
        return(2);
    }

	floatType tdet = -c;
	floatType sdet =  a;
	if(det < 0)
	{
		det=-det;
		sdet=-sdet;
		tdet=-tdet;
	}

	// we return the position on the second segment
	*percent = sdet/det;
	
    return(1);
}



// line,segment
int LineSegIntersect(register floatType *l1p1,register floatType *l1p2,register floatType *l2p1,register floatType *l2p2)
{
//    floatType max,min;
    register floatType a,b;//,c,d;

    // see if the endpoints of the second segment lie on opposite
    // sides of the first line
    a = (l2p1[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p1[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);
    b = (l2p2[X]-l1p1[X]) * (l1p2[Y]-l1p1[Y]) -
        (l2p2[Y]-l1p1[Y]) * (l1p2[X]-l1p1[X]);

    if(a!=0.0f && b!=0.0f && a*b>0.0f)
    {
        return(0);
    }
    
    // are they colinear?
    if(a-b==0.0f)
    {
        return(2);
    }

    // skip the actual determination of the point and just return

    return(1);
}



// scrooit, I'm just gonna put this here

// don't seed with 0!
static unsigned long g_randomSeed;
void GameRandomSeed(register unsigned long seed)
{
	g_randomSeed = seed;
}

int GameRandom(void)
{
	return((int)(g_randomSeed=(16807*g_randomSeed)&0x7ffffff));
}
unsigned long GetGameRandomSeed(void)
{
	return(g_randomSeed);
}


// note that this leaves the matrix alone if the lookat is up or down the
// worldUp vector, on the assumption that the last frame's orientation is
// still stored there and is closer to valid than any arbitrary setup we
// could do here.
void M4LookAtAgain(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3])
{
	register floatType distToTarget;
	floatType toTarget[3];
	
	V3Sub(toTarget, target,eye);
	if(distToTarget = V3Len(toTarget))
	{
		V3Cross(mf[SIDE], worldUp,    mf[FORWARD]);
		V3Norm (mf[SIDE]);
		V3Cross(mf[UP],   mf[FORWARD],mf[SIDE]);
	}
//	else
//	{
		// we're sitting right on the target, which makes
		// life hard.

		// use whatever mf[FORWARD] was the last time,
		// since we probably just moved straight forward to land
		// right on the target.  side and up won't change
		// either.
//	}

	V3Set  (mf[POSITION],eye);
}



// use this if you don't have a recent and presumably similar matrix in mf
void M4LookAt(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3])
{
	register floatType distToTarget;
	floatType toTarget[3];
	
	V3Sub(toTarget, target,eye);
	if(!(distToTarget = V3Len(toTarget)))
	{
#if defined(GAME)
#  if !FINAL_ROM
		PrintCustomFault("Bad M4LookAt() matrix, eye==target\n");
#  endif
#else
		printf("Bad M4LookAt() matrix, eye==target\n");
#endif
		// arbitrary but legal substitute
		V3Set(mf[FORWARD],(floatType)1.0,(floatType)0.0,(floatType)0.0);
	}
	else
	{
		V3Div  (mf[FORWARD], toTarget, distToTarget);
	}
	V3Cross(mf[SIDE],     worldUp,     mf[FORWARD]);
	V3Norm (mf[SIDE]);
	V3Cross(mf[UP],       mf[FORWARD], mf[SIDE]);
	V3Set  (mf[POSITION], eye);

	mf[0][3] = (floatType)0.0;
	mf[1][3] = (floatType)0.0;
	mf[2][3] = (floatType)0.0;
	mf[3][3] = (floatType)1.0;
}



// use this if you don't have a recent and presumably similar matrix in mf
void M4LookAtD3D(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3])
{
	register floatType distToTarget;
	floatType toTarget[3];
	

	V3Sub(toTarget, eye, target);
	if(!(distToTarget = V3Len(toTarget)))
	{
#if defined(GAME)
#  if !FINAL_ROM
		PrintCustomFault("Bad M4LookAt() matrix, eye==target\n");
#  endif
#else
		printf("Bad M4LookAt() matrix, eye==target\n");
#endif
		// arbitrary but legal substitute
		V3Set(mf[2], (floatType)-1.0, (floatType)0.0, (floatType)0.0);
	}
	else
	{
		V3Div(mf[2], toTarget, distToTarget);
	}
	V3Cross(mf[0], worldUp, mf[2]);
	V3Norm (mf[0]);
	V3Cross(mf[1], mf[2],   mf[0]);

	// before transpose,
	// mf[2] is forward
	// mf[1] is up
	// mf[0] is side
	
	M4Transpose(mf, mf);

	V3Mul3x3(mf[3], eye, mf);
	V3Neg(mf[3]);

	mf[0][3] = (floatType)0.0;
	mf[1][3] = (floatType)0.0;
	mf[2][3] = (floatType)0.0;
	mf[3][3] = (floatType)1.0;
}



void M4LookAtD3DFlipX(register floatType mf[4][4], floatType eye[3], floatType target[3], floatType worldUp[3])
{
	register floatType distToTarget;
	floatType toTarget[3];
	

	V3Sub(toTarget, eye, target);
	if(!(distToTarget = V3Len(toTarget)))
	{
#if defined(GAME)
#  if !FINAL_ROM
		PrintCustomFault("Bad M4LookAt() matrix, eye==target\n");
#  endif
#else
		printf("Bad M4LookAt() matrix, eye==target\n");
#endif
		// arbitrary but legal substitute
		V3Set(mf[2], (floatType)-1.0, (floatType)0.0, (floatType)0.0);
	}
	else
	{
		V3Div(mf[2], toTarget, distToTarget);
	}
	V3Cross(mf[0], worldUp, mf[2]);
	V3Norm (mf[0]);
	V3Cross(mf[1], mf[2],   mf[0]);

	// before transpose,
	// mf[2] is forward
	// mf[1] is up
	// mf[0] is side
	
	V3Neg(mf[0]);
	M4Transpose(mf, mf);

	V3Mul3x3(mf[3], eye, mf);
	V3Neg(mf[3]);

	mf[0][3] = (floatType)0.0;
	mf[1][3] = (floatType)0.0;
	mf[2][3] = (floatType)0.0;
	mf[3][3] = (floatType)1.0;
}



void M4Frust(floatType dest[4][4], floatType l, floatType r, floatType b, floatType t, floatType n, floatType f)
{
	floatType     n2 = n*2.0f;
	floatType   oo_w =  1.0f/(r-l);
	floatType   oo_h =  1.0f/(t-b);
	floatType n_oo_d = -1.0f/(f-n);

	dest[0][0] =  n2 * oo_w;
	dest[0][1] =  0.0f;
	dest[0][2] =  0.0f;
	dest[0][3] =  0.0f;

	dest[1][0] =  0.0f;
	dest[1][1] =  n2 * oo_h;
	dest[1][2] =  0.0f;
	dest[1][3] =  0.0f;

	dest[2][0] =  (r+l) *   oo_w;
	dest[2][1] =  (t+b) *   oo_h;
#define PERSP_OPENGL
#ifdef PERSP_OPENGL
	dest[2][2] =  (f+n) * n_oo_d;
	dest[2][3] = -1.0f;
#else
	dest[2][2] =    f   * n_oo_d;
	dest[2][3] = -1.0f;
#endif

	dest[3][0] =  0.0f;
	dest[3][1] =  0.0f;
#ifdef PERSP_OPENGL
	dest[3][2] =  (n2*f) * n_oo_d;
#else
	dest[3][2] =  (n*f) * n_oo_d;
#endif
	dest[3][3] =  0.0f;
}



// NOTE: the angle is in RADIANS and is HALF the vertical angle
void M4Persp(floatType dest[4][4], floatType angle, floatType aspect, floatType n, floatType f)
{
	floatType hh = TAN(angle) * n;
	floatType hw = hh * aspect;
	M4Frust(dest,-hw,hw,-hh,hh,n,f);
}

void M4PerspFlipX(floatType dest[4][4], floatType angle, floatType aspect, floatType n, floatType f)
{
	floatType hh = TAN(angle) * n;
	floatType hw = hh * aspect;
	M4Frust(dest,hw,-hw,-hh,hh,n,f);
}


// NOTE: the angle is in RADIANS and is HALF the vertical angle
void M4Ortho(floatType dest[4][4], floatType l, floatType r, floatType b, floatType t, floatType n, floatType f)
{
	floatType oo_w = 1.0f/(r-l);
	floatType oo_h = 1.0f/(t-b);
	floatType oo_d = 1.0f/(f-n);

	dest[0][0] =  2.0f * oo_w;
	dest[0][1] =  0.0f;
	dest[0][2] =  0.0f;
	dest[0][3] =  0.0f;

	dest[1][0] =  0.0f;
	dest[1][1] =  2.0f * oo_h;
	dest[1][2] =  0.0f;
	dest[1][3] =  0.0f;

	dest[2][0] =  0.0f;
	dest[2][1] =  0.0f;
	dest[2][2] = -2.0f * oo_d;
	dest[2][3] =  0.0f;

	dest[3][0] =  (r+l) * oo_w;
	dest[3][1] =  (t+b) * oo_h;
	dest[3][2] =  (f+n) * oo_d;
	dest[3][3] =  1.0f;
}


// rotate the upper 3x3 of dest by angle around axis
void M4Rot3x3(floatType dest[4][4], floatType angle, floatType axis[3])
{
    register floatType s = SIN(angle);
    register floatType c = COS(angle);
    register floatType t = (floatType)1.0-c;

	register floatType ax = axis[X];
	register floatType ay = axis[Y];
	register floatType az = axis[Z];
	
	register floatType taxay  = t*ax;
	register floatType taxaz  = taxay*az;
	taxay *= ay;
	register floatType tayaz  = t*ay*az;

	register floatType sax = s*ax;
	register floatType say = s*ay;
	register floatType saz = s*az;
	
	register floatType x = dest[0][X];
	register floatType y = dest[0][Y];
	register floatType z = dest[0][Z];
	
    dest[0][X] =
        x*(t*ax*ax + c  ) +
        y*(taxay   + saz) +
        z*(taxaz   - say);
    
    dest[0][Y] =
        x*(taxay   - saz) +
        y*(t*ay*ay + c  ) +
        z*(tayaz   + sax);
    
    dest[0][Z] =
        x*(taxaz   + say) +
        y*(tayaz   - sax) +
        z*(t*az*az + c  );
	
	x = dest[1][X];
	y = dest[1][Y];
	z = dest[1][Z];
	
    dest[1][X] =
        x*(t*ax*ax + c  ) +
        y*(taxay   + saz) +
        z*(taxaz   - say);
    
    dest[1][Y] =
        x*(taxay   - saz) +
        y*(t*ay*ay + c  ) +
        z*(tayaz   + sax);
    
    dest[1][Z] =
        x*(taxaz   + say) +
        y*(tayaz   - sax) +
        z*(t*az*az + c  );
	
	x = dest[2][X];
	y = dest[2][Y];
	z = dest[2][Z];
	
    dest[2][X] =
        x*(t*ax*ax + c  ) +
        y*(taxay   + saz) +
        z*(taxaz   - say);
    
    dest[2][Y] =
        x*(taxay   - saz) +
        y*(t*ay*ay + c  ) +
        z*(tayaz   + sax);
    
    dest[2][Z] =
        x*(taxaz   + say) +
        y*(tayaz   - sax) +
        z*(t*az*az + c  );
}



void M4Set(D3DMATRIX *md, floatType mf[4][4])
{
	M4Transpose(md->m,mf);
}


void M4Set(floatType mf[4][4], D3DMATRIX *md)
{
	M4Transpose(mf,md->m);
}



bool V4Equal(floatType *src1, floatType *src2)
{
	return(src1[0] == src2[0] &&
	       src1[1] == src2[1] &&
		   src1[2] == src2[2] &&
		   src1[3] == src2[3]);
}
bool V3Equal(floatType *src1, floatType *src2)
{
	return(src1[0] == src2[0] &&
	       src1[1] == src2[1] &&
		   src1[2] == src2[2]);
}
bool V2Equal(floatType *src1, floatType *src2)
{
	return(src1[0] == src2[0] &&
	       src1[1] == src2[1]);
}



#undef TOLERANCE
#define TOLERANCE 0.000009953976
#undef TOLERABLE
#define TOLERABLE(x) ((x) < -TOLERANCE || (x) > TOLERANCE)

void V2Gouraud4(floatType dest[4], floatType point[2],
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[4],  floatType triSrc1[4],  floatType triSrc2[4])
{
}
void V2Gouraud3(floatType out[3], floatType pos[2],
			    floatType v0[2], floatType v1[2], floatType v2[2],
			    floatType c0[3], floatType c1[3], floatType c2[3])
{
	{	
		// order with respect to Y (tiny bubble sort)
		register floatType *vt;
		#define VSWAP(a,b) vt=v##a;v##a=v##b;v##b=vt;vt=c##a;c##a=c##b;c##b=vt
		if(v1[Y] < v0[Y])
		{
			VSWAP(0,1);
		}
		if(v2[Y] < v1[Y])
		{
			VSWAP(1,2);
		}
		if(v1[Y] < v0[Y])
		{
			VSWAP(0,1);
		}

		// rearrange so that we are between 0 and 1
		// and between 0 and 2
		if(v1[Y] <= pos[Y] && v1[Y]!=v2[Y])
		{
			VSWAP(0,2);
		}
	}

	// get "scan line" endpoint values for gouraud shade
	// TODO - make sure len can't be 0.0
	floatType3 ci0,ci1;
	
	register floatType len;
	register floatType frac;
	register floatType vi0x,vi1x;
	
	register floatType dist = (pos[Y] - v0[Y]);

	// v0-v1 interpolate
	len = v1[Y] - v0[Y];
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Lerp(ci0,c1,c0,frac);
	ScalarLerp(vi0x,v1[X],v0[X],frac);
	
	// v0-v2 interpolate
	len = v2[Y] - v0[Y];
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Lerp(ci1,c2,c0,frac);
	ScalarLerp(vi1x,v2[X],v0[X],frac);
	
	// interpolated points v01-v02 interpolate
	dist = pos[X] - vi0x;
	len = vi1x - vi0x;
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Lerp(out,ci1,ci0,frac);
}
void V2GouraudN(floatType dest[3], floatType point[2], int n,
			    floatType triVert0[2], floatType triVert1[2], floatType triVert2[2],
			    floatType triSrc0[3],  floatType triSrc1[3],  floatType triSrc2[3])
{
}
void V2GouraudFactor(floatType out[3], floatType pos[2],
			    floatType v0[2], floatType v1[2], floatType v2[2])
{
	int from0=0,from1=1,from2=2;
	{	
		// order with respect to Y (tiny bubble sort)
		register floatType *vt;
		register int     ft;
		#undef VSWAP
		#define VSWAP(a,b) vt=v##a;v##a=v##b;v##b=vt;ft=from##a;from##a=from##b;from##b=ft;
		if(v1[Y] < v0[Y])
		{
			VSWAP(0,1);
		}
		if(v2[Y] < v1[Y])
		{
			VSWAP(1,2);
		}
		if(v1[Y] < v0[Y])
		{
			VSWAP(0,1);
		}

		// rearrange so that we are between 0 and 1
		// and between 0 and 2
		if(v1[Y] <= pos[Y] && v1[Y]!=v2[Y])
		{
			VSWAP(0,2);
		}
	}

	// get "scan line" endpoint values for gouraud shade
	// TODO - make sure len can't be 0.0
	floatType3 ci0,ci1;
	
	register floatType len;
	register floatType frac;
	register floatType vi0x,vi1x;
	
	register floatType dist = (pos[Y] - v0[Y]);

	// v0-v1 interpolate
	len = v1[Y] - v0[Y];
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Set(ci0, 1.0f-frac, frac, 0.0f);
	ScalarLerp(vi0x,v1[X],v0[X],frac);
	
	// v0-v2 interpolate
	len = v2[Y] - v0[Y];
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Set(ci1, 1.0f-frac, 0.0f, frac);
	ScalarLerp(vi1x,v2[X],v0[X],frac);
	
	// interpolated points v01-v02 interpolate
	dist = pos[X] - vi0x;
	len = vi1x - vi0x;
	frac = TOLERABLE(len)? (dist/len) : 0.0f;
	V3Lerp(ci0,ci1,ci0,frac);

	out[from0] = ci0[0];
	out[from1] = ci0[1];
	out[from2] = ci0[2];
}

// I always use a divisor of a power of 2 because that
// typically executes faster.  not sure of intel processors mind you.
// 1/8192 = ~0.0001
#define V2ONSEG_EPSILON (1.0f / 8192.0f)
bool V2SnapToSeg(floatType *teFrac, floatType p[2], floatType tev0[2], floatType tev1[2])
{
	floatType v[2], n[2], tmp[2];
	V2Sub(v, tev1, tev0);
	floatType len = V2Len(v);
	// TODO - trap len==0.0
	if(len == 0.0f)
	{
//		printf("argh\n");
	}
	V2Div(v, len);
	V2Rot90(n, v);
	V2Sub(tmp, p, tev0);
	floatType dist = V2Dot(n, tmp);
	if(dist < V2ONSEG_EPSILON && dist > -V2ONSEG_EPSILON)
	{
		// on line, is it on seg?
		floatType pos = V2Dot(v, tmp);

		// is it in the latter half of the line?
		if(pos >= (len * 0.5f))
		{
			// yes, reorient so we have more precision near the far vertex
			V2Sub(tmp, p, tev1);
			pos = V2Dot(v, tmp);
			if(pos <= V2ONSEG_EPSILON)  // not too far past far point
			{
				if(pos > 0.0f)
				{
					// if indeed past endpoint, check semicircle on far point
					if(V2DistSq(tev1, p) < (V2ONSEG_EPSILON*V2ONSEG_EPSILON))
					{
						*teFrac = 1.0f;
						return(true);
					}
				}
				else if(pos >= -V2ONSEG_EPSILON)  // close enough to far point?
				{
					// within snapping distance of far point
					*teFrac = 1.0f;
					return(true);
				}
				else
				{
					// else put it where it is on line
					*teFrac = 1.0f + (pos / len);
					return(true);
				}
			}
		}
		else
		{
			if(pos >= -V2ONSEG_EPSILON)
			{
				if(pos < 0.0f)
				{
					// past the endpoint, check a circle around it
					if(V2DistSq(tev0, p) < (V2ONSEG_EPSILON*V2ONSEG_EPSILON))
					{
						*teFrac = 0.0f;
						return(true);
					}
				}
				else if(pos <= V2ONSEG_EPSILON)
				{
					*teFrac = 0.0f;
					return(true);
				}
				else
				{
					*teFrac = pos / len;
					return(true);
				}
			}
		}
	}
	return(false);
}



void V3Ceil(floatType vec[3])
{
	vec[0] = CEIL(vec[0]);
	vec[1] = CEIL(vec[1]);
	vec[2] = CEIL(vec[2]);
}



void V2Ceil(floatType vec[2])
{
	vec[0] = CEIL(vec[0]);
	vec[1] = CEIL(vec[1]);
}



void V3Floor(floatType vec[3])
{
	vec[0] = FLOOR(vec[0]);
	vec[1] = FLOOR(vec[1]);
	vec[2] = FLOOR(vec[2]);
}



void V2Floor(floatType vec[2])
{
	vec[0] = FLOOR(vec[0]);
	vec[1] = FLOOR(vec[1]);
}
