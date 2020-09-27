//-----------------------------------------------------------------------------
//  
//  File: animate.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//  Implementation of the XBContent class for skeleton animation, i.e.
//  the <Frame> <Matrix> and <animate> tags.
//-----------------------------------------------------------------------------
#include "stdafx.h"

//////////////////////////////////////////////////////////////////////
// FrameExtra routines.
//
FrameExtra::FrameExtra()
{
	D3DXMatrixIdentity(&m_Matrix);			// initialize frame
	ZeroMemory(m_strName, sizeof(m_strName)); // empty identifier
	m_pChild = NULL;
	m_pNext = NULL;
	m_pAnim = NULL;
	m_pAnimLocal = NULL;
	m_Offset = 0;
}

FrameExtra::~FrameExtra()
{
	// does nothing
}

HRESULT FrameExtra::AddChild(FrameExtra *pFrame)
{
	// Add to end of child list
	Frame **ppFrameEnd = &m_pChild;
	while (*ppFrameEnd != NULL)
		ppFrameEnd = &((*ppFrameEnd)->m_pNext);
	*ppFrameEnd = pFrame;
	return S_OK;
}

HRESULT FrameExtra::Premult(AnimFrame *pAnim)
{
	if (pAnim == NULL)
		return S_OK;	// nothing to premultiply
	
	// Add to animation expression
	if (m_pAnim == NULL)
	{
#if 1
		m_pAnim = pAnim;
		return S_OK;
#else
		if (D3DXMatrixIsIdentity(&m_Matrix))
		{
			// No animation has been set, and the current matrix
			// is the identity, so no need to concatenate.
			m_pAnim = pAnim;
			return S_OK;
		}
		else
		{
			// If the current transformation has already been set
			// with scale's and translate's, etc., then we need
			// to include it in the animation expression.
			m_pAnim = new AnimMatrix(&m_Matrix);
			if (m_pAnim == NULL)
				return E_OUTOFMEMORY;
		}
#endif
	}

	AnimConcat *pAnimConcat = new AnimConcat;
	if (pAnimConcat == NULL)
		return E_OUTOFMEMORY;
	pAnimConcat->m_pAnimA = pAnim;		// prepend matrix...
	pAnimConcat->m_pAnimB = m_pAnim;	// ... to what was there before
	m_pAnim = pAnimConcat;
	return S_OK;
}

HRESULT FrameExtra::DeleteHierarchy(FrameExtra *pFrame)
{
	FrameExtra *rFrameStack[FRAME_MAX_STACK];
	int iFrameStack = 1;	// depth of stack
	rFrameStack[0] = pFrame;
	while (iFrameStack)
	{
		// Pop the stack and process the top element
		iFrameStack--;
		pFrame = rFrameStack[iFrameStack];
		
		// Push sibling
		if (pFrame->m_pNext != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack] = (FrameExtra *)(pFrame->m_pNext);
			iFrameStack++;
		}
		
		// Push child
		if (pFrame->m_pChild != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack] = (FrameExtra *)(pFrame->m_pChild);
			iFrameStack++;
		}
	
		// Delete the current frame
		delete pFrame;
	}
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// Frame Handler
//
HRESULT XBContent::BeginFrame(ISAXAttributes *pAttributes)
{
	FrameExtra *pFrameParent = NULL;
	if (m_rContextStack[m_iContext].m_Context == Frame_Context)
		pFrameParent = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	else
	{
		// We're starting a frame context, so the current animation list should be empty
		// TODO: What about multiple, independent animations of the same hierarchy?
		// TODO: What about separate animation time-lines for separate parts?
		if (m_pAnimList != NULL)
			ignorableWarning(m_pLocator, L"Clearing animation list (which should have been empty already.)\n", E_NOTIMPL);
		m_pAnimList = NULL;
	}
	PushContext(Frame_Context);

	// Create a new frame and add to the parent list
	FrameExtra *pFrame = new FrameExtra;						// make a new frame
	if (pFrame == NULL)
		return E_OUTOFMEMORY;
	if (pFrameParent)
		pFrameParent->AddChild(pFrame);
	m_rContextStack[m_iContext].m_pData = pFrame;	// keep frame pointer in context data

	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(id))
		{
			// Set frame identifier
			CHAR *strID = CharString(pwchValue, cchValue);
			ZeroMemory(pFrame->m_strName, FRAME_IDENTIFIER_SIZE);
			strncpy(pFrame->m_strName, strID, FRAME_IDENTIFIER_SIZE);
			pFrame->m_strName[FRAME_IDENTIFIER_SIZE - 1] = 0;	// make sure string is terminated
		}
		else if (MATCH(name))
		{
			// TODO: make names and ids work differently.  Id's must be unique.  Names should generate id's.
			
			// Set frame name
			CHAR *strID = CharString(pwchValue, cchValue);
			ZeroMemory(pFrame->m_strName, FRAME_IDENTIFIER_SIZE);
			strncpy(pFrame->m_strName, strID, FRAME_IDENTIFIER_SIZE);
			pFrame->m_strName[FRAME_IDENTIFIER_SIZE - 1] = 0;	// make sure string is terminated
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::EndFrame()
{
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PopContext();

	if (m_rContextStack[m_iContext].m_Context == Frame_Context)
	{
		// Not the last frame in the hierarchy, so continue without
		// dumping skeleton resource
		return S_OK;
	}
	else
	{
		// This is the closing of a top-level frame, so write skeleton
		// and associated animation resources.
		HRESULT hr = WriteSkeleton(pFrame);
		if (FAILED(hr))
			return hr;
	
		// If the skeleton is animated, also make an animation resource that
		// points to the skeleton.
		if (m_pAnimList != NULL)
		{
			hr = WriteAnimation(m_pAnimList);
			if (FAILED(hr))
				return hr;

			// Clear animation list
			AnimLink *pAnimSrc = m_pAnimList;
			while (pAnimSrc)
			{
				AnimLink *pNext = pAnimSrc->m_pNext;
				delete pAnimSrc;
				
				// TODO: delete AnimFrame hierarchy, too
				
				pAnimSrc = pNext;
			}
			m_pAnimList = NULL;
		}

		// Clear the frame hierarchy
		hr = FrameExtra::DeleteHierarchy(pFrame);
		if (FAILED(hr))
			return hr;
		
		return S_OK;
	}
}

//////////////////////////////////////////////////////////////////////
// Matrix Handler
//
HRESULT XBContent::BeginMatrix(ISAXAttributes *pAttributes)
{
	// sanity check
	if (m_rContextStack[m_iContext].m_Context != Frame_Context
		&& m_rContextStack[m_iContext].m_Context != Matrix_Context)
		return fatalError(m_pLocator,
						  L"<Matrix> tag must be within <Frame> or <Matrix> context\n",
						  E_INVALIDARG);
	
	// Save context
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Matrix_Context);
	m_rContextStack[m_iContext].m_pData = pFrame;
	
	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(value))
		{
			D3DXMATRIX mat;
			UINT iValueCount = 0;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strSeparators = " \t\n";
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			while (strBuff != NULL)
			{
				if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
				{
					FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
						// leave as just a warning
					}
					if (iValueCount >= 16)
						return fatalError(m_pLocator, L"<Matrix> has too many values (>16).\n", E_INVALIDARG);
					((FLOAT *)mat)[iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg("Can not parse matrix value \"%s\"\n", strBuff);
					return E_FAIL;
				}

				// Get next token
				strBuff = strtok(NULL, strSeparators);
			}
			if (iValueCount < 16)
				return fatalError(m_pLocator, L"<Matrix> has too few values.\n", E_INVALIDARG);
			
			// Premultiply frame matrix
			D3DXMatrixMultiply(&pFrame->m_Matrix, &mat, &pFrame->m_Matrix);

			// If an animation expression is already active, prepare to add matrix as a key
			// If there's an <animate> tag following, it will overwrite this 1-key animation.
			/*
			if (pFrame->m_pAnim)
			{
				if (!D3DXMatrixIsIdentity(&mat))
				{
				*/
					if (pFrame->m_pAnimLocal != NULL)
						return fatalError(m_pLocator, L"Recursive <Matrix> elements not implemented.\n", E_NOTIMPL);
					pFrame->m_pAnimLocal = new AnimMatrix( &mat );	// static matrix key
					if (pFrame->m_pAnimLocal == NULL)
						return E_OUTOFMEMORY;
					/*
				}
			}
			*/
		}
		else if (MATCH(name))
		{
			// TODO: save the name for use later
			// CHAR *strName = CharString(pwchValue, cchValue);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::EndMatrix()
{
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PopContext();
 
	// Add to frame's animation list
	if (pFrame->m_pAnimLocal != NULL)
	{
		pFrame->Premult(pFrame->m_pAnimLocal);
		pFrame->m_pAnimLocal = NULL;
	}

	// TODO: why do we need m_pAnimList?  Can't we just leave the animations
	// in the frame hierarchy using the m_pAnim pointers?   For more general
	// animation, we do need to keep track of the animations somewhere.
	
	// If this is the final close </Matrix> tag before we go back to
	// a frame context, add animation to skeleton animation list.
	if (m_rContextStack[m_iContext].m_Context == Frame_Context)
	{
		FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
		if (pFrame->m_pAnim != NULL)
		{
#if ANIM_INTERPRETED
			// Simple optimization.  If the animation is constant, skip it.
			if (pFrame->m_pAnim->IsConstant())
			{
				// TODO: delete pAnim
			}
			else
#endif
			{
			AnimLink *pAnimLink = new AnimLink;
			if (pAnimLink == NULL)
				return fatalError(m_pLocator, L"<Matrix> can not allocate linked list element.\n", E_OUTOFMEMORY);
			pAnimLink->m_pAnimFrame = pFrame->m_pAnim;
			pAnimLink->m_pFrame = pFrame;	// bind animation to current frame
			pAnimLink->m_pNext = NULL;

			// Add to end of list
			AnimLink **ppAnimEnd = &m_pAnimList;
			while (*ppAnimEnd != NULL)
				ppAnimEnd = &(*ppAnimEnd)->m_pNext;
			*ppAnimEnd = pAnimLink;
			}
		}
	}
	return S_OK;
}




//////////////////////////////////////////////////////////////////////
// Translate 
//
HRESULT XBContent::BeginTranslate(ISAXAttributes *pAttributes)
{
 	// sanity check
	if (m_rContextStack[m_iContext].m_Context != Matrix_Context)
		return fatalError(m_pLocator, L"<Translate> tag must be within <Matrix> context\n", E_INVALIDARG);
	
	// Save context
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Translate_Context);
	m_rContextStack[m_iContext].m_pData = pFrame;
	
	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(value))
		{
			D3DXVECTOR3 vTranslate(0.f, 0.f, 0.f);
			UINT iValueCount = 0;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strSeparators = " \t\n";
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			while (strBuff != NULL)
			{
				if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
				{
					FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
						// leave as just a warning
					}
					if (iValueCount >= 3)
						return fatalError(m_pLocator, L"<Translate> has too many values (>3).\n", E_INVALIDARG);
					((FLOAT *)vTranslate)[iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg("Can not parse <Translate> value \"%s\"\n", strBuff);
					return E_FAIL;
				}

				// Get next token
				strBuff = strtok(NULL, strSeparators);
			}
			if (iValueCount < 3)
				return fatalError(m_pLocator, L"<Translate> has too few values.\n", E_INVALIDARG);
			
			// Premultiply frame matrix
			D3DXMATRIX mat;
			D3DXMatrixTranslation(&mat, vTranslate.x, vTranslate.y, vTranslate.z);
			D3DXMatrixMultiply(&pFrame->m_Matrix, &mat, &pFrame->m_Matrix);

			// If an animation expression is already active, prepare to add translate as a key.
			// If there's an <animate> tag following, it will overwrite this 1-key animation.
			/*
			if (pFrame->m_pAnim)
			{
				if (!D3DXMatrixIsIdentity(&mat))
				{
			*/
					if (pFrame->m_pAnimLocal != NULL)
						return fatalError(m_pLocator, L"Recursive <Matrix> elements not implemented.\n", E_NOTIMPL);
					pFrame->m_pAnimLocal = new AnimTranslate( &vTranslate );	// static translate key
					if (pFrame->m_pAnimLocal == NULL)
						return E_OUTOFMEMORY;
			/*
				}
			}
			*/
		}
		else if (MATCH(name))
		{
			// TODO: save the name for use later
			// CHAR *strName = CharString(pwchValue, cchValue);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::EndTranslate(void)
{
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
 	PopContext();
	
	// Add 1-key animation to frame's animation list, since we did not
	// encounter an <animate> tag
	if (pFrame->m_pAnimLocal != NULL)
	{
		pFrame->Premult(pFrame->m_pAnimLocal);
		pFrame->m_pAnimLocal = NULL;
	}
	return S_OK;
}




//////////////////////////////////////////////////////////////////////
// Scale
//
HRESULT XBContent::BeginScale(ISAXAttributes *pAttributes)
{
 	// sanity check
	if (m_rContextStack[m_iContext].m_Context != Matrix_Context)
		return fatalError(m_pLocator, L"<Scale> tag must be within <Matrix> context\n", E_INVALIDARG);
	
	// Save context
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Scale_Context);
	m_rContextStack[m_iContext].m_pData = pFrame;
	
	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(value))
		{
			D3DXVECTOR3 vScale(1.f, 1.f, 1.f);
			UINT iValueCount = 0;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strSeparators = " \t\n";
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			while (strBuff != NULL)
			{
				if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
				{
					FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
						// leave as just a warning
					}
					if (iValueCount >= 3)
						return fatalError(m_pLocator, L"<Scale> has too many values (>3).\n", E_INVALIDARG);
					((FLOAT *)vScale)[iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg("Can not parse <Scale> value \"%s\"\n", strBuff);
					return E_FAIL;
				}

				// Get next token
				strBuff = strtok(NULL, strSeparators);
			}
			if (iValueCount < 3)
				return fatalError(m_pLocator, L"<Scale> has too few values.\n", E_INVALIDARG);
			
			// Concatenate frame matrix
			D3DXMATRIX mat;
			D3DXMatrixScaling(&mat, vScale.x, vScale.y, vScale.z);
			D3DXMatrixMultiply(&pFrame->m_Matrix, &mat, &pFrame->m_Matrix);
			
			// If an animation expression is already active, prepare to add scale as a key.
			// If there's an <animate> tag following, it will overwrite this 1-key animation.
			/*
			if (pFrame->m_pAnim)
			{
				if (!D3DXMatrixIsIdentity(&mat))
				{
				*/
					if (pFrame->m_pAnimLocal != NULL)
						return fatalError(m_pLocator, L"Recursive <Matrix> elements not implemented.\n", E_NOTIMPL);
					pFrame->m_pAnimLocal = new AnimScale( &vScale );	// static scale key
					if (pFrame->m_pAnimLocal == NULL)
						return E_OUTOFMEMORY;
			/*
				}
			}
			*/
		}
		else if (MATCH(name))
		{
			// TODO: save the name for use later
			// CHAR *strName = CharString(pwchValue, cchValue);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	return S_OK;
}

HRESULT XBContent::EndScale(void)
{
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
 	PopContext();
	
	// Add 1-key animation to frame's animation list, since we did not
	// encounter an <animate> tag
	if (pFrame->m_pAnimLocal != NULL)
	{
		pFrame->Premult(pFrame->m_pAnimLocal);
		pFrame->m_pAnimLocal = NULL;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Rotate tag accepts either quaternions or axis/angle.
//
enum RotateContextMode {
	RCM_NONE,
	RCM_QUATERNION,
	RCM_AXIS_ONLY,
	RCM_ANGLE_ONLY,
	RCM_AXISANGLE
};

struct RotateContext {
	FrameExtra *m_pFrame;
	RotateContextMode m_mode;
	D3DXVECTOR3 m_vAxis;
	FLOAT m_fAngle;
	D3DXQUATERNION m_quat;
	AnimRotate *m_pAnim;
};

HRESULT XBContent::BeginRotate(ISAXAttributes *pAttributes)
{
	D3DXMATRIX mat;

	// sanity check
	if (m_rContextStack[m_iContext].m_Context != Matrix_Context)
		return fatalError(m_pLocator, L"<Rotate> tag must be within <Matrix> context\n", E_INVALIDARG);

	// Save context
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Rotate_Context);
	RotateContext *pRotateContext = new RotateContext;
	if (pRotateContext == NULL)
		return fatalError(m_pLocator, L"<Rotate> allocation failed\n", E_OUTOFMEMORY);
	pRotateContext->m_pFrame = pFrame;
	pRotateContext->m_mode = RCM_NONE;
	D3DXQuaternionIdentity(&pRotateContext->m_quat);
	pRotateContext->m_pAnim = NULL;
	m_rContextStack[m_iContext].m_pData = pRotateContext;
	
	// Loop through attributes
	CHAR *strSeparators = " \t\n";
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
		if (MATCH(value))
		{
			if (pRotateContext->m_mode != RCM_NONE)
				goto e_BothQuaternionAndAngleAxis;
			pRotateContext->m_mode = RCM_QUATERNION;
			UINT iValueCount = 0;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			while (strBuff != NULL)
			{
				if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
				{
					FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
						// leave as just a warning
					}
					if (iValueCount >= 4)
						return fatalError(m_pLocator, L"<Rotate> quaternion has too many values (>4).\n", E_INVALIDARG);
					((FLOAT *)pRotateContext->m_quat)[iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg("Can not parse <Rotate> value \"%s\"\n", strBuff);
					return E_FAIL;
				}

				// Get next token
				strBuff = strtok(NULL, strSeparators);
			}
			if (iValueCount < 4)
				return fatalError(m_pLocator, L"<Rotate> has too few quaternion values.\n", E_INVALIDARG);
			
		}
		else if (MATCH(angle))
		{
			if (pRotateContext->m_mode == RCM_QUATERNION)
				goto e_BothQuaternionAndAngleAxis;
			else if (pRotateContext->m_mode == RCM_AXIS_ONLY)
				pRotateContext->m_mode = RCM_AXISANGLE;
			else // (pRotateContext->m_mode == RCM_NONE)
				pRotateContext->m_mode = RCM_ANGLE_ONLY;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			pRotateContext->m_fAngle = (FLOAT)strtod(strBuff, &pEnd);
			if (*pEnd != '\0')
			{
				m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
				// leave as just a warning
			}
			strBuff = strtok(NULL, strSeparators);
			if (strBuff != NULL)
			{
				m_pBundler->ErrorMsg( "Extra token \"%s\" on angle attribute.\n", strBuff);
				// leave as just a warning
			}
		}
		else if (MATCH(axis))
		{
			if (pRotateContext->m_mode == RCM_QUATERNION)
				goto e_BothQuaternionAndAngleAxis;
			else if (pRotateContext->m_mode == RCM_ANGLE_ONLY)
				pRotateContext->m_mode = RCM_AXISANGLE;
			else // (pRotateContext->m_mode == RCM_NONE)
				pRotateContext->m_mode = RCM_AXIS_ONLY;
			UINT iValueCount = 0;
			CHAR *strFull = CharString(pwchValue, cchValue);
			CHAR *strBuff = strtok(strFull, strSeparators);
			CHAR *pEnd;
			while (strBuff != NULL)
			{
				if (m_pBundler->ValidateType(strBuff, TT_FLOAT) == S_OK)
				{
					FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
						// leave as just a warning
					}
					if (iValueCount >= 3)
						return fatalError(m_pLocator, L"<Rotate> axis has too many values (>3).\n", E_INVALIDARG);
					((FLOAT *)pRotateContext->m_vAxis)[iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg("Can not parse <Rotate> axis value \"%s\"\n", strBuff);
					return E_FAIL;
				}

				// Get next token
				strBuff = strtok(NULL, strSeparators);
			}
			if (iValueCount < 3)
				return fatalError(m_pLocator, L"<Rotate> has too few axis values.\n", E_INVALIDARG);
		}
		else if (MATCH(name))
		{
			// TODO: save the name for use later
			// CHAR *strName = CharString(pwchValue, cchValue);
		}
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH
	}
	if (pRotateContext->m_mode == RCM_AXISANGLE)
	{
		// Create angle-axis quaternion
		D3DXQuaternionRotationAxis(&pRotateContext->m_quat, &pRotateContext->m_vAxis, pRotateContext->m_fAngle);
	}
	else if (pRotateContext->m_mode == RCM_AXIS_ONLY)
		return fatalError(m_pLocator, L"<Rotate> axis specified, but missing angle attribute.\n", E_FAIL);
	else if (pRotateContext->m_mode == RCM_ANGLE_ONLY)
		return fatalError(m_pLocator, L"<Rotate> angle specified, but missing axis attribute.\n", E_FAIL);

	// Concatenate frame matrix
	D3DXMatrixRotationQuaternion(&mat, &pRotateContext->m_quat);
	D3DXMatrixMultiply(&pFrame->m_Matrix, &mat, &pFrame->m_Matrix);
	
	// If an animation expression is already active, prepare to add quaternion as a key.
	// If there's an <animate> tag following, it will overwrite this 1-key animation.
	/*
	if (pFrame->m_pAnim)
	{
		if (!D3DXMatrixIsIdentity(&mat))
		{
	*/
			if (pFrame->m_pAnimLocal != NULL)
				return fatalError(m_pLocator, L"Recursive <Matrix> elements not implemented.\n", E_NOTIMPL);
			pFrame->m_pAnimLocal = new AnimRotate( &pRotateContext->m_quat );
			if (pFrame->m_pAnimLocal == NULL)
				return E_OUTOFMEMORY;
	/*
		}
	}
	*/
	return S_OK;
	
 e_BothQuaternionAndAngleAxis:
	return fatalError(m_pLocator,
					  L"<Rotate> tag may have either quaternion \"value\", or \"axis\" and \"angle\", but not both.\n",
					  E_INVALIDARG);
}

HRESULT XBContent::EndRotate(void)
{
	RotateContext *pRotateContext = (RotateContext *)m_rContextStack[m_iContext].m_pData;
 	PopContext();
	
	// Add 1-key animation to frame's animation list, since we did not
	// encounter an <animate> tag
	FrameExtra *pFrame = pRotateContext->m_pFrame;
	if (pFrame->m_pAnimLocal != NULL)
	{
		pFrame->Premult(pFrame->m_pAnimLocal);
		pFrame->m_pAnimLocal = NULL;
	}

	delete pRotateContext;	// clean up
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// animate Handlers
//

HRESULT XBContent::animate(AnimCurve *pCurve, ISAXAttributes *pAttributes, CONST WCHAR *wstrAttribute)
{
	// Loop through attributes
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
#define MATCH_VALUE(str) Match(str, pwchValue, cchValue)
		if (MATCH(attribute))
		{
			if (wstrAttribute != NULL)
			{
				// Make sure attribute tag matches desired attribute
				if (!MATCH_VALUE(wstrAttribute))
				{
					const int buflen = 200;
					const int namelen = 50;
					WCHAR buf[buflen];
					_snwprintf(buf, buflen,
							   L"<animate> attribute name \"%.*s\" does not match desired attribute name \"%.*s\"",
							   cchLocalName < namelen ? cchLocalName : namelen, pwchLocalName,
							   namelen, wstrAttribute);
					return fatalError(m_pLocator, buf, E_INVALIDARG);
				}
			}
		}
		else if (MATCH(begin))
		{
			CHAR *strBuff = CharString(pwchValue, cchValue);
			CHAR *pEnd;
			FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
			if (*pEnd != 0)
			{
				if (!(pEnd[0] == 's' && pEnd[1] == 0))	// look for seconds suffix
				{
					// print warning
					m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
				}
			}
			pCurve->m_fBegin = fValue;
		}
		else if (MATCH(dur))
		{
			CHAR *strBuff = CharString(pwchValue, cchValue);
			CHAR *pEnd;
			FLOAT fValue = (FLOAT)strtod(strBuff, &pEnd);
			if (*pEnd != 0)
			{
				if (!(pEnd[0] == 's' && pEnd[1] == 0))	// look for seconds suffix
				{
					// print warning
					m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, strBuff);
				}
			}
			pCurve->m_fDurationInverse = 1.f / fValue;
		}
		else if (MATCH(values))
		{
			CHAR *str = CharString(pwchValue, cchValue);

			// Count the number of keys
			UINT KeyCount = 0;
			CHAR *strSemi = strchr(str, ';');
			while (strSemi && *strSemi == ';')
			{
				KeyCount++;
				strSemi = strchr(strSemi + 1, ';');
			}
			if (pCurve->m_KeyCount == 0) // first of values, keyTimes, or keySplines, so set number of keys
				pCurve->m_KeyCount = KeyCount;
			else if (pCurve->m_KeyCount != KeyCount)
			{
				const int buflen = 200;
				WCHAR buf[buflen];
				_snwprintf(buf, buflen,
						   L"<animate> number of key values (%d) does not match other key counts (%d)\n",
						   KeyCount, pCurve->m_KeyCount);
				return fatalError(m_pLocator, buf, E_INVALIDARG);
			}

			// Allocate the key arrays
			pCurve->m_rKeys = new FLOAT [ pCurve->m_KeyCount * pCurve->m_KeySize ];
			if (pCurve->m_rKeys == NULL)
				return fatalError(m_pLocator,
								  L"Memory allocation failed for <animate> key array.\n",
								  E_OUTOFMEMORY);

			// Fill in the key array
			UINT iKey = 0;
			UINT iValueCount = 0;
			strSemi = strchr(str, ';');	// keep track of end of key
			CHAR *strSeparators = "; \t\n";
			CHAR *tok = strtok(str, strSeparators);
			CHAR *pEnd;
			while (tok != NULL)
			{
				if (m_pBundler->ValidateType(tok, TT_FLOAT) == S_OK)	// TODO: move validation code
				{
					FLOAT fValue = (FLOAT)strtod(tok, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, tok);
						// leave as just a warning
					}
					if (iValueCount >= pCurve->m_KeySize)
					{
						const int buflen = 200;
						WCHAR buf[buflen];
						_snwprintf(buf, buflen,
								   L"<animate> key has too many values (%d > %d).\n",
								   iValueCount, pCurve->m_KeyCount);
						return fatalError(m_pLocator, buf, E_INVALIDARG);
					}
					pCurve->m_rKeys[iKey * pCurve->m_KeySize + iValueCount] = fValue;
					iValueCount++;
				}
				else
				{
					m_pBundler->ErrorMsg( "Unrecognized value \"%s\"\n", tok );
					return E_FAIL;
				}

				// If tok is beyond semicolon, it's time to look for a new key
				if (tok + strlen(tok) >= strSemi)
				{
					if (iValueCount != pCurve->m_KeySize)
						return fatalError(m_pLocator, L"<animate> key has too few values\n", E_INVALIDARG);
					iKey++;
					if (iKey > pCurve->m_KeyCount)
						return fatalError(m_pLocator, L"<animate> keys mismatched count\n", E_FAIL);
					strSemi = strchr(strSemi + 1, ';');
					iValueCount = 0;
				}
				
				// Get next token
				tok = strtok(NULL, strSeparators);
			}
		}
		else if (MATCH(keyTimes))
		{
			CHAR *str = CharString(pwchValue, cchValue);

			// Count the number of keys
			UINT KeyCount = 0;
			CHAR *strSemi = strchr(str, ';');
			while (strSemi && *strSemi == ';')
			{
				KeyCount++;
				strSemi = strchr(strSemi + 1, ';');
			}
			if (pCurve->m_KeyCount == 0)		// first of values, keyTimes, or keySplines, so set number of keys
				pCurve->m_KeyCount = KeyCount;
			else if (pCurve->m_KeyCount != KeyCount)
			{
				const int buflen = 200;
				WCHAR buf[buflen];
				_snwprintf(buf, buflen,
						   L"<animate> number of keyTimes values (%d) does not match other key counts (%d)\n",
						   KeyCount, pCurve->m_KeyCount);
				return fatalError(m_pLocator, buf, E_INVALIDARG);
			}

			// Allocate the key arrays
			pCurve->m_rKeyTimes = new FLOAT [ pCurve->m_KeyCount ];
			if (pCurve->m_rKeyTimes == NULL)
				return fatalError(m_pLocator,
								  L"Memory allocation failed for <animate> keyTimes array.\n",
								  E_OUTOFMEMORY);

			// Fill in the key array
			UINT iKey = 0;
			CHAR *strSeparators = "; \t\n";
			CHAR *tok = strtok(str, strSeparators);
			CHAR *pEnd;
			while (tok != NULL)
			{
				if (m_pBundler->ValidateType(tok, TT_FLOAT) == S_OK)	// TODO: move validation code
				{
					FLOAT fValue = (FLOAT)strtod(tok, &pEnd);
					if (*pEnd != '\0')
					{
						m_pBundler->ErrorMsg( "Extra chars \"%s\" on value \"%s\".\n", pEnd, tok);
						// leave as just a warning
					}
					pCurve->m_rKeyTimes[iKey] = fValue;
					iKey++;
				}
				else
				{
					m_pBundler->ErrorMsg( "Unrecognized constant value \"%s\"\n", tok );
					return E_FAIL;
				}

				// Get next token
				tok = strtok(NULL, strSeparators);
			}
			if (iKey != pCurve->m_KeyCount)
			{
				const int buflen = 200;
				WCHAR buf[buflen];
				_snwprintf(buf, buflen,
						   L"<animate> number of keyTimes (%d) does not match other key counts (%d)\n",
						   iKey, pCurve->m_KeyCount);
				return fatalError(m_pLocator, buf, E_INVALIDARG);
			}
		}
/*	// TODO: More SMIL attributes
		else if (MATCH(calcMode))
		{
		}
		else if (MATCH(keySplines))
		{
		}
		else if (MATCH(from))
		{
		}
		else if (MATCH(to))
		{
		}
*/		
		else
			return UnexpectedToken(pwchLocalName, cchLocalName, E_FAIL);
#undef MATCH_VALUE
#undef MATCH
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// animate validation
//
HRESULT XBContent::ValidateAnimation(AnimCurve *pCurve, BOOL bWrapAngle)
{
	if ( pCurve->m_rKeys == NULL 
		|| pCurve->m_KeyCount == 0 )
		return fatalError(m_pLocator, L"<animate> missing key values\n", E_FAIL);
	if (pCurve->m_rKeyTimes == NULL)
	{
		// make an evenly spaced key time array
		// TODO: space keys differently for calcMode=paced animation
		// TODO: consider leaving m_rKeyTimes array as NULL to
		// avoid passing around this data. 
		pCurve->m_rKeyTimes = new FLOAT [ pCurve->m_KeyCount ];
		if (pCurve->m_rKeyTimes == NULL)
			return fatalError(m_pLocator,
							  L"Memory allocation failed for <animate> keyTimes array.\n",
							  E_OUTOFMEMORY);
		pCurve->m_rKeyTimes[0] = 0.f;
		if (pCurve->m_KeyCount > 1)
		{
			FLOAT fScale = 1.f / (pCurve->m_KeyCount - 1);
			for (UINT iKey = 1; iKey < pCurve->m_KeyCount - 1; iKey++)
				pCurve->m_rKeyTimes[iKey] = (FLOAT)iKey * fScale;
			pCurve->m_rKeyTimes[pCurve->m_KeyCount - 1] = 1.f;
		}
	}
	if (pCurve->m_rKeyTimes[0] != 0.f
		|| (pCurve->m_KeyCount > 1 
		    && pCurve->m_rKeyTimes[pCurve->m_KeyCount-1] != 1.f))
	{
		ignorableWarning(m_pLocator, L"<animate> keyTimes do not start at 0.0 and end at 1.0\n", E_FAIL);
	}
	if (pCurve->m_KeyCount > 1 /* && bLooping */ )
	{
		FLOAT *pKey0 = &pCurve->m_rKeys[0];
		FLOAT *pKey1 = &pCurve->m_rKeys[(pCurve->m_KeyCount-1) * pCurve->m_KeySize];
		if (bWrapAngle)
		{
			if (pCurve->m_KeySize != 1)
				return fatalError(m_pLocator, L"<animate> expected single-valued keys for angle parameter\n", E_INVALIDARG);
			CONST FLOAT twopi = 2.f * D3DX_PI;
			CONST FLOAT feps = 1e-6f;
			FLOAT fAngle0 = fmodf(fmodf(*pKey0, twopi) + twopi, twopi);
			FLOAT fAngle1 = fmodf(fmodf(*pKey1, twopi) + twopi, twopi);
			if (fabsf(fAngle0 - fAngle1) > feps)
				ignorableWarning(m_pLocator, L"<animate> looping angle animation start and end key values do not match.\n", E_INVALIDARG);
		}
		else if (memcmp(pKey0, pKey1, sizeof(FLOAT) * pCurve->m_KeySize) != 0)
			ignorableWarning(m_pLocator, L"<animate> looping animation start and end key values do not match.\n", E_INVALIDARG);
	}
	return S_OK;
}
	
HRESULT XBContent::BeginMatrix_animate(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	if (m_rContextStack[m_iContext].m_Context != Matrix_Context) // sanity check
		return fatalError(m_pLocator, L"<animate> tag must appear within an enclosing <Matrix>\n", E_FAIL);
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Matrix_animate_Context);
	if (pFrame->m_pAnim != NULL)
		return fatalError(m_pLocator, L"<animate> must appear before any other type of animation (<Scale>, <Rotate>, or <Translate>)\n", E_FAIL);

	// Overwrite or create new matrix animation
	AnimMatrix *pAnim;
	if ( pFrame->m_pAnimLocal != NULL )
	{
		pAnim = (AnimMatrix *)pFrame->m_pAnimLocal;
		delete pAnim->m_curve.m_rKeyTimes;
		delete pAnim->m_curve.m_rKeys;
		pFrame->m_pAnimLocal = NULL;
	}
	else
	{
		pAnim = new AnimMatrix;
		if (pAnim == NULL)
			return fatalError(m_pLocator, L"<animate> matrix allocation failed\n", E_OUTOFMEMORY);
	}
	pAnim->m_curve.m_KeyCount = 0;
	pAnim->m_curve.m_KeySize = 16;
	pAnim->m_curve.m_rKeys = NULL;
	pAnim->m_curve.m_rKeyTimes = NULL;
	pAnim->m_curve.m_fBegin = 0.f;
	pAnim->m_curve.m_fDurationInverse = 1.f;

	// Parse <animate> attributes
	hr = animate(&pAnim->m_curve, pAttributes, L"value");
	if (FAILED(hr))
		return hr;
	hr = ValidateAnimation(&pAnim->m_curve, false);
	if (FAILED(hr))
		return hr;

	// Add to frame animation expression
 	return pFrame->Premult(pAnim);
}


HRESULT XBContent::EndMatrix_animate()
{
	PopContext();
	return S_OK;
}

HRESULT XBContent::BeginTranslate_animate(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	if (m_rContextStack[m_iContext].m_Context != Translate_Context) // sanity check
		return fatalError(m_pLocator, L"<animate> tag must appear within an enclosing <Translate>\n", E_FAIL);
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Translate_animate_Context);

	// Overwrite or create new translate animation
	AnimTranslate *pAnim;
	if ( pFrame->m_pAnimLocal != NULL )
	{
		pAnim = (AnimTranslate *)pFrame->m_pAnimLocal;
		delete pAnim->m_curve.m_rKeyTimes;
		delete pAnim->m_curve.m_rKeys;
		pFrame->m_pAnimLocal = NULL;
	}
	else
	{
		pAnim = new AnimTranslate;
		if (pAnim == NULL)
			return fatalError(m_pLocator, L"<animate> translate allocation failed\n", E_OUTOFMEMORY);
	}
	pAnim->m_curve.m_KeyCount = 0;
	pAnim->m_curve.m_KeySize = 3;
	pAnim->m_curve.m_rKeys = NULL;
	pAnim->m_curve.m_rKeyTimes = NULL;
	pAnim->m_curve.m_fBegin = 0.f;
	pAnim->m_curve.m_fDurationInverse = 1.f;

	// Parse <animate> attributes
	hr = animate(&pAnim->m_curve, pAttributes, L"value");
	if (FAILED(hr))
		return hr;
	hr = ValidateAnimation(&pAnim->m_curve, false);
	if (FAILED(hr))
		return hr;

	// Add to frame animation expression
 	return pFrame->Premult(pAnim);
}

HRESULT XBContent::EndTranslate_animate(void)
{
	PopContext();
	return S_OK;
}

HRESULT XBContent::BeginScale_animate(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	if (m_rContextStack[m_iContext].m_Context != Scale_Context) // sanity check
		return fatalError(m_pLocator, L"<animate> tag must appear within an enclosing <Scale>\n", E_FAIL);
	FrameExtra *pFrame = (FrameExtra *)m_rContextStack[m_iContext].m_pData;
	PushContext(Scale_animate_Context);
	
	// Overwrite or create new scale animation
	AnimScale *pAnim;
	if ( pFrame->m_pAnimLocal != NULL )
	{
		pAnim = (AnimScale *)pFrame->m_pAnimLocal;
		delete pAnim->m_curve.m_rKeyTimes;
		delete pAnim->m_curve.m_rKeys;
		pFrame->m_pAnimLocal = NULL;
	}
	else
	{
		pAnim = new AnimScale;
		if (pAnim == NULL)
			return fatalError(m_pLocator, L"<animate> scale allocation failed\n", E_OUTOFMEMORY);
	}
	pAnim->m_curve.m_KeyCount = 0;
	pAnim->m_curve.m_KeySize = 3;
	pAnim->m_curve.m_rKeys = NULL;
	pAnim->m_curve.m_rKeyTimes = NULL;
	pAnim->m_curve.m_fBegin = 0.f;
	pAnim->m_curve.m_fDurationInverse = 1.f;

	// Parse <animate> attributes
	hr = animate(&pAnim->m_curve, pAttributes, L"value");
	if (FAILED(hr))
		return hr;
	hr = ValidateAnimation(&pAnim->m_curve, false);
	if (FAILED(hr))
		return hr;
	
	// Add to frame animation expression
 	return pFrame->Premult(pAnim);
}

HRESULT XBContent::EndScale_animate(void)
{
	PopContext();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Rotate animation handling
//
HRESULT XBContent::BeginRotate_animate(ISAXAttributes *pAttributes)
{
	HRESULT hr;
	if (m_rContextStack[m_iContext].m_Context != Rotate_Context) // sanity check
		return fatalError(m_pLocator, L"<animate> tag must appear within an enclosing <Rotate>\n", E_FAIL);
	RotateContext *pRotateContext = (RotateContext *)m_rContextStack[m_iContext].m_pData;
	PushContext(Rotate_animate_Context);
	FrameExtra *pFrame = pRotateContext->m_pFrame;

	// Overwrite or create new rotate animation
	AnimRotate *pAnim;
	if ( pFrame->m_pAnimLocal != NULL )
	{
		pAnim = (AnimRotate *)pFrame->m_pAnimLocal;
		delete pAnim->m_curve.m_rKeyTimes;
		delete pAnim->m_curve.m_rKeys;
		pFrame->m_pAnimLocal = NULL;
	}
	else
	{
		pAnim = new AnimRotate;
		if (pAnim == NULL)
			return fatalError(m_pLocator, L"<animate> rotate allocation failed\n", E_OUTOFMEMORY);
	}
	pAnim->m_curve.m_KeyCount = 0;
	pAnim->m_curve.m_KeySize = 4;
	pAnim->m_curve.m_rKeys = NULL;
	pAnim->m_curve.m_rKeyTimes = NULL;
	pAnim->m_curve.m_fBegin = 0.f;
	pAnim->m_curve.m_fDurationInverse = 1.f;

	// Look for attribute tag to see if the user wants axis/angle animation or standard quaternion animation
	bool bAnimateAngle = false;
	int iAttributeCount;
	pAttributes->getLength(&iAttributeCount);
	for ( int iAttribute = 0; iAttribute < iAttributeCount; iAttribute++ ) {
		WCHAR *pwchLocalName;
		int cchLocalName;
		pAttributes->getLocalName(iAttribute, &pwchLocalName, &cchLocalName); 
		WCHAR *pwchValue;
		int cchValue;
		pAttributes->getValue(iAttribute, &pwchValue, &cchValue);
#define MATCH(TOKEN) Match(L#TOKEN, pwchLocalName, cchLocalName)
#define MATCH_VALUE(TOKEN) Match(L#TOKEN, pwchValue, cchValue)
		if (MATCH(attribute))
		{
			if (MATCH_VALUE(value))
			{
				break;	// standard quaternion animation
			}
			else if (MATCH_VALUE(angle))
			{
				bAnimateAngle = true;
				break;
			}
			else if (MATCH_VALUE(axis))
			{
				return fatalError(m_pLocator,
								  L"<animate> \"axis\" attribute not supported. Use quaternion \"value\" attribute instead.",
								  E_INVALIDARG);
			}
			else
			{
				const int buflen = 200;
				const int namelen = 150;
				WCHAR buf[buflen];
				_snwprintf(buf, buflen,
						   L"<animate> attribute \"%.*s\" not supported.",
						   cchValue < namelen ? cchValue : namelen, pwchValue);
				return fatalError(m_pLocator, buf, E_INVALIDARG);
			}
		}
#undef MATCH_VALUE
#undef MATCH
	}
	
	// Parse <animate> attributes
	if (bAnimateAngle)
	{
		if (pRotateContext->m_mode == RCM_QUATERNION)
		{
			// convert quaternion to angle axis
			D3DXQuaternionNormalize( &pRotateContext->m_quat, &pRotateContext->m_quat);
			D3DXQuaternionToAxisAngle( &pRotateContext->m_quat, &pRotateContext->m_vAxis, &pRotateContext->m_fAngle);
		}

		// Parse <animate attribute="angle"> values
		AnimCurve curveAngle;
		curveAngle.m_KeyCount = 0;
		curveAngle.m_KeySize = 1;
		curveAngle.m_rKeys = NULL;
		curveAngle.m_rKeyTimes = NULL;
		curveAngle.m_fBegin = 0.f;
		curveAngle.m_fDurationInverse = 1.f;
		hr = animate(&curveAngle, pAttributes, L"angle");
		if (FAILED(hr))
			return hr;
		hr = ValidateAnimation(&curveAngle, true);
		if (FAILED(hr))
			return hr;

		// Convert axis/angle keys to quaternion keys, with reasonable precision. 
		hr = MatchAxisAngleCurve(pAnim, &pRotateContext->m_vAxis, &curveAngle, 1e-6f);
		if (FAILED(hr))
			return hr;
	}
	else
	{
		// Standard quaternion animation keys
		hr = animate(&pAnim->m_curve, pAttributes, L"value");
		if (FAILED(hr))
			return hr;
		hr = ValidateAnimation(&pAnim->m_curve, false);
		if (FAILED(hr))
			return hr;
	}

	// Add to frame animation expression
 	return pFrame->Premult(pAnim);
}

HRESULT XBContent::EndRotate_animate(void)
{
	PopContext();
	return S_OK;
}





//////////////////////////////////////////////////////////////////////
// Write the skeleton as a resource with frame pointers as offsets
// from beginning of structure.
//
HRESULT XBContent::WriteSkeleton(FrameExtra *pFrameRoot)
{
	struct UserData {
		DWORD dwType;				// XBRC_SKELETON
		DWORD dwSize;				// total size (not including this header)
	};
	Skeleton skeletonSrc;
	skeletonSrc.m_pRoot = NULL;
	skeletonSrc.m_FrameCount = 0;
	skeletonSrc.m_rMatrix = NULL;
	HRESULT hr = skeletonSrc.SetRoot(pFrameRoot);
	if (FAILED(hr))
		return fatalError(m_pLocator, L"Frame hierarchy can not be created.\n", hr);
	skeletonSrc.UpdateTransformations();	// calculate cumulative transformations to be written to file
	
	// Make a resource identifier
	m_Count.m_Skeleton++;
	int cch = strlen(pFrameRoot->m_strName);
	if (cch == 0)
	{
		// Generate a name for the skeleton if the top frame's name is blank
		ZeroMemory(pFrameRoot->m_strName, FRAME_IDENTIFIER_SIZE);
		_snprintf(pFrameRoot->m_strName, FRAME_IDENTIFIER_SIZE, "Skeleton%d", m_Count.m_Skeleton);
		pFrameRoot->m_strName[FRAME_IDENTIFIER_SIZE - 1] = 0;
		cch = strlen(pFrameRoot->m_strName);
	}
	if (cch > BUNDLER_IDENTIFIER_MAX_LENGTH - 1)
		cch = BUNDLER_IDENTIFIER_MAX_LENGTH - 1;
	CHAR *strSkeletonID = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strSkeletonID, BUNDLER_IDENTIFIER_MAX_LENGTH);
	strncpy(strSkeletonID, pFrameRoot->m_strName, cch);
	strSkeletonID[cch] = 0;	// make sure the string is null terminated
	
	// Keep track of offset into header as a bundler resource
    DWORD cbSkeletonHeader = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;
	cbSkeletonHeader += sizeof(UserData);	// skip user data
	m_pBundler->m_nResourceCount++;
		
	// Calculate size of variable-length arrays
	DWORD dwSize = sizeof(Skeleton) + skeletonSrc.m_FrameCount * (sizeof(D3DXMATRIX) + sizeof(Frame));
	DWORD dwAlign = 16; 	// matrices need 16-byte alignment
	DWORD cbFill = 0;
	if ((cbSkeletonHeader + sizeof(Skeleton)) % 16)
		cbFill = dwAlign - ( (cbSkeletonHeader + sizeof(Skeleton)) % dwAlign );
	dwSize += cbFill;

	// Copy skeleton frame count and matrix data
	BYTE *pData0 = new BYTE [ dwSize ];	// skeleton + matrix array + frame array
	if (pData0 == NULL)
		return fatalError(m_pLocator, L"Memory allocation failed for Skeleton.\n", E_OUTOFMEMORY);
	BYTE *pData = pData0;
	Skeleton *pSkeletonDst = (Skeleton *)pData;
	*pSkeletonDst = skeletonSrc;	// copy skeleton data, pointers will be overwritten below
	pData += sizeof(Skeleton);
	if (cbFill) // Write alignment filler
	{
		memset(pData, 0xAA, cbFill);
		pData += cbFill;
	}
	*(DWORD *)&pSkeletonDst->m_rMatrix = pData - pData0 + cbSkeletonHeader; // set file offset for matrices
	memcpy(pData, skeletonSrc.m_rMatrix, skeletonSrc.m_FrameCount * sizeof(FrameMatrix));
	pData += skeletonSrc.m_FrameCount * sizeof(FrameMatrix);

	// Flatten frame hierarchy and get file offsets
	struct FrameStackElement {
		Frame *pFrame;
		DWORD *pPreviousOffset;	// file offset to be filled in when node is processed
	} rFrameStack[FRAME_MAX_STACK];
	int iFrameStack = 1;	// depth of stack
	rFrameStack[0].pFrame = skeletonSrc.m_pRoot;	// put root on top of stack
	rFrameStack[0].pPreviousOffset = (DWORD *)&pSkeletonDst->m_pRoot;	// pRoot will be filled in below as first offset
	UINT iMatrix = 0;	// index to current entry in m_rMatrix
	while (iFrameStack)
	{
		// Pop the stack and process the top element
		iFrameStack--;
		Frame *pFrameSrc = rFrameStack[iFrameStack].pFrame;
		DWORD *pPreviousOffset = rFrameStack[iFrameStack].pPreviousOffset;
		((FrameExtra *)pFrameSrc)->m_Offset			// set offset in original FrameExtra struct for later use when writing Animation data
			= *pPreviousOffset						// set file offset of parent or sibling in destination
			= pData - pData0 + cbSkeletonHeader;
		Frame *pFrameDst = (Frame *)pData;
		*pFrameDst = *pFrameSrc;	// NULLS are copied as 0's
		pData += sizeof(Frame);	// go to next slot
		
		// Look through all the vertex shader parameter sets
		AnimVertexShaderParameter *pAnimVSP = m_pAnimVSPList;
		while (pAnimVSP)
		{
			// TODO: handle simple expressions, such as Inverse(foo) and foo * bar and world * view * projection
			if (strcmp(pAnimVSP->m_strExpression, pFrameSrc->m_strName) == 0)
			{
				if (!m_bErrorSuppress)
				{
					CONST INT buflen=1000;
					WCHAR buf[buflen];
					_snwprintf(buf, buflen, L"Found duplicate frame id=\"%S\"", pFrameSrc->m_strName);
					buf[buflen-1] = 0;	// make sure string is null-terminated
					ignorableWarning(m_pLocator, buf, E_FAIL);
					m_bErrorSuppress = true;
				}
				pAnimVSP->m_pSource = pSkeletonDst->m_rMatrix + iMatrix;	// file offset of current source matrix
			}
			pAnimVSP = pAnimVSP->m_pNext;
		}

		// Push sibling
		if (pFrameSrc->m_pNext != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrameSrc->m_pNext;
			rFrameStack[iFrameStack].pPreviousOffset = (DWORD *)&pFrameDst->m_pNext; // overwrite with local frame file offset
			iFrameStack++;
		}
		
		// Push child
		if (pFrameSrc->m_pChild != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrameSrc->m_pChild;	// source pointer will be overwritten...
			rFrameStack[iFrameStack].pPreviousOffset = (DWORD *)&pFrameDst->m_pChild; // with local frame file offset
			iFrameStack++;
		}

		// Increment location in matrix paletter
		iMatrix++;
	}
	ASSERT(pData - pData0 == (INT)dwSize);
	
	// Helper writing macro
#define WRITE(POINTER, SIZE)								\
	if (FAILED(m_pBundler->WriteHeader( POINTER, SIZE )))	\
		return E_FAIL;										\
	m_pBundler->m_cbHeader += SIZE;	// TODO: this should be done in WriteHeader
	
	// write to bundle
	UserData SkeletonUserData;
	SkeletonUserData.dwType = XBRC_SKELETON;
	SkeletonUserData.dwSize = dwSize;
	WRITE( &SkeletonUserData, sizeof(UserData) );
	WRITE( pData0, dwSize );
	
	// Cleanup, since we will no longer refer to data within the block, only offsets
	delete [] pData0;
	if (skeletonSrc.m_rMatrix)
		delete [] skeletonSrc.m_rMatrix;
	
    printf("Skeleton: wrote %s (%d bytes)\n", strSkeletonID, m_pBundler->m_cbHeader - cbSkeletonHeader + 8);
	
	return S_OK;
}




//////////////////////////////////////////////////////////////////////
// Write the current animation list as a resource with frame pointers
// as offsets from beginning of structure.
//
HRESULT XBContent::WriteAnimation(AnimLink *pAnimList)
{
	// TODO: the animation data should be in fast GPU memory, since
	// that's where the fixup is going to come from.
	struct UserData {
		DWORD dwType;				// XBRC_ANIMATION
		DWORD dwSize;				// total size (not including this header)
	};
	
	// Generate a name for the animation resource
	m_Count.m_Animation++;	
	CHAR *strAnimID = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].strIdentifier;
	ZeroMemory(strAnimID, BUNDLER_IDENTIFIER_MAX_LENGTH);
	_snprintf(strAnimID, BUNDLER_IDENTIFIER_MAX_LENGTH, "Animation%d", 	m_Count.m_Animation);
	strAnimID[BUNDLER_IDENTIFIER_MAX_LENGTH - 1] = 0;	// make sure the string is null terminated
	
	// Keep track of offset into header as a bundler resource
	DWORD cbAnimHeader = m_pBundler->m_aResources[m_pBundler->m_nResourceCount].dwOffset = m_pBundler->m_cbHeader;
	cbAnimHeader += sizeof(UserData);	// skip UserData
	m_pBundler->m_nResourceCount++;

	// Add up sizes of vertex shader parameters and animation lists
	DWORD dwSize = 0;
	dwSize += sizeof(Animation);
	
	// Get size of vertex shader parameters
	DWORD VSPCount = 0;
	AnimVertexShaderParameter *pAnimVSP;
	for (pAnimVSP = m_pAnimVSPList; pAnimVSP != NULL; pAnimVSP = pAnimVSP->m_pNext)
	{
		// Write only those parameters that are active
		if (pAnimVSP->m_pSource != NULL)
		{
			dwSize += sizeof(AnimVSP);
			VSPCount++;
		}
	}
	
	// Get size and count of animation list
	DWORD AnimCount = 0;
	AnimLink *pAnimSrc;
	for (pAnimSrc = pAnimList; pAnimSrc; pAnimSrc = pAnimSrc->m_pNext)
	{
		AnimCount++;
		dwSize += sizeof(AnimLink);
	}

	// Get size of animation expressions
	for (pAnimSrc = pAnimList; pAnimSrc; pAnimSrc = pAnimSrc->m_pNext)
	{
		AnimFrame *pAnimFrame = pAnimSrc->m_pAnimFrame;
		dwSize += pAnimFrame->GetSize();
	}

	// Make a copy of the data, with pointers converted to file offsets
	BYTE *pAnimData0 = new BYTE [ dwSize ];
	BYTE *pAnimData = pAnimData0;
	
	// Make a copy of the VSP list
	Animation *pAnimation = (Animation *)pAnimData;
	pAnimData += sizeof(Animation);
	pAnimation->m_VSPCount = VSPCount;
	if (VSPCount == 0)
		pAnimation->m_rVSP = NULL;
	else
		*(DWORD *)&pAnimation->m_rVSP = pAnimData - pAnimData0 + cbAnimHeader;	// file offset
	pAnimation->m_AnimCount = AnimCount;
	*(DWORD *)&pAnimation->m_pAnimList = pAnimData + VSPCount * sizeof(AnimVSP) - pAnimData0 + cbAnimHeader;	// file offset
	for (pAnimVSP = m_pAnimVSPList; pAnimVSP != NULL; pAnimVSP = pAnimVSP->m_pNext)
	{
		if (pAnimVSP->m_pSource != NULL)
		{
			// Lookup vertex shader resource
			DWORD ResourceIndex;
			if (!m_pBundler->IsExistingIdentifier( pAnimVSP->m_strVertexShaderReference, &ResourceIndex))
			{
				m_pBundler->ErrorMsg("VertexShader \"%s\" not defined as a resource for parameter \"%s\".\n", 
					pAnimVSP->m_strVertexShaderReference, pAnimVSP->m_strExpression);
				return E_FAIL;
			}
			AnimVSP *pVSP = (AnimVSP *)pAnimData;
			pVSP->m_VertexShaderReference = ResourceIndex;
			pVSP->m_Index = pAnimVSP->m_Index;
			pVSP->m_Count = pAnimVSP->m_Count;
			pVSP->m_pSource = pAnimVSP->m_pSource;	// file offset set above
			pAnimVSP->m_pSource = NULL;	// clear source setting
			pAnimData += sizeof(AnimVSP);	// go to next record
		}
	}
	
	// Make a copy of the animation list, replacing pointers with file offsets
	AnimLink *pAnimDst = (AnimLink *)pAnimData;
	BYTE *pAnimFrameData = pAnimData + sizeof(AnimLink) * AnimCount;
	for (pAnimSrc = m_pAnimList; pAnimSrc; pAnimSrc = pAnimSrc->m_pNext, pAnimDst++)
	{
		// set file offsets in header
		if (pAnimSrc->m_pNext == NULL)
			pAnimDst->m_pNext = NULL;
		else
			*(DWORD *)&pAnimDst->m_pNext = (BYTE *)(pAnimDst + 1) - pAnimData0 + cbAnimHeader;
		*(DWORD *)&pAnimDst->m_pAnimFrame = pAnimFrameData - pAnimData0 + cbAnimHeader;
		*(DWORD *)&pAnimDst->m_pFrame = ((FrameExtra *)pAnimSrc->m_pFrame)->m_Offset;

		// fill in variably-sized animation-expression data
		AnimFrame *pAnimFrameSrc = pAnimSrc->m_pAnimFrame;
		pAnimFrameSrc->RelocateCopy(pAnimFrameData, (CONST BYTE *)((DWORD)pAnimData0 - cbAnimHeader));
		pAnimFrameData += pAnimFrameSrc->GetSize();
	}

	// Write to bundle
	UserData AnimUserData;
	AnimUserData.dwType = XBRC_ANIMATION;
	AnimUserData.dwSize = dwSize;
	WRITE( &AnimUserData, sizeof(UserData) );
	WRITE( pAnimData0, dwSize );
#undef WRITE
		
	printf("Animation: wrote %s (%d bytes)\n", strAnimID, m_pBundler->m_cbHeader - cbAnimHeader + 8);
		
	// Cleanup animation data
	delete [] pAnimData0;
	return S_OK;
}
