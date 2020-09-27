// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/ast.h 22    5/16/96 10:27a Erikruf2 $
// $NoKeywords: $
//
 
// Main include file for Ast library.

#ifndef INC_AST_H 
#define INC_AST_H

// Configuration macros set by makefile
//
// AST_LIBRARY                  True for library, false for client
// AST_STATIC                   True for static library/.LIB client,
//                              False for dynamic library/.DLL client;
// AST_USE_NAMESPACE            Wraps decls in namespace Ast { }
// AST_HIDE_NON_ESSENTIAL_DEFS  Exports fewer names to client.
//                              NOP if AST_LIBRARY is defined

#ifdef AST_STATIC
#  define AST_IMPORT_EXPORT
#else
#  ifdef AST_LIBRARY
#    define AST_IMPORT_EXPORT __declspec(dllexport)
#  else
#    define AST_IMPORT_EXPORT __declspec(dllimport)
#  endif // AST_LIBRARY
#endif // AST_STATIC

#ifndef ASTAPI
#define ASTAPI AST_IMPORT_EXPORT
#endif
#ifndef AST_TEMPLATE_API
#define AST_TEMPLATE_API
#endif

#ifndef ASTCALL
#define ASTCALL  __stdcall
#endif

#ifdef AST_USE_NAMESPACE
#  define BEGIN_AST_NAMESPACE namespace Ast {
#  define END_AST_NAMESPACE }
#  define AST_NAMESPACE_IZE(foo) Ast::##foo
#else
#  define BEGIN_AST_NAMESPACE
#  define END_AST_NAMESPACE
#  define AST_NAMESPACE_IZE(foo) foo
#endif 

#ifdef AST_LIBRARY
#  undef AST_HIDE_NON_ESSENTIAL_DEFS
#  define AST_INCLUDE_NON_ESSENTIAL
#endif // AST_LIBRARY

#ifdef AST_HIDE_NON_ESSENTIAL_DEFS
#  undef AST_INCLUDE_NON_ESSENTIAL
#else
#  define AST_INCLUDE_NON_ESSENTIAL
#endif

// Visibility class "internal:" is used in the implementation
// for symbols that are internal to the library.
// Client should not use "internal" symbols.
#define internal public

//
// System includes
//
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef AST_LIBRARY              // implementation needs these; client doesn't
#  define WIN32_EXTRA_LEAN
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h> 
#  include <malloc.h>
#  include <string.h>
#  include <io.h>
#  include <errno.h>
#  include <typeinfo.h>
#endif 

#ifdef _CRTDBG_MAP_ALLOC
   // confusion occurs if client wants malloc
   // but crtdbg.h is included first, thus...
#  include <malloc.h>
#  include <crtdbg.h>
#endif

#ifndef AST_LIBRARY             
// Clients don't need complete definitions.
  struct PDB;
  struct TPI;
#endif

//
// Library includes
//
BEGIN_AST_NAMESPACE

// Basic types, macros, utilities
#include "base.h"
#include "object.h"
#include "stream.h"
#include "alloc.h"
#include "debug.h"
#include "version.h"

// IL reading utilities
#ifdef AST_LIBRARY
#  include "ilutil.h" 
#else
#endif
#include "ilpath.h"

// AST data structures known to IL utils
#include "literal.h"
#include "attrib.h"
#ifdef AST_LIBRARY
#include "typeio.h"
#else
class TypeIO;  // all clients need is a forward declaration
#endif


// AST data structures
#include "source.h"
#include "type.h"
#include "symbol.h"
#include "asm.h"
#include "node.h"
#include "module.h"
#include "util.h"
#include "astlib.h"

#ifdef AST_LIBRARY
#include "walker.h"
#include "stats.h"
#endif

END_AST_NAMESPACE
 
#endif // INC_AST_H

