/*
	          EXCERPTED FROM SIGGRAPH 92, COURSE 23
	                  PROCEDURAL MODELING

	                       Ken Perlin
	                   New York University

	3.6 TURBULENCE AND NOISE

	3.6.1 The turbulence function

	The turbulence function, which you use  to  make  marble,
	clouds,  explosions,  etc., is just a simple fractal gen-
	erating loop built on top of the noise function.   It  is
	not a real turbulence model at all.  The key trick is the
	use of the fabs() function, which makes the function have
	gradient discontinuity "fault lines" at all scales.  This
	fools the eye into thinking it is seeing the  results  of
	turbulent flow.  The turbulence() function gives the best
	results when used as a phase shift, as  in  the  familiar
	marble trick:

	    sin(point + turbulence(point) * point.x);

	Note the second argument below, lofreq,  which  sets  the
	lowest  desired  frequency  component  of the turbulence.
	The third, hifreq, argument is used by  the  function  to
	ensure  that  the  turbulence  effect reaches down to the
	single pixel level, but no further.  I usually  set  this
	argument equal to the image resolution.

*/

#include "d3dlocus.h"
#include "perlin.h"

const int		max_terms = 32;
static float	scale[max_terms];	// scaling factor for renormalization

float 
PerlinTurbulence1D (D3DXVECTOR3 & point, int terms)
{
	float	freq = 1.0f, 
			result = 0.0f;

	for (int i=0; i<terms; i++) {
		result += PerlinNoise1D(point) * freq;
		point *= 2.0f;
		freq *= 0.5f;
	}

	if (terms>=max_terms)
		terms = max_terms-1;

	return result * scale[terms];
}
/*
D3DVECTOR
PerlinTurbulence3D (D3DVECTOR & point, int terms)
{
	float		freq = 1.0f;
	D3DVECTOR	result = D3DVECTOR(0.0f);

	for (int i=0; i<terms; i++) {
		result += PerlinNoise3D(point) * freq;
		point *= 2.0f;
		freq *= 0.5f;
	}

	if (terms>=max_terms)
		terms = max_terms-1;

	return result * scale[terms];
}
*/
/*
	3.6.2 The noise function

	noise3 is a rough approximation to "pink"  (band-limited)
	noise,  implemented  by  a  pseudorandom tricubic spline.
	Given a vector in  3-space,  it returns a  value  between
	-1.0  and 1.0.  There are two principal tricks to make it
	run fast:

	- Precompute an array of pseudo-random unit  length  gra-
	dients g[n].

	- Precompute a permutation  array  p[]  of  the  first  n
	integers.

	Given the above two arrays,  any  integer  lattice  point
	(i,j,k)  can be quickly mapped to a pseudorandom gradient
	vector by:

	    g[ (p[ (p[i] + j) % n ] + k) % n]

	By extending the g[] and p[] arrays, so that  g[n+i]=g[i]
	and  p[n+i]=p[i], the above lookup can be replaced by the
	(somewhat faster):

	    g[ p[ p[i] + j ] + k ]

	Now for any point in 3-space,  we just  have  to  do  the
	following two steps:

	(1) Get the  gradient  for  each  of  its  surrounding  8
	integer lattice points as above.

	(2) Do a tricubic hermite  spline  interpolation,  giving
	each lattice point the value 0.0.

	The second step above is just an evaluation of  the  her-
	mite  derivative basis function 3 * t * t - 2 * t * t * t
	in each by a dot product of the gradient at the lattice.

	Here is my implementation in C  of  the  noise  function.
	Feel  free  to use it, as long as you reference where you
	got it. :-)
*/


// noise function over R3 - implemented by a pseudorandom tricubic spline

static const int	B = 256;

static int			p[B + B + 2];
static D3DXVECTOR3	g[B + B + 2];
static int			start = 1;

#define setup(i,b0,b1,r0,r1) \
	t = vec[i] + 10000.0f; \
	b0 = ((int)t) & (B-1); \
	b1 = (b0+1) & (B-1); \
	r0 = t - (int)t; \
	r1 = r0 - 1.0f;

float 
rnd(void)
{
	return float(rand())/RAND_MAX;
}	// end of rnd()


float 
PerlinNoise1D (D3DXVECTOR3 & vec)
{
	int 		bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	float		rx0, rx1, ry0, ry1, rz0, rz1,
				sx, sy, sz, a, b, c, d, t, u, v;
	D3DXVECTOR3	q;
	int			i, j;

	if (start) {
		start = 0;
		InitPerlinNoise(1);
	}

	setup(0, bx0,bx1, rx0,rx1);
	setup(1, by0,by1, ry0,ry1);
	setup(2, bz0,bz1, rz0,rz1);

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

#define at(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

#define surve(t) ( t * t * (3.0f - 2.0f * t) )

#define lerp(t, a, b) ( a + t * (b - a) )

	sx = surve(rx0);
	sy = surve(ry0);
	sz = surve(rz0);


	q = g[ b00 + bz0 ] ; u = at(rx0,ry0,rz0);
	q = g[ b10 + bz0 ] ; v = at(rx1,ry0,rz0);
	a = lerp(sx, u, v);

	q = g[ b01 + bz0 ] ; u = at(rx0,ry1,rz0);
	q = g[ b11 + bz0 ] ; v = at(rx1,ry1,rz0);
	b = lerp(sx, u, v);

	c = lerp(sy, a, b);	// interpolate in y at lo x 

	q = g[ b00 + bz1 ] ; u = at(rx0,ry0,rz1);
	q = g[ b10 + bz1 ] ; v = at(rx1,ry0,rz1);
	a = lerp(sx, u, v);

	q = g[ b01 + bz1 ] ; u = at(rx0,ry1,rz1);
	q = g[ b11 + bz1 ] ; v = at(rx1,ry1,rz1);
	b = lerp(sx, u, v);

	d = lerp(sy, a, b);	// interpolate in y at hi x

	return 1.5f * lerp(sz, c, d);	// interpolate in z
}	// end of PerlinNoise1D()
/*
D3DVECTOR 
PerlinNoise3D (D3DVECTOR & vec)
{
	D3DVECTOR	result;

	result[0] = PerlinNoise1D(vec);
	vec += D3DVECTOR(543.4f, 67834.432f, 98734.432f);
	result[1] = PerlinNoise1D(vec);
	vec += D3DVECTOR(5784.4783f, 382.75832f, 6954.7823f);
	result[2] = PerlinNoise1D(vec);

	return result;
}	// end of PerlinNoise3D()
*/
void
InitPerlinNoise(int seed)
{
	int			i, j, k;
	float		s;
	D3DXVECTOR3	v;

	// Create an array of random gradient vectors 
	// uniformly on the unit sphere

	srand(seed);

	for (i=0; i<B; i++) {
		do {	// Choose uniformly in a cube */
			v = D3DXVECTOR3((float)(rnd() - rnd()),
						 (float)(rnd() - rnd()),
						 (float)(rnd() - rnd()));
			s = D3DXVec3Dot(&v,&v);
		} while (s > 1.0);	// If not in sphere try again
		s = (float)sqrt(s);
		g[i] = v / s;		// normalize
	}

	// Create a pseudorandom permutation of [1..B]

	for (i=0; i<B; i++) {
		p[i] = i;
	}
	for (i=B; i>0; i-=2) {
		k = p[i];
		p[i] = p[j = rand() % B];
		p[j] = k;
	}

	// Extend g and p arrays to allow for faster indexing

	for (i=0; i<B+2; i++) {
		p[B + i] = p[i];
		g[B+i] = g[i];
	}

	// create renormaliztion scaling array

	for (i=0; i<max_terms; i++) {
		scale[i] = (float)(pow(2.0, (double)i) / (pow(2.0, (double)(i+1))-1.0));
	}

}	// end of InitPerlinNoise()







