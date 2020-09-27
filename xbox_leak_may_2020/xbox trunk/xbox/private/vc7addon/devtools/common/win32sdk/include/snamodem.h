
/*****************************************************************************/
/* snamodem.h - include for the SNA Modem monitor interface.                 */
/* Copyright (c) Microsoft Corporation.  All rights reserved.                */
/*****************************************************************************/


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct _ModemStatus
{
  DWORD InUse;

  char LSName[12];  // Link Service Name
  char V24In;       // V.24 input flags
#define MASK_CTS 1
#define MASK_DSR 2
#define MASK_DCD 4
#define MASK_DRI 8

  char V24Out;      // V.24 output flags
#define MASK_RTS 1
#define MASK_DTR 2

  unsigned short RxFrameCount;
  unsigned short TxFrameCount;

  char Reserved[10]; // Padding / Expansion
} MODEM_STATUS;


extern void WINAPI SNAModemInitialize();
extern void WINAPI SNAModemAddLink(MODEM_STATUS**);
extern void WINAPI SNAModemDeleteLink(MODEM_STATUS*);
extern void WINAPI SNAModemTerminate();


#ifdef __cplusplus
}
#endif

