/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	One.h
 *
 ***************************************************************************/

#ifndef __ONE_H__
#define __ONE_H__

// One/Zero Class definitions
class COneZeroTest: public CAlphaBldTest
{
	public:
	COneZeroTest();
	~COneZeroTest();

	bool SetDefaultRenderStates(void);
};

// One/One Class definitions
class COneOneTest: public CAlphaBldTest
{
	public:
	COneOneTest();
	~COneOneTest();

	bool SetDefaultRenderStates(void);
};

// One/SrcColor Class definitions
class COneSrcColorTest: public CAlphaBldTest
{
	public:
	COneSrcColorTest();
	~COneSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// One/InvSrcColor Class definitions
class COneInvSrcColorTest: public CAlphaBldTest
{
	public:
	COneInvSrcColorTest();
	~COneInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// One/SrcAlpha Class definitions
class COneSrcAlphaTest: public CAlphaBldTest
{
	public:
	COneSrcAlphaTest();
	~COneSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// One/InvSrcAlpha Class definitions
class COneInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	COneInvSrcAlphaTest();
	~COneInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// One/DestAlpha Class definitions
class COneDestAlphaTest: public CAlphaBldTest
{
	public:
	COneDestAlphaTest();
	~COneDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// One/InvDestAlpha Class definitions
class COneInvDestAlphaTest: public CAlphaBldTest
{
	public:
	COneInvDestAlphaTest();
	~COneInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// One/DestColor Class definitions
class COneDestColorTest: public CAlphaBldTest
{
	public:
	COneDestColorTest();
	~COneDestColorTest();

	bool SetDefaultRenderStates(void);
};

// One/InvDestColor Class definitions
class COneInvDestColorTest: public CAlphaBldTest
{
	public:
	COneInvDestColorTest();
	~COneInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// One/SrcAlphaSat Class definitions
class COneSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	COneSrcAlphaSatTest();
	~COneSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif