// -*- Mode:C++ -*-
//
//      Copyright (c) 1996 Microsoft Corporation.  All rights reserved.
//
//    This work is a trade secret of Microsoft Corporation.
//    Unauthorized disclosure is unlawful.
//
// $Header: /ast/driver\60/AstCL.h 1     8/27/98 4:30p Lindaog $
// $NoKeywords: $
//
//
// File:    driver/AstCL.h
// Creator: 

// Exported Function List: 
#ifndef INC_AstCL_H
#define INC_AstCL_H

#include "astdriver.h"

BEGIN_AST_NAMESPACE

// This class contains methods that are called by the driver to
// emulate the various compiler passes. Each pass receives an argv-style
// argument list (use GetPassname() to get the name of the executable).
// The result of these functions is treated as a program exit code.

// For a particular analysis frontend, the user should create a subclass
// of AstCL that defines appropriate c1/c1xxx/c2/link methods. See
// the calltree project for a simple complete example.

class AstCL
{
 public:
    ASTDRIVERAPI AstCL();

    // Default pass implementations.
    // c1 and c1xx call the AST frontend with RunRaw.
    // c2 creates (with ReadModule) and then immediately deletes the
    // AST tree.
    // link executes the link.exe program found in the AST\bin
    // directory.
    ASTDRIVERAPI virtual int c1(char **args);
    ASTDRIVERAPI virtual int c1xx(char **args);
    ASTDRIVERAPI virtual int c2(char **args);
    ASTDRIVERAPI virtual int link(char **args);

    // Utility functions, these should probably be used by inheritors of
    // AstCL to run the frontend and build the AST tree

    ASTDRIVERAPI void ReadModule(char **args);
    // Requires: args (c2 arguments) must specify filename (-f), object name
    //   (-Fo) and IL path (-il).
    // Effects: Reads the IL files specified by the c2 arguments in 'args',
    //   and builds an AST tree from that. ilPath is initialised with
    //   the IL path, module points to the loaded AST tree.
    //   fileName, objName and ilName are also initialised from the c2
    //   arguments.

    ASTDRIVERAPI static int ASTDRIVERCALL
      RunRawPath(char *astc1path, char **args, bool fDll = TRUE);
    // Requires:
    //   astc1path (of astc1(xx) to execute)
    //   args (original c1 arguments),
    //   fDll indicates whether args were produced
    //   by c1.dll (TRUE) or c1.exe (FALSE)
    // Effects:
    //   Executes the AST frontend (astc1path)
    //   with c1 arguments 'args', modified as follows:
    //     c1 (c1xx) in args[0] becomes astc1 (astc1xx)
    //     -Zi/-Z7 is added
    //     -Yd is added if -Yc is present
    //     if arguments are in DLL form (i.e., unquoted)
    //       and AST frontend is an .EXE,
    //       arguments will be quoted before being passed

    ASTDRIVERAPI static int ASTDRIVERCALL
      RunRaw(char **args, bool fDll = TRUE)
      { return RunRawPath(args[0], args, fDll); }
    // RunRaw is being deprecated;
    // replace RunRaw(args) with
    //         RunRawPath(GetPassname(), args,
    //                    FPassIsDLL(pkC1{XX}))

    ASTDRIVERAPI static char *GetPassExecutable();
    // When invoking a pass, the driver supplies both an argument vector
    // and the name of the executable to be invoked (which won't *always*
    // be the same as args[0]; args[0] can have quotes in it...)
    // The former is passed to the hook routine as ARGS, GetPassExecutable()
    // retrieves the latter.  Since this returns a pointer to static
    // storage, you must copy it if you need it beyond the return of
    // the hook routine.

    enum PassKind { 
        pkC1 = 1,               // C front end
        pkC1XX,                 // C++ front end
        pkC2,                   // back end
        pkLINK,                 // linker

        pkERROR = -1,
    };

    ASTDRIVERAPI static bool FPassIsDLL(PassKind);
    // Is the given pass a .dll pass?
    // TRUE  => arguments are pristine.
    // FALSE => arguments are quoted.
    // Should be called only from hook routines.

    ASTDRIVERAPI static PassKind 
      PathnameToPassKind(char *name, bool *fDebug = NULL);
    // Return the PassKind corresponding to the executable NAME
    // no matter whether this points to
    //   the real version of a pass executable (e.g., ...\c1.dll)
    //   or our own version (e.g., ...\astcl.exe).
    // If FDEBUG non-null, set *FDEBUG true according as NAME appears
    // to be a debug version (e.g., ...\c1d.dll).

    ASTDRIVERAPI static bool fAllowC1Warnings;
    // If set FALSE, warnings will be suppressed in astc1(xx) runs.
    // Defaults to TRUE unless environment variable ASTC1WARN="no"

 protected:
    // Current state, set by ReadModule
    char *fileName;
    char *objName; 
    char *ilName;
    IlPath *ilPath;
    Module *module;
};

ASTDRIVERAPI int ASTDRIVERCALL RunCL(AstCL *hks, int argc, char **argv);
// Effects: Runs the usual CL driver, intercepting calls to execute program
//   and calling the methods in hks instead.
// Result: CL's exit code.

END_AST_NAMESPACE

#endif
