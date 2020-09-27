// -*- Mode:C++ -*-
//
//      Copyright (c) 1997 Microsoft Corporation.  All rights reserved.
//
//    This work is a trade secret of Microsoft Corporation.
//    Unauthorized disclosure is unlawful.
//
// $Header: $
// $NoKeywords: $
//
//
// File:    clients/cmdcl/c1cmdln.h
// Creator: rfc

#ifndef INC_C1CMDLN_H
#define INC_C1CMDLN_H

struct C1Cmdline
{
    char *objname;              // name of object file
    char *srcname;              // name of source file
    char *cwd;                  // working directory for CL
    char **args;                // argv for CL
    short ilindex;              // ilname = args[ilindex]
    short efindex;
    short zindex;
    short ycindex;
    bool  fDll;                 // is argv in DLL format?
    //                (in DLL format, args are unquoted)
    C1Cmdline():
        objname(NULL), srcname(NULL), cwd(NULL), args(NULL),
        ilindex(0), efindex(0), zindex(0), ycindex(0),
        fDll(0)
      { }

    bool          FCppOnly();   // does .args contain -E?
    int           ArgCount()    { return CountArgs(const_cast<const char **>(args)); }

    // Perform various sanity checks on OARGV.
    // Assign ilindex, efindex, zindex, ycindex.
    // Return TRUE iff successful;
    // otherwise return FALSE
    // and DigestError() will say what went wrong.
    // If nargv is non-NULL, copy oargv there.
    bool          DigestArgs(int oargc, char **oargv, char **nargv = NULL);
    char *        DigestError() { return objname; }

    // ensure that ilindex, efindex, zindex, ycindex,
    // read in from elsewhere are correct,
    // Return NULL iff so,
    // otherwise return a warning message
    char *        CheckIndices();

    // utilities
    static const char * GetCWD();
    static int          CountArgs(const char **argv)
      { for (const char **a = argv; *a; ++a); return a - argv; }

  protected:
    void SetSrcName(char *newname);
};

#endif // INC_C1CMDLN_H
