/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	Zero.h
 *
 ***************************************************************************/

#ifndef __ZERO_H__
#define __ZERO_H__

// Zero/Zero Class definitions
class CZeroZeroTest: public CAlphaBldTest
{
	public:
	CZeroZeroTest();
	~CZeroZeroTest();

	bool SetDefaultRenderStates(void);
};

// Zero/One Class definitions
class CZeroOneTest: public CAlphaBldTest
{
	public:
	CZeroOneTest();
	~CZeroOneTest();

	bool SetDefaultRenderStates(void);
};

// Zero/SrcColor Class definitions
class CZeroSrcColorTest: public CAlphaBldTest
{
	public:
	CZeroSrcColorTest();
	~CZeroSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// Zero/InvSrcColor Class definitions
class CZeroInvSrcColorTest: public CAlphaBldTest
{
	public:
	CZeroInvSrcColorTest();
	~CZeroInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// Zero/SrcAlpha Class definitions
class CZeroSrcAlphaTest: public CAlphaBldTest
{
	public:
	CZeroSrcAlphaTest();
	~CZeroSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Zero/InvSrcAlpha Class definitions
class CZeroInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CZeroInvSrcAlphaTest();
	~CZeroInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Zero/DestAlpha Class definitions
class CZeroDestAlphaTest: public CAlphaBldTest
{
	public:
	CZeroDestAlphaTest();
	~CZeroDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Zero/InvDestAlpha Class definitions
class CZeroInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CZeroInvDestAlphaTest();
	~CZeroInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// Zero/DestColor Class definitions
class CZeroDestColorTest: public CAlphaBldTest
{
	public:
	CZeroDestColorTest();
	~CZeroDestColorTest();

	bool SetDefaultRenderStates(void);
};

// Zero/InvDestColor Class definitions
class CZeroInvDestColorTest: public CAlphaBldTest
{
	public:
	CZeroInvDestColorTest();
	~CZeroInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// Zero/SrcAlphaSat Class definitions
class CZeroSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CZeroSrcAlphaSatTest();
	~CZeroSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif