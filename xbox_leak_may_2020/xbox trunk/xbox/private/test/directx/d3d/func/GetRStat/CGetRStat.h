//
//  CGetRStat.h
//

#ifndef __CGetRStat_H__
#define __CGetRStat_H__


#define TESTDXVER 0x0800

//USETESTFRAME

//
// handy stuff
//

#define COUNTOF(a)  (sizeof(a)/sizeof((a)[0]))
//#define DecodeHResult(hr) (GetHResultString(hr).c_str())



//extern CD3DWindowFramework *g_pD3DFramework;



//
//  The CDevice8Test specification
//


class CGetRStat : public CD3DTest
{
public:
//	LPDIRECT3DDEVICE8 m_pDevice;
//	LPDIRECT3D8       m_pD3D;

public:
    
	CGetRStat();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();
};


#endif

