//===========================================================================
//  htmlguid.h
//
//	HTM Package GUIDS
//
//	Copyright (C) 1996 Microsoft Corporation,
//	All rights reserved.
//===========================================================================

#ifndef __HTMLGUID_H__
#define __HTMLGUID_H__

// {E6899811-0C72-11d0-8BF1-00A0C90F55D6}
DEFINE_GUID(IID_IHtmlLayoutDocument, 0xe6899811, 0xc72, 0x11d0, 0x8b, 0xf1, 0x0, 0xa0, 0xc9, 0xf, 0x55, 0xd6);

// {C92A2284-E3DD-11cf-A96F-00A0C90F55D6}
DEFINE_GUID(IID_IHtmlDocument, 0xc92a2284, 0xe3dd, 0x11cf, 0xa9, 0x6f, 0x0, 0xa0, 0xc9, 0xf, 0x55, 0xd6);

// {E95101F1-BA66-11cf-A40E-00AA00C00940}
DEFINE_GUID(SID_SHtmlSourceEditorSupport, 0xe95101f1, 0xba66, 0x11cf, 0xa4, 0xe, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);

// {E95101F2-BA66-11cf-A40E-00AA00C00940}
DEFINE_GUID(IID_IHtmlSourceEditorSupport, 0xe95101f2, 0xba66, 0x11cf, 0xa4, 0xe, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);

// {D18C18C1-304C-11d0-8158-00A0C91BBEE3}
DEFINE_GUID(SID_SWebDesignControlContext, 
0xd18c18c1, 0x304c, 0x11d0, 0x81, 0x58, 0x0, 0xa0, 0xc9, 0x1b, 0xbe, 0xe3);


#ifndef __DEFINE_DISPID_WDCC_SERVERLANGUAGE
#define __DEFINE_DISPID_WDCC_SERVERLANGUAGE
#define DISPID_WDCC_SERVERLANGUAGE	100
#endif


#endif // __HTMLGUID_H__
