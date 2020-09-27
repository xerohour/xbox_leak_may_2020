#pragma once
#define QWORD _int64

// Disable the compiler warning for a non threatening condition.
// C4512: unable to generate an assignment operator for the given class. 

#pragma warning (disable:4512)
#include "..\parameter.h"

// Naming of class test and module classes.
// 
// module class: CXModule<module>
// test class:   CXModule<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//
#define LAN_MODNUM   16

#define NO_FLAGS_SET 0
#define PORT (u_short)  44966
#define DATASIZE 1024

class CXModuleLAN : public CTestObj
{
public: 
	// Get module number from numbers.h
	// If this is a new module, add the number to the file.
	//
	DECLARE_XMTAMODULE (CXModuleLAN, "LAN", LAN_MODNUM);

	virtual bool InitializeParameters ();
//	virtual ~CXModuleLAN();

protected:
	wchar_t * TestNamePtr;
	void MessageLog( char *);
	void SendPattern();
	BOOL RecvPattern( BOOL ErrorsOn, char WaitTime);
	void NoisePattern();
	void OFFONPattern();
	void RandomPattern();
	char SendBuffer[DATASIZE];
	char RecvBuffer[DATASIZE];
	SOCKET destSocket;
	WSADATA Data;
	SOCKADDR_IN destSockAddr;
	unsigned char Code;
};