/*===========================================================================*\
 | 
 |  FILE:	Shader.h
 |			Skeleton project and code for a Material Shader
 |			3D Studio MAX R3.0
 | 
 |  AUTH:   Harry Denholm
 |			Developer Consulting Group
 |			Copyright(c) Discreet 1999
 |
 |  HIST:	Started 26-1-99
 | 
\*===========================================================================*/
/*===========================================================================*\
 |	A couple of handy little functions, from ShaderUtil.cpp in SDK Samples
\*===========================================================================*/


// For binding values to a specific limit
inline float Bound( float x, float min = 0.0f, float max = 1.0f )
	{ return x < min? min:( x > max? max : x); }
inline Color Bound( Color& c )
	{ return Color( Bound(c.r), Bound(c.g), Bound(c.b) ); }

inline float UBound( float x, float max = 1.0f ){ return x > max ? max : x; }
inline float LBound( float x, float min = 0.0f ){ return x < min ? min : x; }

inline float Min( float a, float b ) { return (a < b) ? a : b; }
inline float Min( float a, float b, float c ) { return (a < b) ? Min(a,c) : Min(b,c); }
inline float Min( Color& c ){ return Min( c.r, c.g, c.b ); }
inline float Max( float a, float b ) { return (a < b) ? b : a; }
inline float Max( float a, float b, float c ) { return (a < b) ? Max( b, c ) : Max(a,c); }
inline float Max( Color& c ){ return Max( c.r, c.g, c.b ); }


// Conversion to fractions/viceversa
inline float PcToFrac(int pc) { return (float)pc/100.0f; }

inline int FracToPc(float f) {
	if (f<0.0) return (int)(100.0f*f - .5f);
	else return (int) (100.0f*f + .5f);
}

