/***********************************************************************
* Microsoft BBT
*
* Microsoft Confidential.  Copyright 1996-1997 Microsoft Corporation.
*
* Component:
*
* File: capsize.h
*
* File Comments:
*
*
***********************************************************************/

#include "vulcanapi.h"
#include "vulcancmdfile.h"
#include "vulcancmdline.h"
#include "cmddict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FALSE false

#define DEFAULTCAPDIR "capsize\\"
#define DEFAULTPDBEXT ".pdb"
#define DEFAULTCMDEXT ".bbtcmd"
#define DEFAULTASMEXT ".asm"

class CMDFILEDEF : public CMDFILE
{
public:

   static   const CMDDEF rgcmddef[];

private:
   void Exclude(unsigned, const char *);
};
