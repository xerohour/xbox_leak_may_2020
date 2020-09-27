/*++

  Copyright (c) 2000 Microsoft Corporation

  Module Name:

	mmTest.cpp

  Abstract:

	XBox memory management API tests.

  Author:

	Toby Dziubala (a-tobydz) 01-May-2000

  Enviornment:

	Windows 2000

  Revision History:

--*/
extern "C" {
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
}

#include <windows.h>
#include "dll.h"
#include "prototypes.h"
#include "xlog.h"




/*********************************************************************************************/
//Function:	StartTest()			Author: Toby Dziubala				Date: 4 / 25 / 00
//
//Purpose: Calls other test dll functions.
//		   
//
//Parameters:		
//				
/********************************************************************************************/


// XBox logging handle
HANDLE XMmApiLogHandle = INVALID_HANDLE_VALUE;


/*++

Routine Description:
              
    Calls subsequent test cases.
    
Arguments:

	- NONE -
    
Return:
    
    None

--*/
void XMmApiStartTest( HANDLE XMmApiLogHandle )
{	
	
	// Setup XBox logging
	ASSERT( XMmApiLogHandle != INVALID_HANDLE_VALUE );
		
	KdPrint(( "XMmAPI:StartTest\n" ));
	
	// Run the Test cases //
	TestCase1( XMmApiLogHandle );	
	TestCase2( XMmApiLogHandle );
	TestCase3( XMmApiLogHandle );
	TestCase4( XMmApiLogHandle );
	TestCase5( XMmApiLogHandle );
	TestCase6( XMmApiLogHandle );
	TestCase7( XMmApiLogHandle );
	TestCase8( XMmApiLogHandle );
	TestCase9( XMmApiLogHandle );
	TestCase10( XMmApiLogHandle );
	TestCase11( XMmApiLogHandle );
	TestCase12( XMmApiLogHandle );
	TestCase13( XMmApiLogHandle );
	TestCase14( XMmApiLogHandle );
	TestCase15( XMmApiLogHandle );
	TestCase16( XMmApiLogHandle );
	TestCase17( XMmApiLogHandle );

}



/*++

Routine Description:
              
    Dll exit point.
    
Arguments:

	- NONE -
    
Return:
    
    None

--*/
void XMmApiEndTest( void )
{
	KdPrint(( "XMmAPI:EndTest\n" ));
}




/*++

	=== TEST CASE 1 ===

Routine Description:
              
    Test to verify you can reserve virtual memory then free it.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

--*/
void TestCase1( HANDLE XMmApiLogHandle )
{
	LPVOID	lpStartAddress	=	NULL;		// Returned starting memory location
	BOOL	fFail;							// Failure flag
	char	cBuffer[128];					// Logging text buffer

	
	// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 1==\n" ));

	// Reset the fFail flag
	fFail = FALSE;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( 0L, 32768, MEM_RESERVE, PAGE_READWRITE )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 1== Test to verify you can reserve virtual memory then free it." );

	// Reserve 32 megs of virtual memory
	lpStartAddress = VirtualAlloc( 0L, 32768, MEM_RESERVE, PAGE_READWRITE );
	
	// Make sure we got a valid address
	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "VirtualAlloc Failed to Allocate 32 megs of memory."
			  );
		KdPrint(( "VirtualAlloc( ) Failed!\n" ));

		fFail = TRUE;
	}
		
	// Free the committed memory
	if ( FALSE == fFail )
	{
		fFail = VirtualFree( lpStartAddress, 0, MEM_RELEASE );

		if ( FALSE == fFail )	// VirtualFree() returns FALSE if an error occurs (inverse)
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "VirtualFree() Failed!"
				  );
			
			KdPrint(( "VirtualFree() Failed!\n" ));

			fFail = TRUE;		// Reset the fail flag to indicate an error
		}
		else
		{
			lpStartAddress = 0L;
			fFail = FALSE;		// Reset the fail flag
		}	
	}
	
	// LogEndCase
	if ( FALSE == fFail || ( FALSE == fFail && 0L == lpStartAddress) )
	{
		// TestCase passed
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "TestCase 1: PASS"
			  );
		
		KdPrint(( ">>>TestCase 1 PASSED<<<\n" ));
	}
	else
	{	
		// TestCase failed
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "TestCase 1: FAILED!"
			  );
		
		KdPrint(( ">>>TestCase 1 FAILED<<<\n" ));
	}
}



/*++

	=== TEST CASE 2 ===

Routine Description:
              
    Test to verify you can reserve, commit, and free virtual memory.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

--*/
void TestCase2( HANDLE XMmApiLogHandle )
{
	LPVOID	lpStartAddress	=	NULL;		// Returned starting memory location
	BOOL	fFail;							// Failure flag

	
	// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 2==\n" ));

	// Reset the fFail flag
	fFail = FALSE;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( 0L, 32768, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 2== Test to verify you can reserve, commit, and free virtual memory." );

	// Reserve 32 megs of virtual memory
	lpStartAddress = VirtualAlloc( 0L, 32768, MEM_RESERVE, PAGE_READWRITE );
	
	// Make sure we got a valid address
	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle, 
			  XLL_FAIL,
			  "Failed to allocate 32768 megz!" 
			);
		
		KdPrint(( "VirtualAlloc() Failed!\n" ));

		fFail = TRUE;
	}
	else
		fFail = FALSE;
	
	// Commit the memory
	if ( FALSE == fFail )
	{
		lpStartAddress = VirtualAlloc( 0L, 32768, MEM_COMMIT, PAGE_READWRITE );

		if ( 0L == lpStartAddress )
		{
			xLog( XMmApiLogHandle, 
				  XLL_FAIL,
				  "Failed to commit memory!" 
				);
			
			KdPrint(( "VirtualAlloc( MEM_COMMIT ) Failed!\n" ));
			
			fFail = TRUE;
		}
	}
	
		
	// Free the committed memory
	if ( FALSE == fFail )
	{
		fFail = VirtualFree( lpStartAddress, 32768, MEM_DECOMMIT );

		if ( FALSE == fFail )	// VirtualFree() returns FALSE if an error occurs (inverse)
		{
			xLog( XMmApiLogHandle, 
			  XLL_FAIL,
			  "VirtualFree() Failed!" 
			);

			KdPrint(( "VirtualFree() Failed!\n" ));

			fFail = TRUE;		// Reset the fail flag to indicate an error
		}
		else
		{
			lpStartAddress = 0L;
			fFail = FALSE;		// Reset the fail flag
		}
	}
	
	// LogEndCase
	if ( FALSE == fFail || ( FALSE == fFail && 0L == lpStartAddress) )
	{
		xLog( XMmApiLogHandle, 
			  XLL_PASS,
			  "TestCase 2: PASSED" 
			);

		KdPrint(( ">>>TestCase 2 PASSED<<<\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_INFO,
			  "TestCase 2: FAILED"
			);

		KdPrint(( ">>>TestCase 2 FAILED<<<\n" ));
	}
}




/*++

	=== TEST CASE 3 ===

Routine Description:
              
    Test to verify you can touch allocated virtual memory.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

--*/

void TestCase3( HANDLE XMmApiLogHandle )
{
	LPVOID	lpStartAddress = NULL;		// Returned starting memory location
	BOOL	fFail;						// Failure flag

	// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 3==\n" ));

	// Reset the fFail flag
	fFail = FALSE;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( 0L, 32768, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 3== Test to verify you can touch allocated virtual memory." );

	// Reserve 32 megs of virtual memory
	lpStartAddress = VirtualAlloc( 0L, 32768, MEM_RESERVE, PAGE_READWRITE );
	
	// Make sure we got a valid address
	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle,
		  XLL_FAIL,
		  "Failed to allocate 32768 megz!" );

		KdPrint(( "VirtualAlloc( ) Failed!\n" ));
		
		goto END;
	}
	
	
	// Commit the memory
	lpStartAddress = VirtualAlloc( 0L, 32768, MEM_COMMIT, PAGE_READWRITE );

	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle,
		  XLL_FAIL,
		  "Failed to commit memory!" );
		fFail = TRUE;
		
		KdPrint(( "VirtualAlloc( MEM_COMMIT ) Failed!\n" ));

	}
		
	
	// Touch the committed memory
	if ( FALSE == fFail )
	{
		ZeroMemory( lpStartAddress, 32768 );
	}
	
	
	
	// Free the committed memory
	if ( FALSE == fFail )
	{
		fFail = VirtualFree( lpStartAddress, 32768, MEM_DECOMMIT );

		if ( FALSE == fFail )	// VirtualFree() returns FALSE if an error occurs (inverse)
		{
			xLog( XMmApiLogHandle,
				  XLL_INFO,
				  "Couldn't free memory!" );
			fFail = TRUE;		// Reset the fail flag to indicate an error

			KdPrint(( "VirtualFree() Failed!\n" ));
		}
		else
		{
			lpStartAddress = 0L;
			fFail = FALSE;		// Reset the fail flag
		}
	}
	
	// LogEndCase
	if ( FALSE == fFail || ( FALSE == fFail && 0L == lpStartAddress) )
	{
		xLog( XMmApiLogHandle,
		  XLL_PASS,
		  "TestCase 3: PASS" );

		KdPrint(( ">>>TestCase 3 PASS<<<\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
		  XLL_FAIL,
		  "TestCase 3: FAIL" );

		KdPrint(( ">>>TestCase 3 FAIL<<<\n" ));
	}

END: ;

}




/*++

	=== TEST CASE 4 ===

Routine Description:
              
    Test to verify you can reserve, commit, and free virtual memory using various protection flags.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

--*/
void TestCase4( HANDLE XMmApiLogHandle )
{
	LPVOID	lpMemory[8];		// Returned starting memory location
	BOOL	fFail;				// Failure flag
	INT		i = 0;				// Counter for memory blocks
	
	// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 4==\n" ));

	// Reset the fFail flag
	fFail = FALSE;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( 0L, 32768, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 4== Test to verify you can reserve, commit, and free virtual memory using various protection flags." );

	for ( i = 0; i < 8; i++ )
	{
		switch ( i )
			{

			case 0:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_READWRITE" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_READWRITE );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_READWRITE Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_READWRITE );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_READWRITE!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 0
				
			case 1:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_READONLY" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_READONLY );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_READONLY Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_READONLY );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_READWRITE!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 1
			
			case 2:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_EXECUTE" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_EXECUTE );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_EXECUTE Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_EXECUTE );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_EXECUTE!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 2

			case 3:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_EXECUTE_READ" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_EXECUTE_READ );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_EXECUTE_READ Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_EXECUTE_READ );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_EXECUTE_READ!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 3

			case 4:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_EXECUTE_READWRITE" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_EXECUTE_READWRITE );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_EXECUTE_READWRITE Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_EXECUTE_READWRITE!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 4

			case 5:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_GUARD" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_GUARD );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_GUARD Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_GUARD );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_GUARD!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 5

			case 6:
				{
					xStartVariation( XMmApiLogHandle, "PAGE_NOACCESS" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_NOACCESS );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_NOACCESS Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_NOACCESS );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_NOACCESS!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 6

			case 7:
				{	
					xStartVariation( XMmApiLogHandle, "PAGE_NOCACHE" );

					// Reserve 32 megs of virtual memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_NOCACHE );
					
					// Make sure we got a valid address
					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "PAGE_NOCACHE Failed!" );
						xEndVariation( XMmApiLogHandle );
						break;
					}
					
					// Commit the memory
					lpMemory[i] = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_NOCACHE );

					if ( 0L == lpMemory[i] )
					{
						xLog( XMmApiLogHandle,
							  XLL_INFO,
							  "Couldn't commit PAGE_NOCACHE!" );
						fFail = TRUE;
						xEndVariation( XMmApiLogHandle );
						break;
					}
					else
					{
						xEndVariation( XMmApiLogHandle );
						break;
					}
				} // end case 7
			} // end switch
		} // end for

		
	
	// Free the committed memory
	if ( FALSE == fFail )
	{
	 	xStartVariation( XMmApiLogHandle, "Freeing memory" );

		
		// Free memory in array
		for ( i = 0; i < 8; i++ )
			fFail = VirtualFree( lpMemory[i], 0, MEM_DECOMMIT );

		if ( FALSE == fFail )	// VirtualFree() returns FALSE if an error occurs (inverse)
		{
			xLog( XMmApiLogHandle,
				  XLL_INFO,
				  "Couldn't free memory!" );

			KdPrint(( "VirtualFree() Failed!\n" ));

			fFail = TRUE;		// Reset the fail flag to indicate an error
		}
		else
			fFail = FALSE;		// Reset the fail flag
		
		xEndVariation( XMmApiLogHandle );
	}
	
	// LogEndCase
	if ( FALSE == fFail )
	{
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "TestCase 4: PASS" );

		KdPrint(( ">>>TestCase 4 PASS<<<\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "TestCase 4: FAIL" );

		KdPrint(( ">>>TestCase 4 FAILED<<<\n" ));
	}


}	// end TestCase 4




/*++

	=== TEST CASE 5 ===

Routine Description:
              
    Test to verify you can reserve / commit the minimum page size of memory (4096 bytes).
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

--*/

void TestCase5( HANDLE XMmApiLogHandle )
{
	LPVOID	lpStartAddress = NULL;		// Returned starting memory location
	BOOL	fFail;						// Failure flag

		// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 5==\n" ));

	// Reset the fFail flag
	fFail = FALSE;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( 0L, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 5== Test to verify you can reserve / commit the minimum page size of memory (4096 bytes)." );

	// Reserve 32 megs of virtual memory
	lpStartAddress = VirtualAlloc( 0L, 4096, MEM_RESERVE, PAGE_READWRITE );

	// Make sure we got a valid address
	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "VirtualAlloc() failed to allocate 4096 bytes" );
		
		KdPrint(( "VirtualAlloc() Failed!\n" ));

		fFail = TRUE;
	}

	// Commit the memory
	// Reserve 32 megs of virtual memory
	lpStartAddress = VirtualAlloc( 0L, 4096, MEM_COMMIT, PAGE_READWRITE );
	
	// Make sure we got a valid address
	if ( 0L == lpStartAddress )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "VirtualAlloc() failed to commit 4096 bytes" );

		KdPrint(( "VirtualAlloc( MEM_COMMIT ) Failed!\n" ));

		fFail = TRUE;
	}
	else
	{
		ZeroMemory( lpStartAddress, 4096 );
	}
	
	// Free the committed memory
	if ( FALSE == fFail )
	{
		xStartVariation( XMmApiLogHandle, "VirtualFree()" );
		
		fFail = VirtualFree( lpStartAddress, 0, MEM_DECOMMIT );

		if ( FALSE == fFail )	// VirtualFree() returns FALSE if an error occurs (inverse)
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "VirtualFree() failed!" );
			
			KdPrint(( "VirtualFree() Failed!\n" ));

			fFail = TRUE;		// Reset the fail flag to indicate an error
		}
		else
		{
			lpStartAddress = 0L;
			fFail = FALSE;		// Reset the fail flag
		}
	}
	
		xEndVariation( XMmApiLogHandle );

	// LogEndCase
	if ( FALSE == fFail || ( FALSE == fFail && 0L == lpStartAddress) )
	{
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "TestCase 5: PASS" );

		KdPrint(( ">>>TestCase 5 PASSED<<<\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "TestCase 5: FAIL" );

		KdPrint(( ">>>TestCase 3 FAILED<<<\n" ));

	}
} // end TestCase 5



/*++

	=== TEST CASE 6 ===

Routine Description:
              
    Test to verify you can reserve / commit all available memory.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	This function will eat up to 256 megs of RAM.

--*/
void TestCase6( HANDLE XMmApiLogHandle )
{
	LPVOID	lpmMemoryArray[16];				// Returned starting memory location
	LPVOID  lpmMemTemp = NULL;				// Temporary address location
	BOOL	fFail;							// Failure flag
	INT		i = 0;							// Counter / reference
	DWORD	size = 0;						// Size of memmory chunk to allocate
	DWORD	dwTotalSize = 0;				// Total amount of memory allocated

	// Log location with the Kernel Debugger
	KdPrint(( "XMmAPI: ==TestCase 6==\n" ));

	// Reset the fFail flag
	fFail = FALSE;

	// Set the size variable
	size = 16384;		// 1 meg RAM or 4 pages (4096 bytes per page)
	dwTotalSize = 0;
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 6== Test to verify you can reserve / commit all available memory." );
	
	
	// Keep allocating memory until the size has been reduced to zero.
	// For each VirtualAlloc() that fails, we reduce the size by 1/2.
	
	// TODO: Are pages the same size on the XBox
	while( i < 16 && size > 0 )
	{
		lpmMemTemp = VirtualAlloc( 0L, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE );
		
		// If the call to VirtualAlloc didn't fail...
		if( lpmMemTemp != 0L )
		{
			// Place the new address into our array
			lpmMemoryArray[i] = lpmMemTemp;

			// Try touching the allocated memory
			_try {

				ZeroMemory( lpmMemoryArray[i], size );

			} _except( EXCEPTION_EXECUTE_HANDLER ) {
				xLog( XMmApiLogHandle,
					  XLL_INFO,
					  "ZeroMemory raised an exception at address 0x%p",
					  lpmMemoryArray[i] );
				
				// An error occured
				fFail = TRUE;
				KdPrint(( "Exception touching all reserved Memory!\n" ));
				_asm int 3; // Pause for personal reflection...
			}

			i++;
			dwTotalSize += size;
		}
		else
		{
			// If VirtualAlloc failed, try cutting the allocation size in half (prob won't be necessary)
			size /= 2;
		}
	}
	
	// Log total allocation size
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "Allocated %d bytes of memory in %d allocations .",
		  dwTotalSize,
		  i
		  );

	// Log some status with the Kernel Debugger
	KdPrint(( "Memory Eaten.\n" ));
	
	_asm int 3;

	KdPrint(( "Freeing Memory.\n" ));

	_asm int 3;

	// Free all the memory blocks we have allocated
	for( i ; i > 0; i-- )
	{
		KdPrint(( "Entering loop...\n" ));
		
		_asm int 3;
		_try {
			
			// Free up our memory
			
			KdPrint(( "Freeing Memory..." ));

			VirtualFree( lpmMemoryArray[i], 0, MEM_DECOMMIT );
			
			} _except( EXCEPTION_EXECUTE_HANDLER ) {
				xLog( XMmApiLogHandle, 
					  XLL_INFO,
					  "VirtualFree raised and exception at address 0x%p",
					  lpmMemoryArray[i] );
				
				// An error occured
				fFail = TRUE;

				KdPrint(( "VirtualFree() threw an exception." ));
				
				_asm int 3; // More personal reflection (in hex of course)
			
			}

			// Set the array location to 0L
			lpmMemoryArray[i] = 0L;
		
	} // end for

	// Make sure all the memory was deallocated by checking the value of i
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "%d blocks not free'd.",
		  i
		  );

	// LogEndCase
	if ( FALSE == fFail )
	{
		KdPrint(( "Memory free'd\n" ));
		KdPrint(( ">>>TestCase 6 PASSED<<<\n" ));

		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "TestCase 6: PASS" );
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "TestCase 6: FAIL" );

		KdPrint(( ">>>TestCase 6 FAILED<<<\n" ));
	}
} // end TestCase 6



/*++

	=== TEST CASE 7 ===

Routine Description:
              
    Test to verify accessing read-only memory raises and exception.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase7( HANDLE XMmApiLogHandle )
{
	
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 7== Test to verify accessing read-only memory raises and exception." );


	// PAGE_READONLY: Attempt to ZeroMemory read-only memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_READONLY );

	// Verify the memory was allocated
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Couldn't allocate read-only memory"
			);
		
		KdPrint(( "VirtualAlloc() Failed!\n" ));

		KdPrint(( ">>>TestCase 7 FAILED<<<\n" ));
		
		goto OTHER_END;

	} 
	
		
		// Now try writing to the memory
		_try {
			
			ZeroMemory( lpmMemory, 16384 );
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

			xLog( XMmApiLogHandle,
				  XLL_PASS,
				  "Expected exception occured at address: 0x%p",
				  lpmMemory
				);
			KdPrint(( "Expected exception occured..." ));
			KdPrint(( ">>>TestCase 7 PASSED<<<\n" ));
			
			goto END;
		}
		
		// The system did not throw an exception
		fFail = TRUE;		
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "An exception did not occur.",
				  lpmMemory
				);
			
		KdPrint(( "An expected exception did not occur...\n" ));

		KdPrint(( ">>>TestCase 7 FAILED<<<\n" ));
		
		
		// END
END:
		// Free the memory
		fFail = VirtualFree( lpmMemory, 0, MEM_DECOMMIT );

		// Make sure it worked
		if ( FALSE == fFail )
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "Couldn't free memory!"
				);

OTHER_END: ;

} // end TestCase7()


/*++

	=== TEST CASE 8 ===

Routine Description:
              
    Test to verify accessing protected memory raises and exception.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase8( HANDLE XMmApiLogHandle )
{
	
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 8== Test to verify accessing protected (execute only) memory raises and exception." );


	// PAGE_READONLY: Attempt to ZeroMemory read-only memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE );

	// Verify the memory was allocated
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Couldn't allocate execute only memory"
			);
		
		KdPrint(( ">>>TestCase 8 FAILED<<<\n" ));

		goto OTHER_END;

	} 
	
		
		// Now try writing to the memory
		_try {
			
			ZeroMemory( lpmMemory, 16384 );
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

			xLog( XMmApiLogHandle,
				  XLL_PASS,
				  "Expected exception occured at address: 0x%p",
				  lpmMemory
				);
			KdPrint(( "Expected exception occured..." ));
			KdPrint(( ">>>TestCase 8 PASS<<<" ));
			
			goto END;
		}
		
		// The system did not throw an exception
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "An exception did not occur.",
				  lpmMemory
				);	
		KdPrint(( ">>>TestCase 8 FAIL<<<" ));
		
		
		// END
END:
		// Free the memory
		fFail = VirtualFree( lpmMemory, 0, MEM_DECOMMIT );

		// Make sure it worked
		if ( FALSE == fFail )
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "Couldn't free memory!"
				);

OTHER_END: ;

} // end TestCase8()




/*++

	=== Test Case 9 ===

Routine Description:
              
    Test to verify accessing EXECUTE_READ memory raises and exception.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase9( HANDLE XMmApiLogHandle )
{
	
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 9== Test to verify accessing protected (EXECUTE_READ) memory raises and exception." );


	// PAGE_READONLY: Attempt to ZeroMemory read-only memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READ );

	// Verify the memory was allocated
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Couldn't allocate execute only memory"
			);
		
		KdPrint(( ">>>TestCase 9 FAILED<<<\n" ));
			
		goto OTHER_END;

	} 
	
		// Now try writing to the memory
		_try {
			
			ZeroMemory( lpmMemory, 16384 );
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

			xLog( XMmApiLogHandle,
				  XLL_PASS,
				  "Expected exception occured at address: 0x%p",
				  lpmMemory
				);
			
			goto END;
		}
		
		// The system did not throw an exception
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "An exception did not occur.",
				  lpmMemory
				);	
		
		
		// END
END:
		// Free the memory
		fFail = VirtualFree( lpmMemory, 0, MEM_DECOMMIT );

		// Make sure it worked
		if ( FALSE == fFail )
			xLog( XMmApiLogHandle,
				  XLL_INFO,
				  "Couldn't free memory!"
				);

OTHER_END: ;

} // end TestCase9()



/*++

	=== Test Case 10 ===

Routine Description:
              
    Test to verify accessing PAGE_GUARD memory raises and exception.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase10( HANDLE XMmApiLogHandle )
{
	
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 10== Test to verify accessing protected (PAGE_GUARD) memory raises and exception." );


	// PAGE_READONLY: Attempt to ZeroMemory read-only memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_GUARD );

	// Verify the memory was allocated
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Couldn't allocate execute only memory"
			);

		KdPrint(( ">>>TestCase 10 FAILED<<<\n" ));

		goto OTHER_END;
	} 
	
		// Now try writing to the memory
		_try {
			
			ZeroMemory( lpmMemory, 16384 );
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

			xLog( XMmApiLogHandle,
				  XLL_PASS,
				  "Expected exception occured at address: 0x%p",
				  lpmMemory
				);
			
			goto END;
		}
		
		// The system did not throw an exception
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "An exception did not occur.",
				  lpmMemory
				);	
		
		
		// END
END:
		// Free the memory
		fFail = VirtualFree( lpmMemory, 0, MEM_DECOMMIT );

		// Make sure it worked
		if ( FALSE == fFail )
			xLog( XMmApiLogHandle,
				  XLL_INFO,
				  "Couldn't free memory!"
				);

OTHER_END: ;


} // end TestCase10()


/*++

	=== TEST CASE 11 ===

Routine Description:
              
    Test to verify accessing freed memory raises an exception.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase11( HANDLE XMmApiLogHandle )
{
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 11== Test to verify accessing freed memory raises an exception." );

	// Allocate memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );

	// Make sure we got our memory
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "Couldn't allocate memory!"
			);

		KdPrint(( ">>>TestCase 11 FAILED<<<\n" ));

		goto END;
	}

	// Free the memory
	fFail = VirtualFree( lpmMemory, 0, MEM_DECOMMIT );

	if ( FALSE == fFail )
	{
		xLog( XMmApiLogHandle,
			  XLL_INFO,
			  "Couldn't free memory!"
			);
	}

	// Touch the freed address
	// Now try writing to the memory
		_try {
			
			ZeroMemory( lpmMemory, 16384 );
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

			xLog( XMmApiLogHandle,
				  XLL_PASS,
				  "Expected exception occured at address: 0x%p",
				  lpmMemory
				);

			KdPrint(( "Expected exception occured\n" ));
			KdPrint(( ">>>TestCase 11 PASSED<<<\n" ));
			
			goto END;
		}
		
		// The system did not throw an exception
		xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "An exception did not occur.",
				  lpmMemory
			 );
		
		KdPrint(( "An expected exception did not occur\n" ));

		KdPrint(( ">>>TestCase 11 FAILED<<<\n" ));
		
		
		// END
END: ;

}// End TestCase11()



/*++

	=== TEST CASE 12 ===

Routine Description:
              
    Test to verify allocating memory in a protected region fails.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/

void TestCase12( HANDLE XMmApiLogHandle )
{
	
	LPVOID lpmMemory = NULL;	// Block of allocated memory
	BOOL   fFail;				// General-purpose flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 12== Test to verify allocating memory in a protected region fails." );

	// Allocate memory
	lpmMemory = VirtualAlloc( (LPVOID)0x1000, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );

	// Check for 0L
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "VirtualAlloc() failed to allocate memory in a protected region."
			);
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "VirtualAlloc() reserved memory in a protected region."
			);
	} 
}// End TestCase12



/*++

	=== TEST CASE 13 ===

Routine Description:
              
    Test to verify VirutalProtect will change memory protection.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase13( HANDLE XMmApiLogHandle )
{
	
	LPVOID						lpmMemory = NULL;		// Pointer to allocated memory
	MEMORY_BASIC_INFORMATION	lpMemInfo;				// Pointer to the memory info structure
	PDWORD						lpflOldProtect = 0L;	// Pointer to store old protection type
	BOOL						fFail;					// Fail flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "VirtualProtect( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 13== Test to verify VirutalProtect will change memory protection." );
	
	// Allocate 1 meg of memory
	lpmMemory = VirtualAlloc( 0L, 16384, MEM_RESERVE | MEM_COMMIT, PAGE_READONLY );

	
	// Make sure the memory was allocated
	if ( 0L == lpmMemory )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Failed to allocate memory!"
			);

		KdPrint(( "VirtualAlloc() Failed!\n" ));
	}
	
	// Now change the protection attributes
	fFail = VirtualProtect( lpmMemory, 16 * 1024, PAGE_EXECUTE_READWRITE, lpflOldProtect );

	
	// Check the return value
	if ( FALSE == fFail )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "VirtualProtect Failed!"
			);

		KdPrint(( "VirutalProtect() Failed!\n" ));
	}
	else
	{
		// Fill another memory info structure element for comparison
		VirtualQuery( lpmMemory, &lpMemInfo, sizeof( lpMemInfo ) );

		// Check the protection attributes
		if ( lpMemInfo.Protect == PAGE_EXECUTE_READWRITE )
		{
			xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "AllocationProtect: %d Expected: %d\n",
			  lpMemInfo.Protect,
			  PAGE_EXECUTE_READWRITE
			);
			
			KdPrint(( ">>>TestCase 13 PASSED<<<\n" ));

		}
		else
		{
			xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "AllocationProtect: %d Expected: %d\n",
			  lpMemInfo.Protect,
			  PAGE_EXECUTE_READWRITE
			);

			KdPrint(( ">>>TestCase 13 FAILED<<<\n" ));

		}
	}


} // end TestCase13()



/*++

	=== TEST CASE 14 ===

Routine Description:
              
    Test to verify GetProcessHeap() returns a vaild handle to the default heap.
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase14 ( HANDLE XMmApiLogHandle )
{
	HANDLE	hHeapHandle = NULL;			// Heap handle
		
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "GetProcessHeap( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 14= Test to verify GetProcessHeap() returns a vaild handle to the default heap." );

	// Attempt to get a handle to the default heap
	_try {
		
		hHeapHandle = GetProcessHeap();
	} _except( EXCEPTION_EXECUTE_HANDLER ) {

		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GetProcessHeap() Failed!"
			);

		KdPrint(( "GetProcessHeap() Failed!\n" ));
	}

	// Verify the handle is not 0L
	if ( 0L == hHeapHandle )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GetProcessHeap() returned a 0L handle!"
			);
		
		KdPrint(( "GetProcessHeap returned NULL...\n" ));

		KdPrint(( ">>>TestCase 14 FAILED<<<\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "GetProcessHeap() returned a valid handle."
			);

		KdPrint(( ">>>TestCase 14 PASSED<<<\n" ));

	}
}// end TestCase14




/*++

	=== TEST CASE 15 ===

Routine Description:
              
    Test functionality of Global memory APIs
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase15( HANDLE XMmApiLogHandle )
{
	
	HGLOBAL	hGlobal			= NULL;		// Handle to a global memory object
	HGLOBAL hGlobalBak		= NULL;		// Backup handle used to free memory
	HGLOBAL hGlobalObj		= NULL;		// Handle returned from 'GlobalHandle()'
	DWORD	dwSize			= 0;		// Size of global memory object
	DWORD	dwError;					// Used for calls to GetLastError()
	LPVOID	lpGlobalPtr		= NULL;		// Pointer to global memory (first byte in block)
	BOOL	fFail;						// Generic fail flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "GlobalAlloc( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 15= Test functionality of Global memory APIs" );

	// Variation: GlobalAlloc()
	xStartVariation( XMmApiLogHandle, "GlobalAlloc()" );

	// Allocate a 32 meg moveable block of memory
	hGlobal = GlobalAlloc( GMEM_MOVEABLE, 32768 );

	// Check for a 0L return
	if ( 0L == hGlobal )
	{
		// xLog failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalAlloc() Failed to allocate memory!"
			);

		KdPrint(( "GlobalAlloc() Failed!\n" ));

		xEndVariation( XMmApiLogHandle );

		goto END;
	}
	else
	{
		// End Variation: GlobalAlloc()
		xEndVariation( XMmApiLogHandle );

		// Start Variation: GlobalSize()
		xStartVariation( XMmApiLogHandle, "GlobalSize()" );
		
		// Make sure it allocated the right size
		dwSize = GlobalSize( hGlobal );

		if ( dwSize != 32768 )
		{
			// xLog failure
			xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalAlloc() allocated only %d bytes!",
			  dwSize
			);

			KdPrint(( "GlobalAlloc() allocated insufficient memory!\n" ));

		}

		// End Variation: GlobalSize()
		xEndVariation( XMmApiLogHandle );

		// Save the handle
		hGlobalBak = hGlobal;
	}
	
	// Start Variation: GlobalLock()
	xStartVariation( XMmApiLogHandle, "GlobalLock()" );

	// Lock the memory object and get a pointer to the first byte of the memory block
	lpGlobalPtr = GlobalLock( hGlobal );

	// Check for 0L
	if ( 0L == lpGlobalPtr )
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalLock() Failed!", dwSize
			);

		KdPrint(( "GlobalLock() Failed!\n" ));
	}
	
	// End Variation: GlobalLock()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: GlobalHandle()
	xStartVariation( XMmApiLogHandle, "GlobalHandle()" );

	hGlobalObj = GlobalHandle( lpGlobalPtr );

	// Check for NULL
	if ( 0L == hGlobalObj )
	{
		// xLog failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalHandle() returned 0L!"
			);

		KdPrint(( "GlobalHandle() returned NULL\n" ));
	}
	
	// End Variation: GlobalHandle()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: GlobalUnlock()
	xStartVariation( XMmApiLogHandle, "GlobalUnlock()" );

	// Unlock the memory
	fFail = GlobalUnlock( hGlobal );

	// Check the return value ( 0 is failure, but not always, so call GetLastError() to be sure )
	if ( FALSE == fFail )
	{
		dwError = GetLastError();

		if ( NO_ERROR != dwError )
		{
			xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalUnlock() Failed!"
			);

			KdPrint(( "GlobalUnlock() Failed!\n" ));

		}
	}

	// End Variation: GlobalUnlock()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: GlobalReAlloc()
	xStartVariation( XMmApiLogHandle, "GlobalReAlloc()" );

	// Reallocate the memory to 64 megs
	hGlobal = GlobalReAlloc( hGlobal, 65536, GMEM_MOVEABLE );

	// Check for 0L
	if ( 0L == hGlobal )
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalReAlloc() Failed!"
			);
		
		KdPrint(( "GlobalReAlloc() Failed!\n" ));

		// Reset the handle
		hGlobal = hGlobalBak;
	}
	else
	{
		// Make sure it allocated the right size
		dwSize = GlobalSize( hGlobal );
		
		if ( dwSize != 65536 )
		{
			xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalReAlloc() failed to allocate specified amount of memory!"
			);

			KdPrint(( "GlobalReAlloc() failed to allocate specified amount of memory! \n" ));
		}
	}
	
	// End Variation: GlobalReAlloc()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: GlobalFree()
	xStartVariation( XMmApiLogHandle, "GlobalFree()" );

	// Free the memory
	hGlobal = GlobalFree( hGlobal );

	if ( hGlobal == 0L )
	{
		// xLog Pass
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "===Test Case 15=== PASSED"
			);

		KdPrint(( ">>>TestCase 15 PASSED<<<\n" ));
	}
	else
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "GlobalFree() Failed!"
			);

		KdPrint(( ">>>TestCase 15 FAILED<<<\n" ));
	}

	// End Variation: GlobalFree()
	xEndVariation( XMmApiLogHandle );

END: ;

} //end TestCase15()





/*++

	=== TEST CASE 16 ===

Routine Description:
              
    Test functionality of Local memory APIs
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase16( HANDLE XMmApiLogHandle )
{
	
	HLOCAL	hLocal		= NULL;		// Handle to a global memory object
	HLOCAL	hLocalBak	= NULL;		// Backup handle
	HLOCAL  hLocalObj	= NULL;		// Handle returned from 'LocalHandle()'
	DWORD	dwSize;					// Size of global memory object
	DWORD	dwError;				// Used for calls to GetLastError()
	LPVOID	lpLocalPtr;				// Pointer to global memory (first byte in block)
	BOOL	fFail;					// Generic fail flag
	
	
	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "GetProcessHeap( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 16= Test functionality of Local memory APIs" );

	// Variation: LocalAlloc()
	xStartVariation( XMmApiLogHandle, "LocalAlloc()" );

	// Allocate a 32 meg moveable block of memory
	hLocal = LocalAlloc( LMEM_MOVEABLE, 32768 );

	// Check for a 0L return
	if ( 0L == hLocal )
	{
		// xLog failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalAlloc() Failed to allocate memory!"
			);

		KdPrint(( "LocalAlloc() Failed to allocate memory!\n" ));

	}
	else
	{
		// Backup the handle
		hLocalBak = hLocal;

		// End Variation: LocalAlloc()
		xEndVariation( XMmApiLogHandle );

		// Start Variation: LocalSize()
		xStartVariation( XMmApiLogHandle, "LocalSize()" );
		
		// Make sure it allocated the right size
		dwSize = LocalSize( hLocal );

		if ( dwSize != 32768 )
		{
			// xLog failure
			xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalAlloc() allocated only %d bytes!",
			  dwSize
			);

			KdPrint(( "LocalAlloc() allocated insufficient memory!\n" ));
		}

		// End Variation: LocalSize()
		xEndVariation( XMmApiLogHandle );
	}
	
	// Start Variation: LocalLock()
	xStartVariation( XMmApiLogHandle, "LocalLock()" );

	// Lock the memory object and get a pointer to the first byte of the memory block
	lpLocalPtr = LocalLock( hLocal );

	// Check for 0L
	if ( 0L == lpLocalPtr )
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalLock() Failed!", dwSize
			);

		KdPrint(( "LocalLock() Failed!\n" ));
	}
	
	// End Variation: LocalLock()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: LocalHandle()
	xStartVariation( XMmApiLogHandle, "LocalHandle()" );

	hLocalObj = LocalHandle( lpLocalPtr );

	// Check for 0L
	if ( 0L == hLocalObj )
	{
		// xLog failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalHandle() returned NULL!"
			);
		
		KdPrint(( "LocalHandle() returned NULL\n" ));

	}
	
	// End Variation: LocalHandle()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: LocalUnlock()
	xStartVariation( XMmApiLogHandle, "LocalUnlock()" );

	// Unlock the memory
	fFail = LocalUnlock( hLocal );

	// Check the return value ( 0 is failure, but not always, so call GetLastError() to be sure )
	if ( FALSE == fFail )
	{
		dwError = GetLastError();

		if ( NO_ERROR != dwError )
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "LocalUnlock() Failed!"
				);

			KdPrint(( "LocalUnlock Failed!\n" ));
		}
	}

	// End Variation: LocalUnlock()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: LocalReAlloc()
	xStartVariation( XMmApiLogHandle, "LocalReAlloc()" );

	// Reallocate the memory to 64 megs
	hLocal = LocalReAlloc( hLocal, 65536, LMEM_MOVEABLE );

	// Check for 0L
	if ( 0L == hLocal )
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalReAlloc() Failed!"
			);
		
		KdPrint(( "LocalReAlloc() Failed!\n" ));

		// Reset the handle
		hLocal = hLocalBak;
	}
	else
	{
		// Make sure it allocated the right size
		dwSize = LocalSize( hLocal );
		
		if ( dwSize != 65536 )
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "LocalReAlloc() failed to allocate specified amount of memory!"
				);
			
			KdPrint(( "LocalReAlloc() failed to allocate the specified amount of memory!\n" ));

		}
	}
	
	// End Variation: LocalReAlloc()
	xEndVariation( XMmApiLogHandle );

	// Start Variation: LocalFree()
	xStartVariation( XMmApiLogHandle, "LocalFree()" );

	// Free the memory
	hLocal = LocalFree( hLocal );

	if ( hLocal == 0L )
	{
		// xLog Pass
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "===Test Case 16=== PASSED"
			);

		KdPrint(( ">>>TestCase 16 PASSED<<<\n" ));
	}
	else
	{
		// xLog Failure
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "LocalFree() Failed!"
			);

		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "===Test Case 16=== FAILED"
			);

		KdPrint(( ">>>TestCase 16 FAILED<<<\n" ));
	}

	// End Variation: LocalFree()
	xEndVariation( XMmApiLogHandle );

} //end TestCase16()


/*++

	=== TEST CASE 17 ===

Routine Description:
              
    Test functionality of Heap memory APIs
    
Arguments:

	- HANDLE	XMmApiLogHandle		// Handle to the XBox Logging Object
    
Return:
    
    None

Comments:
	
	
--*/
void TestCase17( HANDLE XMmApiLogHandle )
{
	HANDLE	hHeap		= NULL;		// Handle to the heap
	HANDLE	hHeapBak	= NULL;		// Backup heap handle
	LPVOID	lpHeapMem	= NULL;		// Pointer to the heap
	DWORD	dwSize;					// Size of the heap
	BOOL	fFail;					// Generic fail flag


	// LogBeginCase
	xSetComponent( XMmApiLogHandle, "Kernel", "Mm" );
	xSetFunctionName( XMmApiLogHandle, "Heap*( )" );
	
	xLog( XMmApiLogHandle,
		  XLL_INFO,
		  "==TestCase 17= Test functionality of Heap memory APIs" );

	xStartVariation( XMmApiLogHandle, "HeapCreate()" );

	_try {
		
		hHeap = HeapCreate( HEAP_GENERATE_EXCEPTIONS | HEAP_NO_SERIALIZE, 32768, 131072 ); 
		
		} _except( EXCEPTION_EXECUTE_HANDLER ) {

		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "HeapCreate() threw an exception!"
			);
		
		KdPrint(( "HeapCreate() threw an exception\n" ));

		KdPrint(( ">>>TestCase 17 FAILED<<<\n" ));

		goto END;
	}

	// Backup the handle
	hHeapBak = hHeap;

	xEndVariation( XMmApiLogHandle );

	xStartVariation( XMmApiLogHandle, "HeapAlloc()" );
	
	// Allocate 32 megs of the heaps memory and zero it
	_try {

		lpHeapMem = HeapAlloc( hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, 100000 );
	} _except( EXCEPTION_EXECUTE_HANDLER ) {

		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "HeapAlloc() threw an exception!"
			);

		KdPrint(( "HeapAlloc() threw an exception!\n" ));

		goto END;
	}

	xEndVariation( XMmApiLogHandle );

	xStartVariation( XMmApiLogHandle, "HeapSize()" );

	// Get the size of the allocated block for verification
	dwSize = HeapSize( hHeap, 0L, lpHeapMem );

	if ( dwSize != 100000 )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Heap is only %d bytes!",
			  dwSize
			);
		
		KdPrint(( "HeapAlloc() allocated insufficient memory!\n" ));
	}

	xEndVariation( XMmApiLogHandle );

	xStartVariation( XMmApiLogHandle, "HeapReAlloc()" );

	// Resize the allocation block to incorporate the entire heap
	_try {
		
		lpHeapMem = HeapReAlloc( hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_REALLOC_IN_PLACE_ONLY | HEAP_ZERO_MEMORY, lpHeapMem, 65536 );
	} _except( EXCEPTION_EXECUTE_HANDLER ) {

		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "HeapReAlloc() threw an exception!",
			  dwSize
			);

		KdPrint(( "HeapReAlloc() failed!\n" ));

		// Free the allocated heap memory
		fFail = HeapFree( hHeap, 0L, lpHeapMem );

		// Check for failure ( 0 indicates a failure [inverse] )
		if ( FALSE == fFail )
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "HeapFree() failed!"
				);
		}

		// Destroy the heap
		fFail = HeapDestroy( hHeap );

		// Check for 0
		if ( fFail == FALSE )
		{
			xLog( XMmApiLogHandle,
				  XLL_FAIL,
				  "HeapDestroy() Failed!"
				);
		}
			goto END;
		}
	

	xStartVariation( XMmApiLogHandle, "HeapSize()" );

	// Get the size of the allocated block for verification
	dwSize = HeapSize( hHeap, 0L, lpHeapMem );

	if ( dwSize != 65536 )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "Heap is only %d bytes!",
			  dwSize
			);

	KdPrint(( "HeapReAlloc() allocated insufficient memory!\n" ));

	}

	xEndVariation( XMmApiLogHandle );

	xStartVariation( XMmApiLogHandle, "HeapFree()" );

	// Free the allocated heap memory
	fFail = HeapFree( hHeap, 0L, lpHeapMem );

	// Check for failure ( 0 indicates a failure [inverse] )
	if ( FALSE == fFail )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "HeapFree() failed!"
			);

		KdPrint(( "HeapFree() failed!\n" ));
	}

	xEndVariation( XMmApiLogHandle );

	xStartVariation( XMmApiLogHandle, "HeapDestroy()" );

	// Destroy the heap
	fFail = HeapDestroy( hHeap );

	// Check for 0
	if ( fFail == FALSE )
	{
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "HeapDestroy() Failed!"
			);
		
		xLog( XMmApiLogHandle,
			  XLL_FAIL,
			  "===Test Case 17=== FAILED"
			);

		KdPrint(( "HeapDestroy() Failed!\n" ));

		KdPrint(( ">>>TestCase17 FAILED\n" ));
	}
	else
	{
		xLog( XMmApiLogHandle,
			  XLL_PASS,
			  "===TestCase 17=== PASSED"
			);

		KdPrint(( ">>>TestCase 17<<< PASSED" ));
	}


END: ;



} //end TestCase17()