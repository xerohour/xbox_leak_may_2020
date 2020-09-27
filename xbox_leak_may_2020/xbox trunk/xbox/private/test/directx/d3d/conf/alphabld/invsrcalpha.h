/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	InvSrcAlpha.h
 *
 ***************************************************************************/

#ifndef __INVSRCALPHA_H__
#define __INVSRCALPHA_H__

// InvSrcAlpha/Zero Class definitions
class CInvSrcAlphaZeroTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaZeroTest();
	~CInvSrcAlphaZeroTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/One Class definitions
class CInvSrcAlphaOneTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaOneTest();
	~CInvSrcAlphaOneTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/SrcColor Class definitions
class CInvSrcAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcColorTest();
	~CInvSrcAlphaSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/InvSrcColor Class definitions
class CInvSrcAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvSrcColorTest();
	~CInvSrcAlphaInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/SrcAlpha Class definitions
class CInvSrcAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcAlphaTest();
	~CInvSrcAlphaSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/InvSrcAlpha Class definitions
class CInvSrcAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvSrcAlphaTest();
	~CInvSrcAlphaInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/DestAlpha Class definitions
class CInvSrcAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaDestAlphaTest();
	~CInvSrcAlphaDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/InvDestAlpha Class definitions
class CInvSrcAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvDestAlphaTest();
	~CInvSrcAlphaInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/DestColor Class definitions
class CInvSrcAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaDestColorTest();
	~CInvSrcAlphaDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/InvDestColor Class definitions
class CInvSrcAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaInvDestColorTest();
	~CInvSrcAlphaInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcAlpha/SrcAlphaSat Class definitions
class CInvSrcAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvSrcAlphaSrcAlphaSatTest();
	~CInvSrcAlphaSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif