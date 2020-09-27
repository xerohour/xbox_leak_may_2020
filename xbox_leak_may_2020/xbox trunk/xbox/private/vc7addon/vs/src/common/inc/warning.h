//--------------------------------------------------------------------------
// warning.h
//--------------------------------------------------------------------------
// Warnings we want to ignore
//--------------------------------------------------------------------------
// Copyright (c) 1997, Microsoft Corporation
//		       All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//--------------------------------------------------------------------------

#ifndef __warning_h
#define __warning_h

// Warning that symbols are > 255 characters
#pragma warning (disable : 4786 )

// Warning that formal parameters are unused (common with OLE interfaces)
#pragma warning (disable : 4100 )

#endif // __warning_h
