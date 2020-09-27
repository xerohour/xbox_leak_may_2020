/*++

  Copyright (c) 2000 Microsoft Corporation

  Module Name:

	dll.h

  Abstract:

	TBD

  Author:

	Toby Dziubala (a-tobydz) 25-April-2000

  Revision History:

--*/



/// Function Prototypes ///


/// TestCase Prototypes ///
void TestCase1( HANDLE LogHandle );			// Test to verify you can reserve virtual memory then free it.
void TestCase2( HANDLE LogHandle );			// Test to verify you can reserve, commit, and free virtual memory.
void TestCase3( HANDLE LogHandle );			// Test to verify you can touch allocated virtual memory.
void TestCase4( HANDLE LogHandle );			// Test to verify you can reserve, commit, and free virtual memory 
											// using various protection flags.				
void TestCase5( HANDLE LogHandle );			// Test to verify you can reserve / commit the minimum page size 
											// of memory (4096 bytes).
void TestCase6( HANDLE LogHandle );			// Test to verify you can reserve / commit all available memory.
void TestCase7( HANDLE LogHandle );			// Test to verify accessing read-only memory raises and exception.
void TestCase8( HANDLE LogHandle );			// Test to verify accessing protected memory raises and exception.
void TestCase9( HANDLE LogHandle );			// Test to verify accessing EXECUTE_READ memory raises and exception.
void TestCase10( HANDLE LogHandle );		// Test to verify accessing PAGE_GUARD memory raises and exception.
void TestCase11( HANDLE LogHandle );		// Test to verify accessing freed memory raises an exception.
void TestCase12( HANDLE LogHandle );		// Test to verify allocating memory in a protected region fails.
void TestCase13( HANDLE LogHandle );		// Test to verify VirutalProtect will change memory protection.
void TestCase14( HANDLE LogHandle );		// Test to verify GetProcessHeap() returns a vaild handle to the default heap.
void TestCase15( HANDLE LogHandle );		// Test functionality of Global memory APIs.
void TestCase16( HANDLE LogHandle );		// Test functionality of Local memory APIs.
void TestCase17( HANDLE LogHandle );		// Test functionality of Heap memory APIs.