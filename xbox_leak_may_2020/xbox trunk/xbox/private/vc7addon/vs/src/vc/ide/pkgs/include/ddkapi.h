//
// DDKAPI.H
//    Debug package interface declarations.

#ifndef __DBGAPI_H__
#define __DBGAPI_H__

interface IDdkCmd;

typedef IDdkCmd *     LPDDKCMD;

//
// IDebugCmd interface
//
#undef  INTERFACE
#define INTERFACE IDdkCmd

DECLARE_INTERFACE_(IDdkCmd, IUnknown)
{
   // IUnknown methods
   STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
   STDMETHOD_(ULONG,AddRef)(THIS) PURE;
   STDMETHOD_(ULONG,Release)(THIS) PURE;
};

#endif   // __DDKAPI_H__
