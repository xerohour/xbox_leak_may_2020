//-----------------------------------------------------------------------------
//  
//  File: axisangle.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Axis-angle curve to quaternion curve conversion.
//-----------------------------------------------------------------------------
#include "stdafx.h"

inline FLOAT Lerp(FLOAT A, FLOAT B, FLOAT fLerpValue)
{
	return A + fLerpValue * (B - A);
}

//////////////////////////////////////////////////////////////////////
// Set quaternion keys to match axis/angle rotation, with reasonable precision.
//
// NOTE: The other alternative is to add axis/angle animated rotation
// to AnimRotate, but this makes the animation package more complex.
//
HRESULT MatchAxisAngleCurve(AnimRotate *pRotate, CONST D3DXVECTOR3 *pvAxis, CONST AnimCurve *pAngle, FLOAT feps)
{
	// clear arrays and copy over the curve duration, etc.
	if (pRotate->m_curve.m_rKeyTimes) delete pRotate->m_curve.m_rKeyTimes;
	if (pRotate->m_curve.m_rKeys) delete pRotate->m_curve.m_rKeys;
	pRotate->m_curve.m_fBegin = pAngle->m_fBegin;
    pRotate->m_curve.m_fDurationInverse = pAngle->m_fDurationInverse;
	pRotate->m_curve.m_KeyCount = 0;
	pRotate->m_curve.m_KeySize = 4;
	pRotate->m_curve.m_rKeyTimes = NULL;
	pRotate->m_curve.m_rKeys = NULL;
	
	// Set the quaternion directly if there's just one key
	if (pAngle->m_KeyCount == 1)
	{
		pRotate->m_curve.m_KeyCount = 1;
		pRotate->m_curve.m_rKeyTimes = new FLOAT [ 1 ];
		pRotate->m_curve.m_rKeyTimes[0] = pAngle->m_rKeyTimes[0];
		pRotate->m_curve.m_rKeys = new FLOAT [ 1 * 4 ];
		D3DXQuaternionRotationAxis((D3DXQUATERNION *)pRotate->m_curve.m_rKeys, pvAxis, pAngle->m_rKeys[0] );
		return S_OK;
	}

	// Scratch space
	const int MAX_KEYS = 100;
	if (pAngle->m_KeyCount > MAX_KEYS)
		return E_NOTIMPL;
	FLOAT rKeyTimes[MAX_KEYS];
	D3DXQUATERNION rKeys[MAX_KEYS];
	UINT KeyCount = 0;
	
	// For each segment of the input curve, start by setting the quaternion keys
	// to be equal to the input curve, then keep on increasing the number of samples
	// until the desired precision is matched.
	FLOAT fTime0 = pAngle->m_rKeyTimes[0];
	FLOAT fAngle0 = pAngle->m_rKeys[0];
	rKeyTimes[ 0 ] = fTime0;
	D3DXQuaternionRotationAxis(&rKeys[0], pvAxis, fAngle0);
	KeyCount = 1;
	CONST UINT nSample = 10;	// intermediate samples to test quality of match
	CONST FLOAT fLerpSampleScale = 1.f / (nSample - 1);
	CONST FLOAT fFractionMin = 1e-6f;
	for (UINT iKeyAngle = 1; iKeyAngle < pAngle->m_KeyCount; iKeyAngle++)
	{
		FLOAT fFraction = 1.f;	// start with whole interval
		
	try_again: // goto makes restarting from within for-loop easier
		if (KeyCount >= MAX_KEYS)
			return E_NOTIMPL;	// too many keys needed to get desired precision
		FLOAT fTime1 = Lerp(fTime0, pAngle->m_rKeyTimes[ iKeyAngle ], fFraction);
		FLOAT fAngle1 = Lerp(fAngle0, pAngle->m_rKeys[ iKeyAngle ], fFraction);
		rKeyTimes[ KeyCount ] = fTime1;
		D3DXQuaternionRotationAxis(&rKeys[ KeyCount ], pvAxis, fAngle1);

		// Check the error at a few intermediate points
		for (UINT iSample = 1; iSample < nSample; iSample++)
		{
			FLOAT fLerp = (FLOAT)iSample * fLerpSampleScale;
			FLOAT fAngle = fAngle0 + fLerp * (fAngle1 - fAngle0);
			D3DXQUATERNION A;
			D3DXQuaternionRotationAxis(&A, pvAxis, fAngle);
			D3DXQUATERNION B;
			D3DXQuaternionSlerp(&B, &rKeys[ KeyCount - 1 ], &rKeys[ KeyCount ], fLerp);
			D3DXQUATERNION C = B - A;
			FLOAT fError = D3DXQuaternionLengthSq(&C);
			if (fError > feps)
			{
				fFraction *= 0.5f;	// try again with a smaller fraction
				if (fFraction < fFractionMin)
					return E_FAIL;	// Could not find a good quaternion key, no matter how small the fraction.
				goto try_again;
			}
		}

		// Error was small enough at all the intermediate points, so keep the key
		KeyCount++;
		fTime0 = fTime1;
		fAngle0 = fAngle1;
		if (fFraction < 1.f)
		{
			fFraction = 1.f;
			goto try_again; // try again to span all the way to current key
		}
	}

	// Copy keys to quaternion curve
	pRotate->m_curve.m_KeyCount = KeyCount;
	pRotate->m_curve.m_KeySize = 4;
	pRotate->m_curve.m_rKeyTimes = new FLOAT [ KeyCount ];
	pRotate->m_curve.m_rKeys = new FLOAT [ KeyCount * 4 ];
	if (pRotate->m_curve.m_rKeyTimes == NULL
		|| pRotate->m_curve.m_rKeys == NULL)
		return E_OUTOFMEMORY;
	memcpy(pRotate->m_curve.m_rKeyTimes, rKeyTimes, sizeof(FLOAT) * KeyCount );
	memcpy(pRotate->m_curve.m_rKeys, rKeys, sizeof(FLOAT) * KeyCount * 4);
	return S_OK;
}


