                          
//                                        Ruler
//       1         2         3         4         5         6         7         8
//345678901234567890123456789012345678901234567890123456789012345678901234567890

    /********************************************************************/
    /*                                                                  */
    /*   The standard layout.                                           */
    /*                                                                  */
    /*   The standard layout for 'cpp' files in this code is as         */
    /*   follows:                                                       */
    /*                                                                  */
    /*      1. Include files.                                           */
    /*      2. Constants local to the class.                            */
    /*      3. Data structures local to the class.                      */
    /*      4. Data initializations.                                    */
    /*      5. Static functions.                                        */
    /*      6. Class functions.                                         */
    /*                                                                  */
    /*   The constructor is typically the first function, class         */
    /*   member functions appear in alphabetical order with the         */
    /*   destructor appearing at the end of the file.  Any section      */
    /*   or function this is not required is simply omitted.            */
    /*                                                                  */
    /********************************************************************/

#include "HeapPCH.hpp"

#include "New.hpp"
#include "ThreadSafe.hpp"

#ifndef ENABLE_RECURSIVE_LOCKS
    /********************************************************************/
    /*                                                                  */
    /*   Static member initialization.                                  */
    /*                                                                  */
    /*   Static member initialization sets the initial value for all    */
    /*   static members.                                                */
    /*                                                                  */
    /********************************************************************/

SPINLOCK THREAD_SAFE::Spinlock;

#endif
    /********************************************************************/
    /*                                                                  */
    /*   Class constructor.                                             */
    /*                                                                  */
    /*   Create a thread safe class and prepare it for use.  This is    */
    /*   pretty simple but needs to be done with a bit of thought.      */
    /*                                                                  */
    /********************************************************************/

THREAD_SAFE::THREAD_SAFE( BOOLEAN NewThreadSafe )
    {
	RecursiveCount = NULL;
	ThreadSafe = NewThreadSafe;
    }

    /********************************************************************/
    /*                                                                  */
    /*   Decrement the reference count.                                 */
    /*                                                                  */
    /*   We decrement the TLS recursive reference count so we can       */
    /*   detect recursive lock calls.  If there is no suitable TLS      */
    /*   variable we fail as something is badly wrong.                  */
    /*                                                                  */
    /********************************************************************/

VOID THREAD_SAFE::DecrementLockCount( VOID )
	{
#ifndef ENABLE_RECURSIVE_LOCKS
	//
	//   We only need to do something when we have locking
	//   enabled for this heap.
	//
	if ( ThreadSafe )
		{
#ifdef DEBUGGING
		//
		//   We just make sure that there is a suitable
		//   TLS variable available.
		//
		if ( RecursiveCount != NULL )
			{
#endif
			//
			//   Update the recursive reference count.
			//
			RecursiveCount -> SetPointer
				( 
				((VOID*) (((SBIT32) RecursiveCount -> GetPointer()) - 1))
				);
#ifdef DEBUGGING
			}
		else
			{ Failure( "No reference count in DecrementLockCount" ); }
#endif
		}
#endif
	}

    /********************************************************************/
    /*                                                                  */
    /*   Increment the reference count.                                 */
    /*                                                                  */
    /*   We increment the TLS recursive reference count so we can       */
    /*   detect recursive lock calls.  We create a suitable TLS         */
    /*   variable as needed.                                            */
    /*                                                                  */
    /********************************************************************/

VOID THREAD_SAFE::IncrementLockCount( VOID )
	{
#ifndef ENABLE_RECURSIVE_LOCKS
	//
	//   We only need to do something when we have locking
	//   enabled for this heap.
	//
	if ( ThreadSafe )
		{
		//
		//   We create the TLS variable on an as needed
		//   basis so lets we if we need to do anything
		//   here.
		//
		if ( RecursiveCount == NULL )
			{
			//
			//   Claim a global lock to avoid any nasty 
			//   race conditions.
			//
			Spinlock.ClaimLock();

			//
			//   We need to reverify in case someone beat 
			//   us to it.
			//
			if ( RecursiveCount == NULL )
				{
				//
				//   Create a new TLS variable.
				//
				RecursiveCount = ((TLS*) & Tls);

				PLACEMENT_NEW( RecursiveCount,TLS );
				}

			//
			//   Release any locks we claimed earlier.
			//
			Spinlock.ReleaseLock();
			}

		//
		//   Update the recursive reference count.
		//
		RecursiveCount -> SetPointer
			( 
			((VOID*) (((SBIT32) RecursiveCount -> GetPointer()) + 1))
			);
		}
#endif
	}

    /********************************************************************/
    /*                                                                  */
    /*   Class destructor.                                              */
    /*                                                                  */
    /*   Delete the thread safe calls and free any associated           */
	/*   resources.                                                     */
    /*                                                                  */
    /********************************************************************/

THREAD_SAFE::~THREAD_SAFE( VOID )
    { 
#ifndef ENABLE_RECURSIVE_LOCKS
	//
	//   We delete the TLS variable if one was created
	//   at some point during the run.
	//
	if ( RecursiveCount != NULL )
		{
		//
		//   Claim a global lock to avoid any nasty 
		//   race conditions.
		//
		Spinlock.ClaimLock();

		//
		//   We need to reverify in case someone beat 
		//   us to it.
		//
		if ( RecursiveCount != NULL )
			{
			//
			//   Delete the TLS variable.
			//
			PLACEMENT_DELETE( RecursiveCount,TLS );
			}

		//
		//   Release any locks we claimed earlier.
		//
		Spinlock.ReleaseLock();
		}
#endif
    }
