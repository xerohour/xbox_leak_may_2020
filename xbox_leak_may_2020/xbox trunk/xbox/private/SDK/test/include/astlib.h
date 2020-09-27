// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/astlib.h 4     5/09/96 3:40p Erikruf $
// $NoKeywords: $
//
// Top=level methods for AST library


#ifndef INC_ASTLIB_H 
#define INC_ASTLIB_H

class Astlib
{
  public:
    // Initialization, finalization
    ASTAPI static void ASTCALL     Initialize(FILE *debugOutputFile);
    ASTAPI static void ASTCALL     Finalize();
    ASTAPI static Version* ASTCALL GetVersion()
#ifdef AST_LIBRARY
      { return &version; }
#else
    ;
#endif

  private:
    static Version version;
};

#endif // INC_ASTLIB_H
