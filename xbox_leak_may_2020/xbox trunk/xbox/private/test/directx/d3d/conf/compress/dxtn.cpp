//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "3dmath.h"
#include "Compress.h"

//************************************************************************
// DXT1 Test functions

CDXT1Test::CDXT1Test()
{
	m_szTestName = TEXT("DXT1 Compressed Texture");
	m_szCommandKey = TEXT("DXT1");

	// Tell parent our FourCC code
//	dwFourCC = FOURCC_DXT1;
    fmtCompressed = D3DFMT_DXT1;
}

CDXT1Test::~CDXT1Test()
{
}
   
//************************************************************************
// DXT2 Test functions

CDXT2Test::CDXT2Test()
{
	m_szTestName = TEXT("DXT2 Compressed Texture");
	m_szCommandKey = TEXT("DXT2");

	// Tell parent our FourCC code
//	dwFourCC = FOURCC_DXT2;
    fmtCompressed = D3DFMT_DXT2;
}

CDXT2Test::~CDXT2Test()
{
}
   
//************************************************************************
// DXT3 Test functions

CDXT3Test::CDXT3Test()
{
	m_szTestName = TEXT("DXT3 Compressed Texture");
	m_szCommandKey = TEXT("DXT3");

	// Tell parent our FourCC code
//	dwFourCC = FOURCC_DXT3;
    fmtCompressed = D3DFMT_DXT3;
}

CDXT3Test::~CDXT3Test()
{
}
   
//************************************************************************
// DXT4 Test functions

CDXT4Test::CDXT4Test()
{
	m_szTestName = TEXT("DXT4 Compressed Texture");
	m_szCommandKey = TEXT("DXT4");

	// Tell parent our FourCC code
//	dwFourCC = FOURCC_DXT4;
    fmtCompressed = D3DFMT_DXT4;
}

CDXT4Test::~CDXT4Test()
{
}
   
//************************************************************************
// DXT5 Test functions

CDXT5Test::CDXT5Test()
{
	m_szTestName = TEXT("DXT5 Compressed Texture");
	m_szCommandKey = TEXT("DXT5");

	// Tell parent our FourCC code
//	dwFourCC = FOURCC_DXT5;
    fmtCompressed = D3DFMT_DXT5;
}

CDXT5Test::~CDXT5Test()
{
}

