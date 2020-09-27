#include "std.h"
#include "xapp.h"
#include "surfx.h"





#define DEC_SIZE 5

DeltaField::DeltaField()
{
	mWidth = mHeight = 0;
	mCurrentY = -1;
	m_nStyle = 0;
}

DeltaFieldData::DeltaFieldData()
{
	mNegYExtents = 0;
	mField = NULL;
}

DeltaFieldData::~DeltaFieldData()
{
	delete [] mField;
}


DeltaFieldData* DeltaField::GetField()
{
	bool didCalc = false;

	if (mCurrentY >= 0)
	{
		while (!IsCalculated())
			CalcSome();

		return &mFieldData;
	}

	return NULL;
}


void DeltaField::Assign()
//bcvoid DeltaField::Assign(ArgList& inArgs, UtilStr& inName)
{
//bc	UtilStr fx, fy;

//bc	mName.Assign( inName );

	// Compile and link the temp exprs.  By spec, A-vars are evaluated now
//bc	mAVars.Compile( inArgs, 'A', mDict );
//bc	mAVars.Evaluate();		

//bc	mDVars.Compile( inArgs, 'D', mDict );

//bc	mAspect1to1	= inArgs.GetArg( 'Aspc' );
//bc	mPolar		= inArgs.ArgExists( 'srcR' );

	mAspect1to1 = true;
	mPolar = true;

	// Compile the 2D vector field that expresses the source point for a given point
/*bc
	if (mPolar)
	{
		inArgs.GetArg('srcR', fx);
		inArgs.GetArg('srcT', fy);
	}
	else
	{
		inArgs.GetArg('srcX', fx);
		inArgs.GetArg('srcY', fy);
	}

	mXField.Compile( fx, mDict );
	mYField.Compile( fy, mDict );

	mHasRTerm		= mXField.IsDependent( "R" )		|| mYField.IsDependent( "R" );
	mHasThetaTerm	= mXField.IsDependent( "THETA" )	|| mYField.IsDependent( "THETA" );
*/

	mHasRTerm = true;
	mHasThetaTerm = true;

	// Reset all computation of this delta field	
	SetSize(mWidth, mHeight, true);
}


void DeltaField::SetSize(long inWidth, long inHeight, bool inForceRegen)
{
	// Only resize if the new size is different...
	if (inWidth != mWidth || inHeight != mHeight || inForceRegen)
	{
		mWidth = inWidth;
		mHeight = inHeight;

		// Each pixel needs 3 bytes of info
//bc		mFieldData.mField = mTempMem.Dim( 3 * mWidth * mHeight + 64 );
		delete [] mFieldData.mField;
		mFieldData.mField = new char [3 * mWidth * mHeight + 64];

		mXScale = 2.0f / ((float)mWidth);
		mYScale = 2.0f / ((float)mHeight);

		// If we're to keep the xy aspect ratio to 1, change the dim that will get stretched
		if (mAspect1to1)
		{
			if (mYScale > mXScale)
				mXScale = mYScale;
			else
				mYScale = mXScale;
		}

		// Reset all computation of this delta field
		mCurrentY = 0;
		mNegYExtents = 0;

		// The current implementation of PixPort for Win32 flips all the Y Cords
		mYScale *= -1;
	}
}



#define __encode(x, y)	sx = x + 0x7E0;	/* 0b0111 1110 0000 */			\
						sy = y + 0x7E0; 								\
						*((unsigned long*)g) = (sx << 12) | (sy);		\
						g += 3;

int DeltaField::CalcSome()
{
	float xscale2, yscale2, r, fx, fy;
	long px, sx, sy, t;
	char* g;

	// Calc where we left off
	g = mFieldData.mField + 3 * mWidth * mCurrentY;

	// If we're computing the 1st or last row...
	if (mCurrentY == 0 || mCurrentY == mHeight - 1)
	{
		// Calc the mCurrentY row of the grad field
		for ( px = 0; px < mWidth; px++ )
		{
			__encode(0, 0)
		} 

		mCurrentY++; 
	}

	// If we're still have stuff left to compute...
	if (mCurrentY > 0 && mCurrentY < mHeight - 1)
	{
		// Calc the y we're currently at
		mY_Cord = 0.5f * mYScale * (mHeight - 2 * mCurrentY);


		// Save some cycles be pre-computing indep stuff
		xscale2 = ((float)(1 << DEC_SIZE)) / mXScale;
		yscale2 = ((float)(1 << DEC_SIZE)) / mYScale;

		// The first and last pixels stay the background color
		__encode(0, 0)

		// Calc the mCurrentY row of the grad field
		for (px = 1; px < mWidth - 1; px++)
		{
			mX_Cord = 0.5f * mXScale * (2 * px - mWidth);

			// Calculate R and THETA only if the field uses it (don't burn cycles on sqrt() and atan())
			if (mHasRTerm)
				mR_Cord = sqrtf(mX_Cord * mX_Cord + mY_Cord * mY_Cord);
			if (mHasThetaTerm)
				mT_Cord = atan2f(mY_Cord, mX_Cord);

			// Evaluate any temp variables
//bc			mDVars.Evaluate();	

			// Evaluate the source point for (mXCord, mYCord)
//bc			fx = mXField.Evaluate();
//bc			fy = mYField.Evaluate();

			GetXY(mR_Cord, mT_Cord, mX_Cord, mY_Cord, fx, fy, mPolar, px == 1 && mCurrentY == 1);

			if (mPolar)
			{
				r = fx;
				fx = r * cosf(fy);
				fy = r * sinf(fy);
			}

			sx = (long)(xscale2 * (fx - mX_Cord));
			sy = (long)(yscale2 * (mY_Cord - fy));

			// Clip the x source coordinate
			t = px + (sx >> DEC_SIZE);
			if (t >= mWidth - 1)
				sx = ((mWidth - 1 - px) << DEC_SIZE);
			else if (t < 0)
				sx = ((-px) << DEC_SIZE);

			// Clip the y source coordinate
			t = mCurrentY + (sy >> DEC_SIZE);
			if (t >= mHeight - 1)
				sy = ((mHeight - 1 - mCurrentY) << DEC_SIZE);
			else if (t < 0)
				sy = ((-mCurrentY) << DEC_SIZE);

			// This lets us know how 
			if (sy < mNegYExtents)
				mNegYExtents = sy;

			// Encode the gradient
			__encode(sx, sy)
		}

		// The first and last pixels stay the background color
		__encode(0, 0)

		// Signal the compution of the next row
		mCurrentY += 1;
	}

	if (IsCalculated())
	{
		// Tell PixPort how big the trailing buffer has to be
		mFieldData.mNegYExtents = 1 - ( mNegYExtents >> DEC_SIZE );
	}

	return mHeight - mCurrentY;
}


void DeltaField::GetXY(float r, float theta, float x, float y, float& X, float& Y, bool& bPolar, bool bInit)
{
	static float A0, A1, A2;
	bPolar = true;

	switch (m_nStyle)
	{
	default:
		m_nStyle = 0;
		// FALL THROUGH

	case 0: // Radial Breakaway
		X = r * (1.0f + 0.16f * atanf(0.55f - r));
		Y = theta - 0.01f;
		break;

	case 1: // Hip-no-therapy
		X = r * 0.87f;
		Y = theta - 0.075f;
		break;

	case 2: // Sunburst - Many
		if (bInit)
			A0 = trunc(8.0f * rnd(5.0f)) * 3.141592653f;
		X = ((1.0f + sinf(A0 * theta)) * 0.5f * 0.06f + 0.92f) * r;
		Y = theta;
		break;

	case 3: // Theta Divergence
		if (bInit)
		{
			A0 = 2.0f + rnd(16.0f);
			A1 = 0.01f + rnd(0.05f);
			A2 = 0.002f + rnd(0.006f);
		}

		X = pos(r - A2);
		Y = theta + A1 * sinf(A0 * r);
		break;

	case 4: // Turbo Flow Out
		X = 0.87f * r;
		Y = theta - 0.009f;
		break;

	case 5: // Turbo Flow Out - More
		X = 0.8f * r;
		Y = theta - 0.008f;
		break;

	case 6: // Boxilite
		if (bInit)
		{
			A0 = 2.0f + rnd(16.0f);
			A1 = 0.01f + rnd(0.05f);
		}

		X = x + A1 * sinf(A0 * y);
		Y = y + A1 * sinf(A0 * x);

		bPolar = false;
		break;

	case 7: // Collapse & Turn
		X = 1.01f * r;
		Y = theta + 0.021f;
		break;

	case 8: // Constant Out
		X = r - 0.01f;
		Y = theta;
		break;

	case 9: // Directrix Expand - X
		if (bInit)
		{
			A0 = rnd(0.0045f);
			A1 = 0.10f + rnd(0.06f);
			A2 = 0.003f + rnd(0.005f);
		}

		X = (0.99f - A1 * powf(fabsf(x), 1.3f)) * r - rnd(A0);
		Y = theta - A2;
		break;

	case 10: // Directrix Expand - Y
		if (bInit)
		{
			A0 = rnd(0.0045f); // noise offset
			A1 = 0.10f + rnd(0.06f); // scale of outflow
			A2 = 0.003f + rnd(0.006f); // rotation factor
		}

		X = (0.99f - A1 * powf(fabsf(y), 1.3f)) * r - rnd(A0);
		Y = theta + A2;
		break;

	case 11: // Equalateral Hyperbola
		X = r - 0.18f * x * y;
		Y = theta - 0.005f;
		break;

	case 12: // Expand & Turn
		X = 0.96f * r;
		Y = theta - 0.021f;
		break;

	case 13: // Gravity
		X = x + rnd(0.01f) - 0.005f;
		Y = y - rnd(0.01f) - 0.005f - (y - 1.3f) * 0.04f;
		bPolar = false;
		break;

	case 14: // In or Out, Inner Turn
		if (bInit)
			A0 = 0.95f + rnd(0.06f);

		X = r * A0;
		Y = theta - pos(1.0f - r) * 0.035f;
		break;

	case 15: // Left turn & Flow Out
		X = 0.94f * r;
		Y = theta - 0.007f;
		break;

	case 16: // Linear Spread
		X = 0.9f * x + rnd(0.008f) - 0.004f;
		Y = 0.9f * y + rnd(0.008f) - 0.004f;
		bPolar = false;
		break;

	case 17: // Noise Field
		X = x + 0.1f * (rnd(2.0f) - 1.0f);
		Y = y + 0.1f * (rnd(2.0f) - 1.0f);
		bPolar = false;
		break;

	case 18: // Right Turn
		X = r * 0.99f;
		Y = theta + 0.009f;
		break;

	case 19: // Scattered Flow Out
		X = (0.92f + rnd(0.05f)) * r;
		Y = theta + 0.003f;
		break;

	case 20: // Simple Sine-Sphere
		X = r + 0.04f * sinf(6.2831853f * r);
		Y = theta + 0.015f;
		break;

	case 21: // Sine Multi-Circ
		X = r * (0.87f + 0.05f * (1.0f + sinf(r * 15.0f)));
		Y = theta;
		break;

	case 22: // Sphere
		if (bInit)
		{
			A0 = rnd(0.1f); // osc magnitde
			A1 = 3.0f + rnd(7.0f); // osc freq
			A2 = 0.9f + rnd(0.3f); // Sphere radius
		}

		X = r * (1.0f + 0.13f * (r - A2));
		Y = theta + A0 * sinf(A1 * r);
		break;

	case 23: // Sunburst - Few
		if (bInit)
			A0 = trunc(1.0f + rnd(2.1f)) * 3.141592653f;

		X = ((1.0f + cosf(A0 * theta)) * 0.5f * 0.1f + 0.89f) * r;
		Y = theta + 0.005f;
		break;
	}
}
