//-----------------------------------------------------------------------------
//  
//  File: frame.cpp
//  Copyright (C) 2001 Microsoft Corporation
//  All rights reserved.
//  
//-----------------------------------------------------------------------------
#include "frame.h"
#include "print.h"

//////////////////////////////////////////////////////////////////////
// Set root of frame hierarchy and allocate matrix array
//
HRESULT Skeleton::SetRoot(Frame *pRoot)
{
	m_pRoot = pRoot;
	m_FrameCount = 0;
	if (m_rMatrix != NULL)
	{
		delete [] m_rMatrix;
		m_rMatrix = NULL;
	}
		
	// Count the number of frames
	Frame *rpFrameStack[FRAME_MAX_STACK];
	int iFrameStack = 1;	// depth of stack
	rpFrameStack[0] = pRoot;	// put root on top of stack
	while (iFrameStack)
	{
		// Pop the stack
		iFrameStack--;
		Frame *pFrame = rpFrameStack[iFrameStack];
		m_FrameCount++;

		// Push sibling
		if (pFrame->m_pNext != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rpFrameStack[iFrameStack] = pFrame->m_pNext;
			iFrameStack++;
		}
		
		// Push child
		if (pFrame->m_pChild != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rpFrameStack[iFrameStack] = pFrame->m_pChild;
			iFrameStack++;
		}
	}

	// Allocate the matrix array
	m_rMatrix = new XGMATRIX [ m_FrameCount ];
	if (m_rMatrix == NULL)
		return E_OUTOFMEMORY;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Do depth-first traversal of frame hierarchy to compute the
// matrix palette.
//
// TODO: Not all of these transformations are actually used
// in the final result.  Do dependency analysis and avoid
// computing matrices that are not used.
//
HRESULT Skeleton::UpdateTransformations()
{
	// Iterate through frame hierarchy
	struct FrameStackElement {
		Frame *pFrame;
		int iMatrixParent;	// index into m_rMatrix
	} rFrameStack[FRAME_MAX_STACK];
	rFrameStack[0].pFrame = m_pRoot;
	rFrameStack[0].iMatrixParent = -1;
	int iMatrix = 0;	// index to current entry in m_rMatrix
	int iFrameStack = 1;	// depth of stack
	while (iFrameStack)
	{
		// Pop the stack
		iFrameStack--;
		Frame *pFrame = rFrameStack[iFrameStack].pFrame;
		int iMatrixParent = rFrameStack[iFrameStack].iMatrixParent;

		// Compute cumulative transformation
		if (iMatrixParent == -1)
			m_rMatrix[iMatrix] = pFrame->m_Matrix;	// set initial transformation
		else
			XGMatrixMultiply(&m_rMatrix[iMatrix], &pFrame->m_Matrix, &m_rMatrix[iMatrixParent]); // compute transformation from parent

		// Push sibling
		if (pFrame->m_pNext != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrame->m_pNext;
			rFrameStack[iFrameStack].iMatrixParent = iMatrixParent;	// use same parent as before
			iFrameStack++;
		}
		
		// Push child
		if (pFrame->m_pChild != NULL)
		{
			if (iFrameStack >= FRAME_MAX_STACK)
				return E_FAIL; // we hit our hard-coded stack-depth limit
			rFrameStack[iFrameStack].pFrame = pFrame->m_pChild;
			rFrameStack[iFrameStack].iMatrixParent = iMatrix;	// use our matrix as child's parent matrix
			iFrameStack++;
		}

		iMatrix++;
	}
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PrintMatrix
// Desc: Print a formatted version of a matrix
//-----------------------------------------------------------------------------
HRESULT PrintMatrix(XGMATRIX *pMatrix)
{
	for (UINT iRow = 0; iRow < 4; iRow++)
		Printf(L"  %f %f %f %f\n",
			   (*pMatrix)( iRow, 0 ),
			   (*pMatrix)( iRow, 1 ),
			   (*pMatrix)( iRow, 2 ),
			   (*pMatrix)( iRow, 3 ));
	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: PrintFrame
// Desc: Print a formatted version of a frame
//-----------------------------------------------------------------------------
HRESULT PrintFrame(Frame *pFrame)
{
	WCHAR strName[FRAME_IDENTIFIER_SIZE];
	mbstowcs( strName, pFrame->m_strName, FRAME_IDENTIFIER_SIZE );
	strName[FRAME_IDENTIFIER_SIZE - 1] = 0;	// make sure string is null-terminated
	Out(strName); 
	Out(L"\n  local\n");
	PrintMatrix(&pFrame->m_Matrix);
	return S_OK;
}
				   
