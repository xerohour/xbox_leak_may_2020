//-----------------------------------------------------------------------------
//  
//  File: anim.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//
//  Key frame animation based on SkinnedMesh DirectX sample.
//-----------------------------------------------------------------------------
#include "anim.h"

//////////////////////////////////////////////////////////////////////
// Look up keys based on local time
//
HRESULT AnimCurve::FindKeys(float fLocalTime, UINT *piKey1, UINT *piKey2, FLOAT *pfLerpValue) CONST
{
    UINT iKey;
    UINT iKey1;
    UINT iKey2;
	iKey1 = iKey2 = 0;
	FLOAT fTime = (float)fmod(fLocalTime, m_rKeyTimes[m_KeyCount-1]);
	if (fTime < 0.f) fTime += m_rKeyTimes[m_KeyCount-1];
	for (iKey = 0 ;iKey < m_KeyCount ; iKey++)
	{
		if (m_rKeyTimes[iKey] > fTime)
		{
			iKey2 = iKey;
			
			if (iKey > 0)
			{
				iKey1= iKey - 1;
			}
			else  // when iKey == 0, then iKey1 == 0
			{
				iKey1 = iKey;
			}
			
			break;
		}
	}
	FLOAT fTime1 = (float)m_rKeyTimes[iKey1];
	FLOAT fTime2 = (float)m_rKeyTimes[iKey2];
	FLOAT fLerpValue;
	if ((fTime2 - fTime1) ==0)
		fLerpValue = 0;
	else
		fLerpValue =  (fTime - fTime1)  / (fTime2 - fTime1);
	
	*piKey1 = iKey1;
	*piKey2 = iKey2;
	*pfLerpValue = fLerpValue;
	return S_OK;
}
	
//////////////////////////////////////////////////////////////////////
// Find the bracketing key times, then lerp the values
//
HRESULT AnimCurve::Sample(FLOAT fLocalTime, FLOAT *rfValue) CONST
{
	if (m_KeyCount == 0)
		return E_FAIL;
	if (m_KeyCount == 1)
	{
		for (UINT iValue = 0; iValue < m_KeySize; iValue++)
			rfValue[iValue] = m_rKeys[iValue];
		return S_OK;
	}
	UINT iKey1, iKey2;
	FLOAT fLerpValue;
	HRESULT hr = FindKeys(fLocalTime, &iKey1, &iKey2, &fLerpValue);
	if (FAILED(hr))
		return hr;
	FLOAT *rfKey1 = m_rKeys + iKey1 * m_KeySize;
	FLOAT *rfKey2 = m_rKeys + iKey2 * m_KeySize;
	for (UINT iValue = 0; iValue < m_KeySize; iValue++)
		rfValue[iValue] = rfKey1[iValue] + fLerpValue * (rfKey2[iValue] - rfKey1[iValue]);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Look up matrix key based on local time
//
HRESULT AnimMatrix::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
    if (m_curve.m_rKeys == NULL
		|| m_curve.m_KeyCount == 0)
	{
		D3DXMatrixIdentity(pMatrix);
		return S_OK;
	}
	if (m_curve.m_KeyCount == 1)
	{
		memcpy(&pMatrix->m[0][0], m_curve.m_rKeys, 16 * sizeof(FLOAT));
		return S_OK;
	}
	FLOAT fLocalTime = m_curve.LocalTime(fGlobalTime);
	UINT iKey, iKey1, iKey2;
	FLOAT fLerpValue;
	m_curve.FindKeys(fLocalTime, &iKey1, &iKey2, &fLerpValue);
	
	// Instead of lerping between the matrices, switch
	// to the next matrix when past the half-way mark.
	if (fLerpValue > 0.5)
	{
		iKey = iKey2;
	}
	else
	{
		iKey = iKey1;
	}
	memcpy(&pMatrix->m[0][0], m_curve.m_rKeys + 16 * iKey, 16 * sizeof(FLOAT));
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Interpolate between nearest rotate keys
//
HRESULT AnimRotate::SetTime(D3DXQUATERNION *pQuaternion, float fGlobalTime)
{
	if (m_curve.m_rKeys == NULL
		|| m_curve.m_KeyCount == 0)
	{
		D3DXQuaternionIdentity(pQuaternion);
		return S_OK;
	}
	if (m_curve.m_KeyCount == 1)
	{
		*pQuaternion = *(D3DXQUATERNION *)m_curve.m_rKeys;
		return S_OK;
	}
	FLOAT fLocalTime = m_curve.LocalTime(fGlobalTime);
	UINT iKey1, iKey2;
	FLOAT fLerpValue;
	m_curve.FindKeys(fLocalTime, &iKey1, &iKey2, &fLerpValue);
//#define USE_SQUAD
#ifdef USE_SQUAD
	int i1 = (int)iKey1;
	int i2 = (int)iKey2;
	int i0 = i1 - 1;
	int i3 = i2 + 1;
	if(i0 < 0)
		i0 += m_curve.m_KeyCount;
	if(i3 >= (INT) m_curve.m_KeyCount)
		i3 -= m_curve.m_KeyCount;
	D3DXQUATERNION qA, qB, qC;
	D3DXQuaternionSquadSetup(&qA, &qB, &qC, 
							 (D3DXQUATERNION *)m_curve.m_rKeys + i0,
							 (D3DXQUATERNION *)m_curve.m_rKeys + i1,
							 (D3DXQUATERNION *)m_curve.m_rKeys + i2,
							 (D3DXQUATERNION *)m_curve.m_rKeys + i3);
	D3DXQuaternionSquad(pQuaternion, (D3DXQUATERNION *)m_curve.m_rKeys + i1, &qA, &qB, &qC, fLerpValue);
#else
	D3DXQuaternionSlerp(pQuaternion,
						(D3DXQUATERNION *)m_curve.m_rKeys + iKey1,
						(D3DXQUATERNION *)m_curve.m_rKeys + iKey2,
						fLerpValue);
#endif
	return S_OK;
}

HRESULT AnimRotate::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXQUATERNION quat;
	HRESULT hr = SetTime(&quat, fGlobalTime);
//	quat.w = -quat.w;
	D3DXMatrixRotationQuaternion(pMatrix, &quat);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Interpolate between two rotate animations
//
HRESULT AnimRotateBlend::SetTime(D3DXQUATERNION *pRotate, float fGlobalTime)
{
	D3DXQUATERNION quatA;
	HRESULT hr = m_pAnimRotateA->SetTime(&quatA, fGlobalTime);
	if (FAILED(hr))
		return hr;
	
	D3DXQUATERNION quatB;
	hr = m_pAnimRotateB->SetTime(&quatB, fGlobalTime);
	if (FAILED(hr))
		return hr;

	D3DXQuaternionSlerp(pRotate, &quatA, &quatB, m_fBlend);
	return S_OK;
}

HRESULT AnimRotateBlend::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXQUATERNION quat;
	HRESULT hr = SetTime(&quat, fGlobalTime);
//	quat.w = -quat.w;
	D3DXMatrixRotationQuaternion(pMatrix, &quat);
	return hr;
}



//////////////////////////////////////////////////////////////////////
// Interpolate between nearest translation keys
//
HRESULT AnimTranslate::SetTime(D3DXVECTOR3 *pTranslate, float fGlobalTime)
{
	if (m_curve.m_rKeys == NULL
		|| m_curve.m_KeyCount == 0)
	{
		pTranslate->x = pTranslate->y = pTranslate->z = 0.f;
		return S_OK;
	}
	if (m_curve.m_KeyCount == 1)
	{
		*pTranslate = *(D3DXVECTOR3 *)m_curve.m_rKeys;
		return S_OK;
	}
	FLOAT fLocalTime = m_curve.LocalTime(fGlobalTime);
	UINT iKey1, iKey2;
	FLOAT fLerpValue;
	m_curve.FindKeys(fLocalTime, &iKey1, &iKey2, &fLerpValue);
	D3DXVec3Lerp(pTranslate,
				 (D3DXVECTOR3 *)m_curve.m_rKeys + iKey1,
				 (D3DXVECTOR3 *)m_curve.m_rKeys + iKey2,
				 fLerpValue);
	return S_OK;
}

HRESULT AnimTranslate::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXVECTOR3 vTranslate;
	HRESULT hr = SetTime(&vTranslate, fGlobalTime);
	D3DXMatrixTranslation(pMatrix, vTranslate.x, vTranslate.y, vTranslate.z);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Interpolate between two translate animations
//
HRESULT AnimTranslateBlend::SetTime(D3DXVECTOR3 *pTranslate, float fGlobalTime)
{
	D3DXVECTOR3 vTranslateA;
	HRESULT hr = m_pAnimTranslateA->SetTime(&vTranslateA, fGlobalTime);
	if (FAILED(hr))
		return hr;
	
	D3DXVECTOR3 vTranslateB;
	hr = m_pAnimTranslateB->SetTime(&vTranslateB, fGlobalTime);
	if (FAILED(hr))
		return hr;

	D3DXVec3Lerp(pTranslate, &vTranslateA, &vTranslateB, m_fBlend);
	return S_OK;
}

HRESULT AnimTranslateBlend::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXVECTOR3 vTranslate;
	HRESULT hr = SetTime(&vTranslate, fGlobalTime);
	D3DXMatrixTranslation(pMatrix, vTranslate.x, vTranslate.y, vTranslate.z);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Interpolate between nearest scale keys
//
HRESULT AnimScale::SetTime(D3DXVECTOR3 *pScale, float fGlobalTime)
{
	if (m_curve.m_rKeys == NULL
		|| m_curve.m_KeyCount == 0)
	{
		pScale->x = pScale->y = pScale->z = 1.f;
		return S_OK;
	}
	if (m_curve.m_KeyCount == 1)
	{
		*pScale = *(D3DXVECTOR3 *)m_curve.m_rKeys;
		return S_OK;
	}
	FLOAT fLocalTime = m_curve.LocalTime(fGlobalTime);
	UINT iKey1, iKey2;
	FLOAT fLerpValue;
	m_curve.FindKeys(fLocalTime, &iKey1, &iKey2, &fLerpValue);
	D3DXVec3Lerp(pScale,
				 (D3DXVECTOR3 *)m_curve.m_rKeys + iKey1,
				 (D3DXVECTOR3 *)m_curve.m_rKeys + iKey2,
				 fLerpValue);
	return S_OK;
}

HRESULT AnimScale::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXVECTOR3 vScale;
	HRESULT hr = SetTime(&vScale, fGlobalTime);
	D3DXMatrixScaling(pMatrix, vScale.x, vScale.y, vScale.z);
	return hr;
}

//////////////////////////////////////////////////////////////////////
// Interpolate between two scale animations
//
HRESULT AnimScaleBlend::SetTime(D3DXVECTOR3 *pScale, float fGlobalTime)
{
	D3DXVECTOR3 vScaleA;
	HRESULT hr = m_pAnimScaleA->SetTime(&vScaleA, fGlobalTime);
	if (FAILED(hr))
		return hr;
	
	D3DXVECTOR3 vScaleB;
	hr = m_pAnimScaleB->SetTime(&vScaleB, fGlobalTime);
	if (FAILED(hr))
		return hr;

	D3DXVec3Lerp(pScale, &vScaleA, &vScaleB, m_fBlend);
	return hr;
}

HRESULT AnimScaleBlend::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	D3DXVECTOR3 vScale;
	HRESULT hr = SetTime(&vScale, fGlobalTime);
	D3DXMatrixScaling(pMatrix, vScale.x, vScale.y, vScale.z);
	return hr;
}



#if ANIM_INTERPRETED

//////////////////////////////////////////////////////////////////////
// For writing to a file, we need to compute the size of the key data.
//
DWORD AnimCurve::GetSizeKeys() CONST
{
	return m_KeyCount * (1 /* keyTimes */ + m_KeySize) * sizeof(FLOAT);
}

//////////////////////////////////////////////////////////////////////
// Write the keys to the given memory location, then adjust
// pointer offsets to be relative to pBase, i.e. pDest - pBase.
// The number of bytes written must match the value returned by
// GetSizeKeys().
//
HRESULT AnimCurve::RelocateCopyKeys(BYTE *pDest, const BYTE *pBase) CONST
{
	UINT KeyTimesSize = sizeof(FLOAT) * m_KeyCount;
	memcpy(pDest, m_rKeyTimes, KeyTimesSize);
	*(DWORD *)&m_rKeyTimes = pDest - pBase;	// convert to file offset
	pDest += KeyTimesSize;
	memcpy(pDest, m_rKeys, sizeof(FLOAT) * m_KeyCount * m_KeySize);
	*(DWORD *)&m_rKeys = pDest - pBase;	        // convert to file offset
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Patch file offsets to memory offsets, i.e. convert file offsets
// to pointers relative to pBase
//
HRESULT AnimCurve::PatchKeys(const BYTE *pBase)
{
	*(DWORD *)&m_rKeyTimes += (DWORD)pBase;	// convert to pointer
	*(DWORD *)&m_rKeys += (DWORD)pBase;	    // convert to pointer
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Compute the transformation matrix based on the current animation keys.
HRESULT AnimFrame::SetTime(D3DXMATRIX *pMatrix, float fGlobalTime)
{
	switch (m_Type)
	{
	case ANIM_MATRIX: 			return ((AnimMatrix         *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_ROTATE: 			return ((AnimRotate         *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_TRANSLATE: 		return ((AnimTranslate      *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_SCALE: 			return ((AnimScale          *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_ROTATEBLEND:		return ((AnimRotateBlend    *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_TRANSLATEBLEND:	return ((AnimTranslateBlend *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_SCALEBLEND: 		return ((AnimScaleBlend     *)this)->SetTime(pMatrix, fGlobalTime);
	case ANIM_CONCAT: 			return ((AnimConcat         *)this)->SetTime(pMatrix, fGlobalTime);
	default:
		return E_NOTIMPL;
	}
}

//////////////////////////////////////////////////////////////////////
// For writing to a file, we need to compute the size of the
// structure data and the size of the key data.
//
DWORD AnimFrame::GetSize()
{
	DWORD Size;
	switch (m_Type)	{
	case ANIM_MATRIX:
	{
		AnimMatrix *pAnimMatrix = (AnimMatrix *)this;
		Size = sizeof(AnimMatrix);
		Size += pAnimMatrix->m_curve.GetSizeKeys();
		return Size;
	}
	case ANIM_ROTATE:
	{
		AnimRotate *pAnimRotate = (AnimRotate *)this;
		Size = sizeof(AnimRotate);
		Size += pAnimRotate->m_curve.GetSizeKeys();
		return Size;
	}
	case ANIM_TRANSLATE:
	{
		AnimTranslate *pAnimTranslate = (AnimTranslate *)this;
		Size = sizeof(AnimTranslate);
		Size += pAnimTranslate->m_curve.GetSizeKeys();
		return Size;
	}
	case ANIM_SCALE:
	{
		AnimScale *pAnimScale = (AnimScale *)this;
		Size = sizeof(AnimScale);
		Size += pAnimScale->m_curve.GetSizeKeys();
		return Size;
	}
	case ANIM_ROTATEBLEND:
	{
		AnimRotateBlend *pAnimRotateBlend = (AnimRotateBlend *)this;
		Size = sizeof(AnimRotateBlend);
		Size += pAnimRotateBlend->m_pAnimRotateA->GetSize();
		Size += pAnimRotateBlend->m_pAnimRotateB->GetSize();
		return Size;
	}
	case ANIM_TRANSLATEBLEND:
	{
		AnimTranslateBlend *pAnimTranslateBlend = (AnimTranslateBlend *)this;
		Size = sizeof(AnimTranslateBlend);
		Size += pAnimTranslateBlend->m_pAnimTranslateA->GetSize();
		Size += pAnimTranslateBlend->m_pAnimTranslateB->GetSize();
		return Size;
	}
	case ANIM_SCALEBLEND:
	{
		AnimScaleBlend *pAnimScaleBlend = (AnimScaleBlend *)this;
		Size = sizeof(AnimScaleBlend);
		Size += pAnimScaleBlend->m_pAnimScaleA->GetSize();
		Size += pAnimScaleBlend->m_pAnimScaleB->GetSize();
		return Size;
	}
	case ANIM_CONCAT:
	{
		AnimConcat *pAnimConcat = (AnimConcat *)this;
		Size = sizeof(AnimConcat);
		Size += pAnimConcat->m_pAnimA->GetSize();
		Size += pAnimConcat->m_pAnimB->GetSize();
		return Size;
	}
	default:
		return 0;	// E_NOTIMPL
	}
}

//////////////////////////////////////////////////////////////////////
// Write the expression to the given memory location, adjusting
// pointer offsets to be relative to pBase, i.e. pDest - pBase.
// The number of bytes written must match the value returned by
// GetSize().
//
HRESULT AnimFrame::RelocateCopy(BYTE *pDest, const BYTE *pBase)
{
	HRESULT hr;
	switch (m_Type) {
	case ANIM_MATRIX:
	{
		AnimMatrix *pAnimMatrix = (AnimMatrix *)pDest;
		*pAnimMatrix = *(AnimMatrix *)this;
		pDest += sizeof(AnimMatrix);
		return pAnimMatrix->m_curve.RelocateCopyKeys(pDest, pBase);
	}
	case ANIM_ROTATE:
	{
		AnimRotate *pAnimRotate = (AnimRotate *)pDest;
		*pAnimRotate = *(AnimRotate *)this;
		pDest += sizeof(AnimRotate);
		return pAnimRotate->m_curve.RelocateCopyKeys(pDest, pBase);
	}
	case ANIM_TRANSLATE:
	{
		AnimTranslate *pAnimTranslate = (AnimTranslate *)pDest;
		*pAnimTranslate = *(AnimTranslate *)this;
		pDest += sizeof(AnimTranslate);
		return pAnimTranslate->m_curve.RelocateCopyKeys(pDest, pBase);
	}
	case ANIM_SCALE:
	{
		AnimScale *pAnimScale = (AnimScale *)pDest;
		*pAnimScale = *(AnimScale *)this;
		pDest += sizeof(AnimScale);
		return pAnimScale->m_curve.RelocateCopyKeys(pDest, pBase);
	}
	case ANIM_ROTATEBLEND:
	{
		AnimRotateBlend *pAnimRotateBlend = (AnimRotateBlend *)pDest;
		*pAnimRotateBlend = *(AnimRotateBlend *)this;
		pDest += sizeof(AnimRotateBlend);
		hr = pAnimRotateBlend->m_pAnimRotateA->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		DWORD SizeA = pAnimRotateBlend->m_pAnimRotateA->GetSize();
		*(DWORD *)&pAnimRotateBlend->m_pAnimRotateA = pDest - pBase;	// convert to file offset
		pDest += SizeA;
		hr = pAnimRotateBlend->m_pAnimRotateB->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimRotateBlend->m_pAnimRotateB = pDest - pBase;	// convert to file offset
		return S_OK;
	}
	case ANIM_TRANSLATEBLEND:
	{
		AnimTranslateBlend *pAnimTranslateBlend = (AnimTranslateBlend *)pDest;
		*pAnimTranslateBlend = *(AnimTranslateBlend *)this;
		pDest += sizeof(AnimTranslateBlend);
		hr = pAnimTranslateBlend->m_pAnimTranslateA->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		DWORD SizeA = pAnimTranslateBlend->m_pAnimTranslateA->GetSize();
		*(DWORD *)&pAnimTranslateBlend->m_pAnimTranslateA = pDest - pBase;	// convert to file offset
		pDest += SizeA;
		hr = pAnimTranslateBlend->m_pAnimTranslateB->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimTranslateBlend->m_pAnimTranslateB = pDest - pBase;	// convert to file offset
		return S_OK;
	}
	case ANIM_SCALEBLEND:
	{
		AnimScaleBlend *pAnimScaleBlend = (AnimScaleBlend *)pDest;
		*pAnimScaleBlend = *(AnimScaleBlend *)this;
		pDest += sizeof(AnimScaleBlend);
		hr = pAnimScaleBlend->m_pAnimScaleA->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		DWORD SizeA = pAnimScaleBlend->m_pAnimScaleA->GetSize();
		*(DWORD *)&pAnimScaleBlend->m_pAnimScaleA = pDest - pBase;	// convert to file offset
		pDest += SizeA;
		hr = pAnimScaleBlend->m_pAnimScaleB->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimScaleBlend->m_pAnimScaleB = pDest - pBase;	// convert to file offset
		return S_OK;
	}
	case ANIM_CONCAT:
	{
		AnimConcat *pAnimConcat = (AnimConcat *)pDest;
		*pAnimConcat = *(AnimConcat *)this;
		pDest += sizeof(AnimConcat);
		hr = pAnimConcat->m_pAnimA->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		DWORD SizeA = pAnimConcat->m_pAnimA->GetSize();
		*(DWORD *)&pAnimConcat->m_pAnimA = pDest - pBase;	// convert to file offset
		pDest += SizeA;
		hr = pAnimConcat->m_pAnimB->RelocateCopy(pDest, pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimConcat->m_pAnimB = pDest - pBase;	// convert to file offset
		return S_OK;
	}
	default:
		return E_NOTIMPL;
	}
}

//////////////////////////////////////////////////////////////////////
// Patch file offsets to memory offsets, i.e. convert file offsets
// to pointers.
//
HRESULT AnimFrame::Patch(const BYTE *pBase)
{
	HRESULT hr;
	switch (m_Type) {
	case ANIM_MATRIX:
	{
		AnimMatrix *pAnimMatrix = (AnimMatrix *)this;
		return pAnimMatrix->m_curve.PatchKeys(pBase);
	}
	case ANIM_ROTATE:
	{
		AnimRotate *pAnimRotate = (AnimRotate *)this;
		return pAnimRotate->m_curve.PatchKeys(pBase);
	}
	case ANIM_TRANSLATE:
	{
		AnimTranslate *pAnimTranslate = (AnimTranslate *)this;
		return pAnimTranslate->m_curve.PatchKeys(pBase);
	}
	case ANIM_SCALE:
	{
		AnimScale *pAnimScale = (AnimScale *)this;
		return pAnimScale->m_curve.PatchKeys(pBase);
	}
	case ANIM_ROTATEBLEND:
	{
		AnimRotateBlend *pAnimRotateBlend = (AnimRotateBlend *)this;
		*(DWORD *)&pAnimRotateBlend->m_pAnimRotateA += (DWORD)pBase;	// convert to pointer
		hr = pAnimRotateBlend->m_pAnimRotateA->Patch(pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimRotateBlend->m_pAnimRotateB += (DWORD)pBase;	// convert to pointer
		hr = pAnimRotateBlend->m_pAnimRotateB->Patch(pBase);
		if (FAILED(hr))
			return hr;
		return S_OK;
	}
	case ANIM_TRANSLATEBLEND:
	{
		AnimTranslateBlend *pAnimTranslateBlend = (AnimTranslateBlend *)this;
		*(DWORD *)&pAnimTranslateBlend->m_pAnimTranslateA += (DWORD)pBase;	// convert to pointer
		hr = pAnimTranslateBlend->m_pAnimTranslateA->Patch(pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimTranslateBlend->m_pAnimTranslateB += (DWORD)pBase;	// convert to pointer
		hr = pAnimTranslateBlend->m_pAnimTranslateB->Patch(pBase);
		if (FAILED(hr))
			return hr;
		return S_OK;
	}
	case ANIM_SCALEBLEND:
	{
		AnimScaleBlend *pAnimScaleBlend = (AnimScaleBlend *)this;
		*(DWORD *)&pAnimScaleBlend->m_pAnimScaleA += (DWORD)pBase;	// convert to pointer
		hr = pAnimScaleBlend->m_pAnimScaleA->Patch(pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimScaleBlend->m_pAnimScaleB += (DWORD)pBase;	// convert to pointer
		hr = pAnimScaleBlend->m_pAnimScaleB->Patch(pBase);
		if (FAILED(hr))
			return hr;
		return S_OK;
	}
	case ANIM_CONCAT:
	{
		AnimConcat *pAnimConcat = (AnimConcat *)this;
		*(DWORD *)&pAnimConcat->m_pAnimA += (DWORD)pBase;	// convert to pointer
		hr = pAnimConcat->m_pAnimA->Patch(pBase);
		if (FAILED(hr))
			return hr;
		*(DWORD *)&pAnimConcat->m_pAnimB += (DWORD)pBase;	// convert to pointer
		hr = pAnimConcat->m_pAnimB->Patch(pBase);
		if (FAILED(hr))
			return hr;
		return S_OK;
	}
	default:
		return E_NOTIMPL;
	}
}

//////////////////////////////////////////////////////////////////////
// Check to see if the animation curve is static, i.e. it
// has only single-key animations or all the keys are the same.
BOOL AnimCurve::IsConstant()
{
	// Check if all keys are the same
	FLOAT *rfKey0 = m_rKeys;
	for (UINT iKey = 1; iKey < m_KeyCount ; iKey++)
	{
		FLOAT *rfKey = m_rKeys + iKey * m_KeySize;
		if (memcmp(rfKey0, rfKey, sizeof(FLOAT) * m_KeySize) != 0)
			return FALSE;
	}
	return TRUE;
}

 //////////////////////////////////////////////////////////////////////
// Check to see if the animation expression is static, i.e. it
// has only single-key animations.
BOOL AnimFrame::IsConstant()
{
	switch (m_Type) {
	case ANIM_MATRIX:
	{
		AnimMatrix *pAnimMatrix = (AnimMatrix *)this;
		return pAnimMatrix->m_curve.IsConstant();
	}
	case ANIM_ROTATE:
	{
		AnimRotate *pAnimRotate = (AnimRotate *)this;
		return pAnimRotate->m_curve.IsConstant();
	}
	case ANIM_TRANSLATE:
	{
		AnimTranslate *pAnimTranslate = (AnimTranslate *)this;
		return pAnimTranslate->m_curve.IsConstant();
	}
	case ANIM_SCALE:
	{
		AnimScale *pAnimScale = (AnimScale *)this;
		return pAnimScale->m_curve.IsConstant();
	}

	// TODO: if blend parameter is animated, then we need to compare the
	// two animations, too.

	case ANIM_ROTATEBLEND:
	{
		AnimRotateBlend *pAnimRotateBlend = (AnimRotateBlend *)this;
		if (!pAnimRotateBlend->m_pAnimRotateA->IsConstant())
			return FALSE;
		if (!pAnimRotateBlend->m_pAnimRotateB->IsConstant())
			return FALSE;
		return TRUE;
	}
	case ANIM_TRANSLATEBLEND:
	{
		AnimTranslateBlend *pAnimTranslateBlend = (AnimTranslateBlend *)this;
		if (!pAnimTranslateBlend->m_pAnimTranslateA->IsConstant())
			return FALSE;
		if (!pAnimTranslateBlend->m_pAnimTranslateB->IsConstant())
			return FALSE;
		return TRUE;
	}
	case ANIM_SCALEBLEND:
	{
		AnimScaleBlend *pAnimScaleBlend = (AnimScaleBlend *)this;
		if (!pAnimScaleBlend->m_pAnimScaleA->IsConstant())
			return FALSE;
		if (!pAnimScaleBlend->m_pAnimScaleB->IsConstant())
			return FALSE;
		return TRUE;
	}


	case ANIM_CONCAT:
	{
		AnimConcat *pAnimConcat = (AnimConcat *)this;
		if (!pAnimConcat->m_pAnimA->IsConstant())
			return FALSE;
		if (!pAnimConcat->m_pAnimB->IsConstant())
			return FALSE;
		return TRUE;
	}
	default:
		return FALSE;
	}
}


#endif
 
