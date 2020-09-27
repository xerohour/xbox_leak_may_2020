#include "ehutil.h"
#include "nlgmain.h"

extern int counter;

void Case3() {
	//	try-block with unhandled throw
	try {
		++counter;
		ThrowObject t(3);
		throw t;
	}
	catch( int ) {
		++counter; // Should not execute
	}
	++counter; // Should not execute
}

void Case7() {
	//	Rethrow from inner try-block handler caught in outer handler
	try {
		++counter;
		try {
			++counter;
			ThrowObject t(7);
			throw t;
//  int ii=5;
//  throw ii;
			++counter; // Should not execute
		}
		catch( int ) {			
			++counter; // Should execute
			throw;

			++counter; // Should not execute
		}
		catch( ThrowObject& ) {			
			++counter; // Should execute
			throw;

			++counter; // Should not execute
		}
		++counter; // Should not execute
	}
	catch( int ) {
		++counter; // Should execute
	}
	catch( ThrowObject o ) {
		++counter; // Should execute
	}

	++counter; // Should execute

	Case5();   // throws t and catches it
}


void Case8() {
	//	Throw from inner try-block caught in outer handler; Ctor throws an int
	try {
		++counter;
		try {			
			++counter;
			ThrowObject t(8);
			t = ThrowObject(80);
			throw t;

			++counter; // Should not execute
		}
		catch( int ) {
			++counter; // Should execute
		}
		++counter; // Should execute ?
	}
	catch( ThrowObject o ) {
		++counter; // Should execute
	}
	++counter; // Should execute
}
