/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	InvSrcColor.h
 *
 ***************************************************************************/

#ifndef __INVSRCCOLOR_H__
#define __INVSRCCOLOR_H__

// InvSrcColor/Zero Class definitions
class CInvSrcColorZeroTest: public CAlphaBldTest
{
	public:
	CInvSrcColorZeroTest();
	~CInvSrcColorZeroTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/One Class definitions
class CInvSrcColorOneTest: public CAlphaBldTest
{
	public:
	CInvSrcColorOneTest();
	~CInvSrcColorOneTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/SrcColor Class definitions
class CInvSrcColorSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcColorTest();
	~CInvSrcColorSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/InvSrcColor Class definitions
class CInvSrcColorInvSrcColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvSrcColorTest();
	~CInvSrcColorInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/SrcAlpha Class definitions
class CInvSrcColorSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcAlphaTest();
	~CInvSrcColorSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/InvSrcAlpha Class definitions
class CInvSrcColorInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvSrcAlphaTest();
	~CInvSrcColorInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/DestAlpha Class definitions
class CInvSrcColorDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorDestAlphaTest();
	~CInvSrcColorDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/InvDestAlpha Class definitions
class CInvSrcColorInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvDestAlphaTest();
	~CInvSrcColorInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/DestColor Class definitions
class CInvSrcColorDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorDestColorTest();
	~CInvSrcColorDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/InvDestColor Class definitions
class CInvSrcColorInvDestColorTest: public CAlphaBldTest
{
	public:
	CInvSrcColorInvDestColorTest();
	~CInvSrcColorInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// InvSrcColor/SrcAlphaSat Class definitions
class CInvSrcColorSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CInvSrcColorSrcAlphaSatTest();
	~CInvSrcColorSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif