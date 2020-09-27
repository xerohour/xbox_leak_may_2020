/*++

Copyright (c) 1989-2000  Microsoft Corporation

Module Name:

    romdata.h

Abstract:

    Header file for XBox boot loader (contains datatbl format)


--*/

#ifndef _DATATBL_H
#define _DATATBL_H

//
// Data struct contains all of the drive/slew parameters.
//
typedef struct _drv_slw_cal_params {
    USHORT MaxMClk;

    UCHAR SlowCountExt;
    UCHAR SlowCountBExt;
    UCHAR SlowCountAvg;
    UCHAR SlowCountBAvg;
    UCHAR TypiCount;
    UCHAR TypiCountB;
    UCHAR FastCountAvg;
    UCHAR FastCountBAvg;
    UCHAR FastCountExt;
    UCHAR FastCountBExt;
} DRVSLWCALPARAMS, *PDRVSLWCALPARAMS;

//
// Data structure contains the drive/slew per-speed process parameters.
//
typedef struct _drv_slw_pad_params {
    UCHAR AdrDrvFall;
    UCHAR AdrDrvRise;
    UCHAR AdrSlwFall;
    UCHAR AdrSlwRise;

    UCHAR ClkDrvFall;
    UCHAR ClkDrvRise;
    UCHAR ClkSlwFall;
    UCHAR ClkSlwRise;

    UCHAR DatDrvFall;
    UCHAR DatDrvRise;
    UCHAR DatSlwFall;
    UCHAR DatSlwRise;

    UCHAR DqsDrvFall;
    UCHAR DqsDrvRise;
    UCHAR DqsSlwFall;
    UCHAR DqsSlwRise;

    UCHAR DataInbDely;
    UCHAR ClkIcDely;
    UCHAR DqsInbDely;
} DRVSLWPADPARAMS, *PDRVSLWPADPARAMS;


enum {
    DrvSlwExtFast = 0,
    DrvSlwAvgFast,
    DrvSlwTypical,
    DrvSlwAvgSlow,
    DrvSlwExtSlow,
    DrvSlwTotal,
};


//
// Complete layout of the ROM data table
//
typedef struct _rom_datatbl {
    DRVSLWCALPARAMS calParams;
    DRVSLWPADPARAMS SamsungParams[DrvSlwTotal];
    DRVSLWPADPARAMS MicronParams[DrvSlwTotal];
} ROM_DATATBL, *PROM_DATATBL;

#endif // _DATATBL_H
