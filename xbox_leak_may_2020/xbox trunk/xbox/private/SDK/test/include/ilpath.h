// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/ilpath.h 2     4/30/96 6:53p Erikruf $
// $NoKeywords: $
//
// 


#ifndef INC_ILPATH_H 
#define INC_ILPATH_H

//
// Descriptor for a source module's Il streams.
// Right now, it's completely specified by the prefix used to
// generate the IL file names, but in the future it will also
// include PDB paths, etc.
//

class IlPath: public Object, private WithOwnArena
{
  public: 
    ASTAPI IlPath();
    ASTAPI ~IlPath();
    ASTAPI void         Instantiate(char *buf);
    ASTAPI void         Serialize(char *buf, int len);
    ASTAPI void         DeleteIl();
    ASTAPI static void  DeleteIlFiles(char *rootname);
    // overrides
    ASTAPI virtual void DumpMethod(DumpContext *pDC);
    ASTAPI FrontEndType GetLanguage() { return language; }

    ASTAPI void Open();
    ASTAPI void Close();
    ASTAPI FILE *GetEXFile() { return exFile; }
    ASTAPI FILE *GetGLFile() { return glFile; }
    ASTAPI FILE *GetSYFile() { return syFile; }
    ASTAPI FILE *GetDBFile() { return dbFile; }
    ASTAPI FILE *GetINFile() { return inFile; }
    ASTAPI char *GetPath()   { return path; }
  private: 
    char *path;
    Bool fOpen;
    FILE *exFile, *glFile, *syFile, *dbFile, *inFile;
    FrontEndType language;
};

// Forward declarations
class IlPath;                   // Also declared in type.h
class IlStream;                 // Also declared in type.h

#endif // INC_ILPATH_H


