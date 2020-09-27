#pragma once

#define RELEASE( p )                                                 \
{                                                                    \
	if( NULL != p )                                                  \
		p->Release();                                                \
                                                                     \
    p = NULL;                                                        \
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

#define CHECKALLOC( p )                                              \
if ( NULL == p )                                                     \
{                                                                    \
	Output( #p ## " was NULL\n" );                                 \
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

#define SETLOG( handle, name, comp, subcomp, func )                  \
{                                                                    \
   g_hLog = handle;                                                  \
   xSetOwnerAlias( g_hLog, name );                                   \
   xSetComponent( g_hLog, comp, subcomp );                           \
   xSetFunctionName( g_hLog, func );                                 \
}

#define EXECUTE( func )                                              \
{                                                                    \
	hr = func;                                                       \
	if ( FAILED( hr ) )                                              \
	{                                                                \
	    Output( #func ## " returned: 0x%x\n", hr );                \
	}                                                                \
	else                                                             \
	{                                                                \
	}                                                                \
	                                                                 \
}                                                                    

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

#define NUMELEMS(x) ( sizeof( x ) / sizeof( ( x ) [0] ) )

#define SUCCEED_ON_FAIL( hr, expected ) expected == hr ? S_OK : hr