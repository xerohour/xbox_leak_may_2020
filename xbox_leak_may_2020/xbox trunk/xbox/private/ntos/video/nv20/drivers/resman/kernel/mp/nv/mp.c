#include <nv_ref.h>
#include <nvrm.h>
#include <class.h>
#include <mp.h>
#include "nvhw.h"

//
// Forwards.
//
V032 mpService(PHWINFO);

//
// mpService
//
// Field MediaPort exceptions by invoking class-dependent handlers.
//
V032 mpService
(
    PHWINFO pDev
)
{
    V032 intrStatus = 0;

    //
    // Decompressor.
    //
    (void) class04EService(pDev);

    //
    // Decoder.
    //
    (void) class04DService(pDev);

    //
    // Parallel bus.
    // This is the interrupt status value we'll return.
    //
    intrStatus = class04FService(pDev);

    return intrStatus;
}


