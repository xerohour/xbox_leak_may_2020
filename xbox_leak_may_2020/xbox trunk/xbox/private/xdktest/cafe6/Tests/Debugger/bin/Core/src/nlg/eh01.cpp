#include "ehutil.h"
#include "nlgmain.h"

int counter = 0;
int ctorCounter = 0;
int dtorCounter = 0;

void eh01(void)
{
	int caseNum = 0;
	try 
	{
		++caseNum;
		Case1();   //throws t; no handler
		++caseNum; // should not execute
	}
	catch( ... ) // after Case1
	{
		++caseNum;
		Case2();   // doesn't throw anything
		++caseNum; // should execute

		try {
			++caseNum;
			Case3();   // throws t; doesn't catch it
			++caseNum; // should not execute
		}
		catch( ... ){ // after Case3
			try {
				++caseNum;
				Case4();   // throws t and catches it
				++caseNum; // should execute
			}
			catch( ... ) { // after Case4
				++caseNum; // should not execute
			}
		}
	}

 	try {
		++caseNum;
		Case5();   // throws t and catches it
		++caseNum; // should execute
	}
	catch( ... ) {
		++caseNum; // should not execute
	}

	try {
		++caseNum;
		Case6();   // throws t, rethrows it and catches it
		++caseNum; // should execute
	}
	catch( ... ) {
		++caseNum; // should not execute
	}

	try {
		++caseNum;
		Case7();   // throws t, rethrows it and catches it
		++caseNum; // should execute
	}
	catch( ... ) {
		++caseNum; // should not execute
	}

//	try {
//		++caseNum;
//		Case8();   // throws t, Ctor throws an int, Case8 doesn't catch t ??
//		++caseNum; // should execute ?
//	}
//	catch( ... ) {
//		++caseNum; // should not execute
//	}

	++caseNum; // should execute

}


/////////////////////////////////////////////////////////////////////////////

void Case1() {
	//	Throw without handler.
	ThrowObject t(1);
	throw t;	

	++counter; // Should not execute
}

void Case2() {
	//	try-block without a throw
	try {
		++counter;
	}
	catch( ... ) {
		++counter; // Should not execute
	}
	++counter; // Should execute
}

void Case4() {
	//	try-block with multiple handlers
	try { ++counter; ThrowObject t(4); throw t; }
	catch( int ) {
		++counter; // Should not execute
	}
	catch( ThrowObject c ) { ++counter; /* Should execute */ }
	catch( ... ) {
		++counter; // Should not execute
	}
	++counter; // Should execute
}

void Case5() {
	//	Throw from inner try-block caught in outer handler
	try {
		++counter;
		try {			
			++counter;
			ThrowObject t(5);
			throw t;

			++counter; // Should not execute
		}
		catch( int ) {
			++counter; // Should not execute
		}
		++counter; // Should not execute
	}
	catch( ThrowObject o ) {
		++counter; // Should execute
	}
	++counter; // Should execute
}

void Case6() {
	//	Throw from inner try-block handler caught in outer handler
	try {
		++counter;
		try {
			++counter;
			ThrowObject t(6);
			throw t;

			++counter; // Should not execute
		}
		catch( ThrowObject c ) {			
			++counter; // Should execute
			throw c;

			++counter; // Should not execute
		}
		++counter; // Should not execute
	}
	catch( ThrowObject o ) {
		++counter; // Should execute
	}

	++counter; // Should execute
}

