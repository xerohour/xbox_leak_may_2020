#include "sehutil.h"
#include "seh01.h"

int main()
{
	Seh01();

	return 0;
}

void Seh01()
{
	SehCase1();
	SehCase2();
	SehCase3();
	SehCase4();
	SehCase6();
	SehCase8();

	SehCase5();
	SehCase7();

}

void SehCase1() 
{
	++counter;

	//	Simple __try/__except with no exception.

	__try {
		++counter;
		RaiseNoException();
	}
	__except( ++counter, ExecuteHandler() ) {
		++counter; // Code should be skipped.
	}
	++counter; // should execute	
}

void SehCase2() 
{
	++counter;

	//	Simple __try/__finally with no exception.

	__try {
		++counter;
		RaiseNoException();
	}
	__finally {
		++counter; // should execute	
	}	
}

void SehCase3() 
{
	++counter;

	//	Simple __try/__except: EXCEPTION_CONTINUE_EXECUTION.

	__try {
		++counter;
		//	Use software exception from RaiseIntDivideByZero() instead of 
		//	hardware exception from IntDivideByZero() since continuing execution
		//	without fixing fault condition causes an infinite loop.

		RaiseIntDivideByZero();

		++counter; // Should continue here
	}
	__except( ++counter, ContinueExecution() ) {
		++counter; // Code should be skipped.
	}	
	++counter; // should execute	
}

void SehCase4() 
{
	++counter;

	//	Simple __try/__except: EXCEPTION_EXECUTE_HANDLER.

	__try {
		++counter;
		IntDivideByZero();
		//	Should no reach here.
		++counter;
	}
	__except( ++counter, ExecuteHandler() ) {
		++counter; // should execute	
	}	
}

void SehCase5() 
{
	++counter;

	//	Simple __try/__except: EXCEPTION_CONTINUE_SEARCH.

	__try {
		++counter;

		IntDivideByZero();
		//	Code could be executed if final handler returns
		//	EXCEPTION_CONTINUE_EXECUTION.
		++counter;
	}
	__except( ++counter, ContinueSearch() ) {
		++counter; // Code should be skipped
	}	
	++counter; // should execute	
}

void SehCase6() 
{
	++counter;

	//	Simple __try/__finally with exception.

	__try {
		++counter;
		__try {
			++counter;
			IntDivideByZero();
			//	Should no reach here.
			++counter;
		}
		__finally {
			++counter; // should execute	
		}
		//	Should not reach here.
		++counter;
	}
	__except( ++counter, ExecuteHandler() ) {
		++counter; // should execute	
	}
}


void SehCase7() 
{
	++counter;

	//	__try/__except: EXCEPTION_CONTINUE_SEARCH.

	__try {
		++counter;
		__try {
			++counter;

			IntDivideByZero();
			//	Code could be executed if final handler returns
			//	EXCEPTION_CONTINUE_EXECUTION.
			++counter;
		}
		__except( ++counter, ContinueSearch() ) {
			++counter; // Code should be skipped
		}	
		++counter; // should execute
	}	
	__except( ++counter, ContinueExecution() ) {
		++counter; // Code should be skipped
	}	
	++counter; // should execute	
}

void SehCase8() 
{
	++counter;

	//	Simple __try/__finally with exception.

	__try {	
		++counter; 
		__try { ++counter; RaiseIntDivideByZero(); /* Should no reach here */ ++counter;}
		__finally { ++counter; /* should execute */ }
		//	Should not reach here.
		++counter;
	}
	__except( ++counter, ExecuteHandler() ) {
		++counter; // should execute	
	}

	__try { ++counter; RaiseIntDivideByZero(); ++counter; /* Should continue here */ }
	__except( ++counter, ContinueExecution() ) { ++counter; /* Code should be skipped */ }	
	++counter; // should execute	
}


