/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	SrcColor.h
 *
 ***************************************************************************/

#ifndef __SRCCOLOR_H__
#define __SRCCOLOR_H__

// SrcColor/Zero Class definitions
class CSrcColorZeroTest: public CAlphaBldTest
{
	public:
	CSrcColorZeroTest();
	~CSrcColorZeroTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/One Class definitions
class CSrcColorOneTest: public CAlphaBldTest
{
	public:
	CSrcColorOneTest();
	~CSrcColorOneTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/SrcColor Class definitions
class CSrcColorSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcColorTest();
	~CSrcColorSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/InvSrcColor Class definitions
class CSrcColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcColorInvSrcColorTest();
	~CSrcColorInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/SrcAlpha Class definitions
class CSrcColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcAlphaTest();
	~CSrcColorSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/InvSrcAlpha Class definitions
class CSrcColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorInvSrcAlphaTest();
	~CSrcColorInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/DestAlpha Class definitions
class CSrcColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorDestAlphaTest();
	~CSrcColorDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/InvDestAlpha Class definitions
class CSrcColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcColorInvDestAlphaTest();
	~CSrcColorInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/DestColor Class definitions
class CSrcColorDestColorTest: public CAlphaBldTest
{
	public:
	CSrcColorDestColorTest();
	~CSrcColorDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/InvDestColor Class definitions
class CSrcColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcColorInvDestColorTest();
	~CSrcColorInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcColor/SrcAlphaSat Class definitions
class CSrcColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcColorSrcAlphaSatTest();
	~CSrcColorSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif