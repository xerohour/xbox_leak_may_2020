/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    myassert.h

Abstract:

    a shared header for slixbox and slix2k, defines simple macros for 
	doing error codes on xbox or win2k


Author:

    Jason Gould (a-jasgou) June 2000

--*/

#undef MyAssertPreCode1
#undef MyAssertPreCode2
#undef MyAssertPreCode3
#undef MyAssertPostCode1	
#undef MyAssertPostCode2	
#undef MyAssertPostCode3	
#undef MyAssertPreCodeDebug1
#undef MyAssertPreCodeDebug2
#undef MyAssertPreCodeDebug3
#undef MyAssertPostCodeDebug1
#undef MyAssertPostCodeDebug2
#undef MyAssertPostCodeDebug3
#undef MyAssertFunctionTitle
#undef MyAssertStr1Level1
#undef MyAssertStr1Level2
#undef MyAssertStr1Level3
#undef MyAssertStr2Level1
#undef MyAssertStr2Level2
#undef MyAssertStr2Level3
#undef MyAssertOut1
#undef MyAssertOut2
#undef MyAssertOut3

/******************************************************************************
/*	#undef then #define these in your program appropriately...
/*****************************************************************************/

#define MyAssertPreCode	
//#define MyAssertPreCode0	
#define MyAssertPreCode1	MyAssertPreCode //warning pre-message code
#define MyAssertPreCode2	MyAssertPreCode //problem pre-message code
#define MyAssertPreCode3	MyAssertPreCode //error pre-message code

#define MyAssertPostCode
//#define MyAssertPostCode0
#define MyAssertPostCode1	MyAssertPostCode //warning after-message code
#define MyAssertPostCode2	MyAssertPostCode //problem after-message code
#define MyAssertPostCode3	MyAssertPostCode //error after-message code

//#define MyAssertPreCodeDebug0	//message pre-message debug code 
#define MyAssertPreCodeDebug1	//warning pre-message debug code 
#define MyAssertPreCodeDebug2	//problem pre-message debug code
#define MyAssertPreCodeDebug3	//error pre-message debug code
	
//#define MyAssertPostCodeDebug0					//message post-message debug code
#define MyAssertPostCodeDebug1					//warning post-message debug code
#define MyAssertPostCodeDebug2	//__asm {int 3};	//problem post-message debug code
#define MyAssertPostCodeDebug3	//__asm {int 3};	//error post-message debug code

#define MyAssertFunctionTitle	"Global"	//function name... "global" until declared otherwise

//#define MyAssertStr1Level0(_Code, _Title) MyAssertFunctionTitle "" _Title " -- File: " __FILE__ ", Line: " MyAssertLine
#define MyAssertStr1Level1(_Code, _Title) MyAssertFunctionTitle " Warning: " _Title " -- File: " __FILE__ ", Line: " MyAssertLine
#define MyAssertStr1Level2(_Code, _Title) MyAssertFunctionTitle " Problem: " _Title " -- File: " __FILE__ ", Line: " MyAssertLine
#define MyAssertStr1Level3(_Code, _Title) MyAssertFunctionTitle " Error: " _Title " : " #_Code " -- File: " __FILE__ ", Line: " MyAssertLine

//#define MyAssertStr2Level0(_Code, _Title) MyAssertFunctionTitle ": Notice"
#define MyAssertStr2Level1(_Code, _Title) MyAssertFunctionTitle ": Warning"
#define MyAssertStr2Level2(_Code, _Title) MyAssertFunctionTitle " Problem: " _Title
#define MyAssertStr2Level3(_Code, _Title) MyAssertFunctionTitle " Error: " _Title

#ifndef _XBOX_

	#ifdef _DEBUG
		#define MyAssertOut1(_Code, _Title) MessageBox(NULL, MyAssertStr1Level1(_Code, _Title), MyAssertStr2Level1(_Code, _Title), MB_OK);
	#else
		#define MyAssertOut1(_Code, _Title)
	#endif

	#define MyAssertOut2(_Code, _Title) MessageBox(NULL, MyAssertStr1Level2(_Code, _Title), MyAssertStr2Level2(_Code, _Title), MB_OK);
	#define MyAssertOut3(_Code, _Title) MessageBox(NULL, MyAssertStr1Level3(_Code, _Title), MyAssertStr2Level3(_Code, _Title), MB_OK);

#else

//	#define MyAssertOut0(_Code, _Title) DebugPrint(MyAssertStr1Level0(_Code, _Title) "\n");
	#define MyAssertOut1(_Code, _Title) DebugPrint(MyAssertStr1Level1(_Code, _Title) "\n");
	#define MyAssertOut2(_Code, _Title) DebugPrint(MyAssertStr1Level2(_Code, _Title) "\n");
	#define MyAssertOut3(_Code, _Title) DebugPrint(MyAssertStr1Level3(_Code, _Title) "\n");

#endif


/******************************************************************************
/*	Don't change this...
/*****************************************************************************/

#ifdef _DEBUG

	#define MyAssert(_code, _level, _title)									\
		if(!(_code)) {														\
			MyAssertPreCode##_level;										\
			MyAssertPreCodeDebug##_level;									\
			MyAssertOut##_level(_code, _title);								\
			MyAssertPostCodeDebug##_level;									\
			MyAssertPostCode##_level;										\
		}

#else

	#define MyAssert(_code, _level, _title)									\
		if(!_code) {														\
			MyAssertPreCode##_level;										\
			MyAssertOut##_level(_code, _title);								\
			MyAssertPostCode##_level;										\
		}

#endif

#define MyAssertIntToStr_(_narf) #_narf
#define MyAssertIntToStr(_narf) MyAssertIntToStr_(_narf)
#define MyAssertLine MyAssertIntToStr(__LINE__)

