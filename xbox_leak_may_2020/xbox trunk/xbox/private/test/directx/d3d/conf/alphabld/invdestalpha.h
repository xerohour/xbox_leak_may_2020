/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	InvDestAlpha.h
 *
 ***************************************************************************/

#ifndef __INVDESTALPHA_H__
#define __INVDESTALPHA_H__

// InvDestAlpha/Zero Class definitions
class CInvDestAlphaZeroTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaZeroTest();
	~CInvDestAlphaZeroTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/One Class definitions
class CInvDestAlphaOneTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaOneTest();
	~CInvDestAlphaOneTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/SrcColor Class definitions
class CInvDestAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcColorTest();
	~CInvDestAlphaSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/InvSrcColor Class definitions
class CInvDestAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvSrcColorTest();
	~CInvDestAlphaInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/SrcAlpha Class definitions
class CInvDestAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcAlphaTest();
	~CInvDestAlphaSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/InvSrcAlpha Class definitions
class CInvDestAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvSrcAlphaTest();
	~CInvDestAlphaInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/DestAlpha Class definitions
class CInvDestAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaDestAlphaTest();
	~CInvDestAlphaDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/InvDestAlpha Class definitions
class CInvDestAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvDestAlphaTest();
	~CInvDestAlphaInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/DestColor Class definitions
class CInvDestAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaDestColorTest();
	~CInvDestAlphaDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/InvDestColor Class definitions
class CInvDestAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaInvDestColorTest();
	~CInvDestAlphaInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestAlpha/SrcAlphaSat Class definitions
class CInvDestAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvDestAlphaSrcAlphaSatTest();
	~CInvDestAlphaSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif