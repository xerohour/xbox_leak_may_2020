/*-----------------------------------------------------------------------------
Microsoft Sterling

Microsoft Confidential
Copyright 1994-1996 Microsoft Corporation.  All Rights Reserved.

File:		oleipc.h
Contents:	This file describes the OLE in-place component interfaces guids
-----------------------------------------------------------------------------*/

// These two GUID's are duplicated from the office header file
// msoguids.h.
//
#undef DEFINE_MSO_GUID
#ifdef INIT_MSO_GUIDS
	#define DEFINE_MSO_GUID(name, bCategory, bIndex) \
		EXTERN_C const GUID name = {0x000C0000 | MAKEWORD(bIndex, bCategory), \
													  0, 0, {0xC0,0,0,0,0,0,0,0x46}}
#else
	#define DEFINE_MSO_GUID(name, bCategory, bIndex) \
		EXTERN_C const GUID name
#endif

DEFINE_MSO_GUID(SID_MsoComponentManager,		0x06, 0x0B);


///////////////////////////////////////////////////////////////////////////////
// All of these values are taken from oleguids.h

// {5efc7970-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(IID_IOleInPlaceComponent,
0x5efc7970, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {5efc7971-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(IID_IOleInPlaceComponentSite,
0x5efc7971, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {5efc7972-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(IID_IOleComponentUIManager,
0x5efc7972, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {5efc7973-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(IID_IOleInPlaceComponentUIManager,
0x5efc7973, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {5efc7974-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(SID_OleComponentUIManager,
0x5efc7974, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {5efc7975-14bc-11cf-9b2b-00aa00573819}
DEFINE_GUID(CMDSETID_StandardCommandSet97,
0x5efc7975, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

//$UNDONE 5.0
// The name CLSID_StandardCommandSet97 has been changed to CMDSETID_...
// This define allows clients to compile using the old name; once
// everyone has switched over to the new name, this define can
// be removed.
//
#define CLSID_StandardCommandSet97	  CMDSETID_StandardCommandSet97

// The private commands for the DaVinci tools are associated with
// this GUID.
//$UNDONE 4.5 -- this define should relocate to a new header file.
//
DEFINE_GUID(CMDSETID_DaVinciCommandSet97,
0x5efc7976, 0x14bc, 0x11cf, 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19);

// {CC5C9672-FAA4-11ce-88F5-00AA00A28331}
DEFINE_GUID(IID_IOleHostRegistry,
0xcc5c9672, 0xfaa4, 0x11ce, 0x88, 0xf5, 0x0, 0xaa, 0x0, 0xa2, 0x83, 0x31);

// {CC5C9671-FAA4-11ce-88F5-00AA00A28331}
DEFINE_GUID(SID_OleHostRegistry,
0xcc5c9671, 0xfaa4, 0x11ce, 0x88, 0xf5, 0x0, 0xaa, 0x0, 0xa2, 0x83, 0x31);

//INTERFACENAME = { /* 5efc7977-14bc-11cf-9b2b-00aa00573819 */
//	 0x5efc7977,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc7978-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc7978,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc7979-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc7979,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797a-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797a,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797b-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797b,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797c-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797c,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797d-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797d,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797e-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797e,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
//INTERFACENAME = { /* 5efc797f-14bc-11cf-9b2b-00aa00573819 */
//	  0x5efc797f,
//	  0x14bc,
//	  0x11cf,
//	  {0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19}
//	};
