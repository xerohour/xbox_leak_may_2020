//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1996 - 1998  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// Interface to parser to get current time - useful for multifile playback

#ifndef __AMPARSE__
#define __AMPARSE__

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


DEFINE_GUID(IID_IAMParse,
0xc47a3420, 0x005c, 0x11d2, 0x90, 0x38, 0x00, 0xa0, 0xc9, 0x69, 0x72, 0x98);

//
//  Parser interface - supported by MPEG-2 splitter filter
//
DECLARE_INTERFACE_(IAMParse, IUnknown) {
    STDMETHOD(GetParseTime) (THIS_
                             REFERENCE_TIME *prtCurrent
                            ) PURE;
    STDMETHOD(SetParseTime) (THIS_
                             REFERENCE_TIME rtCurrent
                            ) PURE;
    STDMETHOD(Flush) (THIS) PURE;
};

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __AMPARSE__
