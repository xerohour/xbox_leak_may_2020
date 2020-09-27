/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	DestAlpha.h
 *
 ***************************************************************************/

#ifndef __DESTALPHA_H__
#define __DESTALPHA_H__

// DestAlpha/Zero Class definitions
class CDestAlphaZeroTest: public CAlphaBldTest
{
	public:
	CDestAlphaZeroTest();
	~CDestAlphaZeroTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/One Class definitions
class CDestAlphaOneTest: public CAlphaBldTest
{
	public:
	CDestAlphaOneTest();
	~CDestAlphaOneTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/SrcColor Class definitions
class CDestAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcColorTest();
	~CDestAlphaSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/InvSrcColor Class definitions
class CDestAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvSrcColorTest();
	~CDestAlphaInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/SrcAlpha Class definitions
class CDestAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcAlphaTest();
	~CDestAlphaSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/InvSrcAlpha Class definitions
class CDestAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvSrcAlphaTest();
	~CDestAlphaInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/DestAlpha Class definitions
class CDestAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaDestAlphaTest();
	~CDestAlphaDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/InvDestAlpha Class definitions
class CDestAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvDestAlphaTest();
	~CDestAlphaInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/DestColor Class definitions
class CDestAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaDestColorTest();
	~CDestAlphaDestColorTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/InvDestColor Class definitions
class CDestAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CDestAlphaInvDestColorTest();
	~CDestAlphaInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// DestAlpha/SrcAlphaSat Class definitions
class CDestAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CDestAlphaSrcAlphaSatTest();
	~CDestAlphaSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif