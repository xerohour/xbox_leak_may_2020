/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	InvDestColor.h
 *
 ***************************************************************************/

#ifndef __INVDESTCOLOR_H__
#define __INVDESTCOLOR_H__

// InvDestColor/Zero Class definitions
class CInvDestColorZeroTest: public CAlphaBldTest
{
	public:
	CInvDestColorZeroTest();
	~CInvDestColorZeroTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/One Class definitions
class CInvDestColorOneTest: public CAlphaBldTest
{
	public:
	CInvDestColorOneTest();
	~CInvDestColorOneTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/SrcColor Class definitions
class CInvDestColorSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcColorTest();
	~CInvDestColorSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/InvSrcColor Class definitions
class CInvDestColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvSrcColorTest();
	~CInvDestColorInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/SrcAlpha Class definitions
class CInvDestColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcAlphaTest();
	~CInvDestColorSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/InvSrcAlpha Class definitions
class CInvDestColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvSrcAlphaTest();
	~CInvDestColorInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/DestAlpha Class definitions
class CInvDestColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorDestAlphaTest();
	~CInvDestColorDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/InvDestAlpha Class definitions
class CInvDestColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvDestAlphaTest();
	~CInvDestColorInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/DestColor Class definitions
class CInvDestColorDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorDestColorTest();
	~CInvDestColorDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/InvDestColor Class definitions
class CInvDestColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvDestColorInvDestColorTest();
	~CInvDestColorInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvDestColor/SrcAlphaSat Class definitions
class CInvDestColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvDestColorSrcAlphaSatTest();
	~CInvDestColorSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif