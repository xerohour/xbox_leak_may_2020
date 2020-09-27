//===================================================================
//
// Copyright Microsoft Corporation. All Right Reserved.
//
//===================================================================
#ifndef _2KIDMASTER_H_
#define _2KIDMASTER_H_

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "2kId_Tests.h"
#include "2kId_Utils.h"
   
CONST DWORD DO_LOCALHOST = 0x01;

class C2kIdMaster : C2kIdTests { //, C2kIdUtils( m_dwLoop ) {

public:
   
   VOID ValidateArgs(int argc, char **argv);
   VOID PrintHelp( VOID );


   C2kIdMaster(){ m_dwLoop=0; 
                  m_szXboxName[0] ='\0';}
   ~C2kIdMaster(){}

   
private:
   //Globals
   CHAR  m_szXboxName[MAX_PATH];
   DWORD m_dwLoop;
};

#endif
