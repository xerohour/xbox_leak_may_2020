// FILE:      library\hardware\mpeg2enc\specific\ivacprop.h
// AUTHOR:    Martin Stephan
// COPYRIGHT: (c) 1999 Viona Development GmbH. All Rights Reserved.
// CREATED:   19.10.1999
//
// PURPOSE:   Properties for the iVac Encoder Chip
//
// HISTORY:	  taken from the iCompression iVac WDM driver sources -> iVacCtrlProp.h


// ------------------------------------------------------------------------
// Property set for iVacControl
// ------------------------------------------------------------------------

#define STATIC_PROPSETID_IVAC_CONTROL\
    0xee6c0342, 0xe3a9, 0x11d2, 0xa5, 0x3e, 0x0, 0xa0, 0xc9, 0xef, 0x50, 0x6a
DEFINE_GUIDSTRUCT("0xee6c0342-0xe3a9-0x11d2-a53e-00a0c9ef506a", PROPSETID_IVAC_CONTROL);
#define PROPSETID_IVAC_CONTROL DEFINE_GUIDNAMED(PROPSETID_IVAC_CONTROL)

#define STATIC_PROPSETID_IVAC_PROPERTIES\
    0xd2185a40, 0x0398, 0x11d3, 0xa5, 0x3e, 0x0, 0xa0, 0xc9, 0xef, 0x50, 0x6a
DEFINE_GUIDSTRUCT("0xd2185a40-0x0398-0x11d3-a53e-00a0c9ef506a", PROPSETID_IVAC_PROPERTIES);
#define PROPSETID_IVAC_PROPERTIES DEFINE_GUIDNAMED(PROPSETID_IVAC_PROPERTIES)

typedef enum 
	{
	VIVACE_GET_IRQ_VAL,			// Get only
	VIVACE_GET_SDSMEM_VAL,		// Get only
	VIDACE_GET_REGMEM_VAL,		// Get only
   VIVACE_READ_REG_VAL,			// Get only
   VIVACE_WRITE_REG_VAL,		// Set only
	VIVACE_READ_MEM_VAL,			// Get only
	VIVACE_WRITE_MEM_VAL,		// Set only
	VIVACE_START_CAPTURE,		// Set only
	VIVACE_STOP_CAPTURE,			// Set only
	VIVACE_PREPARE_GET			// Set only : Set some values needed for the next Gt
	} KSPROPERTY_IVAC_CONTROL;

typedef struct
	{
   DWORD		dwAddress;	// Address offset of the memory area we want to read
	DWORD		dwCount;		// Number of bytes we want to read (have to be smaller than 4096
	BOOL		bReady;		// Are we ready to receive a Get command
	} PREPARE_GET;

typedef struct 
	{
   WORD wIrq;
	} KSPROPERTY_IVACCONTROL_GET_IRQ, *PKSPROPERTY_IVACCONTROL_GET_IRQ;

typedef struct 
	{
   DWORD dwSdMem;
	} KSPROPERTY_IVACCONTROL_GET_SDMEM, *PKSPROPERTY_IVACCONTROL_GET_SDMEM;

typedef struct 
	{
   DWORD dwRegMem;
	} KSPROPERTY_IVACCONTROL_GET_REGMEM, *PKSPROPERTY_IVACCONTROL_GET_REGMEM;

typedef struct 
	{
   DWORD		dwAddress; // In:  Address offset of the register we want to read
	DWORD		dwValue;   // Out: Value of the register that we read
	} KSPROPERTY_IVACCONTROL_READ_REG, *PKSPROPERTY_IVACCONTROL_READ_REG;

typedef struct 
	{
   DWORD		dwAddress; // In:  Address offset of the register where we want to write
	DWORD		dwValue;   // Out: Value we want to write in that register
	} KSPROPERTY_IVACCONTROL_WRITE_REG, *PKSPROPERTY_IVACCONTROL_WRITE_REG;

typedef struct 
	{
   DWORD		dwAddress; // In:  Address offset of the memory area we want to read
	DWORD		dwCount;   // In: Number of bytes we want to read (have to be smaller than 4096
	PBYTE		pOutBuffer;// Out: Read buffer (the first dwCount bytes are filled up)
	} KSPROPERTY_IVACCONTROL_READ_MEM, *PKSPROPERTY_IVACCONTROL_READ_MEM;

typedef struct 
	{
   DWORD		dwAddress; // In:  Address offset of the memory area we want to read
	DWORD		dwCount;   // In: Number of bytes we want to read (have to be smaller than 4096
	PBYTE		pInBuffer; // In: Buffer containing dwCount bytes of data to write
	} KSPROPERTY_IVACCONTROL_WRITE_MEM, *PKSPROPERTY_IVACCONTROL_WRITE_MEM;

typedef struct 
	{
	OUTPUT_TYPE eOutputType;		// In: Type of the output (hardcoded to program for now)
	} KSPROPERTY_IVACCONTROL_START_CAPTURE, *PKSPROPERTY_IVACCONTROL_START_CAPTURE;

typedef struct 
	{
	OUTPUT_TYPE eOutputType;		// In: Type of the output (hardcoded to program for now)
	} KSPROPERTY_IVACCONTROL_STOP_CAPTURE, *PKSPROPERTY_IVACCONTROL_STOP_CAPTURE;


