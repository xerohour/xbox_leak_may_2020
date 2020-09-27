#ifndef _WKSELSATOOLSESCAPE_H
#define _WKSELSATOOLSESCAPE_H
//*****************************Module*Header******************************
//
// Module Name: wkselsatoolsescape.h
//
// FNicklisch 09/14/2000: New, derived from EscapeTools.h
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1999-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//

#ifdef USE_WKS_ELSA_TOOLS

#define ESC_SUCCESS         1     // function succeded without error
#define ESC_NOTSUPPORTED    0     // function not supported
#define ESC_ERROR         (-1)    // function supported but error while working

LONG lEscapeTools( 
  SURFOBJ *pso,
  ULONG    cjIn,
  VOID    *pvIn,
  ULONG    cjOut,
  VOID    *pvOut);

#endif //USE_WKS_ELSA_TOOLS
#endif // _WKSELSATOOLSESCAPE_H
