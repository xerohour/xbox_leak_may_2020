#ifndef _ALL_INCS_H
#define _ALL_INCS_H

#include "nv32.h"
//#include "nvhw.h"
//#include "nvrmwin.h"
//#include "class.h"
//#include "gr.h"
//#include "dma.h"
//#include "modular.h"
//#include "mp.h"
//#include "tmr.h"
//#include "fifo.h"
//#include "au.h"
//#include "vnvrmd.h"
//#include "oswin.h"
#include "nvtypes.h"
#include "nv_ref.h"
#include "nvromdat.h"
#include "class.h"
// #include "nv1a_ref.h"
// #include "nv1b_ref.h"
// #include "nv1c_ref.h"
// #include "nv2_ref.h"
// #include "nv3_ref.h"

//#include "nvhw.h"

#include "fifo.h"
#include "gr.h"
#include "modular.h"
#include "control.h"
#include "cursor.h"
#include "dac.h"
#include "dma.h"
#include "dmapush.h"
#include "fb.h"
#include "mc.h"
#include "mp.h"
#include "vid.h"
//#include "nvrm.h"
#include "os.h"
#include "state.h"
#include "tmr.h"
#include "tv.h"
#include "vblank.h"
#include "vga.h"
#include "btree.h"

#ifndef LEGACY_ARCH
#include "client.h"
#endif

#ifdef NEWWIN31
#include "client.h"
#endif

#include "arb_fifo.h"

#endif
