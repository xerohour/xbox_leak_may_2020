//*****************************Module*Header******************************
//
// Module Name: eewinman2.h
//
// WINman display propterty page interface version 2
// for board detection, mode set,...
//
// FNicklisch 09/14/2000: 
//
// This part of code was taken from the code bought from ELSA. Parts of 
// it is used to get the workstation tools, application and features up
// and running. It is implemented on a temporary base and will be replaced
// by a NVIDIA propritary interface as soon as possible.
// Don't care about the code not necessariliy used.
//
// Copyright (c) 1998-2000, ELSA AG, Aachen (Germany)
// Copyright (c) 2000 NVidia Corporation. All Rights Reserved.
//
#ifndef _EeWINma2_h_
#define _EeWINma2_h_


/*
** $Header$
**
** MODULE:   EeWINma2.h
**
** AUTHOR:   Torsten Horn, Copyright (c) 1999 ELSA AG, Aachen (Germany)
**
** PROJECT:  ELSA Graphics Software
**
** PURPOSE:  ELSA Escape Windows driver functions for WINman
**
** NOTES:    1998-10-01 THorn: First release.
**           1999-05-04 THorn: Only comment extended/modified.
**           1999-08-03 THorn: New WINMAN_MOD_DFP and comments extended.
**           1999-08-18 THorn: New WINMAN_TIM_... flags.
**           1999-11-12 THorn: ET_WINMAN2_DDC_EDID_GET extended for EDID2.
*/


/*-----------------------------------------------------------------
  For the principle architecture of this kind of ELSA escape functions see 
  the header elsa\ElsaEsc.h (which must be included before this header).

  This header EeWINma2.h defines the escape functions:
    SUBESC_ET_WINMAN2_DATA_GET
    SUBESC_ET_WINMAN2_DATA_SET
    SUBESC_ET_WINMAN2_DDC_EDID_GET
  
  Simplyfied example for getting data from the driver (in case of 16 boards):
    ET_QUERY.dwSubEsc  = SUBESC_ET_WINMAN2_DATA_GET;
    ET_QUERY.dwSize    = sizeof(ET_QUERY);
    ET_QUERY.dwOutSize = sizeof(ET_WINMAN2_DATA_GET) +
                         sizeof(ET_WINMAN2_DATA_BRD_R) * 15;  // when 16 boards
    ExtEscape( hDC, ESC_ELSA_TOOLS,
               ET_QUERY.dwSize,    (LPCSTR)&ET_QUERY,
               ET_QUERY.dwOutSize,  (LPSTR)&ET_WINMAN2_DATA_GET );

  Simplyfied example for setting data to the driver (in case of 16 boards):
    ET_WINMAN2_DATA_SET.hdr.dwSubEsc  = SUBESC_ET_WINMAN2_DATA_SET;
    ET_WINMAN2_DATA_SET.hdr.dwSize    = sizeof(ET_WINMAN2_DATA_SET);
    ET_WINMAN2_DATA_SET.hdr.dwOutSize = sizeof(ET_WINMAN2_DATA_GET) +
                                        sizeof(ET_WINMAN2_DATA_BRD_R) * 15;
    ExtEscape( hDC, ESC_ELSA_TOOLS,
               ET_WINMAN2_DATA_SET.hdr.dwSize,    (LPCSTR)&ET_WINMAN2_DATA_SET,
               ET_WINMAN2_DATA_SET.hdr.dwOutSize,  (LPSTR)&ET_WINMAN2_DATA_GET );

  dwOutSize (in ET_QUERY and ET_WINMAN2_DATA_SET.hdr) must match
    sizeof(ET_WINMAN2_DATA_GET) + sizeof(ET_WINMAN2_DATA_BRD_R) * n;
  where n usually is equal to
    ET_WINMAN2_DATA_GET.dwNumBoardsInstalled - 1
  but n may be less eg. with WINMAN_SET_CHECK, see below.

  The first call usually is a SUBESC_ET_WINMAN2_DATA_GET with a return 
  structure size (dwOutSize) fitting for only one board. If
    ExtEscape() > 0  &&
    lResult == WINMAN_RESULT_ERROR_SIZE  &&
    dwNumBoardsInstalled > 1,
  then the next SUBESC_ET_WINMAN2_DATA_GET call has the correct 
  structure size in dwOutSize for getting the data of all boards.

  SUBESC_ET_WINMAN2_DATA_SET may be called for less than all boards 
  (especially with WINMAN_SET_CHECK). In this case the return structure 
  size (dwOutSize) may fit to this reduced number of boards (what should 
  not produce an error code).
  SUBESC_ET_WINMAN2_DATA_SET must always return either with 
  WINMAN_RESULT_ERROR or with valid and complete data in ET_WINMAN2_DATA_GET.

  You must carefully distinguish between the different return values:
    ExtEscape() must return:
      - zero if the escape function or the subescape is not implemented,
      - less than zero if an error in the escape interface occures (may 
        be ET_QUERY.dwSize is too small),
      - greater than zero if the escape interface is successful, even 
        when another error occures. Other error codes (for problems with 
        the data) are returned in lResult.
    ET_WINMAN2_DATA_GET.lResult must return the fitting WINMAN_RESULT_...
      When the size of the output structure was too small for all needed 
      boards, you must return WINMAN_RESULT_ERROR_SIZE.
      When all data send to the driver are ok, but eg. only the pixel 
      clock (may be of only one board) is too high, you either must return 
      WINMAN_RESULT_ERROR, or better would be, you would return 
      WINMAN_RESULT_MODIFIED and reduce the pixel clock to the max. allowed 
      value.
      With WINMAN_RESULT_ERROR the returned ET_WINMAN2_DATA_GET structure 
      may have invalid data. With WINMAN_RESULT_OK or WINMAN_RESULT_MODIFIED 
      the returned ET_WINMAN2_DATA_GET data must be o.k. and complete for 
      both kind of calls (SUBESC_ET_WINMAN2_DATA_GET and 
      SUBESC_ET_WINMAN2_DATA_SET).
    ET_WINMAN2_DATA_BRD_RW.dwFlags has flags for each board.
      With WINMAN_BRD_MODIFIED you can decide, for which board the data 
      were modified.
    ET_WINMAN2_DATA_GET.dwDrvModFlags returns with the WINMAN_MOD_... flag 
      bits some modification possibilities/capabilities (common for all 
      boards).

  -----------------------------------------------------------------*/


/* #include <elsa\ElsaEsc.h> */


/* Defines for driver result value in lResult
   (used eg. in ET_WINMAN2_DATA_GET and ET_WINMAN2_DDC_EDID_GET). */
#define WINMAN_RESULT_OK          0  /* o.k., no modification */
#define WINMAN_RESULT_ERROR      -1  /* error (returned data invalid) */
#define WINMAN_RESULT_ERROR_SIZE -2  /* error: dwOutSize wrong, 
                                        see comment to dwOutSize in header.
                                        Only the following members of 
                                        ET_WINMAN2_DATA_GET are guaranted to 
                                        be valid: dwSize, lResult, 
                                        dwNumBoardsInstalled, dwNumBoardsActive;
                                        other returned data may be invalid */
#define WINMAN_RESULT_MODIFIED    1  /* data modified;
                                        all data are ok, but only eg. the 
                                        pixel clock was too high; pixel 
                                        clock is reduced to max. allowed 
                                        value */

/* Defines for modification possibilities/capabilities flags in dwDrvModFlags
   (used in ET_WINMAN2_DATA_GET). */
#define WINMAN_MOD_BPP            1  /* bpp changable */
#define WINMAN_MOD_HZ             2  /* new Hz values definable */
#define WINMAN_MOD_TIM            4  /* complete timing definable */
#define WINMAN_MOD_PAN_HOR        8  /* horizontal panning possible */
#define WINMAN_MOD_PAN_VER       16  /* vertical panning possible */
#define WINMAN_MOD_MULTI_HOR     32  /* multiscreen hor. possible */
#define WINMAN_MOD_MULTI_VER     64  /* multiscreen ver. possible */
#define WINMAN_MOD_MULTI_H_V    128  /* multiscreen hor.+ver. both */
#define WINMAN_MOD_RES          256  /* new resolution values definable */
#define WINMAN_MOD_DFP          512  /* DFP Digital Flat Panel (different 
                                        resolutions with one fix timing) */

/* Defines for some board flags in dwFlags
   (used in ET_WINMAN2_DATA_BRD_RW). */
#define WINMAN_BRD_MODIFIED       1  /* data of this board modified */

/* Defines for some timing flags (sync polarity, interlace) in dwTimFlags
   (used in ET_WINMAN2_DATA_BRD_RW). */
#define WINMAN_TIM_HSYNC_POS      8  /* same as GFX_HSYNC_POS */
#define WINMAN_TIM_VSYNC_POS      4  /* same as GFX_VSYNC_POS */
#define WINMAN_TIM_ILC            1  /* same as GFX_ILC */
#define WINMAN_TIM_FIXED_TIMING  16  /* same as GFX_FIXED_TIMING */

/* Defines for action in dwCheckAddRemove
   (used in ET_WINMAN2_DATA_SET). */
#define WINMAN_SET_CHECK          1  /* check (may be modify) this mode */
#define WINMAN_SET_ADD            2  /* add this mode to allowed modes */
#define WINMAN_SET_REMOVE         3  /* remove this mode */
/* The following two are not used till now (for future use): */
#define WINMAN_SET_DO_TEST        4  /* switch only for test screen */
#define WINMAN_SET_DO_PERM        5  /* switch graphics mode permanent */


/* Read/write data of each board
   (used in ET_WINMAN2_DATA_GET and ..._DATA_SET). */
typedef struct tagET_WINMAN2_DATA_BRD_RW
{
  /* These values must always be valid: */
  DWORD dwPanOriginX;            /* panning x-origin */
  DWORD dwPanOriginY;            /* panning y-origin */
  DWORD dwVisOriginX;            /* visible x-origin */
  DWORD dwVisOriginY;            /* visible y-origin */
  DWORD dwPanSizeX;              /* panning x-size */
  DWORD dwPanSizeY;              /* panning y-size */
  DWORD dwVisSizeX;              /* visible x-size */
  DWORD dwVisSizeY;              /* visible y-size */
  DWORD dwBpp;                   /* bpp */
  DWORD dwColorsNum;             /* num. of colors */
  DWORD dwHz;                    /* Hz refresh rate */
  /* These values must be zero if not used: */
  DWORD dwFlags;                 /* flag bits, eg. WINMAN_BRD_MODIFIED */
  DWORD dwTimPixFreq;            /* pixel clock frequency in Hz */
  DWORD dwTimHTot;               /* horizontal total number of pixels */
  DWORD dwTimHFp;                /* horizontal frontporch in pixels */
  DWORD dwTimHSw;                /* horizontal sync width in pixels */
  DWORD dwTimHBp;                /* horizontal backporch in pixels */
  DWORD dwTimVTot;               /* vertical total number of lines */
  DWORD dwTimVFp;                /* vertical frontporch in lines */
  DWORD dwTimVSw;                /* vertical sync width in lines */
  DWORD dwTimVBp;                /* vertical backporch in lines */
  DWORD dwTimFlags;              /* tim. flag bits: WINMAN_TIM_... */
  DWORD dwReserved1;             /* == 0 (reserved) */
  DWORD dwReserved2;             /* == 0 (reserved) */
} ET_WINMAN2_DATA_BRD_RW, *PET_WINMAN2_DATA_BRD_RW;


/* Read-only data of each board
   (used in ET_WINMAN2_DATA_GET). */
typedef struct tagET_WINMAN2_DATA_BRD_R
{
  /* Board data used in ET_WINMAN2_DATA_GET and ..._DATA_SET: */
  ET_WINMAN2_DATA_BRD_RW rw;     /* read/write data */
  /* These values must be zero if not used: */
  DWORD dwPixFreqMax08;          /* max. possible pixel clock (in Hz) at  8bpp */
  DWORD dwPixFreqMax16;          /* max. possible pixel clock (in Hz) at 16bpp */
  DWORD dwPixFreqMax24;          /* max. possible pixel clock (in Hz) at 24bpp */
  DWORD dwPixFreqMax32;          /* max. possible pixel clock (in Hz) at 32bpp */
  DWORD dwMemSizePan;            /* RAM usable for visible and virtual screen */
  DWORD dwMemSizeAll;            /* size of all RAM (e.g. VRAM + DRAM) */
  DWORD dwReserved1;             /* == 0 (reserved) */
  DWORD dwReserved2;             /* == 0 (reserved) */
  char  sBoardname[40];          /* board name */
  char  sSerNum[24];             /* serial number */
} ET_WINMAN2_DATA_BRD_R, *PET_WINMAN2_DATA_BRD_R;


/* Data WINman gets from the Windows driver. */
typedef struct tagET_WINMAN2_DATA_GET
{
  DWORD dwSize;                  /* sizeof this struct with correct num of brd */
  LONG  lResult;                 /* WINMAN_RESULT_OK, ...ERROR..., ...MODIFIED */
  DWORD dwDrvModFlags;           /* driver modify flags WINMAN_MOD_... */
  DWORD dwReserved;              /* == 0 (reserved) */
  DWORD dwNumBoardsInstalled;    /* number of all installed boards */
  DWORD dwNumBoardsActive;       /* number of active used boards */
  /* The following struct as often as dwNumBoardsActive: */
  ET_WINMAN2_DATA_BRD_R brd[1];  /* data of all boards */
} ET_WINMAN2_DATA_GET, *PET_WINMAN2_DATA_GET;


/* Data WINman writes to the Windows driver. */
typedef struct tagET_WINMAN2_DATA_SET
{
  ET_QUERY hdr;
  DWORD dwCheckAddRemove;        /* WINMAN_SET_CHECK, ..._ADD, ..._DO_... */
  DWORD dwReserved1;             /* == 0 (reserved) */
  DWORD dwReserved2;             /* == 0 (reserved) */
  DWORD dwSizeX;                 /* x-size of desktop (over all monitors) */
  DWORD dwSizeY;                 /* y-size of desktop (over all monitors) */
  DWORD dwNumBoardsActive;       /* number of active used boards */
  /* The following struct as often as dwNumBoardsActive: */
  ET_WINMAN2_DATA_BRD_RW brd[1]; /* data of all boards */
} ET_WINMAN2_DATA_SET, *PET_WINMAN2_DATA_SET;


/*-----------------------------------------------------------------
  SUBESC_ET_WINMAN2_DDC_EDID_GET
  
  Example (m=0 for EDID1 and m=1 for EDID2):
    ET_QUERY.dwSubEsc  = SUBESC_ET_WINMAN2_DDC_EDID_GET;
    ET_QUERY.dwSize    = sizeof(ET_QUERY);
    ET_QUERY.dwOutSize = sizeof(ET_WINMAN2_DDC_EDID_GET) + m*128;
    ET_QUERY.dwBoard   = n;
    ExtEscape( hDC, ESC_ELSA_TOOLS,
               sizeof(  ET_QUERY ),
               (LPCSTR)&ET_QUERY,
               sizeof(  ET_WINMAN2_DDC_EDID_GET ) + m*128,
               (LPSTR) &ET_WINMAN2_DDC_EDID_GET );
  -----------------------------------------------------------------*/


/* DDC EDID structure WINman gets from the Windows driver.
   This structure  is for EDID 1.x with 128 bytes 
   of data as well as for EDID 2.x with 256 bytes.
   When WINman querries with wrong ET_QUERY.dwOutSize value, 
   the driver has to return WINMAN_RESULT_ERROR_SIZE. 
   Then WINman must try with another ET_QUERY.dwOutSize. */
typedef struct tagET_WINMAN2_DDC_EDID_GET
{
  DWORD dwSize;                   /* sizeof(ET_WINMAN2_DDC_EDID_GET)+m*128 */
  LONG  lResult;                  /* WINMAN_RESULT_OK, WINMAN_RESULT_ERROR... */
  unsigned char ddcEdid[128][1];  /* VESA DDC EDID structure */
} ET_WINMAN2_DDC_EDID_GET, *PET_WINMAN2_DDC_EDID_GET;


#endif  // #ifndef _EeWINma2_h_
