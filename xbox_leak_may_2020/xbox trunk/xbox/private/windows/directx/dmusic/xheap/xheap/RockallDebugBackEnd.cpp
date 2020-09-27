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

#include "InterfacePCH.hpp"

#include "Environment.hpp"
#include "RockallDebugBackEnd.hpp"

    /********************************************************************/
    /*                                                                  */
    /*   Class constructor.                                             */
    /*                                                                  */
    /*   The may be a situation where the Rockall Back End needs a      */
    /*   constructor but this is certainly not expected to be           */
    /*   very common.                                                   */
    /*                                                                  */
    /********************************************************************/

ROCKALL_DEBUG_BACK_END::ROCKALL_DEBUG_BACK_END
		( 
		bool						  NewFormatting,
		bool						  NewNoAccess 
		)
	{
	STATIC ENVIRONMENT Environment;

	//
	//   Store the flags.
	//
	Formatting = NewFormatting; 
	NoAccess = NewNoAccess; 

	//
	//   Extract the OS page size.
	//
	PageSize = Environment.PageSize();
	}

    /********************************************************************/
    /*                                                                  */
    /*   Memory area allocation.                                        */
    /*                                                                  */
    /*   We need to allocate some new memory from the operating         */
    /*   system and prepare it for use in the debugging heap.           */
    /*                                                                  */
    /********************************************************************/

void *ROCKALL_DEBUG_BACK_END::NewArea( int AlignMask,int Size,bool User )
    {
	REGISTER void *Memory = (ROCKALL_BACK_END::NewArea( AlignMask,Size,User ));

	//
	//   If we managed to get a new page then write
	//   the guard value over it to allow us to
	//   verify it has not been overwritten later.
	//
	if ( Memory != ((void*) AllocationFailure) )
		{
		//
		//   Write the guard value into all of the new
		//   heap page to allow it to be checked for
		//   corruption.
		//
		if ( Formatting )
			{
			REGISTER int Count;

			for ( Count=((Size / GuardSize) - 1);Count >= 0;Count -- )
				{ (((int*) Memory)[ Count ]) = GuardValue; }
			}

		//
		//   When 'NoAccess' is requested we remove 
		//   all access rights to the memory area.  So
		//   we will fault if there is any attempt to
		//   read or write from this memory region.
		//
		if ( (NoAccess) && (User) )
			{ ProtectArea( Memory,Size ); }
		}
	
	return Memory;
	}

    /********************************************************************/
    /*                                                                  */
    /*   Memory area allocation.                                        */
    /*                                                                  */
    /*   We need to allocate some new memory from the operating         */
    /*   system and prepare it for use in the debugging heap.           */
    /*                                                                  */
    /********************************************************************/

void ROCKALL_DEBUG_BACK_END::ProtectArea( void *Address,int Size )
    {
	//
	//   Lets be sure that the area that is to be
	//   protected is page aligned.
	//
	if ( ((((long) Address) & (PageSize-1)) == 0) && ((Size % PageSize) == 0) )
		{
		AUTO DWORD Original;

		//
		//   We need to protect the memory area to 
		//   prevent any further access.
		//
		VirtualAlloc( Address,Size,MEM_RESET,PAGE_NOACCESS );

		VirtualProtect( Address,Size,PAGE_NOACCESS,& Original );

		//
		//   Lets be sure the original protection mode
		//   was what we expected.
		//
		if ( Original != PAGE_READWRITE )
			{ Failure( "Area has an unexpected protection mode" ); }
		}
	else
		{ Failure( "Protection area not page aligned" ); }
	}

    /********************************************************************/
    /*                                                                  */
    /*   Memory area allocation.                                        */
    /*                                                                  */
    /*   We need to allocate some new memory from the operating         */
    /*   system and prepare it for use in the debugging heap.           */
    /*                                                                  */
    /********************************************************************/

void ROCKALL_DEBUG_BACK_END::UnprotectArea( void *Address,int Size )
    {
	//
	//   Lets be sure that the area that is to be
	//   un protected is page aligned.
	//
	if ( ((((long) Address) & (PageSize-1)) == 0) && ((Size % PageSize) == 0) )
		{
		AUTO DWORD Original;

		//
		//   We need to unprotect the memory area to 
		//   enable later access.
		//
		VirtualProtect( Address,Size,PAGE_READWRITE,& Original );

		//
		//   Lets be sure the original protection mode
		//   was what we expected.
		//
		if ( Original != PAGE_NOACCESS )
			{ Failure( "Area has an unexpected protection mode" ); }
		}
	else
		{ Failure( "Protection area not page aligned" ); }
	}

    /********************************************************************/
    /*                                                                  */
    /*   Class destructor.                                              */
    /*                                                                  */
    /*   The may be a situation where the Rockall Back End needs a      */
    /*   destructor but this is certainly not expected to be            */
    /*   very common.                                                   */
    /*                                                                  */
    /********************************************************************/

ROCKALL_DEBUG_BACK_END::~ROCKALL_DEBUG_BACK_END( void )
	{ /* void */ }
