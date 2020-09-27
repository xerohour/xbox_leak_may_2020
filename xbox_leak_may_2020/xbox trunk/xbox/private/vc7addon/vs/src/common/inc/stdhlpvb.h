//---------------------------------------------------------------------------
// StdHlpVb.h
//---------------------------------------------------------------------------
// Help IDs for prop/meth/evt structures
//---------------------------------------------------------------------------
// Copyright (c) 1994, Microsoft Corporation
//                 All Rights Reserved
// Information Contained Herein Is Proprietary and Confidential.
//---------------------------------------------------------------------------

#ifndef STDHLPVB_H
#define STDHLPVB_H  // Indicates this file has been included.

#include "ids.h" // HELP_TOC_CONTEXT

// This is the VB.HLP table of contents context id
//#define HELP_TOC_CONTEXT        2008002
// help id for all (Custom) properties on the prop browser
//#define HELP_CUSTOM_PROPERTY    2020930


#define vbNoHelp        65535


// This is the base help context number offset for all intrinsic controls.
// Resource ID# = helpstringcontext - HELPSTRINGCTX_RESID_OFFSET
// helpstringcontext = Resource ID# + HELPSTRINGCTX_RESID_OFFSET
#define HELPSTRINGCTX_RESID_OFFSET 900000

// Note: Constants that used to be defined here (help resources) no longer 
// Note: need to be added here.  All you need to do now to add help to
// Note: intrinsic controls/methods/etc is to add the appropriate constant
// Note: to either RbyObj.ID (objects), RbyMth.ID (methods), RbyEvt.ID
// Note: (events), or RbyPro.ID (properties).  The rest is now automatic.


#endif  // STDHLPVB_H
