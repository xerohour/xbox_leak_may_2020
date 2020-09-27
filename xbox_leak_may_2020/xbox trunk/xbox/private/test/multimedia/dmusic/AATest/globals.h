#include <xtl.h>
#include <xdbg.h>
#include <dmusici.h>
#include <dsound.h>
#include <DSSTDFX.h>
#include <macros.h>
#include <medialoader.h>
#include <stdio.h>

#define TERMINATE NULL,NULL,NULL

HRESULT MyFactory(REFCLSID clsid,
                  LPUNKNOWN pUnkOuter,
                  REFIID iid,
                  LPVOID *ppvInterface);
