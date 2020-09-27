#include "d3dapi.hpp"

/*
In order to not have to maintain a huge 30-page list of all test functions,
BEGINTEST(blah) creates a function named blah, and a function pointer pblah,
that points to blah. 
Data segments (like #pragma data_seg(".d3dapi$test070")) are used to 
collect these function pointers in memory. 
#pragma comment(linker, "/merge:.d3dapi=.data") sorts all of the data_seg sections 
alphabetically. I got this idea from MitchD, who used a similar method to collect 
usb class drivers.

ExecuteTests starts with pInit, and calls every function pointer until a NULL 
pointer is reached, or it gets to Deinit.

Note that no static variables can be used in the test functions without using 
a different data segment for them, since they will be placed in memory in the same 
data_seg as the function pointers. Global variables declared outside of the data_segs 
are recommended instead...

/***************************************************************************************
/***************************************************************************************
/***************************************************************************************
/***************************************************************************************

In order to remove tests for a particular function, just comment the appropriate C_(x) 
from linkme.cpp. 
from linkme.cppremove the appropriate number
from linkme.cpp that matches x in LINKME(x) at the top of that test file. Note that
direct3d8init must be included for any testing, and direct3ddevice8init must be included 
for most everything. 

Linkme.cpp is used to ensure that all files are linked. A really cool, but really
annoying feature of the ms linker is the ability to remove unreferenced modules from the 
final link of .exe files (hence, also, .xbe's). Since there are no direct references to any
functions in the test modules for d3dapi, the linker removes them from any .exe file that 
uses d3dapi.lib. (note that this doesn't happen if d3dapi is built as a .dll).
The macro LINKME(x) creates a function that is called by LinkMe. (x) should be a unique
number or name, and needs to be matched with _C(x) and _D(x) macros in linkme.cpp.

RegisterDeinitFunc can be used to ensure that a deinitialization function is called
when the "end of the line" is reached (end of the line == a NULL function pointer, or pDeinit). 
RegisterDoAgain is used to run more tests when the end of the line is reached. This way,
if you want to try several dozen tests with 2 or 3 variations of the IDirect3dDevice, 
the function that creates the device can register itself (by passing NULL) by calling 
RegisterDoAgain. It can deinitialize the device, and recreate it, and the tests after that
will then be repeated with the new device setup.

DeinitFunc and DoAgain are implimented as a stack, and share the same stack. When the 
"end of the line" is reached, the next item on the stack is called and removed from the stack. 
When the stack is gone, d3dapi testing stops.


A list of the data_segs used:
.d3dapi$test0		Init


.d3dapi$test005 	pre-IDirect3D8 tests
.d3dapi$test010 	IDirect3D8 init
.d3dapi$test020 	IDirect3D8 tests

.d3dapi$test025 	IDirect3DDevice8 Init

.d3dapi$test030		D3DVertexBuffer tests

  51, 52, 53: lock tests
.d3dapi$test060 	IDirect3DDevice8 tests

.d3dapi$test061 	Cursor

.d3dapi$test070 	Resource

.d3dapi$test080 	Base Texture?

.d3dapi$test100 	Texture
.d3dapi$test110 	CubeTexture
.d3dapi$test120 	VolumeTexture

.d3dapi$test220		Volume8

.d3dapi$testzzz 	Deinit
*/

namespace d3dapi {
	HANDLE hlog;

/*
we were having problems with the multi-threaded version of rand(), so I
am just using the non-multithreaded code... D3d doesn't run multi-threaded
anyway, and this code doesn't share any variables with other rand() implimentations, 
so this shouldn't matter.
*/
	static ULONG holdrand = 1;
	void srnd(UINT seed)
	{
		holdrand = seed;
	}
	ULONG rnd() 
	{
		return(((holdrand = holdrand * 214013L + 2531011L) >> 16) & 0x7fff);
	}



	#pragma data_seg(".d3dapi$test0") //before A

	BEGINTEST(Init)
	{ 
	}
	ENDTEST()
	#pragma data_seg()
	
	#pragma data_seg(".d3dapi$testzzz") //last supported char
	BEGINTEST(Deinit)
	{ 
	}
	ENDTEST()
	
	TESTFUNC NullPointerAfterDeinit = 0; //in case I remove hard-coding of Deinit being the last function
	#pragma data_seg()
	
	
	
	#pragma comment(linker, "/merge:.d3dapi=.rdata")
	
	typedef struct _FUNCLIST {
		struct _FUNCLIST * next;
		BOOL doagain;
		union {
			TESTFUNC func;
			TESTFUNC*pfunc;
		};	
	} FUNCLIST;

	FUNCLIST* gpdeinit = NULL;
	TESTFUNC* gCurFunc = 0;
	
	void RegisterDeinitFunc(TESTFUNC proc)
	{
		FUNCLIST* p = gpdeinit;
		gpdeinit = new FUNCLIST;
		gpdeinit->func = proc;
		gpdeinit->next = p;
		gpdeinit->doagain = FALSE;
	}
	
	void RegisterDoAgain(TESTFUNC* newfunc)
	{
		FUNCLIST* p = gpdeinit;
		gpdeinit = new FUNCLIST;
		gpdeinit->pfunc = (newfunc ? newfunc : gCurFunc);
		gpdeinit->next = p;
		gpdeinit->doagain = TRUE;
	}
	
	
	HINSTANCE				 g_hInstance;
	LPDIRECT3D8 			 g_pd3d8 = NULL;
	LPDIRECT3DDEVICE8		 g_pd3dd8 = NULL;
	//LPDIRECT3DSWAPCHAIN8	   g_pd3dsw8 = NULL;
	LPDIRECT3DTEXTURE8		 g_pd3dt8 = NULL;
	LPDIRECT3DCUBETEXTURE8	 g_pd3dtc8 = NULL;
	LPDIRECT3DVOLUMETEXTURE8 g_pd3dtv8 = NULL;
	LPDIRECT3DVERTEXBUFFER8  g_pd3dr8 = NULL;
	LPDIRECT3DINDEXBUFFER8	 g_pd3di8 = NULL;
	LPDIRECT3DTEXTURE8		 g_pd3dtSurf8 = NULL;
	LPDIRECT3DSURFACE8		 g_pd3ds8 = NULL;
	LPDIRECT3DCUBETEXTURE8	 g_pd3dtcSurf8 = NULL;
	LPDIRECT3DSURFACE8		 g_pd3dsc8 = NULL;
	LPDIRECT3DVOLUMETEXTURE8 g_pd3dtvVol8 = NULL;
	LPDIRECT3DVOLUME8		 g_pd3dv8 = NULL;

	//******************************************************************************
	void ExecuteTests() 
	{
		DWORD status;

		CallAllDummyFunctions();

		gCurFunc = &d3dapi::pInit;			//start with Initialization
		RegisterDoAgain(0); 		//pInit is the first pointer in the "array" to test

		FUNCLIST * f;
		TESTFUNC * p;
		TESTFUNC t;

		while(gpdeinit) {
			if(gpdeinit->doagain == TRUE) {
				p = gpdeinit->pfunc;
				f = gpdeinit;
				gpdeinit = gpdeinit->next;
				delete f;
				for( ; p < &d3dapi::pDeinit && *p; p++) {
					gCurFunc = p;
					status = p[0]();
					if(status == XLL_BLOCK) {		//blocked = terminate this block of tests
						break;
					}
				}
			} else {
				t = gpdeinit->func;
				f = gpdeinit;
				gpdeinit = gpdeinit->next;
				delete f;
				status = t();
			}
		}
	}


	extern "C" void DebugPrint(char*,...);

	int int3(EXCEPTION_POINTERS* p)
	{
		DebugPrint("d3dapi: Unhandled exception 0x%x at %p\n  type \"!cxr 0x%p; !kb\" to get stack trace\n\n",
			p->ExceptionRecord->ExceptionCode,
			p->ExceptionRecord->ExceptionAddress,
			p->ContextRecord);

		_asm{int 3}; 
		return EXCEPTION_EXECUTE_HANDLER;
	}

	int D3DAPIExceptionHandler(EXCEPTION_POINTERS* p)
	{
		DebugPrint("d3dapi: exception 0x%x at %p.  type \"!cxr 0x%p; !kb\" to get stack trace\n\n",
			p->ExceptionRecord->ExceptionCode,
			p->ExceptionRecord->ExceptionAddress,
			p->ContextRecord);

		_asm{int 3}; 
		return EXCEPTION_EXECUTE_HANDLER;
	}


	/*++

	StartTest
  
	Routine Description:

		This is your test entry point. It returns only when all tests have
		completed. If you create threads, you must wait until all threads have
		completed. You also have to create your own heap using HeapAlloc, do not
		use GlobalAlloc or LocalAlloc.

		Please see "x-box test harness.doc" for more information

	Arguments:

		LogHandle - Handle used to call logging APIs. Please see "logging.doc"
			for more information regarding logging APIs

	Return Value:

		None

	--*/

	//#pragma comment(compiler, "-GX")

	VOID WINAPI D3DAPIStartTest(HANDLE Log)
	{
		hlog = Log;
		MM_STATISTICS		  mmStats = {sizeof(MM_STATISTICS), 0, 0, 0, 0, 0}, mmStats2 = {sizeof(MM_STATISTICS), 0, 0, 0, 0, 0};
		PS_STATISTICS		  psStats = {sizeof(PS_STATISTICS), 0, 0}, psStats2 = {sizeof(PS_STATISTICS), 0, 0};

		OutputDebugString( TEXT("d3dapi: StartTest is called\n") );

		xSetComponent( hlog, "Direct3D(S)", "Core (s-d3d)" );
		
		DWORD randomseed;
		randomseed = time(NULL);

		xLog(hlog, XLL_INFO, "random seed: %u", randomseed);
		srnd(randomseed);

		int i;
        for(i = 0; i < 2; i++) {
//		_try {
			MmQueryStatistics(&mmStats);
			PsQueryStatistics(&psStats);
			ExecuteTests();
			MmQueryStatistics(&mmStats2);
			PsQueryStatistics(&psStats2);
			xStartVariation(hlog, "D3dapi Overall Leak Test");
			CompareStats(&mmStats, &mmStats2, &psStats, &psStats2);
			xEndVariation(hlog);
		}

//		} _except(int3(GetExceptionInformation())) {
//			_asm int 3;
//		}

		return;

	}

	/*++

	EndTest

	Routine Description:

		This is your test clean up routine. It will be called only ONCE after
		all threads have done. This will allow you to do last chance clean up.
		Do not put any per-thread clean up code here. It will be called only
		once after you have finished your StartTest.

	Arguments:

		None

	Return Value:

		None

	--*/
	VOID WINAPI D3DAPIEndTest(VOID)
	{
		OutputDebugString( TEXT("d3dapi: EndTest is called\n") );
	}

	/*++

	Routine Description:

		main entry point
		
	Arguments:

		HINSTANCE	hInstance
		DWORD		fdwReason
		LPVOID		lpReserved

	Return Value:

		BOOL

	--*/

	/*
	BOOL
	WINAPI
	DllMain(
		HINSTANCE	hInstance,
		DWORD		fdwReason,
		LPVOID		lpReserved
		)
	{
		UNREFERENCED_PARAMETER( hInstance );
		UNREFERENCED_PARAMETER( lpReserved );

		if ( fdwReason == DLL_PROCESS_ATTACH ) {

		} else if ( fdwReason == DLL_PROCESS_DETACH ) {

		}

		return TRUE;
	}

    int __cdecl main( )
	{
		HANDLE h;
		h = xCreateLog(L"t:\\media\\d3dapi.log",
			NULL,
			-1,
			XLL_LOGALL,
			XLO_DEBUG|XLO_CONFIG|XLO_STATE|XLO_REFRESH
		);
		D3DAPIStartTest(h);
		D3DAPIEndTest();
		return 0;
	}
	*/

	#define CScheck(letter, sign, field)						\
	/*if(letter##1->field sign letter##2->field) {	*/			\
		xLog(hlog, (letter##1->field sign letter##2->field) ? XLL_FAIL : XLL_PASS, #field " before: %u, after: %u", \
			letter##1->field, letter##2->field);				\
		d = XLL_FAIL;											\
	/*}*/

	DWORD CompareStats(MM_STATISTICS* m1, MM_STATISTICS* m2, PS_STATISTICS* p1, PS_STATISTICS* p2)
	{
		DWORD d = XLL_PASS;

		xLog(hlog, XLL_INFO, "Available Pages: before, %u, after: %u", m1->AvailablePages, m2->AvailablePages);
		xLog(hlog, XLL_INFO, "CachePagesCommitted: before, %u, after: %u", m1->CachePagesCommitted, m2->CachePagesCommitted);

		CScheck(m, <, ImagePagesCommitted);
		CScheck(m, <, PoolPagesCommitted);
		CScheck(m, <, StackPagesCommitted);
		CScheck(m, <, VirtualMemoryBytesCommitted);
		CScheck(m, <, VirtualMemoryBytesReserved);
		CScheck(p, <, HandleCount);

		return d;
	/*
		if(m1->AvailablePages > m2->AvailablePages) {
			xLog(hlog, XLL_FAIL, "Available Pages before: %u, after: %u", m1->AvailablePages, m2->AvailablePages);
			d = XLL_FAIL;
		}
	*/
	}

} //namespace

extern "C" void D3DAPISTART(HANDLE h) {
	d3dapi::D3DAPIStartTest(h);
	d3dapi::D3DAPIEndTest();
}

#pragma data_seg( EXPORT_SECTION_NAME )
DECLARE_EXPORT_DIRECTORY( D3DAPI )
#pragma data_seg()

BEGIN_EXPORT_TABLE( D3DAPI )
	EXPORT_TABLE_ENTRY( "StartTest", d3dapi::D3DAPIStartTest )
	EXPORT_TABLE_ENTRY( "EndTest", d3dapi::D3DAPIEndTest )
END_EXPORT_TABLE( D3DAPI )

