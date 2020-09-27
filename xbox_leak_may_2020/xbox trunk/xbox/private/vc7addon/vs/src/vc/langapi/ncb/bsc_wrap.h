/*
 * This file exists simply to fake the type of "ATR"; although ATR is
 * typedef'd in <bsc.h> as a USHORT, NCB needs to pretend that it is a
 * DWORD as more bits are needed to support MC++. This lets us get
 * away with not affecting current Bsc code. However, a client's
 * ability to transparently access NCB and BSC stores via the same
 * interface is lost. Accessing NCB stores via any member function
 * that takes an ATR* as an argument is likely to trash the caller's
 * stack frame or cause an access violation.
 */

#ifndef EXTENDED_ATTRIBUTES_HACK
#define EXTENDED_ATTRIBUTES_HACK /* causes <bsc.h> to typedef ATR as a
									DWORD rather than USHORT */		
#endif
#include <bsc.h>
