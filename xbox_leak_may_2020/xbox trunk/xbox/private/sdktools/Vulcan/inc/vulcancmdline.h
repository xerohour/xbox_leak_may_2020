/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1994-1999 Microsoft Corporation.
*
* Component:
*
* File: vulcancmdline.h
*
* File Comments:
*
*
***********************************************************************/

#pragma once

class VULCANDLL CMDLINE
{
public:

	    enum ARGT { argtNil, argtHelp, argtInt, argtString, argtOptString, argtEnd };

	    struct OPTDEF
	    {
	       // The following is statically initialized

	       ARGT  argt;			// Argument type
	       const char *szOptName;		// String for option name
	       bool  fOptRequired;		// Is option required?
	       void  *pv;			// Where to store argument
	       bool  *pfSpecified;		// Is option specified?
	    };

   static   void VULCANCALL DisplayBanner(const char *);
   static   bool VULCANCALL FProcessArgs(const char *, int, const char *[], const OPTDEF *);
   static   bool VULCANCALL FSamePath(const char *, const char *);

private:

   static   const OPTDEF *PoptdefLookup(const OPTDEF *, const char *);
   static   bool FParseValue(const OPTDEF *, const char *);
};
