/****************************************************************************
	MsoUser.h

	Owner: DavePa
 	Copyright (c) 1994 Microsoft Corporation

	Declarations for common functions and interfaces required for apps
	to use the Office DLL.
****************************************************************************/

#ifndef CMUSER_H
#define CMUSER_H

#ifndef MSO_NO_INTERFACES
interface IMsoControlContainer;
#endif // MSO_NO_INTERFACES


/****************************************************************************
	The ISimpleUnknown interface is a variant on IUnknown which supports
	QueryInterface but not reference counts.  All objects of this type
	are owned by their primary user and freed in an object-specific way.
	Objects are allowed to extend themselves by supporting other interfaces
	(or other versions of the primary interface), but these interfaces
	cannot be freed without the knowledge and cooperation of the object's 
	owner.  Hey, it's just like a good old fashioned data structure except
	now you can extend the interfaces.
****************************************************************** DAVEPA **/

#undef  INTERFACE
#define INTERFACE  ISimpleUnknown

DECLARE_INTERFACE(ISimpleUnknown)
{
	/* ISimpleUnknown's QueryInterface has the same semantics as the one in
		IUnknown, except that QI(IUnknown) succeeds if and only if the object
		also supports any real IUnknown interfaces, QI(ISimpleUnknown) always
		succeeds, and there is no implicit AddRef when an non-IUnknown-derived
		interface is requested.  If an object supports both IUnknown-derived
		and ISimpleUnknown-derived interfaces, then it must implement a 
		reference count, but all active ISimpleUnknown-derived interfaces count
		as a single reference count. */
	MSOMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObj) PURE;
};


/****************************************************************************
	HMSOINST is an opaque reference to an Office instance record.  Each
	thread of each EXE or DLL that uses Office must call MsoFInitOffice
	to init Office and get an HMSOINST.

	not anymore -- now it is just something that we ignore and if caller
	passes a pointer, we don't use it. erickn
****************************************************************** DAVEPA **/
#ifndef HMSOINST
typedef void* HMSOINST;  // just make this an ignorable pointer - erickn
#endif

#endif // CMUSER_H
