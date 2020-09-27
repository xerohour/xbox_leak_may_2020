#define INITGUID

// HACK alert: the d3dx8 headers don't compile under C because they don't define
//   : INTERFACE for any of the interfaces they define.  This causes massive headaches
//   : when compiling this stuff for C.
//
//   : This will make it work for now (we're just instantiating GUIDs after all).  It'll 
//   : be fixed for real when we decomize.
//
#define INTERFACE IUnknown

#include <xtl.h>
#include <rmxfguid.h>
#include <rmxftmpl.h>

