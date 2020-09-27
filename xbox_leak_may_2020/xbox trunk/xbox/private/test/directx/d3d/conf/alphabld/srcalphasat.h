/*==========================================================================;
 *
 *  Copyright (C) 1994-1996 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:	SrcAlphaSat.h
 *
 ***************************************************************************/

#ifndef __SRCALPHASAT_H__
#define __SRCALPHASAT_H__

// SrcAlphaSat/Zero Class definitions
class CSrcAlphaSatZeroTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatZeroTest();
	~CSrcAlphaSatZeroTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/One Class definitions
class CSrcAlphaSatOneTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatOneTest();
	~CSrcAlphaSatOneTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/SrcColor Class definitions
class CSrcAlphaSatSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcColorTest();
	~CSrcAlphaSatSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/InvSrcColor Class definitions
class CSrcAlphaSatInvSrcColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvSrcColorTest();
	~CSrcAlphaSatInvSrcColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/SrcAlpha Class definitions
class CSrcAlphaSatSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcAlphaTest();
	~CSrcAlphaSatSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/InvSrcAlpha Class definitions
class CSrcAlphaSatInvSrcAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvSrcAlphaTest();
	~CSrcAlphaSatInvSrcAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/DestAlpha Class definitions
class CSrcAlphaSatDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatDestAlphaTest();
	~CSrcAlphaSatDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/InvDestAlpha Class definitions
class CSrcAlphaSatInvDestAlphaTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvDestAlphaTest();
	~CSrcAlphaSatInvDestAlphaTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/DestColor Class definitions
class CSrcAlphaSatDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatDestColorTest();
	~CSrcAlphaSatDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/InvDestColor Class definitions
class CSrcAlphaSatInvDestColorTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatInvDestColorTest();
	~CSrcAlphaSatInvDestColorTest();

	bool SetDefaultRenderStates(void);
};

// SrcAlphaSat/SrcAlphaSat Class definitions
class CSrcAlphaSatSrcAlphaSatTest: public CAlphaBldTest
{
	public:
	CSrcAlphaSatSrcAlphaSatTest();
	~CSrcAlphaSatSrcAlphaSatTest();

	bool SetDefaultRenderStates(void);
};

#endif