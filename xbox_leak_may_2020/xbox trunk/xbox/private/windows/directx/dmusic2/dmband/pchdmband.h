#ifdef XBOX
#include <xtl.h>
#include "..\shared\critsec.h"
#include "..\shared\xcreate.h"
#include "PChMap.h"
#include "dmksctrl.h"
#include "dmusicc.h"
#include "dmusicip.h"
#include "dmusicf.h"
#include "..\shared\dmstrm.h"
#include "..\shared\validate.h"
#include "dmbandp.h"
#include "bandtrk.h"
#include "debug.h"
#include "..\shared\xsoundp.h" // For GUID_All_Objects
#else
#include <objbase.h>
#include "..\shared\critsec.h"
#include "PChMap.h"
#include "dmksctrl.h"
#include "dmusicc.h"
#include "dmusici.h"
#include "dmusicf.h"
#include "..\shared\dmstrm.h"
#include "..\shared\validate.h"
#include "dmbandp.h"
#include "bandtrk.h"
#include "debug.h"
#include "..\shared\oledll.h"
#endif
