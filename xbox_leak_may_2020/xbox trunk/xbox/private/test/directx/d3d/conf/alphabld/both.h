/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Both.h
 *
 ***************************************************************************/

#ifndef __BOTH_H__
#define __BOTH_H__

#ifndef UNDER_XBOX

// BothSrcAlpha Class definitions
class CBothSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBothSrcAlphaTest();
	~CBothSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// BothInvSrcAlpha Class definitions
class CBothInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CBothInvSrcAlphaTest();
	~CBothInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

#endif // !UNDER_XBOX

#endif