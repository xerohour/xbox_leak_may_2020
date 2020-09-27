/*
 * (c) Copyright 1999-2000 Intel Corp. All rights reserved.
 * Intel Confidential.
 */

#if !defined(_DMIFUNC_H_)
#define _DMIFUNC_H_

#if defined(__cplusplus)
extern "C" {
#endif

extern WORD _F000h;
extern WORD _0040h;

#define NO_MORE_HANDLES 0xFFFF

typedef enum tagMSGTYPE {
	MT_STATUS,
	MT_ADVISORY,
	MT_ERROR
} MSGTYPE;


/*
 @ TYPE DEFINITIONS and MACROS
 */

typedef int (__cdecl *LPPNPENTRY)(int,...);

#pragma pack(1)
#pragma warning(disable:4214 4201)

typedef	struct	tagPNPHEADER
{
	BYTE	baSignature[4];
	BYTE	bVersion;
	BYTE	bLength;
	WORD	wCtrlField;
	BYTE	bChecksum;
	DWORD	dwEventNotifyFlagAddr;
	WORD	wRealOffset;
	WORD	wRealBaseAddr;
	WORD	wProtOffset;
	DWORD	dwProtBaseAddr;
	DWORD	dwOEMDeviceID;
	WORD	wRealDataAddr;
	DWORD	dwProtDataAddr;
} PNPHEADER,*LPPNPHEADER;

typedef struct tagDMIHEADER
{
	BYTE	bType;
	BYTE	bLength;
	WORD	wHandle;
} DMIHEADER,*LPDMIHEADER;

typedef struct tagDMIDATABUFFER
{
	BYTE		bCommand;
	BYTE		bFieldOffset;
	DWORD		dwChangeMask;
	DWORD		dwChangeValue;
	WORD		wDataLength;
	DMIHEADER	stStructHeader;
	char		baUpdatedString[128];
} DMIDATABUFFER,*LPDMIDATABUFFER;

typedef struct tagMFGACCESS
{
	void far	*pWorkBuffer;
	void far	*pKey;
} MFGACCESS,*LPMFGACCESS;

typedef struct tagWRITEONCE
{
	BYTE		bType;
	WORD		wHandle;
	void far	*pWorkBuffer;
	BYTE		bElementOffset;
	char		baSecurity[9];
} WRITEONCE,*LPWRITEONCE;

typedef struct tagSWBRDACCESS
{
	BYTE		bCommand;
	BYTE		bRevision;
	DWORD		dwData;
	void far	*pWorkBuffer;
} SWBRDACCESS,*LPSWBRDACCESS;

#define SBA_READ  0x00
#define SBA_WRITE 0x01

typedef struct tagQWORD
{
	DWORD	Low;
	DWORD	High;
} QWORD,*LPQWORD;


/* BIOS Information (Type 0) */
typedef	struct tagDMITYPE0
{
	BYTE	bType;			/* 0 - BIOS Information Indicator */
	BYTE	bLength;		/* 12h + number of BIOS Characteristics Extension Bytes */
	WORD	wHandle;
	BYTE	bVendor;		/* String number of BIOS Vendor's Name */
	BYTE	bVersion;		/* String number of BIOS Version */
	WORD	wStartAddrSeg;	/* Segment location of BIOS starting address */
	BYTE	bReleaseDate;	/* String number of BIOS release date, in format mm/dd/yy */
	BYTE	bROMSize;		/* BIOS ROM Size - Size (n) is 64K * (n+1) */
	QWORD	qFunctions;		/* Defines which functions the BIOS supports - See 3.2.1.1 */
} DMITYPE0, *PDMITYPE0;

/* System Information (Type 1) */
typedef	struct tagDMITYPE1
{
	BYTE	bType;			/* 1 - Component ID Information Indicator */
	BYTE	bLength;		/* 08h */
	WORD	wHandle;
	BYTE	bManu;			/* Manufacturer - Number of null terminated string */
	BYTE	bProduct;		/* Product - Number of null terminated string */
	BYTE	bVersion;		/* Version - Number of null terminated string */
	BYTE	bSerialNum;		/* Serial Number - Number of null terminated string */
	BYTE    bUUID[16];		/* UUID */
	BYTE	bWakeupType;	/* Enumerated:
							   00h - Reserved
							   01h - Other
							   02h - Unknown
							   03h - APM Timer
							   04h - Modem Ring
							   05h - LAN Remote */
} DMITYPE1, *PDMITYPE1;

/* Base Board Information (Type 2) */
typedef	struct tagDMITYPE2
{
	BYTE	bType;			/* 2 - Base Board Information Indicator */
	BYTE	bLength;		/* 08h */
	WORD	wHandle;
	BYTE	bManu;			/* Manufacturer - Number of null terminated string */
	BYTE	bProduct;		/* Product - Number of null terminated string */
	BYTE	bVersion;		/* Version - Number of null terminated string */
	BYTE	bSerialNum;		/* Serial Number - Number of null terminated string */
} DMITYPE2, *PDMITYPE2;

/* System Enclosure or Chassis (Type 3) */
typedef	struct tagDMITYPE3
{
	BYTE	bType;			/* 3 - System Enclosure Indicator */
	BYTE	bLength;		/* 09h */
	WORD	wHandle;
	BYTE	bManu;			/* Manufacturer - Number of null terminated string */
	BYTE	bType1;			/* Bit 7 - Chassis Lock
								1 - Chassis Lock present
								0 - Either lock not present or unknown
							   Bits 6:0 - Chassis Types - See 3.2.4.1 */
	BYTE	bVersion;		/* Version - Number of null terminated string */
	BYTE	bSerialNum;		/* Serial Number - Number of null terminated string */
	BYTE	bTagNum;		/* Asset Tag Number - Number of null terminated string */
	BYTE	bBootupState;	/* bootup state - Enumerated Value */
	BYTE	bPowerSupplyState; /* power supply state - enumerated */
	BYTE	bThermalState;	/* thermal state - enumerated */
	BYTE	bSecurityStatus;/* security status - enumerated */
	DWORD	dwOEMinfo;		/* oem defined */
} DMITYPE3, *PDMITYPE3;

/* Processor Information (Type 4) */
typedef struct tagDMITYPE4
{
	BYTE	bType;			/* 4 - Processor Information Indicator */
	BYTE	bLength;		/* 1Ah */
	WORD	wHandle;
	BYTE	bSocketDesign;	/* String number for Reference Designation */
	BYTE	bProcType;		/* Processor Type - See 3.2.5.1 */
	BYTE	bProcFamily;	/* Processor Family - See 3.2.5.2 */
	BYTE	bProcManu;		/* String number of Processor Manufacturer */
	QWORD	qProcID;		/* Raw processor indentification data - See 3.2.5.3 */
	BYTE	bProcVersion;	/* String number describing the processor */
	BYTE	bVol;			/* Bits 7:4 - Reserved, must be zero
							   Bits 3:0 - Voltage capability
								Bit 0 - 5.0V
								Bit 1 - 3.3V
								Bit 2 - 2.9V
								Bit 3 - Reserved, must be 0
								Note: Setting of multiple bits indicates
								the socket is configurable */
	WORD	wExtClock;		/* External Clock Frequency */
	WORD	wMaxSpeed;		/* Maximum Speed, in MHz */
	WORD	wCurSpeed;		/* Same as Max Speed */
	BYTE	bStatus;		/* Bit 7 - Reserved, must be 0
							   Bit 6 - CPU Socket Populated
								1 - CPU Socket Populated
								0 - CPU Socket Unpopulated
							   Bits 5:3 - Reserved, must be 0
							   Bits 2:0 - CPU Status
								0h   - Unknown
								1h   - CPU Enabled
								2h   - CPU Disabled by user (via BIOS SETUP)
								3h   - CPU Disabled by System BIOS (POST Error)
								4h   - CPU is idle (waiting to be Enabled)
								5-6h - Reserved
								7h   - Other */
	BYTE	bProcUpgrade;	/* Processor Upgrade - See 3.2.5.4 */
} DMITYPE4,*PDMITYPE4;

/* Memory Controller Information (Type 5) */
typedef	struct tagDMITYPE5
{
	BYTE	bType;			/* 5 - Memory Controller Indicator */
	BYTE	bLength;		/* 15 + (2 * Number of Assoc. Mem. Slots), offset 0Eh */
	WORD	wHandle;
	BYTE	bErrDetectM;	/* Error Detecting Method - See 3.2.6.1 */
	BYTE	bErrCorrect;	/* Error Correcting Capability - See 3.2.6.2 */
	BYTE	bSupInter;		/* Supported Interleave - See 3.2.6.3 */
	BYTE	bCurInter;		/* Current Interleave - See 3.2.6.3 */
	BYTE	bMaxMemMod;		/* Size of largest memory module supported
							   (per slot), specified as n,
							   where 2**n is maximum size in MB */
	WORD	wSupSpeed;		/* Supported Speeds - See 3.2.6.4 */
	WORD	wSupMemType;	/* Supported Memory Types - See 3.2.7.1 */
	BYTE	bMemModVol;		/* Bits 7:3 - Reserved, must be 0
							   Bit 2 - 2.9V
							   Bit 1 - 3.3V
							   Bit 0 - 5V */
	BYTE	bNumMemSlots;	/* Number of Memory Module Information blocks
							   controlled by this controller */
} DMITYPE5,*PDMITYPE5;

/* Memory Module Information (Type 6) */
typedef	struct tagDMITYPE6
{
	BYTE	bType;			/* 6 - Memory Module Configuration Indicator */
	BYTE	bLength;		/* 0Ch */
	WORD	wHandle;
	BYTE	bSocketDesign;	/* String Number for Reference Designation */
	BYTE	bBankConnect;	/* Each nibble indicates a bank connection */
	BYTE	bCurSpeed;		/* Speed of the memory module, in ns */
	WORD	wCurMemType;	/* Current Memory Type - See 3.2.7.1 */
	BYTE	bInstallSize;	/* Installed Size - See 3.2.7.2 */
	BYTE	bEnSize;		/* Enabled Size -  See 3.2.7.2 */
	BYTE	bErrStatus;		/* Bits 7:2 - Reserved, set to 0's
							   Bit 1 - Correctable errors, if set
							   Bit 0 - Uncorrectable errors, if set */
} DMITYPE6, *PDMITYPE6;

/* Cache Information (Type 7) */
typedef	struct tagDMITYPE7
{
	BYTE	bType;			/* 7 - Cache Information Indicator */
	BYTE	bLength;		/* 0Fh */
	WORD	wHandle;
	BYTE	bSocketDesign;	/* String Number for Reference Designation */
	union
	{
		WORD	wCacheConfig;
		struct
		{
			WORD Level:		3;	/* Bits 2:0 - Cache Level - 1 through 8 */
			WORD Socketed:	1;	/* Bit 3 - Cache Socketed
									1b - Socketed
									0b - Not Socketed */
			WORD Reserved1:	1;	/* Bit 4 - Reserved, must be 0 */
			WORD Location:	2;	/* Bits 6:5 - Location, relative to the CPU module
									00b - Internal
									01b - External
									10b - Reserved
									11b - Unknown */
			WORD Enabled:	1;	/* Bit 7 - Enabled/Disabled (at boot time)
									1b - Enabled
									0b - Disabled */
			WORD Mode:		2;	/* Bits 9:8 - Operational Mode
									00b - Write Through
									01b - Write Back
									10b - Varies with Memory Address
									11b - Unknown */
			WORD Reserved2:	6;	/* Bits 15:10 - Reserved, must be 0 */
		} CacheConfig;
	};
	union
	{
		WORD	wMaxCacheSize;	/* Max size that can be installed */
		struct
		{
			WORD Size:15;		/* Bits 14:0 - Size in given granularity */
			WORD Granularity:1;	/* Bit 15 - Granularity
									0 - 1K granularity
									1 - 64K granularity */
		} MaxCacheSize;
	};
	union
	{
		WORD	wInstallSize;	/* Same as Max Cache Size field */
		struct
		{
			WORD Size:15;		/* Bits 14:0 - Size in given granularity */
			WORD Granularity:1;	/* Bit 15 - Granularity
									0 - 1K granularity
									1 - 64K granularity */
		} InstallSize;
	};
	union
	{
		WORD	wSupportedSRAMType;	/* See 3.2.8.1 */
		struct
		{
			WORD Other:1;
			WORD Unknown:1;
			WORD NonBurst:1;
			WORD Burst:1;
			WORD PipelineBurst:1;
			WORD Synchronous:1;
			WORD Asynchronous:1;
			WORD Reserved:9;
		} SupportedSRAMType;
	};
	union
	{
		WORD	wCurrentSRAMType;	/* See 3.2.8.1 */
		struct
		{
			WORD Other:1;
			WORD Unknown:1;
			WORD NonBurst:1;
			WORD Burst:1;
			WORD PipelineBurst:1;
			WORD Synchronous:1;
			WORD Asynchronous:1;
			WORD Reserved:9;
		} CurrentSRAMType;
	};
} DMITYPE7, *PDMITYPE7;

/* Port Connector Information (Type 8) */
typedef	struct tagDMITYPE8
{
	BYTE	bType;				/* 8 - Connector Information Indicator */
	BYTE	bLength;			/* 09h */
	WORD	wHandle;
	BYTE	bInRefDesign;		/* String number for Internal Reference Designator */
	BYTE	bInConnectType;		/* Internal Connector type - See 3.2.9.2 */
	BYTE	bExtRefDesign;		/* String number for External Reference Designator */
	BYTE	bExtConnectType;	/* External Connector type - See 3.2.9.2 */
	BYTE	bPortType;			/* Describes the function of the port - See 3.2.9.3 */
} DMITYPE8, *PDMITYPE8;

/* System Slots (Type 9) */
typedef	struct tagDMITYPE9
{
	BYTE	bType;			/* 9 - System Slot Structure Indicator */
	BYTE	bLength;		/* 0Ch */
	WORD	wHandle;
	BYTE	bSlotDesign;	/* String number for Reference Designation */
	BYTE	bSlotType;		/* Slot type - See 3.2.10.1 */
	BYTE	bSlotDBWidth;	/* Slot Data Bus Width - See 3.2.10.2 */
	BYTE	bCurUsage;		/* Current Usage - See 3.2.10.3 */
	BYTE	bSlotLength;	/* Slot Length - See 3.2.10.4 */
	WORD	wSlotID;		/* Slot ID - See 3.2.10.5 */
	BYTE	bSlotChar;		/* Slot Characteristics - See 3.2.10.6 */
} DMITYPE9,*PDMITYPE9;

/* On Board Devices Information (Type 10) */
typedef struct tagDEVICE
{
	BYTE    bDevType;		/* Bit 7 - Device Status
								1 - Device Enabled
								0 - Device Disabled
							   Bit 6:0 - Type of Device - See 3.2.11.1 */
	BYTE    bDescription;	/* String number of description */
} DEVICE;

typedef struct tagDMITYPE10
{
	BYTE    bType;			/* 10 - On Board Devices Information Indicator */
	BYTE    bLength;		/* 4 + (Number of Devices * 2) */
	WORD    wHandle;
	DEVICE  stDev;			/* See tagDEVICE above */
} DMITYPE10, *PDMITYPE10;

/* OEM Strings (Type 11) */
typedef struct tagDMITYPE11
{
	BYTE	bType;			/* 11 - OEM Strings Indicator */
	BYTE	bLength;		/* 05h */
	WORD	wHandle;
	BYTE	bCount;			/* Number of strings */
} DMITYPE11, *PDMITYPE11;

/* System Configuration Options (Type 12) */
typedef struct tagDMITYPE12
{
	BYTE	bType;			/* 12 - Configuration Information Indicator */
	BYTE	bLength;		/* 05h */
	WORD	wHandle;
	BYTE	bCount;         /* Number of strings */
} DMITYPE12, *PDMITYPE12;

/* BIOS Language Information (Type 13) */
typedef struct tagDMITYPE13
{
	BYTE	bType;			/* 13 - Language Information Indicator */
	BYTE	bLength;		/* 16h */
	WORD	wHandle;
	BYTE	bLanguages;     /* Installable Languages - Number of languages available */
	BYTE	baReserved[16]; /* Reserved for future use */
	BYTE	bCurLanguage;	/* Current Language - String number of current language */
} DMITYPE13, *PDMITYPE13;

/* Group Associations (Type 14) */
typedef struct tagITEM
{
	BYTE	bItemType;		/* Item (Structure) Type of this member */
	WORD	wItemHandle;
} ITEM;

typedef struct tagDMITYPE14
{
	BYTE	bType;			/* 14 - Group Associations Indicator */
	BYTE	bLength;		/* 5 + (3 bytes for each item in the group) */
	WORD	wHandle;
	BYTE	bGroupName;		/* String number of string describing the group */
	ITEM	stItem;			/* See tagITEM above */
} DMITYPE14,*PDMITYPE14;

/* System Event Log (Type 15) */
typedef struct tagDMITYPE15
{
	BYTE	bType;					/* 15 - Event Log Type Indicator */
	BYTE	bLength;				/* 14h */
	WORD	wHandle;
	WORD	wLogAreaLength;			/* Length of the overall event log area */
	WORD	wLogHeaderStartOffset;	/* Starting offset from Access Method Address */
	WORD	wLogDataStartOffset;	/* Starting offset from Access Method Address */
	BYTE	bAccessMethod;			/* Defines the Location and Method used by
									   higher-level software to access the log area
										00h - Indexed I/O: 1 8-bit index port, 1 8-bit data port
										01h - Indexed I/O: 2 8-bit index ports, 1 8-bit data port
										02h - Indexed I/O: 1 16-bit index port, 1 8-bit data port
										03h - Memory-mapped physical 32-bit address
										04h - Available via GPNV data functions
										05h-FFh - Available for future assignment */
	union
	{
		BYTE	bLogStatus;				/* Current status of system event log */
		struct
		{
			BYTE LogAreaValid:1;		/*  Bit 0 - Log area valid, if 1      */
			BYTE LogAreaFull:1;			/*  Bit 1 - Log area full, if 1       */
			BYTE Reserved:6;			/*  Bit 7:2 - Reserved, set to 0's    */
		} LogStatus;
	};
	DWORD	dwLogChangeToken;		/* Unique token that is reassigned every time
									   the event log changes */
	DWORD   dwAccessMethodAddress;	/* Address associated with the access method */
	BYTE	bLogHeaderFormat;		/* Log header format 
											00h		No header (header is 0 bytes in length
											01h		Type 1 header (16 bytes - see 3.3.16.5.1)
											02h-7Fh	Available for future assignment
											80h-FFh	BIOS Vendor or OEM-specific header */
	BYTE	bNumLogTypeDescriptors;	/* number of support log type descriptors */
	BYTE	bLengthEachDescriptor;	/* length of each log type descriptor */
	BYTE	bDescriptorList;		/* Variable list of Event Log Descriptors
										(this variable just a marker for pointer assignment) */

} DMITYPE15, *PDMITYPE15;


typedef struct tagEVENTLOGHEADER
{
	BYTE	baOptionROMHeader[5];
	BYTE	bGTGN;
	BYTE	bGCGN;
	BYTE	baPreBootClearEventLog[2];
	BYTE	baCMOSChecksumRegion[3];
	BYTE	baReserved[3];
	BYTE	bRevision;
} EVENTLOGHEADER;

typedef struct tagEVENTLOGREC
{
	BYTE	bEventType;
	BYTE	bLength;
	BYTE	bcdYear;
	BYTE	bcdMonth;
	BYTE	bcdDay;
	BYTE	bcdHour;
	BYTE	bcdMinute;
	BYTE	bcdSecond;
	DWORD	dwLogVariableData[2];
} EVENTLOGREC;

typedef struct tagBLOCKERASEDATA
{
	BYTE		bBlockEraseCommand;
	BYTE		baSecurityKey[8];
	void far	*pDmiWorkBuffer;
} BLOCKERASEDATA;

typedef struct tagMUXDATA
{
	BYTE		bCommand;
	BYTE		bType;
	DWORD		dwData;
	void far	*lpDmiWorkBuffer;
	BYTE		baSecurityKey[8];
} MUXDATA;

#define MUX_READ  0x00
#define MUX_WRITE 0x01
#define MUX_CHANGE_CPU_SPEED 0x00

#pragma warning(default:4214 4201)
#pragma pack()


#define SUPPORTED_DMI_REVISION			0x20
#define SUPPORTED_SMBIOS_VERSION		0x21

/* SM BIOS STRUCTURE TYPES */
#define SMB_BIOSINFO					 0
#define SMB_SYSTEMINFO					 1
#define SMB_BASEBOARDINFO				 2
#define SMB_CHASSISINFO					 3
#define SMB_PROCESSORINFO				 4
#define SMB_MEMORYCONTROLLERINFO		 5
#define SMB_MEMORYMODULEINFO			 6
#define SMB_CACHEINFO					 7
#define SMB_PORTCONNECTORINFO			 8
#define SMB_SYSTEMSLOTINFO				 9
#define SMB_ONBOARDDEVICEINFO			10
#define SMB_OEMSTRINGINFO				11
#define SMB_SYSTEMCONFIGOPTIONS			12
#define SMB_BIOSLANGUAGEINFO			13
#define SMB_GROUPASSOCIATIONS			14
#define SMB_EVENTLOGINFO				15

/* DMI BIOS Functions */
#define GET_DMI_INFORMATION				0x50
#define GET_DMI_STRUCTURE				0x51
#define SET_DMI_STRUCTURE				0x52
#define GET_DMI_STRUCTURE_CHANGE_INFO	0x53
#define DMI_CONTROL						0x54
#define GET_GPNV_INFORMATION			0x55
#define READ_GPNV_DATA					0x56
#define WRITE_GPNV_DATA					0x57

/* DMI BIOS Error Return Codes */
#define DMI_SUCCESS						0x00
#define DMI_UNKNOWN_FUNCTION			0x81
#define DMI_FUNCTION_NOT_SUPPORTED		0x82
#define DMI_INVALID_HANDLE				0x83
#define DMI_BAD_PARAMETER				0x84
#define DMI_INVALID_SUBFUNCTION			0x85
#define DMI_NO_CHANGE					0x86
#define DMI_ADD_STRUCTURE_FAILED		0x87
#define DMI_READ_ONLY					0x8D
#define DMI_LOCK_NOT_SUPPORTED			0x90
#define DMI_CURRENTLY_LOCKED			0x91
#define DMI_INVALID_LOCK				0x92
#define DMI_NO_ENTRY					0xFF

/* SET_DMI_STRUCTURE - dmiDataBuffer - Command */
#define DMI_CMD_SET_BYTE				0x00
#define DMI_CMD_SET_WORD				0x01
#define DMI_CMD_SET_DWORD				0x02
#define DMI_CMD_ADD_STRUCT				0x03
#define DMI_CMD_DEL_STRUCT				0x04
#define DMI_CMD_SET_STRING				0x05

/* DMI_CONTROL - SubFunction - DMI BIOS SubFunctions */
#define DMI_CLEAR_EVENT_LOG				0x0000	/* AMI Uses 0x4002 - see below */
#define DMI_CONTROL_LOGGING				0x0001

/* DMI_CONTROL - SubFunction - BIOS Vendor SubFunctions */
#define DMI_iSTRUCTURE_ELEMENT_ERASE	0x4000
#define DMI_STRUCTURE_ELEMENT_ERASE		DMI_iSTRUCTURE_ELEMENT_ERASE
#define DMI_iMANUFACTURING_ACCESS		0x4001
#define DMI_MANUFACTURING_ACCESS		DMI_iMANUFACTURING_ACCESS
#define DMI_iCLEAR_EVENT_LOG			0x4002	/* pData points to buffer of size wMinGPNVRWSize */
#define DMI_iHECETA_CONFIG				0x4003
#define DMI_HECETA_CONFIG				DMI_iHECETA_CONFIG
#define DMI_iSWITCHBOARD_ACCESS			0x4004
#define DMI_SWITCHBOARD_ACCESS			DMI_iSWITCHBOARD_ACCESS
#define DMI_iSTRUCTURE_BLOCK_ERASE		0x4005
#define DMI_STRUCTURE_BLOCK_ERASE		DMI_iSTRUCTURE_BLOCK_ERASE
#define DMI_iJUMPERLESS_MB_MUX			0x4006
#define DMI_JUMPERLESS_MB_MUX			DMI_iJUMPERLESS_MB_MUX

/* DMI_CONTROL - Control */
#define VERIFY_PARAMETERS				0x00
#define PERFORM_OPERATION				0x01

/* READ/WRITE_GPNV_DATA - Handle */
#define GPNV_DMI_HANDLE					0
#define GPNV_MFG_HANDLE					1
#define GPNV_CMS_HANDLE					2

#define IGNORE_GPNV_LOCK				-1

/* System Event Log - Access Methods */
#define EVENT_LOG_ACCESS_PHYS32			0x03
#define EVENT_LOG_ACCESS_GPNV			0x04


/* System Event Log - Events */
#define EVENT_ALL_EVENTS				0x00
#define EVENT_SINGLEBIT_ECC				0x01
#define EVENT_MULTIBIT_ECC				0x02
#define EVENT_POST_ERROR				0x08
#define EVENT_LOGGING_DISABLED			0x0E
#define EVENT_SYSTEM_LIMIT				0x10
#define EVENT_SYSTEM_BOOT				0x17
/* System Event Log - OEM Events */
#define EVENT_OEM_BASE					0x80
#define EVENT_VIRUS_SCAN				0x80
/* System Event Log - End of Records */
#define EVENT_END_OF_RECORDS			0xFF

/* System Event Log - Virus Scan */
#define VIRUS_DETECTION_MASK			0x00000001l
#define VIRUS_DETECTION_NOT_DETECTED	0x00000000l
#define VIRUS_DETECTION_DETECTED		0x00000001l

#define VIRUS_CLEAN_MASK				0x00000006l
#define VIRUS_CLEAN_BOOTED_INFECTED		0x00000000l
#define VIRUS_CLEAN_CLEANED				0x00000002l
#define VIRUS_CLEAN_FAILED				0x00000004l
#define VIRUS_CLEAN_RESET_SYSTEM		0x00000006l

#define VIRUS_DEVICE_MASK				0x00000038l
#define VIRUS_DEVICE_NONE				0x00000000l
#define VIRUS_DEVICE_FDD				0x00000008l
#define VIRUS_DEVICE_HDD				0x00000010l
#define VIRUS_DEVICE_OTHER				0x00000038l


typedef enum tagSECURITY
{
	SEC_NONE,
	SEC_MFG,
	SEC_OEM,
	SEC_ADMIN
} SECURITY;

typedef enum tagBLOCKERASECOMMAND
{
	BLOCK_ERASE_SYSTEM	= 0x00,
	BLOCK_ERASE_CHASSIS	= 0x01,
	BLOCK_ERASE_OEM		= 0x02,
	BLOCK_ERASE_BOARD	= 0x80,
	BLOCK_ERASE_ALL		= 0x81
} BLOCKERASECOMMAND;


/*
 @ FUNCTION PROTOTYPES
 */

WORD GetPnpEntry(LPPNPENTRY* pPnpFunction,WORD *pPnpBiosSeg);

/* Function 50h - GET_DMI_INFORMATION */
BOOL PASCAL GetDmiInfo(BYTE *pDmiBIOSRevision,WORD *pNumStructures,
                       WORD *pStructureSize,DWORD *pDmiStorageBase,
                       int *pErrorCode);

/* Function 51h - GET_DMI_STRUCTURE */
BOOL PASCAL GetDmiStruct(WORD *pStructure, void *pDmiStrucBuffer,
                         DWORD DmiStorageBase,int *pErrorCode);

/* Function 52h - SET_DMI_STRUCTURE */
BOOL PASCAL SetDmiStruct(DMIDATABUFFER *pDmiDataBuffer,void *pDmiWorkBuffer,
				         BYTE Control,DWORD DmiStorageBase,int *pErrorCode);

/* Function 53h - GET_DMI_STRUCTURE_CHANGE_INFO */

/* Function 54h - DMI_CONTROL */
BOOL PASCAL DmiControl(short SubFunction,void *pData,BYTE Control,
                       DWORD DmiStorageBase,int *pErrorCode);

/* Function 55h - GET_GPNV_INFORMATION */
BOOL PASCAL GetGpnvInfo(WORD *pHandle, WORD *pMinGPNVRWSize, WORD *pGPNVSize,
                        DWORD *pGPNVStorageBase, int *pErrorCode);

/* Function 56h - READ_GPNV_DATA */
BOOL PASCAL ReadGpnvData(WORD Handle,BYTE *pGPNVBuffer,short *pGPNVLock,
                         DWORD dwGPNVStorageBase,int *pErrorCode);

/* Function 57h - WRITE_GPNV_DATA */
/* BOOL WriteGpnvData(WORD Handle, BYTE *pGPNVBuffer, short *pGPNVLock,
				   WORD wGPNVStorageSeg, int *pErrorCode);
 */

BOOL PASCAL SwitchBoard(BYTE bCommand, DWORD dwDmiStorageSeg, DWORD *pdwData,
                        BYTE *pbRevision, int *pErrorCode);

BOOL JumperlessMbMux(BYTE bCommand, BYTE bType, DWORD* pdwData,
					 WORD wDmiStorageSeg, int* pErrorCode);

BOOL GetGpnvStruct(WORD Handle, void *pDmiStrucBuffer, short *lock,
				   WORD DmiStorageSeg, int *pErrorCode);

BOOL PASCAL ClearEventLog16(int *pErrorCode);

BOOL PASCAL DMI_ThunkConnect16(LPSTR pszDll16,LPSTR pszDll32,WORD hInst,DWORD dwReason);

/*
 @ SECURITY ROUTINES
 */
void MfgAccess(DWORD dmibase, char * mfg_access_key);

/* Type 1,2,3 erase functions */
BOOL PASCAL StructureElementErase(SECURITY sec,BYTE type,WORD handle,BYTE element_offset,DWORD dmibase,
									char * mfg_access_key, char * mfg_security_key);
BOOL PASCAL StructureBlockErase(BLOCKERASECOMMAND command,SECURITY sec,DWORD dmibase,int *pErrorCode,
									char * mfg_access_key, char * mfg_security_key);

#if defined(__cplusplus)
}
#endif

#endif /* _DMIFUNC_H_ */
