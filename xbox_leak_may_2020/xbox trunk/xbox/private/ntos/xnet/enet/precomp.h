#include "xnetp.h"
#include "ethernet.h"
#include "phy.h"

#ifdef SILVER
#include "i82558.h"
#else
#include "xnic.h"
#endif

#include "nicp.h"
#include "ifenet.h"
#include "dbgmon.h"
