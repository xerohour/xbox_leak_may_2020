// -*- Mode:C++ -*-
//
//      Copyright (c) 1996, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/driver\60/astdriver.h 1     8/27/98 4:30p Lindaog $
// $NoKeywords: $
//
 
// Main include file for Ast library.

#ifndef INC_ASTDRIVER_H 
#define INC_ASTDRIVER_H

// We assume that ast.h is already included by any file that includes this one.

// Configuration macros set by makefile
//
// AST_DRIVER                   True for Driver, false for client
// AST_DRIVER_VERSION           4, 5, or 6

#ifndef AST_DRIVER_VERSION
#  define AST_DRIVER_VERSION 6
#elif AST_DRIVER_VERSION != 6
#  error "wrong driver being included"
#endif

#ifdef AST_DRIVER
#  define AST_DRIVER_API __declspec(dllexport)
#else
#  define AST_DRIVER_API __declspec(dllimport)
#endif // AST_LIBRARY

#ifndef ASTDRIVERAPI
#define ASTDRIVERAPI AST_DRIVER_API
#endif

#ifndef ASTDRIVERCALL
#define ASTDRIVERCALL __stdcall
#endif

#endif // INC_ASTDRIVER_H

