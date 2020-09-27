
#ifndef NVAGP_H
#define NVAGP_H

#include <nvrm.h>

#include <os.h>
#ifdef LINUX
#include <os-interface.h>
#endif

// AGP memory allocation tracking
typedef struct _AGP_MEM_INFO
{
    VOID *SysAddr;              // original pages
    U032 PageCount;
    U032 Offset;                // index into AGP Bitmap
    VOID *pMdl;                  // MDL for user mapping
} AGP_MEM_INFO, *PAGP_MEM_INFO;

/* export AGP functionality to rest of resman */
BOOL RmInitAGP (PHWINFO);
BOOL RmTeardownAGP (PHWINFO);
RM_STATUS NvInitChipset(PHWINFO, VOID **handle, U032 *chipset);
RM_STATUS NvSetAGPRate(PHWINFO, VOID *handle, U032 *agprate);
VOID NvUpdateAGPConfig(PHWINFO);
RM_STATUS NvGetAGPBaseLimit(PHWINFO, U032 *base, U032 *limit);
VOID NvEnableAGP(PHWINFO);
VOID NvDisableAGP(PHWINFO);
VOID NvSaveRestoreAGPChipset(PHWINFO pDev, U032 state);
VOID NvSetupAGPConfig(PHWINFO);
RM_STATUS NvAllocAGPPages ( PHWINFO, VOID **, U032, U032, VOID **);
RM_STATUS NvMapAGPPages ( PHWINFO, VOID **, U032, VOID *);
RM_STATUS NvFreeAGPPages ( PHWINFO, VOID **, VOID  *);
BOOL NvSetBusDeviceFunc(PHWINFO);
RM_STATUS NvSetCapIDOffset(PHWINFO, VOID *, U032 *);

/* OS-specific functionality imported from resman */
/* probably need to split this into a different file */
#ifdef LINUX
#include <linux/types.h>		/* for size_t */
#endif

#define AgpInfo(pdev)      (((PRMINFO)(pdev)->pRmInfo)->AGP)

#define PCI_VENDOR_ID		0x00
#define PCI_DEVICE_ID		0x02
#define PCI_COMMAND  		0x04
#define PCI_BASE_ADDRESS_0	0x10   /* Aperture Base */
#define PCI_BASE_ADDRESS_1	0x14   /* Aperture Base */
#define PCI_CAPABILITY_LIST     0x34
#define PCI_DEVICE_SPECIFIC     0x40

#define PCI_MAX_SLOTS            255

#define PCI_MAX_DEVICES                     32
#define PCI_MAX_FUNCTION                    8
#define PCI_INVALID_VENDORID                0xFFFF

#define PCI_CLASS_DISPLAY_DEV               0x03
#define PCI_CLASS_BRIDGE_DEV                0x06
#define PCI_SUBCLASS_BR_HOST                0x00
#define PCI_MULTIFUNCTION                   0x80

#define AGP_NB_ADDR_VALID       0x80000000

#define AGP_CAP_ID              0x02
#define AGP_CAP_STATUS          0x04
#define AGP_CAP_COMMAND         0x08

#define PCI_VENDOR_ID_INTEL       0x8086
#define PCI_VENDOR_ID_AMD         0x1022
#define PCI_VENDOR_ID_VIA         0x1106
#define PCI_VENDOR_ID_RCC         0x1166
#define PCI_VENDOR_ID_MICRON_1    0x1042
#define PCI_VENDOR_ID_MICRON_2    0x1344

#define AGP_STATUS            AgpInfo(pDev).AGPCapPtr+4
#define AGP_COMMAND           AgpInfo(pDev).AGPCapPtr+8

#define INTEL_CHIPSET_CONFIG    0x50
#define INTEL_ERRSTS            0x92
#define INTEL_GART_CONTROL      0xB0
#define INTEL_APER_SIZE         0xB4
#define INTEL_GART_BASE         0xB8
#define INTEL_MULTI_TRANS_TIMER         0xBC
#define INTEL_LOW_PRI_TRANS_TIMER       0xBD

#define VIA_GART_CONTROL        0x80
#define VIA_APER_SIZE           0x84
#define VIA_GART_BASE           0x88
#define VIA_AGP_CONTROL         0xAC

#define AMD_APER_SIZE           0xAC

/* Memory space registers - hex offsets */
#define AMD_GART_STATUS         0x02
#define AMD_GART_BASE           0x04
#define AMD_GART_CONTROL        0x10

#define RCC_APER_SIZE           0x60

#define osPciOrByte(handle, offset, value)			\
    {								\
        volatile unsigned char tmp = osPciReadByte(handle, offset);	\
        tmp |= (U008) value;						\
		osPciWriteByte(handle, offset, tmp);			\
    }

#define osPciOrWord(handle, offset, value)			\
    {								\
        volatile unsigned short tmp = osPciReadWord(handle, offset); \
        tmp |= (U016) value;						\
		osPciWriteWord(handle, offset, tmp);			\
    }

#define osPciOrDword(handle, offset, value)			\
    {								\
        volatile unsigned int tmp = osPciReadDword(handle, offset);	\
        tmp |= (U032) value;						\
		osPciWriteDword(handle, offset, tmp);		\
    }

#define osPciAndByte(handle, offset, value)			\
    {								\
        volatile unsigned char tmp = osPciReadByte(handle, offset);	\
        tmp &= (U008) value;						\
		osPciWriteByte(handle, offset, tmp);			\
    }

#define osPciAndWord(handle, offset, value)			\
    {								\
        volatile unsigned short tmp = osPciReadWord(handle, offset);	\
        tmp &= (U016) value;						\
		osPciWriteWord(handle, offset, tmp);			\
    }

#define osPciAndDword(handle, offset, value)			\
    {								\
        volatile unsigned int tmp = osPciReadDword(handle, offset);	\
        tmp &= (U032) value;						\
		osPciWriteDword(handle, offset, tmp);		\
    }

#endif
