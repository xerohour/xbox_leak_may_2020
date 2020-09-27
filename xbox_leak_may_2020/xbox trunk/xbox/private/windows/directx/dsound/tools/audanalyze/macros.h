#pragma once

#define QUOTE0(a)               #a
#define QUOTE1(a)               QUOTE0(a)

#define MESSAGE(a)              message(__FILE__ "(" QUOTE1(__LINE__) "): " a)
#define TODO(a)                 MESSAGE("TODO: " a)

#define RELEASE( p )                                                 \
{                                                                    \
	if( NULL != p )                                                  \
		p->Release();                                                \
                                                                     \
    p = NULL;                                                        \
}

#define VALIDATEHR( hr )										     \
{																	 \
	if ( S_FALSE == hr )                                             \
	{                                                                \
		printf( "hr was S_FALSE\n" );                              \
	}																 \
}

#define SAFEDELETE( p )                                              \
{                                                                    \
	delete p;                                                        \
	p = NULL;                                                        \
}

#define SAFECLOSEHANDLE( h )                                         \
{                                                                    \
	if ( NULL != h )                                                 \
		CloseHandle( h );                                            \
		                                                             \
	h = NULL;                                                        \
}

#define CHECKPTR( p )												 \
if ( NULL == p )													 \
{																	 \
	printf( "Argument was NULL\n" );								 \
	hr = E_POINTER;													 \
}

#define CHECKALLOC( p )                                              \
if ( NULL == p )                                                     \
{                                                                    \
	printf( #p ## " was NULL\n" );                                 \
	hr = E_OUTOFMEMORY;                                              \
}                                                                    

#define ALLOCATEANDCHECKSTRUCT( p, type )                            \
if ( SUCCEEDED( hr ) )			                                     \
{								                                     \
	p = new type();				                                     \
									                                 \
	CHECKALLOC( p );                                                 \
	                                                                 \
	if ( SUCCEEDED( hr ) )                                           \
	{                                                                \
	   memset( p, 0, sizeof( type ) );                               \
	}                                                                \
}	

#define ALLOCATEANDCHECK( p, type )                                  \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
    p = new type();                                                  \
                                                                     \
    CHECKALLOC( p );                                                 \
                                                                     \
}							

#ifdef NOLOGGING
#define SETLOG( handle, name, comp, subcomp, func )
#else // NOLOGGING
#define SETLOG( handle, name, comp, subcomp, func )                  \
{                                                                    \
   g_hLog = handle;                                                  \
   xSetOwnerAlias( g_hLog, name );                                   \
   xSetComponent( g_hLog, comp, subcomp );                           \
   xSetFunctionName( g_hLog, func );                                 \
}
#endif //NOLOGGING

#ifdef NOLOGGING
#define EXECUTE( func )                                              \
{                                                                    \
	hr = func;                                                       \
	if ( FAILED( hr ) )                                              \
	{                                                                \
	    printf( #func ## " returned: 0x%x\n", hr );                \
	}                                                                \
}
#else // NOLOGGING
#define EXECUTE( func )                                              \
{                                                                    \
	hr = func;                                                       \
	VALIDATEHR( hr );	                                             \
	if ( FAILED( hr ) )                                              \
	{                                                                \
	    printf( #func ## " returned: 0x%x\n", hr );                \
		xLog( g_hLog, XLL_FAIL, #func ## " returned: 0x%x\n", hr );  \
	}                                                                \
	else                                                             \
	{                                                                \
		xLog( g_hLog, XLL_PASS, #func ## " returned: 0x%x\n", hr );  \
	}                                                                \
	                                                                 \
}   
#endif // NOLOGGING     

#ifdef NOLOGGING
#define SHOULDRUNTEST( libName, testName ) TRUE 
#else // NOLOGGING
#define SHOULDRUNTEST( libName, testName ) GetProfileIntA( libName, testName, FALSE ) 
#endif // NOLOGGING
                                                           

#define CHECKEXECUTE( func )                                         \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
    EXECUTE( func );                                                 \
}

#define CHECK( exp )                                                 \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
	exp;                                                             \
}

#define FAIL_ON_CONDITION( exp )                                     \
if ( exp )                                                           \
{                                                                    \
    ASSERT( ! ( exp ) );                                             \
    hr = E_FAIL;                                                     \
}

//Runs the function without printing anything.
#define RUN( func )                                                  \
{                                                                    \
	hr = func;                                                       \
}

// Runs the function for a bool value
#define RUNBOOL( func )                                              \
{                                                                    \
	hr = func ? S_OK : E_FAIL;                                       \
}

// Runs the function for a 0 int value
#define RUNINT( func )                                               \
{                                                                    \
	hr = 0 == func ? S_OK : E_FAIL;                                  \
}                                                                    

// Runs the function for a valid handle value
#define RUNHANDLE( handle, func )                                    \
{                                                                    \
	hr = NULL != ( handle = func ) ? S_OK : E_FAIL;                  \
}  

//Checks and runs the function without printing anything.
#define CHECKRUN( exp )                                              \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
	RUN(exp);                                                        \
	VALIDATEHR( hr );                                                \
}

// Checks and runs the function for a bool value
#define CHECKRUNBOOL( exp )                                          \
if ( SUCCEEDED( hr ) )                                               \
{																	 \
	RUNBOOL( exp );                                                  \
}

// Checks and runs the function for a 0 int return                   
#define CHECKRUNINT( exp )                                           \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
	RUNINT( exp );                                                   \
}

// Checks and runs the function for a valid handle return                   
#define CHECKRUNHANDLE( handle, exp )                                \
if ( SUCCEEDED( hr ) )                                               \
{                                                                    \
	RUNHANDLE( handle, exp );                                        \
}

#define NUMELEMS(x) ( sizeof( x ) / sizeof( ( x ) [0] ) )

#define SUCCEED_ON_FAIL( hr, expected ) expected == hr ? S_OK : S_OK == hr ? E_FAIL : hr

