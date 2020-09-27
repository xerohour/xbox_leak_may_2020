#pragma once

#pragma warning(disable:4035)


/******************************************************************************
class SWIZZLER:
by Jason Gould (JGould)
last modified 18 oct 2000

Purpose: to allow simple manipulations of a swizzled texture, without the 
hassle or overhead of unswizzling the whole thing in order to tweak a few 
points on the texture. This works with texture maps, cube maps, and volume maps.

Depending on your purpose in messing with a swizzled texture, certain 
optimizations can be performed by doing things a little differently than are
done here.

Notes: 
  Most of the time when messing with a texture map, you will be incrementing 
  through it by a constant value in each dimension. In order to speed up this
  process considerably, those deltas are converted to an intermediate value
  (via the Convert(num, dim) method), which can be used to quickly increment
  or change a particular dimension.

  In all methods, the variable dim is used to specify the dimension being
  manipulated. For x (or u), use UCOORD. For y (or v), use VCOORD. For z (or 
  s?), use SCOORD.

  The type SWIZNUM is used to represent numbers returned by Convert(), also
  known as "intermediate values" in this documentation.

  Code in comments, like /* & m_and[dim] */  /* may be uncommented in order
  to provide some sort of parameter sanity. (It assures that any number 
  passed to num will only alter the dimension specified by dim)

Elements:
  m_index -- the index into the swizzled texture at the most recent (x,y,z)
    coordinates specified
  
  m_loc[] -- [UCOORD] = texture map (converted) u coordinate
  m_loc[] -- [VCOORD] = texture map (converted) v coordinate
  m_loc[] -- [SCOORD] = texture map (converted) s coordinate

  m_and[] -- [UCOORD] = internal mask for u coordinate
  m_and[] -- [VCOORD] = internal mask for v coordinate
  m_and[] -- [SCOORD] = internal mask for s coordinate

Methods:
  SWIZNUM Convert(num, dim) -- converts num to an intermediate value that can 
    be used to alter a coordinate of the dimension dim
  void Unconvert(index, dim) -- takes an index to the swizzled texture, and 
    returns the unswizzled coordinate of dimension dim
  void Set (num, dim) -- sets the coordinate of dimension dim to num, where
    num is an intermediate value returned by Convert
  void Add (num, dim) -- adds num to the coordinate of dimension dim, where
    num is an intermediate value returned by Convert
  void Sub (num, dim) -- subtracts num from the coordinate of dimension dim, 
	where num is an intermediate value returned by Convert
  void Inc (dim) -- increments the coordinate of dimension dim by 1
  void Dec (dim) -- decrements the coordinate of dimension dim by 1
  SWIZNUM Get2() -- returns the index to the swizzled standard or cube texture, 
    based on the current texture coordinates specified by the previous 5 
	methods
  SWIZNUM Get3() -- returns the index to the swizzled volume texture, based 
    on the current texture coordinates specified by the previous 5 methods
  void Swizzle2d(Dest, Src) -- Swizzles/unswizzles a 2d texture from Src to 
    Dest.
  void Swizzle3d(Dest, Src) -- Swizzles/unswizzles a volume texture from Src 
    to Dest.

Optimizations:
  Subtraction is used so that only ANDing is necessary (no ORing, as in other
  methods). For this reason, the Sub method is faster than the Add method.
  Inc and Dec are roughly the same speed, because Dec requires a shift, but
  Inc requires accessing an array.
  
  It may be better to have Convert() convert the negative of the value 
  specified, and then to reverse the roles of Add and Sub. That would make the 
  more logical manuvering routine (Add) the faster of the two.

  Arrays could probably also be removed, replacing Add() with AddU, AddV, and 
  AddS methods, in order to remove one layer of pointer dereferencing.
******************************************************************************/

#define UCOORD 0
#define VCOORD 1
#define SCOORD 2

//type returned by Convert, to be used in all other methods
typedef unsigned long DWORD;

typedef DWORD SWIZNUM; 

typedef enum {
    SWIZZLE,
    UNSWIZZLE
} WHICHSWIZ;

class SWIZZLER {
public: //private:
	DWORD m_loc[3]; //location (0 == x, 1 == y, 2 == z)
	//ORing is not needed if we use the subtraction method
	DWORD m_and[3]; //ANDer (0 == x, 1 == y, 2 == z)
	DWORD m_size[3];//size (used in swizzling/unswizzling)
	DWORD m_index;
public:
	SWIZZLER(DWORD width, DWORD height, DWORD depth) : m_index(0)
	{ 
		m_loc[UCOORD] = 0; m_loc[VCOORD] = 0; m_loc[SCOORD] = 0;
		m_and[UCOORD] = 0; m_and[VCOORD] = 0; m_and[SCOORD] = 0;

		m_size[UCOORD] = width; m_size[VCOORD] = height; m_size[SCOORD] = depth;

		DWORD i = 1;
		DWORD j = 1;
		DWORD l;
		DWORD k;
		do {
			k = 0;
			for(l = 0; l < 3; l++) {
				if(i < m_size[l]) {
					m_and[l] |= j;
					k = (j <<= 1);
				}
			}
			i <<= 1;
		} while (k);
	}
	
	__forceinline SWIZNUM Convert(DWORD num, int dim) //num == in, dim == dimension (0 for x, 1 for y, 2 for z)
	{
		DWORD i;
		SWIZNUM r = 0; //result
		for(i = 1; i <= m_and[dim]; i <<= 1) {
			if(m_and[dim] & i) {
				r |= (num & i);
			} else {
				num <<= 1;
			}
		}
		return r;
	}
	SWIZNUM Convert(DWORD x, DWORD y, DWORD z)
	{
		return Convert(x, UCOORD) | Convert(y, VCOORD) | Convert(z, SCOORD);
	}

	__forceinline DWORD Unconvert(SWIZNUM num, int dim) //num == in, dim == dimension
	{
		DWORD i;
		DWORD j;
		DWORD r = 0; //result
		for(i = 1, j = 1; i; i <<= 1) {
			if(m_and[dim] & i) {
				r |= (num & j);
				j <<= 1;
			} else {
				num >>= 1;
			}
		}
		return r;
	}

	__forceinline void Set0()
	{
		m_loc[0] = m_loc[1] = m_loc[2] = 0;
	}
	__forceinline void Set(SWIZNUM x, SWIZNUM y, SWIZNUM z) 
	{
		m_loc[UCOORD] = x; m_loc[VCOORD] = y; m_loc[SCOORD] = z;
	}
	__forceinline void Set(SWIZNUM num, int dim)
	{
		m_loc[dim] = num /* & m_and[dim] */;
	}
	__forceinline void Add(SWIZNUM num, int dim)
	{
		m_loc[dim] = (m_loc[dim] - ((0-num) & m_and[dim])) & m_and[dim];
	}
	__forceinline void Sub(SWIZNUM num, int dim)
	{
		m_loc[dim] = (m_loc[dim] - num /* & m_and[dim] */) & m_and[dim];
	}
	__forceinline DWORD Inc(int dim)
	{
		return m_loc[dim] = (m_loc[dim] - m_and[dim]) & m_and[dim];
	}
	__forceinline void Dec(int dim)
	{
		m_loc[dim] = (m_loc[dim] - 1) & m_and[dim];
	}
	__forceinline SWIZNUM Get2()
	{
		return m_index = m_loc[UCOORD] | m_loc[VCOORD];
	}
	__forceinline SWIZNUM Get3()
	{
		return m_index = m_loc[UCOORD] | m_loc[VCOORD] | m_loc[SCOORD];
	}
	SWIZZLER(){}
};

template <class TYPE>
class SWIZZLER2 : SWIZZLER
{
public:
	SWIZZLER2(DWORD width, DWORD height, DWORD depth) : SWIZZLER(width, height, depth) {}


	/* 
		note! Pass a TYPE that is 2* the size of the actual byte type, for doing FastSwizzle & FastUnswizzle!
		For instance, if the texture is 8 bit, pass WORD instead. 
		If the texture is 16-bit, use DWORD. 
		For 32-bit, use __int64 or QWORD.
	*/
	void FastSwizzle2d(TYPE* Src, TYPE* Dest)
	{
		int s = 0, u = 0, v = 0;
		int uand = m_And[UCOORD] >> 1,
			vand = m_And[VCOORD] >> 1;

		do {
			do {
				Dest[u | v] = *Src++;
			} while (0 != (u = (u - uand) & uand));
		} while (0 != (v = (v - vand) & vand));
	}
	void FastSwizzle3d(TYPE* Src, TYPE* Dest)
	{
		int s = 0, u = 0, v = 0;
		int sand = m_And[SCOORD] >> 1,
			uand = m_And[UCOORD] >> 1,
			vand = m_And[VCOORD] >> 1;

		do {
			do {
				do {
					Dest[u | v | s] = *Src++;
				} while (0 != (u = (u - uand) & uand));
			} while (0 != (v = (v - vand) & vand));
		} while (0 != (s = (s - sand) & sand));
	}

	void FastUnswizzle2d(TYPE* Src, TYPE* Dest)
	{
		int s = 0, u = 0, v = 0;
		int uand = m_And[UCOORD] >> 1,
			vand = m_And[VCOORD] >> 1;

		do {
			do {
				*Dest++ = Src[u | v];
			} while (0 != (u = (u - uand) & uand));
		} while (0 != (v = (v - vand) & vand));
	}
	void FastUnswizzle3d(TYPE* Src, TYPE* Dest)
	{
		int s = 0, u = 0, v = 0;
		int sand = m_And[SCOORD] >> 1,
			uand = m_And[UCOORD] >> 1,
			vand = m_And[VCOORD] >> 1;

		do {
			do {
				do {
					*Dest++ = Src[u | v | s];
				} while (0 != (u = (u - uand) & uand));
			} while (0 != (v = (v - vand) & vand));
		} while (0 != (s = (s - sand) & sand));
	}


	void Swizzle2d(TYPE *Src, TYPE* Dest)
	{ 		
		Set0();
		do {
			do {
				Dest[Get2()] = *Src++;
			} while (Inc(UCOORD));
		} while (Inc(VCOORD));
	}
	void Unswizzle2d(TYPE *Src, TYPE* Dest)
	{
		Set0();
		do {
			do {
				*Dest++ = Src[Get2()];
			} while (Inc(UCOORD));
		} while (Inc(VCOORD));
	}

	void Swizzle3d(TYPE *Src, TYPE* Dest)
	{
		Set0();
		do {
			do {
				do {
					Dest[Get3()] = *Src++;
				} while (Inc(UCOORD));
			} while (Inc(VCOORD));
		} while (Inc(SCOORD));
	}
	void Unswizzle3d(TYPE *Src, TYPE* Dest)
	{
		Set0();
		do {
			do {
				do {
					*Dest++ = Src[Get3()];
				} while (Inc(UCOORD));
			} while (Inc(VCOORD));
		} while (Inc(SCOORD));
	}
};


