#ifndef _CONTROL_H_
#define _CONTROL_H_
/**************************** Graphics Manager *****************************\
*                                                                           *
* Module: CONTROL.H                                                         *
*       Control method implementation.                                      *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Bruce Thompson (brucet)    02/22/96 - wrote it.                     *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  Defines.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
// Miscellaneous structure definitions.
//
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//  Control object defines.
//
//---------------------------------------------------------------------------
typedef struct _def_ctrlmethod CTRLMETHOD, *PCTRLMETHOD;
typedef struct _def_ctrlclass  CTRLCLASS,  *PCTRLCLASS;

// Control method structure
typedef RM_STATUS (*CTRLMETHODPROC)(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
struct _def_ctrlmethod
{
    CTRLMETHODPROC Proc;
    U032           Low;
    U032           High;
};

// Control class structure
struct _def_ctrlclass
{
    PCTRLMETHOD Method;
    U032        MethodMax;
};

// The class which defines the NV control area
extern CTRLCLASS NvControlClass;

//---------------------------------------------------------------------------
//
//  Function prototypes.
//
//---------------------------------------------------------------------------
RM_STATUS mthdSetObject(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);

#ifdef OBSOLETE_FUNCTIONS
RM_STATUS mthdSetStallNotifier(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
RM_STATUS mthdStallChannel(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
RM_STATUS mthdPush(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
RM_STATUS mthdPop(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
RM_STATUS mthdPasswd(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
RM_STATUS mthdDmaPusher(PHWINFO, U032, U032, PCTRLMETHOD, U032, U032);
#endif

#endif // _CONTROL_H_
