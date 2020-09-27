
#ifdef XBOX
//#include "dsoundb.h"
#include <xtl.h>
#include <xdbg.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
DEFINE_GUID(GUID_NULL, 0L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
#include <objbase.h> // Need IClassFactory
//#include <initguid.h>
#include <mmsystem.h>
#undef timeGetTime 
#define timeGetTime GetTickCount
#include "..\shared\dmusiccp.h"
#include "..\shared\xsoundp.h" // For GUID_All_Objects
extern "C" ULONG _cdecl DbgPrint(PCH Format, ...);
#include "..\shared\DoWork.h" 
extern CBossMan g_BossMan;
#else // XBOX
#include <windows.h>
#include <objbase.h> // Need IClassFactory
#include <mmsystem.h>
#endif // XBOX
#include "..\shared\xalloc.h"
#include <mmsystem.h>
#include <time.h>	// to seed random number generator
#include "cmixbins.h"
#include "urlmonhelper.h"
#include "..\shared\critsec.h"
#include "dmsegobj.h"
#include "dmime.h"
#include "song.h"
#include "dmgraph.h"
#include "dmprfdll.h"
#include "dmperf.h"
#include "dmusicip.h"
#include "dmusicf.h"
#include "..\shared\dmstrm.h"
#include "..\shared\validp.h"
#include "dls1.h"
#include "trackhelp.h"
#include "..\shared\Validate.h"
#include "audpath.h"
#include "debug.h"
#include "..\shared\dmusiccp.h"
#include "..\shared\xcreate.h"
#include "..\dmstyle\dmstylep.h"
#include "..\dswave\dswave.h"
#include <ks.h>
#include "dmksctrl.h"
#include "math.h"
#include "plclock.h"
#include "phoneyds.h"
#include "dmscriptautguids.h"
#include "..\shared\dmusiccp.h"
#ifdef XBOX
#include "..\shared\xcreate.h"
#endif
#include "wavtrack.h"
#include "dmperf.h"
#include "smartref.h"
using namespace SmartRef;
#include <xutility>
#include "miscutil.h"
#include "song.h"
#include "seqtrack.h"
#include "sysextrk.h"
#include "tempotrk.h"
#include "tsigtrk.h"
#include "marktrk.h"
#include "wavtrack.h"
#include "segtrtrk.h"
#include "lyrictrk.h"
#include "ParamTrk.h"
#include "dmgraph.h"
#include "..\shared\oledll.h"
#include "..\dmband\dmbndtrk.h"
#include "dmksctrl.h"
#include "dmscriptautguids.h"
#include "audpath.h"
#ifdef XBOX
#include "..\shared\xcreate.h"
#endif

#ifdef XBOX
#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
extern "C" void DMRegisterPMsgPerformanceCounters();
extern "C" void DMUnregisterPMsgPerformanceCounters();
extern "C" void DMRPC_DownloadBuffer();
extern "C" void DMURPC_DownloadBuffer();
#endif
#define NEWCATCH(obj,type) \
    obj = new type;
#else
#define NEWCATCH(obj,type)  \
	try \
    { \
        obj = new type; \
    } \
    catch( ... ) \
    { \
        hr = E_OUTOFMEMORY; \
        break; \
    }
#endif

#define SAMPLERATE  48000   // sample rate

