/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	SrcAlpha.h
 *
 ***************************************************************************/

#ifndef __SRCALPHA_H__
#define __SRCALPHA_H__

// SrcAlpha/Zero Class definitions
class CSrcAlphaZeroTest: public CAlphaBldTest
{
	public:
	CSrcAlphaZeroTest();
	~CSrcAlphaZeroTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/One Class definitions
class CSrcAlphaOneTest: public CAlphaBldTest
{
	public:
	CSrcAlphaOneTest();
	~CSrcAlphaOneTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/SrcColor Class definitions
class CSrcAlphaSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcColorTest();
	~CSrcAlphaSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/InvSrcColor Class definitions
class CSrcAlphaInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvSrcColorTest();
	~CSrcAlphaInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/SrcAlpha Class definitions
class CSrcAlphaSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcAlphaTest();
	~CSrcAlphaSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/InvSrcAlpha Class definitions
class CSrcAlphaInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvSrcAlphaTest();
	~CSrcAlphaInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/DestAlpha Class definitions
class CSrcAlphaDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaDestAlphaTest();
	~CSrcAlphaDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/InvDestAlpha Class definitions
class CSrcAlphaInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvDestAlphaTest();
	~CSrcAlphaInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/DestColor Class definitions
class CSrcAlphaDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaDestColorTest();
	~CSrcAlphaDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/InvDestColor Class definitions
class CSrcAlphaInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaInvDestColorTest();
	~CSrcAlphaInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlpha/SrcAlphaSat Class definitions
class CSrcAlphaSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSrcAlphaSatTest();
	~CSrcAlphaSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif