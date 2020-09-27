//
//  CVertexIndexBufferTest.h
//
//  Class specification for CVertexIndexBufferTest.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//

#ifndef __CVertexIndexBufferTest_H__
#define __CVertexIndexBufferTest_H__


#define TESTDXVER 0x0800

//#define STRICT


//USETESTFRAME

//
// fun macros
//


#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))
#define DecodeHResult(hr) (TEXT(""))//(GetHResultString(hr).c_str())

//#define GROUP_CREATE(GroupName)  \
//GroupName Class##GroupName

#ifdef UNDER_XBOX
#define D3DUSAGE_DONOTCLIP 0
#define D3DUSAGE_NPATCHES 0
#define D3DUSAGE_SOFTWAREPROCESSING 0
#endif

//
// test info structures
//
typedef struct __D3DTEST_NORMALBUFFER
{
    LPTSTR szName;

    CShapes *shape;

    DWORD VBFVF;
    D3DFORMAT IBFormat;

    DWORD Usage;
    D3DPOOL Pool;

} D3DTEST_NORMALBUFFER, *LPD3DTEST_NORMALBUFFER;

typedef struct __D3DTEST_OPTIMIZEDBUFFER
{
    LPTSTR szName;

    CShapes *shape;
    UINT iReps;

    DWORD VBFVF;
    D3DFORMAT IBFormat;

    DWORD SrcUsage;
    D3DPOOL SrcPool;

    DWORD RefUsage;
    D3DPOOL RefPool;

} D3DTEST_OPTIMIZEDBUFFER, *LPD3DTEST_OPTIMIZEDBUFFER;

typedef struct __D3DTEST_LOCKACTIVE
{
    LPCTSTR szName;
    BOOL fCreate;
    BOOL fAppendVB;
    BOOL fAppendIB;
    BOOL fDraw1;
    BOOL fDraw2;

} D3DTEST_LOCKACTIVE, *LPD3DTEST_LOCKACTIVE;

typedef struct __D3DTEST_HUGEBUFFER
{
    LPTSTR szName;

    CShapes *shape;
    UINT iReps;

    DWORD VBFVF;
    D3DFORMAT IBFormat;

    DWORD Usage;
    D3DPOOL Pool;

} D3DTEST_HUGEBUFFER, *LPD3DTEST_HUGEBUFFER;


//
//  The CDevice8Test specification
//


class CVertexIndexBufferTest : public CD3DTest
{
public:
    CDirect3D8*       m_pD3D;
//	LPDIRECT3D8       m_pD3D;
//	LPDIRECT3DDEVICE8 m_pDevice;
//    LPDIRECT3DDEVICE8 m_pRefDevice;

    CShapes m_shape;

    BOOL                    m_bExit;

public:

    CVertexIndexBufferTest();

    virtual UINT TestInitialize();
    UINT GetAvailMemory( D3DPOOL Pool );

    virtual BOOL    AbortedExit(void);

protected:

    virtual void    ProcessInput();
    virtual BOOL    WndProc(LRESULT* plr, HWND hWnd, UINT uMsg, 
                        WPARAM wParam, LPARAM lParam);
};



//
// Declare all groups here
//


class NormalBuffer : public CVertexIndexBufferTest
{

public:

    NormalBuffer();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_NORMALBUFFER *lpCIB, BOOL fRef );
};

/*
class OptimizedBuffer : public CVertexIndexBufferTest
{
    LARGE_INTEGER m_liFreq;
    CShapes m_shape15bit;
    CShapes m_shape16bit;

public:

    OptimizedBuffer();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();
    BOOL ShapeReplicate(UINT iTimes);

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_NORMALBUFFER *lpCIB, UINT iReps, BOOL fMixedMode );
};
*/

class LockActive : public CVertexIndexBufferTest
{
    CVertexBuffer8* m_pSrcVB;
    CIndexBuffer8* m_pSrcIB;
    CVertexBuffer8* m_pRefVB;
    CIndexBuffer8* m_pRefIB;

    CShapes m_shape2;

public:

    LockActive();
    ~LockActive();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, CVertexBuffer8 **ppVB, CIndexBuffer8 **ppIB, DWORD Usage, D3DPOOL Pool, DWORD LockFlags, const D3DTEST_LOCKACTIVE *Test, BOOL fMixedMode );
};

/*
class Priority : public CVertexIndexBufferTest
{

public:

    Priority();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_NORMALBUFFER *lpCIB, DWORD dwThisPriority, BOOL fMixedMode );
};


class SwitchRenderState : public CVertexIndexBufferTest
{

public:

    SwitchRenderState();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_NORMALBUFFER *curtest, BOOL fReference );
};
*/

class MultipleLocks : public CVertexIndexBufferTest
{

public:

    MultipleLocks();


    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, DWORD Usage, D3DPOOL Pool, DWORD LockFlags, BOOL fMixedMode );
};

/*
class LostOnReset : public CVertexIndexBufferTest
{
    D3DPRESENT_PARAMETERS m_SrcParam;

public:

    LostOnReset();
    ~LostOnReset();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* *ppDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_NORMALBUFFER *lpCIB, BOOL fMixedMode );
};
*/

class HugeBuffer : public CVertexIndexBufferTest
{
    CShapes m_shape15bit;
    CShapes m_shape16bit;
    SIZE_T m_MemSize;

public:

    HugeBuffer();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, const D3DTEST_HUGEBUFFER *lpCIB, BOOL fMixedMode );
};

/*
class RelockDynamic : public CVertexIndexBufferTest
{
    CShapes m_shape2;

public:

    RelockDynamic();
    ~RelockDynamic();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, DWORD Usage, D3DPOOL Pool, DWORD LockFlags1, DWORD LockFlags2, BOOL fMixedMode );
};


class DiscardDynamic : public CVertexIndexBufferTest
{
    CShapes m_shape2;

public:

    DiscardDynamic();
    ~DiscardDynamic();

    bool ExecuteTest(UINT nTest);
    UINT TestInitialize();

    UINT TestInitReal( CDevice8* pDevice );
    BOOL ExecuteReal( CDevice8* pDevice, DWORD Usage, D3DPOOL Pool, DWORD LockFlags1, DWORD LockFlags2, BOOL fMixedMode );
};
*/

#endif

