/****************************************************************************
	MsoGUIDs.h

	Owner: DavePa
	Copyright (c) 1994 Microsoft Corporation

	This file defines (if INIT_MSO_GUIDS is defined) or declares all the 
	OLE GUIDs exported by Office.

	Office Development has reserved GUIDs in the range:
		000Cxxxx-0000-0000-C000-000000000046

****************************************************************************/

#ifndef CMGUIDS_H
#define CMGUIDS_H

/* If INIT_MSO_GUIDS is defined then we're going to define all the GUIDS, 
	otherwise we'll just declare them.  Office GUIDS are specified by
	a category 0x00-0xFF and an index 0x00-0xFF. */
#undef DEFINE_MSO_GUID


#ifdef INIT_MSO_GUIDS

	#define DEFINE_MSO_GUID(name, bCategory, bIndex) \
		EXTERN_C const GUID name = {0x000C0000 | MAKEWORD(bIndex, bCategory), \
													  0, 0, {0xC0,0,0,0,0,0,0,0x46}}
#else

	#define DEFINE_MSO_GUID(name, bCategory, bIndex) \
		EXTERN_C const GUID name
		
#endif


/****************************************************************************
	Office GUIDS are specified by a category 0x00-0xFF and an index 0x00-0xFF. 
****************************************************************************/

// Category 6: Component Integration GUIDs
DEFINE_MSO_GUID(IID_IMsoComponent,              0x06, 0x00);
DEFINE_MSO_GUID(IID_IMsoComponentManager,       0x06, 0x01);
DEFINE_MSO_GUID(IID_IMsoStdComponentMgr,        0x06, 0x02);
DEFINE_MSO_GUID(IID_IMsoComponentHost,          0x06, 0x03);
DEFINE_MSO_GUID(IID_IMsoInPlaceComponent,       0x06, 0x04);
DEFINE_MSO_GUID(IID_IMsoInPlaceComponentSite,   0x06, 0x05);
DEFINE_MSO_GUID(IID_IMsoComponentUIManager,     0x06, 0x06);
DEFINE_MSO_GUID(IID_IMsoSimpleRecorder,         0x06, 0x07);
DEFINE_MSO_GUID(SID_SMsoComponentUIManager,     0x06, 0x08);
DEFINE_MSO_GUID(SID_SMsoSimpleRecorder,         0x06, 0x09);
DEFINE_MSO_GUID(IID_IMsoInPlaceComponentUIManager, 0x06, 0x0A);
DEFINE_MSO_GUID(SID_SMsoComponentManager,       0x06, 0x0B);

#endif	// CMGUIDS_H

