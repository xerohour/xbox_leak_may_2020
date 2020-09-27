#include "std.h"
#include "xapp.h"
#include "surfx.h"

int CSurfx::sTempRef = 0;
long CSurfx::sTempSize = 0;
char* CSurfx::sTemp = NULL;

CSurfx::CSurfx(int nWidth, int nHeight)
{
//	m_pSurface = NULL;

//	m_ssd.lPitch = nWidth;
//	m_ssd.lpSurface = new BYTE [nWidth * nHeight];
//	m_pSurface = NULL;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_pels = new BYTE [nWidth * (nHeight + 1)];

	mLineWidth = 1;

	mClipRect.left = 0;
	mClipRect.top = 0;
	mClipRect.right = nWidth;
	mClipRect.bottom = nHeight;

	mBytesPerRow = nWidth;

/*
	m_pSurface = pSurface;

	if (m_pSurface != NULL)
	{
		m_pSurface->AddRef();

		m_ssd.dwSize = sizeof (DDSURFACEDESC2);
		m_pSurface->Lock(NULL, &m_ssd, DDLOCK_WRITEONLY, NULL);
	}
*/
	sTempRef += 1;
}

CSurfx::~CSurfx()
{
/*
	if (m_pSurface != NULL)
	{
		m_pSurface->Unlock(NULL);
		m_pSurface->Release();
	}
	else
	{
		delete [] m_ssd.lpSurface;
	}
*/
	delete [] m_pels;

	sTempRef -= 1;
	if (sTempRef == 0)
	{
		delete [] sTemp;
		sTemp = NULL;
		sTempSize = 0;
	}
}


void CSurfx::Fade(DeltaFieldData* inGrad)
{
	Fade((char*)m_pels/*m_ssd.lpSurface*/, m_nWidth/*m_ssd.lPitch*/, m_nWidth, m_nHeight, inGrad);
}

#define HALFCORD	0x003F  /* 12 bits per cord, 5 bits for fixed decimal, 7 bits for whole number */
#define FIXED_BITS	5


void CSurfx::Fade(const char* inSrce, long inBytesPerSrceRow, char* inDest, long inBytesPerDestRow, long inX, long inY, const char* inGrad)
{
	unsigned long p, x, y, u, v, u1, v1, P1, P2, P3, P4, codedDel, xGrad, yGrad;
	const char* srceMap;


	// Setup the source row base address and offset to allow for negative grad components
	inSrce = inSrce - HALFCORD * inBytesPerSrceRow - HALFCORD;


	// For every pixel in the destination, find it's source via the gradient
	for ( y = 0; y < (unsigned)inY; y++ )
	{
		for ( x = 0; x < (unsigned)inX; x++ )
		{
			// Extract the gradient information
			// Format of a grad short:  ( ( (ddx + 128) * 16 ) << 20 ) | ( (ddy + 128) * 16 ) << 8
			codedDel = *((const unsigned long*) inGrad);	
			inGrad += 3;
			xGrad = 0xFFF & ( codedDel >> 12 );
			yGrad = 0xFFF & ( codedDel );

			// Get the vector to the source pixel
			srceMap = inSrce + ( xGrad >> FIXED_BITS ) + ( yGrad >> FIXED_BITS ) * inBytesPerSrceRow + x;

			u = ( yGrad & 0x1F );		// y - ysrc
			v = ( xGrad & 0x1F );		// x - xsrc

			// P1 - P2
			// |     |
			// P3 - P4
			P1  = ( (unsigned char*) srceMap )[0];
			P2  = ( (unsigned char*) srceMap )[1];

			u1	 = 0x20 - u;
			P1  *= u1;
			P2  *= u1;

			P3  = ( (unsigned char*) srceMap )[ inBytesPerSrceRow ];  
			P4  = ( (unsigned char*) srceMap )[ inBytesPerSrceRow + 1 ];

			v1 	= 0x20 - v;
			P3 *= u;
			P4 *= u;

			// Bilinear interpolation to approximate the source pixel value
			p  = v * ( P2 + P4 ) + v1 * ( P1 + P3 );


			// Store the calculated/dest pixel (rounding the fraction part of the pix value)
			// We divide by (5+5) decimal places because p is units squared (5 places per decimal)
			((unsigned char*) inDest)[ x ] = (unsigned char)(( 31 * p ) >> 15);  // 5 more dec places cuz of the mult by 31
		}

		// Setup the dest row base address		
		inDest += inBytesPerDestRow;		

		// Setup the source row base address, also offset to allow for negative grad components
		inSrce += inBytesPerSrceRow;
	}
}


void CSurfx::Fade(char* inPix, long inBytesPerRow, long inX, long inY, DeltaFieldData* inGrad)
{
	unsigned long p, x, y, u, v, u1, v1, P1, P2, P3, P4, codedDel, xGrad, yGrad, curBufRowNum;
	long bufRows;
	char* srceMap, *curBufRow, *srce, *grad = inGrad->mField;

	// Maintain our temp buf
	bufRows = inGrad->mNegYExtents;
	if (sTempSize < bufRows * inBytesPerRow)
	{
		sTempSize = bufRows * inBytesPerRow;
		delete [] sTemp;
		sTemp = new char [sTempSize];
	}

	// Setup the trailing buffer...
	// To transformation must operate on the orginal pixels, so we can't write over the original pix
	// until the cur pixel is sufficiently far away (> 2^HALFCORD num rows away)

	if (inY <= bufRows)
		bufRows = inY;
	Fade(inPix, inBytesPerRow, sTemp, inBytesPerRow, inX, bufRows, grad);
	inY -= bufRows;
	grad += 3 * inX * bufRows;
	curBufRowNum = 0;

	// Setup the source row base address and offset to allow for negative grad components
	srce = inPix + bufRows * inBytesPerRow - HALFCORD * inBytesPerRow - HALFCORD;

	// For every pixel in the destination, find it's source via the gradient
	for (y = 0; y < (unsigned)inY; y += 1)
	{
		// Calc a loc where the trailing buffer row starts
		curBufRow = sTemp + curBufRowNum * inBytesPerRow;

		for (x = 0; x < (unsigned)inX; x += 1)
		{
			// Extract the gradient information
			// Format of a grad short:  ( ( (ddx + 128) * 16 ) << 20 ) | ( (ddy + 128) * 16 ) << 8
			codedDel = *((const unsigned long*) grad);	
			grad += 3;
			xGrad = 0xFFF & (codedDel >> 12);
			yGrad = 0xFFF & (codedDel);

			// Do something in parallel -- write a pixel from the trailing buf to the pixmap
			inPix[x] = curBufRow[x];

			// Get the vector to the source pixel
			srceMap = srce + (xGrad >> FIXED_BITS) + (yGrad >> FIXED_BITS) * inBytesPerRow + x;

			u = (yGrad & 0x1F);		// y - ysrc
			v = (xGrad & 0x1F);		// x - xsrc

			// P1 - P2
			// |     |
			// P3 - P4
			P1 = ((unsigned char*)srceMap)[0];
			P2 = ((unsigned char*)srceMap)[1];

			u1 = 0x20 - u;
			P1 *= u1;
			P2 *= u1;

			P3 = ((unsigned char*)srceMap)[inBytesPerRow];  
			P4 = ((unsigned char*)srceMap)[inBytesPerRow + 1];

			v1	= 0x20 - v;
			P3 *= u;
			P4 *= u;

			// Bilinear interpolation to approximate the source pixel value
			p  = v * (P2 + P4) + v1 * (P1 + P3);


			// Store the calculated/dest pixel (rounding the fraction part of the pix value)
			// We divide by (5+5) decimal places because p is units squared (5 places per decimal)
			curBufRow[x] = (char)((31 * p) >> 15);  // 5 more dec places cuz of the mult by 31
		}

		// Maintail the row-circular, trailing buffer
		curBufRowNum = (curBufRowNum + 1) % bufRows;
		inPix += inBytesPerRow;
		srce += inBytesPerRow;
	}

	// Flush the the trailing buf to the pixmap
	p = inX >> 2;
	for (y = 0; y < (unsigned)bufRows; y += 1)
	{
		curBufRow = sTemp + ((curBufRowNum + y) % bufRows) * inBytesPerRow;
		for (x = 0; x < p; x += 1)
		{
			((unsigned long*)inPix)[x] = ((unsigned long*)curBufRow)[x];
		}

		inPix	+= inBytesPerRow;
	}
}



int g_nPalette = -1;



inline void SetRGB(int R, int G, int B, DWORD& rgb)
{
	if (R < 0)
		R = 0;
	else if (R > 255)
		R = 255;

	if (G < 0)
		G = 0;
	else if (G > 255)
		G = 255;

	if (B < 0)
		B = 0;
	else if (B > 255)
		B = 255;

	rgb = (R << 16) | (G << 8) | (B);
}

#define __SET_RGB( R, G, B ) 	\
	SetRGB(R, G, B, outRGB); break;

void HSV2RGB(float H, float S, float V, DWORD& outRGB)
{
	// H is given on [0, 1] or WRAPPED. S and V are given on [0, 1]. 
	// RGB are each returned on [0, 1]. 
	long hexQuadrant, m, n, v; 
	H = ( H - floorf( H ) ) * 6.0f;  // Wrap the Hue angle around 1.0, then find quadrant

	hexQuadrant = (long)H; 
	float f = H - hexQuadrant; 

	// Check sat bounds
	if (S < 0.0f)
		S = 0.0f;
	if (S > 1.0f)
		S = 1.0f;

	// Check val bounds
	if (V < 0.0f)
		V = 0.0f;
	if (V > 1.0f)
		V = 1.0f;

	if (!(hexQuadrant & 1))
		f = 1.0f - f; // hexQuadrant i is even 

	V *= 255.0f;
	v = (long)V;
	m = (long)(V * (1.0f - S));
	n = (long)(V * (1.0f - S * f));

	switch ( hexQuadrant ) { 
	case 1: __SET_RGB( n, v, m ); 
	case 2: __SET_RGB( m, v, n ); 
	case 3: __SET_RGB( m, n, v ); 
	case 4: __SET_RGB( n, m, v ); 
	case 5: __SET_RGB( v, m, n ); 
	default: 
	__SET_RGB( v, n, m ); 
	}
} 

void GetHSV(int nPalette, float i, float t, float& H, float& S, float& V)
{
	g_nPalette = nPalette;

	switch (nPalette)
	{
	default:
		g_nPalette = 0;
		// FALL THOUGH

	case 0: // Firestorm
		H = 0.166f * powf(i, 1.9f);
		S = 1;
		V = powf(i, 0.9f);
		break;

	case 1: // Aqua
		H = 0.45f;
		S = powf((1.0f - i), 0.3f);
		V = powf(i, 0.7f);
		break;

	case 2: // Purple & Blues
		H = 0.666f + 0.166f * powf(i, 1.4f);
		S = 0.5f + 0.5f * powf((1.8f * i - 1.0f), 2);
		V = powf(i, 0.4f);
		break;

	case 3: // Color Wheel
		H = wrap(0.03f * t);
		S = 1.0f - 0.6f * powf(i, 2.5f);
		V = i;
		break;

	case 4: // Bizarro Mystery Unveiled
		H = wrap(0.3f - 0.003f * t);
		S = powf(i, 2.9f);
		V = powf(i, 0.9f);
		break;

	case 5: // Bizarro Color Wheel
		H = wrap(0.02f * t);
		S = 1.0f;
		V = 1.0f - powf(i, 1.4f);
		break;

	case 6: // Dark Rainbow
		H = powf(i, 1.6f);
		S = 1.0f;
		V = i;
		break;

	case 7: // Ice Nightshade
		H = wrap(powf((0.7f - 0.4f * i), 0.6f));
		S = 0.9f;
		V = powf(i, 0.9f);
		break;

	case 8: // Mystery Unveiled
		H = wrap(0.002f * t);
		S = powf((1.0f - i), 1.5f);
		V = powf(i, 0.4f);
		break;

	case 9: // Roundabout
		H = wrap(0.2f * (powf(i, 2.0f) + powf(cosf(4.0f * i), 2.0f)) + (0.05f * t));
		S = 0.85f + 0.5f * i;
		V = powf(i, 0.55f);
		break;
	}
}

int g_nBlendPalette;
float g_nBlendPaletteAmount;

#define timeToBlend (2.0f)

void MakePalette(DWORD outPalette [256])
{
	static XTIME nextPaletteChangeTime = 0.0f;
	static XTIME startBlendTime;

	int i;
	float H, S, V, inc = 1.0f / 255.0f;
	float mIntensity = 0.0f;
	XTIME now = XAppGetNow();

	if (now > nextPaletteChangeTime)
	{
		nextPaletteChangeTime = now + 10.0f + rnd(5.0f);
		g_nBlendPalette = g_nPalette;
		g_nPalette += 1;
		g_nBlendPaletteAmount = 0.0f;
		startBlendTime = now;
	}

	if (g_nBlendPalette != -1)
		TRACE(_T("Palette blend: %f\n"), g_nBlendPaletteAmount);

	for (i = 0; i < 256; i++, mIntensity += inc)
	{
		GetHSV(g_nPalette, mIntensity, (float) now, H, S, V);

		if (g_nBlendPalette != -1)
		{
			float H2, S2, V2;

			GetHSV(g_nBlendPalette, mIntensity, (float) now, H2, S2, V2);

			H = g_nBlendPaletteAmount * H + (1.0f - g_nBlendPaletteAmount) * H2;
			S = g_nBlendPaletteAmount * S + (1.0f - g_nBlendPaletteAmount) * S2;
			V = g_nBlendPaletteAmount * V + (1.0f - g_nBlendPaletteAmount) * V2;
		}

		HSV2RGB( H, S, V, outPalette[i]);
	}

	if (g_nBlendPalette != -1)
	{
		g_nBlendPaletteAmount += (float) (now - startBlendTime) / timeToBlend;
		if (g_nBlendPaletteAmount >= 1.0f)
			g_nBlendPalette = -1;
	}
}










#define __doXerr		error_term += dy;				\
						if ( error_term >= dx ) {		\
							error_term -= dx;			\
							basePtr += rowOffset;		\
							ymov--;						\
						}
						
						
#define __doYerr		error_term += dx;				\
						if ( error_term >= dy ) {		\
							error_term -= dy;			\
							basePtr += xDirection;		\
							xmov--;						\
						}

#define __circ( dia, a )	switch ( (dia) )		{									\
								case 2:		a = "\0\0"; break;							\
								case 3:		a = "\1\0\1"; break;						\
								case 4:		a = "\1\0\0\1"; break;						\
								case 5:		a = "\1\0\0\0\1"; break;					\
								case 6:		a = "\1\0\0\0\0\1"; break;					\
								case 7:		a = "\2\1\0\0\0\1\2"; break;				\
								case 8:		a = "\2\1\0\0\0\0\1\2"; break;				\
								case 9:		a = "\3\1\1\0\0\0\1\1\3"; break;			\
								case 10:	a = "\3\1\1\0\0\0\0\1\1\3"; break;			\
								case 11:	a = "\4\2\1\1\0\0\0\1\1\2\4"; break;		\
								case 12:	a = "\4\2\1\1\0\0\0\0\1\1\2\4"; break;		\
							}

void CSurfx::Line(int sx, int sy, int ex, int ey, unsigned char color)
{
	long xDirection, rowOffset, error_term;
	char* basePtr, *center;
	long xmov, ymov, dx, dy, t, j, lw;
	long penExtents;

	// Half the coordinte if it's large (we copy the sign bit in the 2^31 digit)
	// To do: use float clipping
	sx = ( ( (long) (sx & 0x80000000) ) >> 1 ) | ( sx & 0x3FFFFFFF );
	ex = ( ( (long) (ex & 0x80000000) ) >> 1 ) | ( ex & 0x3FFFFFFF );
	sy = ( ( (long) (sy & 0x80000000) ) >> 1 ) | ( sy & 0x3FFFFFFF );
	ey = ( ( (long) (ey & 0x80000000) ) >> 1 ) | ( ey & 0x3FFFFFFF );

	// Modify the line width so that the actual width matches mLineWidth
	lw = mLineWidth;	
	if ( mLineWidth > 3 ) {
		dx = ex - sx;	dx = dx * dx;
		dy = ey - sy;	dy = dy * dy;
		if ( dx > 0 && dx >= dy )
			lw = 128 + 55 * dy / dx; 			// 1/cos( atan( x ) ) is about 1+.43*x^2 from 0 to 1 (55 == .43 * 128)
		else if ( dy > 0 && dy > dx )
			lw = 128 + 55 * dx / dy; 			// 1/cos( atan( x ) ) is about 1+.43*x^2 from 0 to 1 (55 == .43 * 128)
		
		if ( dx > 0 || dy > 0 )
			lw = ( mLineWidth * lw + 64 ) >> 7;		// Add in order to round up
	}
	penExtents = lw >> 1;

	
	
	// Clipping: Set the pen loc to a point that's in and stop drawing once/if the pen moves out
	if ( sx < mClipRect.left + penExtents || sx >= mClipRect.right - penExtents || sy < mClipRect.top + penExtents || sy >= mClipRect.bottom - penExtents ) {

		// Exit if both points are out of bounds (wimpy clipping, eh?)
		if ( ex < mClipRect.left + penExtents || ex >= mClipRect.right - penExtents || ey < mClipRect.top + penExtents || ey >= mClipRect.bottom - penExtents )
			return;

		t = ex; ex = sx; sx = t;
		t = ey; ey = sy; sy = t;
	}
		
	dx = ex - sx;
	dy = ey - sy;

		
	// moving left or right?
	dx = ex - sx;
	xmov = dx;
	if ( dx < 0 ) {
		xmov = -dx;
		if ( sx - xmov < mClipRect.left + penExtents )
			xmov = sx - ( mClipRect.left + penExtents );
		xDirection = - 1;
		dx = -dx; }
	else if ( dx > 0 ) {
		if ( sx + xmov >= mClipRect.right - penExtents )
			xmov = mClipRect.right - penExtents - 1 - sx;
		xDirection = 1;  }
	else 
		xDirection = 0;


	// moving up or down?
	ymov = dy;
	if ( dy < 0 ) {
		ymov = -dy;
		if ( sy - ymov < mClipRect.top + penExtents )
			ymov = sy - ( mClipRect.top + penExtents );
		rowOffset = - mBytesPerRow;
		dy = -dy; }
	else {
		if ( sy + ymov >= mClipRect.bottom - penExtents )
			ymov = mClipRect.bottom - penExtents - sy - 1;
		rowOffset = mBytesPerRow; 
	} 

	// In Win32, everything's upside down
	#if EG_WIN
	sy = mY - sy - 1;
	ey = mY - ey - 1;
	rowOffset = - rowOffset;
	#endif	


	basePtr = (char*)m_pels + sy * mBytesPerRow + sx * 1;
	error_term = 0;
	
	long halfW;

	if ( lw > 1 )
	{
		// Make a circle for the pen
		long c_x, tw = mLineWidth;
		halfW = ( tw ) >> 1;
		
		if ( tw < 12 )
		{
			char* c_shape;
			__circ( tw, c_shape )
			for ( j = 0; j < tw; j++ )
			{
				long tmp = j - halfW;
				c_x = c_shape[ j ];
				center = basePtr + (j-halfW) * mBytesPerRow;
				for ( int k = c_x; k < tw - c_x; k++ )
				{
					((unsigned char*) center)[k-halfW] = color;
				}
			}
		}
		else
		{		
		
			for ( j = 0; j < tw; j++ )
			{
				long tmp = j - halfW;
				c_x = halfW - ( ( long ) sqrt( halfW * halfW - tmp * tmp ) );
				center = basePtr + (j-halfW) * mBytesPerRow;
				for ( int k = c_x; k < tw - c_x; k++ )
				{
					((unsigned char*) center)[k-halfW] = color;
				}
			}
		}
		
		
		halfW = lw >> 1;

		// Draw the line
		if ( dx > dy )
		{
			// Start counting off in x
			for ( ; xmov >= 0 && ymov >= 0; xmov-- )
			{
				// Draw the vertical leading edge of the pen
				center = basePtr - halfW * mBytesPerRow;
				for ( j = 0; j < lw; j++ )
				{
					*((unsigned char*) center) = color;
					center += mBytesPerRow;
				}

				basePtr += xDirection;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doXerr
			}
		}
		else
		{
			// Start counting off in y
			for ( ; ymov >= 0 && xmov >= 0; ymov-- )
			{
				// Draw the horizontal leading edge of the pen
				center = basePtr - ( halfW ) * 1;
				for ( j = 0; j < lw; j++ )
				{
					*((unsigned char*) center) = color;
					center += 1;
				}
				basePtr += rowOffset;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doYerr
			}
		}
	}
	else
	{
		// Draw the (single pixel) line
		if ( dx >= dy )
		{
			// Start counting off in x
			for ( ; xmov >= 0 && ymov >= 0; xmov-- )
			{
				*((unsigned char*) basePtr) = color;
				
				basePtr += xDirection;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doXerr
			}
		}
		else
		{
			// Start counting off in y
			for ( ; ymov >= 0 && xmov >= 0; ymov-- )
			{
			

				*((unsigned char*) basePtr) = color;
				basePtr += rowOffset;

				// Check to see if we need to move the pixelOffset in the y direction.
				__doYerr
			}
		}
	}
}
