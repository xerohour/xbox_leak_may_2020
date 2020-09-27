/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	DestColor.h
 *
 ***************************************************************************/

#ifndef __DESTCOLOR_H__
#define __DESTCOLOR_H__

// DestColor/Zero Class definitions
class CDestColorZeroTest: public CAlphaBldTest
{
	public:
	CDestColorZeroTest();
	~CDestColorZeroTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/One Class definitions
class CDestColorOneTest: public CAlphaBldTest
{
	public:
	CDestColorOneTest();
	~CDestColorOneTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/SrcColor Class definitions
class CDestColorSrcColorTest: public CAlphaBldTest
{
	public:
	CDestColorSrcColorTest();
	~CDestColorSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/InvSrcColor Class definitions
class CDestColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CDestColorInvSrcColorTest();
	~CDestColorInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/SrcAlpha Class definitions
class CDestColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorSrcAlphaTest();
	~CDestColorSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/InvSrcAlpha Class definitions
class CDestColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorInvSrcAlphaTest();
	~CDestColorInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/DestAlpha Class definitions
class CDestColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorDestAlphaTest();
	~CDestColorDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/InvDestAlpha Class definitions
class CDestColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CDestColorInvDestAlphaTest();
	~CDestColorInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/DestColor Class definitions
class CDestColorDestColorTest: public CAlphaBldTest
{
	public:
	CDestColorDestColorTest();
	~CDestColorDestColorTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/InvDestColor Class definitions
class CDestColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CDestColorInvDestColorTest();
	~CDestColorInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// DestColor/SrcAlphaSat Class definitions
class CDestColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CDestColorSrcAlphaSatTest();
	~CDestColorSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif