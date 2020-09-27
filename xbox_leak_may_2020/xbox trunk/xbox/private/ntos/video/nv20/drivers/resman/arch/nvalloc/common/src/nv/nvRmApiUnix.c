 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1999 NVIDIA, Corp.  All rights reserved.             *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/*
 * nvRmApiUnix.c
 *
 *   based on NT/Win9x version of nvRmApi.c
 *
 * NVidia resource manager API.
 *
 * Copyright (c) 1997, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/*
 * This file is linked into programs that talk to the Resource Manager.
 * It does the 'trampoline' as necessary to get to into resman proper.
 */

#if !defined(XFree86LOADER)
#include <sys/mman.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <string.h>
#include <stdlib.h>             /* malloc */
#else
# include <xf86_ansic.h>
# include <asm/ioctl.h>
# ifndef off_t
#  define off_t       xf86size_t
# endif
#endif /* XFree86LOADER */

#include <stdint.h>

#include <nv.h>

#include <nvos.h>
#include <nvrm.h>
#include <nv32.h>

#include <nvrmint.h>
#include <nvEscDef.h>
#include <nvmisc.h>
#include <nvcm.h>

typedef struct nv_allocated_mmap_s {
    void *address;
    int   length;
    unsigned int handle;
    struct nv_allocated_mmap_s *next;
} nv_allocated_mmap_t;

/*
 * track the memory mappings visible by the *current application*
 * so that we can just do pointer arithmetic to get valid mappings
 * instead of having the kernel do it.
 *
 * There is one of these per open device in *this application.
 */

typedef struct
{

    ULONG  client;
    ULONG  device_id;
    int    fd;
    int    agpfd;

    ULONG  reg_phys_base;
    ULONG  reg_length;           // size in bytes

    ULONG  fb_phys_base;
    void  *fb;
    ULONG  fb_length;            // size in bytes

    nv_allocated_mmap_t *mmap_list;    // mmap'd memory to free

    char   path[128];            // pathname for device

	int    agp_enabled;          // indicates AGP is enabled one way or another
    
} nv_mapping_t;

nv_mapping_t  nv_mappings[NV_MAX_DEVICES];

nv_ioctl_card_info_t nv_cards[NV_MAX_DEVICES];


/*
 * file descriptor for the NV control device so we can talk to the
 *   driver before picking a board
 */

int ctl_fd = -1;


/*
 * Initialize the mappings and open the control device
 */
static int init_api_layer(void)
{
    nv_mapping_t *nm;
    char *path = "/dev/nvidiactl";
    int rc;
    
    for (nm = nv_mappings; nm < &nv_mappings[NV_MAX_DEVICES]; nm++)
    {
        (void) memset(nm, 0, sizeof(nv_mapping_t));
        nm->fd = -1;
    }

    ctl_fd = open(path, O_RDWR);
    if (ctl_fd < 0)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: could not open control device %s (%s)\n",
                path,
                strerror(errno));
#endif
        return 0;
    }

    // pull in the card info from the driver
    rc = ioctl(ctl_fd, NV_IOCTL_CARD_INFO, (void *) nv_cards);
    if (rc < 0)
    {
#ifdef DEBUG
        fprintf(stderr, "NV: could not get NV card info (%s)\n",
                strerror(errno));
#endif
        return 0;
    }

    return 1;
}             

/*
 * remember a memory range so that it can be freed when we free the client
 */

static nv_allocated_mmap_t *remember_mmap_memory(
    nv_mapping_t *nm,
    void    *address,
    int      length,
    unsigned int handle
)
{
    nv_allocated_mmap_t *mm;

    mm = malloc(sizeof(nv_allocated_mmap_t));
    if (mm == (void *) 0)
        return (void *) 0;

    mm->address = address;
    mm->length = length;
    mm->next = nm->mmap_list;
    mm->handle = handle;
    nm->mmap_list = mm;        
    
    return mm;
}

static void *mmap_memory(
    nv_mapping_t *nm,
    int           length,
    off_t         offset,
    unsigned int  handle
)
{
    void *ptr;

    ptr = mmap((void *) 0, length, PROT_WRITE | PROT_READ,
               MAP_SHARED, nm->fd, offset);
    if (ptr != MAP_FAILED)
        return remember_mmap_memory(nm, ptr, length, handle);

    return (void *) 0;
}

static void munmap_memory(
    nv_allocated_mmap_t *mm
)
{
    if (mm->address)
    {
        munmap(mm->address, mm->length);
        mm->address = 0;
        mm->length = ~0;
    }
}

static nv_allocated_mmap_t *find_mmap_by_handle(
    nv_mapping_t *nm,
    unsigned int handle
)
{
    nv_allocated_mmap_t *tmp = nm->mmap_list;

    while (tmp)
    {
        if (tmp->handle == handle)
             break;
        tmp = tmp->next;
    }
    return tmp;
}

static void free_all_mmap_memory(
    nv_mapping_t *nm
)
{
    nv_allocated_mmap_t *mm;
    nv_allocated_mmap_t *next;

    for (mm = nm->mmap_list; mm; )
    {
        (void) munmap_memory(mm);
        next = mm->next;
        free(mm);
        mm = next;
    }    
}
   
static nv_mapping_t *find_mapping(
    ULONG client,
    ULONG device_id
)
{
    nv_mapping_t *nm;
    for (nm = nv_mappings; nm < &nv_mappings[NV_MAX_DEVICES]; nm++)
        if (nm->client == client)
            if ((device_id == 0) || (nm->device_id == device_id))
                return nm;
    return (nv_mapping_t *) 0;
}

static nv_mapping_t *find_empty_mapping(void)
{
    nv_mapping_t *nm;
    for (nm = nv_mappings; nm < &nv_mappings[NV_MAX_DEVICES]; nm++)
        if (nm->client == 0)
            return nm;
    return (nv_mapping_t *) 0;
}

static void clear_old_mapping(nv_mapping_t *nm)
{
    if (!nm) return;

    free_all_mmap_memory(nm);
    (void) memset(nm, 0, sizeof(nv_mapping_t));
}

static int fixup_fb_address(
    nv_mapping_t *nm,
    void        **ppAddress,
    ULONG         offset,
    ULONG         length
)
{
    offset -= nm->fb_phys_base;
    if ((offset + length) > nm->fb_length)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: bad offset/length into frame buffer: 0x%lx 0x%lx\n", 
                offset, length);
#endif
        return 1;
    }
    *ppAddress = nm->fb + offset;
    return 0;
}

static int fixup_reg_address(
    nv_mapping_t *nm,
    void        **ppAddress,
    off_t         offset,
    ULONG         length
)
{
    nv_allocated_mmap_t *mm;

    if ((offset + length) > nm->reg_length)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: bad offset/length into register area: 0x%lx 0x%lx\n", 
                offset, length);
#endif
        return 1;
    }

    mm = mmap_memory(nm, length, NV_MMAP_REG_OFFSET + offset, 0x0);
    if (mm == (void *) 0)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: could not map register space on %s at 0x%lx for 0x%lx "
                "bytes (%s)\n",
                nm->path,
                offset,
                length,
                strerror(errno));
#endif
        return 1;
    }

    *ppAddress = mm->address;

    return 0;
}
    
static int open_actual_device(
    nv_mapping_t *nm,
    char         *pathname,
    int           device_number
)
{
    /* build up the path name */
    if (pathname)
        strcpy(nm->path, pathname);
    else
    {
        strcpy(nm->path, "/dev/nvidia#");
        nm->path[strlen(nm->path) - 1] = '0' + device_number;
    }
    
    nm->fd = open(nm->path, O_RDWR);
    if (nm->fd < 0)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: could not open device %s (%s)\n",
                nm->path,
                strerror(errno));
#endif
        return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
    }

    return 0;
}

static int close_actual_device(
    nv_mapping_t *nm
)
{
    (void) close(nm->fd);
    return 0;
}

static int map_actual_device(
    nv_mapping_t *nm
)
{
    nv_allocated_mmap_t *mm;
    nv_ioctl_memory_vtop_t vtop;
    int ret;

    nm->reg_length = 16 * 1024 * 1024;

    // XXX should map size, not hardwired
    nm->fb_length = 64 * 1024 * 1024;
    mm = mmap_memory(nm, nm->fb_length, NV_MMAP_FB_OFFSET, 0x0);
    if (mm == (void *) 0)
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: could not map frame buffer on %s (%s)\n",
                nm->path,
                strerror(errno));
        fprintf(stderr,
                "    hint: this may be due to ulimit limiting the amount of virtual memory available to XFree86. Please check your start-up scripts for something like 'ulimit -v <number>' and either boost this value or remove the line.");
#endif
        return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
    }

    nm->fb = mm->address;
    nm->reg_phys_base = 0;

    /* what is physical address of fb? */
    nm->fb_phys_base = 0;
    vtop.buffer = nm->fb;
    vtop.byte_length = 1;	/* will be rounded up to 1 page */
    vtop.physical_addresses = (unsigned int *) &nm->fb_phys_base;
    ret = ioctl(nm->fd, NV_IOCTL_MEMORY_VTOP, (void *) &vtop);
    if ((ret < 0) || (nm->fb_phys_base == 0))
    {
#ifdef DEBUG
        fprintf(stderr,
                "NV: could not get physical address of frame buffer on %s "
                "(%s)\n",
                nm->path,
                strerror(errno));
#endif
        return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
    }

    return 0;
}
    
static inline int doEscape(
    int fd,
    int cmd,
    void * parms)
{
    return (ioctl(fd, _IOWR(NV_IOCTL_MAGIC, cmd, sizeof(void *)), parms) >= 0) ? 1 : -1;
}


/*
 * NvAllocRoot - allocate root of device tree
 */
ULONG NvRmAllocRoot(
    ULONG *phClient
)
{
    int   ret;
    NVOS01_PARAMETERS Parms;
    static int firstcall = 1;

    if (firstcall)
        if ( ! init_api_layer())
            return NVOS01_STATUS_ERROR_OPERATING_SYSTEM;
    firstcall = 0;

    *phClient = 0;

    Parms.hClass = NV01_ROOT;

    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC_ROOT, (void *) &Parms);
    if (ret <= 0)
        return NVOS01_STATUS_ERROR_OPERATING_SYSTEM;

    *phClient = (ULONG)Parms.hObjectNew;

    return Parms.status;
}


/*
 * NvAllocDevice - allocate a device.
 */
ULONG NvRmAllocDevice(
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName
)
{
    int   ret;
    NVOS06_PARAMETERS Parms;
    nv_mapping_t *nm;
    char saved_file_name[128];
    char *str;

    strcpy(saved_file_name, "");
    str = saved_file_name;

    if (hClass == NV03_DEVICE_XX)
    {
        nv_ioctl_card_info_t *ci;
	nv_ioctl_primary_card_t pc;
        int bus, slot;
        char *p;
        int i;
	int primary_head = 0;
	int rc;

        strcpy(saved_file_name, (char *) szName);

	// first check if the first character is a "*".
	// If it is, then that's X telling us this is a
	// primary head.
	if (str[0] == '*') {
	    primary_head = 1;
	    str++;
	}

        // check to see if the bus:slot device name requested
        // the X server uses this to request a particular card
        // We convert it here into /dev/nvidia0 form, since the
        // the bus:slot style is not exported to /dev
        if (strchr(str, ':'))
        {
            bus = strtol(str, &p, 0);
            slot = strtol(p+1, (char **) 0, 0);

            for (i=0, ci = nv_cards; i<NV_MAX_DEVICES; i++, ci++)
            {
                if ((ci->flags & NV_IOCTL_CARD_INFO_FLAG_PRESENT) &&
                    (ci->bus == bus) &&
                    (ci->slot == slot))
                {
                    hClass = NV01_DEVICE_0 + i;

		    /* let kernel knw if this is primary */
		    if (primary_head) {
                        pc.index = i;
			rc = ioctl(ctl_fd, NV_IOCTL_PRIMARY_CARD, (void *) &pc);
			if (rc < 0) {
#ifdef DEBUG
			    fprintf(stderr, "NV: could not set primary card "
                                    "(%s)\n", strerror(errno));
#endif
			    return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
			}
		    }
                    break;
                }
            }
        }
    }

    Parms.hObjectParent   = (NvV32) hClient;
    Parms.hObjectNew      = (NvV32) hDevice;
    Parms.hClass          = (NvV32) hClass;
    Parms.szName          = (NvP64) ((uintptr_t)szName); // assume 64-bit compliance in linux

    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC_DEVICE, (void *) &Parms);
    if (ret <= 0)
        return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;

    if (Parms.status == NVOS06_STATUS_SUCCESS)
    {
        int devnum = -1;
        char *pathname = 0;


        /*
         * save the client handle in the mapping structure
         */

        nm = find_empty_mapping();
        if (nm == 0)
            return NVOS06_STATUS_ERROR_INSUFFICIENT_RESOURCES;

        nm->client    = hClient;
        nm->device_id = hDevice;

        /*
         * figure out the device name.
         * API allows it to be specified numerically or by string based
         *  on hClass
         */

        if (hClass == NV03_DEVICE_XX)
            pathname = str;
        else
        {
            devnum = hClass - NV01_DEVICE_0;
            if (devnum < 0 || devnum > 8)    // XXX does resman have #define for num devices?
                return NVOS06_STATUS_ERROR_OPERATING_SYSTEM;
        }

        if ((Parms.status = open_actual_device(nm, pathname, devnum)))
            goto done;

        if ((Parms.status = map_actual_device(nm)))
            goto done;
    }

 done:
    return Parms.status;
}

/*
 * NvAllocContextDma - allocate and lock down memory using the resource manager.
 */
ULONG NvRmAllocContextDma(
    ULONG hClient,
    ULONG hDma,
    ULONG hClass,
    ULONG flags,
    PVOID base,
    ULONG limit
)
{
    int   ret;
    NVOS03_PARAMETERS Parms;

    memset(&Parms, 0, sizeof(NVOS03_PARAMETERS));
    Parms.hObjectParent      = (NvV32) hClient;
    Parms.hObjectNew         = (NvV32) hDma;
    Parms.hClass             = (NvV32) hClass;
    Parms.flags              = (NvV32) flags;
    Parms.pBase              = (NvP64) ((uintptr_t)base);
    Parms.limit              = (NvU64) limit;

    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC_CONTEXT_DMA, (void *) &Parms);
    if (ret <= 0)
        return NVOS03_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * AllocChannelPio - allocate a PIO channel by calling RM.
 */
ULONG NvRmAllocChannelPio(
    ULONG  hClient,
    ULONG  hDevice,
    ULONG  hChannel,
    ULONG  hClass,
    ULONG  hErrorCtx,
    PVOID *ppChannel,
    ULONG  flags
)
{
    int   ret;
    NVOS04_PARAMETERS Parms;
    nv_mapping_t *nm;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS04_STATUS_ERROR_BAD_CLASS;

    memset(&Parms, 0, sizeof(NVOS04_PARAMETERS));
    Parms.hRoot           = (NvV32) hClient;
    Parms.hObjectParent   = (NvV32) hDevice;
    Parms.hObjectNew      = (NvV32) hChannel;
    Parms.hClass          = (NvV32) hClass;
    Parms.hObjectError    = (NvV32) hErrorCtx;
    Parms.flags           = (NvV32) flags;

    ret = doEscape(nm->fd, NV_ESC_RM_ALLOC_CHANNEL_PIO, (void *) &Parms);
    if (ret <= 0)
        return NVOS04_STATUS_ERROR_OPERATING_SYSTEM;

    if (Parms.status != RM_OK)
        return Parms.status;

    // convert the offset returned into a virtual ptr for this app
    if (0 == fixup_reg_address(nm, ppChannel, (off_t) Parms.pChannel, CHANNEL_LENGTH))
        return Parms.status;

    return NVOS04_STATUS_ERROR_OPERATING_SYSTEM;
}

/*
 * NvAllocChannelDma - allocate a DMA push channel using RM.
 */
ULONG NvRmAllocChannelDma(
    ULONG  hClient,
    ULONG  hDevice,
    ULONG  hChannel,
    ULONG  hClass,
    ULONG  hErrorCtx,
    ULONG  hDataCtx,
    ULONG  offset,
    PVOID *ppChannel
)
{
    int   ret;
    NVOS07_PARAMETERS Parms;
    nv_mapping_t *nm;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS07_STATUS_ERROR_BAD_CLASS;

    memset(&Parms, 0, sizeof(NVOS07_PARAMETERS));
    Parms.hRoot           = (NvV32) hClient;
    Parms.hObjectParent   = (NvV32) hDevice;
    Parms.hObjectNew      = (NvV32) hChannel;
    Parms.hClass          = (NvV32) hClass;
    Parms.hObjectError    = (NvV32) hErrorCtx;
    Parms.hObjectBuffer   = (NvV32) hDataCtx;
    Parms.offset          = (NvV32) offset;

    ret = doEscape(nm->fd, NV_ESC_RM_ALLOC_CHANNEL_DMA, (void *) &Parms);
    if (ret <= 0)
        return NVOS07_STATUS_ERROR_OPERATING_SYSTEM;

    if (Parms.status != RM_OK)
        return Parms.status;

    // convert the offset returned into a virtual ptr for this app
    if (0 == fixup_reg_address(nm, ppChannel, (off_t) Parms.pControl, CHANNEL_LENGTH))
        return Parms.status;

    return NVOS04_STATUS_ERROR_OPERATING_SYSTEM;
}

/*
 * NvAllocMemory - allocate and lock down a mess 'o memory using RM.
 */
ULONG NvRmAllocMemory(
    ULONG  hClient,
    ULONG  hParent,
    ULONG  hMemory,
    ULONG  hClass,
    ULONG  flags,
    PVOID *ppAddress,
    ULONG *pLimit
)
{
    int   ret;
    int   status;
    ULONG byte_count = 0;
    nv_mapping_t *nm;
    nv_allocated_mmap_t *mm = (void *) 0;
    NVOS02_PARAMETERS Parms;

#if defined(IA64)
    /* we're only playing with the lower 32 bits of pLimit, which is 64 bits.
     * make sure we zero out the high 32 bits, in case the user passes us an
     * uninitialized memory location.
     */
    {
        U032 *ptr = (U032 *) pLimit;
        ptr[1] = 0x0;
    }
#endif

    // set input parameters
    memset(&Parms, 0, sizeof(NVOS02_PARAMETERS));
    Parms.hRoot                = (NvV32) hClient;
    Parms.hObjectParent        = (NvV32) hParent;
    Parms.hObjectNew           = (NvV32) hMemory;
    Parms.hClass               = (NvV32) hClass;
    Parms.flags                = (NvV32) flags;
    Parms.pMemory              = (NvP64) ((uintptr_t)*ppAddress);
    Parms.pLimit               = (NvU64) *pLimit;

    nm = find_mapping(hClient, hParent);
    if (nm == 0)
        return NVOS02_STATUS_ERROR_BAD_CLASS;

    ret = doEscape(nm->fd, NV_ESC_RM_ALLOC_MEMORY, (void *) &Parms);
    if (ret <= 0)
    {
        status = NVOS02_STATUS_ERROR_OPERATING_SYSTEM;
        goto failed;
    }

    if (Parms.status != RM_OK)
    {
        status = Parms.status;
        goto failed;
    }

    /*
     * On Unix, the only approved way to map memory into user
     *   space is via mmap(2).
     * So here in the glue layer, we actually allocate the memory, and
     *   sneak the pointer into resman to validate and hook into its
     *   data structures.
     */

    switch (hClass)
    {
        case NV01_MEMORY_SYSTEM:
        {
            /* if we're AGP, we'll allocate and map from kernel land..
             * otherwise the kernel complaints about pre-existing PTEs
             */
            off_t offset = NV_MMAP_ALLOCATION_OFFSET;

            if (DRF_VAL(OS02, _FLAGS, _LOCATION, flags) == NVOS02_FLAGS_LOCATION_AGP) {
                offset = NV_MMAP_AGP_OFFSET;
            }
            byte_count = *pLimit + 1;

            mm = mmap_memory(nm, byte_count, offset, hMemory);
            if (mm == (void *) 0)
            {
                status = NVOS02_STATUS_ERROR_OPERATING_SYSTEM;
                goto failed;
            }

            break;
        }
        default:
    }

    *pLimit = (NvU64) Parms.pLimit;
        
    switch (hClass)
    {
        /*
         * Frame buffer memory
         *
         * A little different from NV01_MEMORY_SYSTEM, since we don't know where
         * in fbmem we're going to get our memory until afterwards.
         *
         * So the resman just returns an offset to us and we patch it up
         * afterwards.
         */

        case NV01_MEMORY_LOCAL_USER:
        {
            if (fixup_fb_address(nm,
                                 (void **) ppAddress,
                                 (off_t) Parms.pMemory,
                                 *pLimit + 1))
            {
                status = NVOS02_STATUS_ERROR_INSUFFICIENT_RESOURCES;
                goto failed;
            }
            break;
        }

        // return the pointer in our own space
        case NV01_MEMORY_SYSTEM:
			*ppAddress = mm->address;
            break;

        default:
            *ppAddress = (PVOID)((uintptr_t)Parms.pMemory);
            break;
    }

    return Parms.status;

 failed:
    if (mm && mm->address)
        munmap_memory(mm);

    return status;
}

/* 
 * NvAlloc - allocate any arbitrary object
 */
ULONG NvRmAlloc(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject,
    ULONG hClass,
    PVOID pAllocParms
)
{
    int   ret;
    NVOS21_PARAMETERS Parms;

    // set input parameters
    memset(&Parms, 0, sizeof(NVOS21_PARAMETERS));
    Parms.hRoot                       = (NvV32)hClient;
    Parms.hObjectParent               = (NvV32)hParent;
    Parms.hObjectNew                  = (NvV32)hObject;
    Parms.hClass                      = (NvV32)hClass;
    Parms.pAllocParms                 = (NvP64)((uintptr_t)pAllocParms);

    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC, (void *)&Parms);

    if (ret <= 0)
        return NVOS21_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvAllocObject - allocate a object using RM.
 */

ULONG NvRmAllocObject(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    int   ret;
    NVOS05_PARAMETERS Parms;

    // set input parameters
    Parms.hRoot           = (NvV32) hClient;
    Parms.hObjectParent   = (NvV32) hChannel;
    Parms.hObjectNew      = (NvV32) hObject;
    Parms.hClass          = (NvV32) hClass;

    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC_OBJECT, (void *) &Parms);
    if (ret <= 0)
        return NVOS05_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvFree - free a "thing" allocated using RM.
 */
ULONG NvRmFree(
    ULONG hClient,
    ULONG hParent,
    ULONG hObject
)
{
    int   ret;
    NVOS00_PARAMETERS Parms;
    nv_mapping_t *nm;

    Parms.hRoot           = (NvV32) hClient;
    Parms.hObjectParent   = (NvV32) hParent;
    Parms.hObjectOld      = (NvV32) hObject;

    /* if this is mmapped memory, be sure to munmap it */
    /* not a failure if no nm */
    nm = find_mapping(hClient, hParent);
    if (nm)
    {
        nv_allocated_mmap_t *mm = (void *) 0;
        if ((mm = find_mmap_by_handle(nm, hObject)))
        {
            munmap_memory(mm);
        }
    }

    ret = doEscape(ctl_fd, NV_ESC_RM_FREE, (void *) &Parms);
    if (ret <= 0)
        return NVOS00_STATUS_ERROR_OPERATING_SYSTEM;

    /*
     * if we're freeing up the entire client, then we also need to
     * free the memory and close the device allocated in the glue layer here.
     */

    if ((Parms.status == NVOS00_STATUS_SUCCESS) && (hParent == NV01_NULL_OBJECT))
    {
        nv_mapping_t *nm;
		int i;

        // for each device on this client
        for (nm = &nv_mappings[0], i = 0; i < NV_MAX_DEVICES; nm++, i++)
        {
            if (nm->client == hClient)
            {
                /* free_all_mmap_memory(nm); */
                close_actual_device(nm);
                clear_old_mapping(nm);
            }
        }
    }

    return Parms.status;
}

ULONG NvRmAllocEvent(
    ULONG hClient,
    ULONG hObjectParent,
    ULONG hObjectNew,
    ULONG hClass,
    ULONG index,
    ULONG data
)
{
    int ret;
    NVOS10_PARAMETERS Parms;

    memset(&Parms, 0, sizeof(NVOS10_PARAMETERS));
    Parms.hRoot               = hClient;
    Parms.hObjectParent       = hObjectParent;
    Parms.hObjectNew          = hObjectNew;
    Parms.hClass              = hClass;
    Parms.index               = index;
    Parms.hEvent              = (NvU64) data;

    // we don't really know which device is doing this call, so just use control fd
    ret = doEscape(ctl_fd, NV_ESC_RM_ALLOC_EVENT, (void *) &Parms);
    if (ret <= 0)
        return NVOS10_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}


ULONG NvRmArchHeap(
    ULONG hClient,
    ULONG hParent,
    ULONG function,
    ULONG owner,
    ULONG type,
    ULONG height,
    ULONG size,
    ULONG *pitch,
    ULONG *offset,
    ULONG *address,
    ULONG *limit,
    ULONG *free,
    ULONG *total
)
{
    int   ret;
    NVOS11_PARAMETERS Parms;
    nv_mapping_t *nm;

    Parms.hRoot         = (NvV32) hClient;
    Parms.hObjectParent = (NvV32) hParent;
    Parms.function      = (NvU32) function;
    Parms.owner         = (NvU32) owner;
    Parms.type          = (NvU32) type;
    Parms.pitch         = (NvS32) (pitch ? *pitch : 0);
    Parms.height        = (NvU32) height;
    Parms.size          = (NvU32) size;
    Parms.offset        = (NvU32) *offset;

    nm = find_mapping(hClient, hParent);
    if (nm == 0)
        return NVOS11_STATUS_ERROR_INVALID_OWNER;

    ret = doEscape(nm->fd, NV_ESC_RM_ARCH_HEAP, (void *) &Parms);
    if (ret <= 0)
        return NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES;
    if (Parms.status != RM_OK)
        return Parms.status;

    if (pitch)
        *pitch = (ULONG) Parms.pitch;
    *offset  = (ULONG) Parms.offset;
    *address = (ULONG) Parms.address;
    *limit   = (ULONG) Parms.limit;
    *free    = (ULONG) Parms.free;
    *total   = (ULONG) Parms.total;

    //
    // fixup address and limit on successful allocate calls
    //
    if (Parms.status == NVOS11_STATUS_SUCCESS)
    {
        switch (function)
        {
            case NVOS11_HEAP_ALLOC_DEPTH_WIDTH_HEIGHT:
            case NVOS11_HEAP_ALLOC_SIZE:
            case NVOS11_HEAP_ALLOC_TILED_PITCH_HEIGHT:
            {
                if (fixup_fb_address(nm,
                                     (void **) address,
                                     *offset + nm->fb_phys_base,
                                     size))
                    return NVOS11_STATUS_ERROR_INSUFFICIENT_RESOURCES;

                break;
            }
        }
    }

    return Parms.status;
}

ULONG NvRmConfigGet(
    ULONG  hClient,
    ULONG  hDevice,
    ULONG  index,
    ULONG *pValue
)
{
    int   ret;
    NVOS13_PARAMETERS Parms;
    nv_mapping_t *nm;

    Parms.hClient         = (NvV32) hClient;
    Parms.hDevice         = (NvV32) hDevice;
    Parms.index           = (NvV32) index;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS13_STATUS_ERROR_BAD_CLIENT;

    // we handle some of these in here
    switch (Parms.index)
    {
        case  NV_CFG_DEVICE_HANDLE:
            *pValue = nm->fd;
            Parms.status = NVOS13_STATUS_SUCCESS;
            break;

        default:
            ret = doEscape(nm->fd, NV_ESC_RM_CONFIG_GET, (void *) &Parms);
            if (ret <= 0)
                return NVOS13_STATUS_ERROR_OPERATING_SYSTEM;
            *pValue = Parms.value;
    }

    return Parms.status;
}


ULONG NvRmConfigSet(
    ULONG  hClient,
    ULONG  hDevice,
    ULONG  index,
    ULONG  newValue,
    ULONG *pOldValue
)
{
    int   ret;
    NVOS14_PARAMETERS Parms;
    nv_mapping_t *nm;

    Parms.hClient         = (NvV32) hClient;
    Parms.hDevice         = (NvV32) hDevice;
    Parms.index           = (NvV32) index;
    Parms.newValue        = (NvV32) newValue;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS14_STATUS_ERROR_BAD_CLIENT;

    ret = doEscape(nm->fd, NV_ESC_RM_CONFIG_SET, (void *) &Parms);
    if (ret <= 0)
        return NVOS14_STATUS_ERROR_OPERATING_SYSTEM;

    *pOldValue = (ULONG) Parms.oldValue;

    return Parms.status;
}

ULONG NvRmConfigGetEx(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    int   ret;
    NVOS_CONFIG_GET_EX_PARAMS Parms;
    nv_mapping_t *nm;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS06_STATUS_ERROR_BAD_OBJECT_PARENT;

    Parms.hClient         = hClient;
    Parms.hDevice         = hDevice;
    Parms.index           = index;
    Parms.paramStructPtr  = (NvP64) ((uintptr_t)paramStructPtr);
    Parms.paramSize       = paramSize;

    ret = doEscape(nm->fd, NV_ESC_RM_CONFIG_GET_EX, (void *) &Parms);
    if (ret <= 0)
        return NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}


ULONG NvRmConfigSetEx(
    ULONG hClient,
    ULONG hDevice,
    ULONG index,
    VOID *paramStructPtr,
    ULONG paramSize
)
{
    int   ret;
    NVOS_CONFIG_SET_EX_PARAMS Parms;
    nv_mapping_t *nm;

    Parms.hClient         = hClient;
    Parms.hDevice         = hDevice;
    Parms.index           = index;
    Parms.paramStructPtr  = (NvP64) ((uintptr_t)paramStructPtr);
    Parms.paramSize       = paramSize;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS_CSE_STATUS_ERROR_BAD_CLIENT;

    ret = doEscape(nm->fd, NV_ESC_RM_CONFIG_SET_EX, (void *) &Parms);
    if (ret <= 0)
        return NVOS_CSE_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

#if 0
ULONG NvRmClipSlam(
    ULONG    hClient,
    ULONG    hChannel,
    ULONG    clips[],        /* {x0,y0,x1,y1}, {x0,y0,x1,y1}, ... */
    ULONG    nClips,
    ULONG    inclusive
)
{
    int  ret;
    XXX_NVOS_I2C_ACCESS_PARAMS  Parms;
    nv_mapping_t *nm;

    Parms.hClient   = hClient;
    Parms.hDevice   = hDevice;
    Parms.clipList  = clipList;
    memcpy(Parms.nClips, clips, min(8, nClips) * 4 * 4);
    Parms.inclusive = inclusive;

    nm = find_mapping(hClient);
    if (nm == 0)
        return XXX_NVOS_I2C_ACCESS_STATUS_ERROR_BAD_CLIENT;

    ret = doEscape(nm->fd, NV_ESC_RM_CLIPLIST_SLAM, (void *) &Parms);
    if (ret <= 0)
        return XXX_NVOS_I2C_ACCESS_STATUS_ERROR_OPERATING_SYSTEM;
    
    return Parms.status;
}

implementation in resman

if not nv10 or better
    return error;

if count too large return error;

wait for IDLE???
set inclusive bit as appropriate in GLOBALSTATE0
for each
   convert to screen space coords of -2048, 2047
   stuff them all

#endif

ULONG  NvRmI2CAccess(
    ULONG hClient,
    ULONG hDevice,
    VOID  *paramStructPtr
)
{
    int  ret;
    NVOS_I2C_ACCESS_PARAMS  Parms;
    nv_mapping_t *nm;

    Parms.hClient        = (NvV32) hClient;
    Parms.hDevice        = (NvV32) hDevice;
    Parms.paramStructPtr = (NvP64) ((uintptr_t)paramStructPtr);

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS_I2C_ACCESS_STATUS_ERROR_BAD_CLIENT;

    ret = doEscape(nm->fd, NV_ESC_RM_I2C_ACCESS, (void *) &Parms);
    if (ret <= 0)
        return NVOS_I2C_ACCESS_STATUS_ERROR_OPERATING_SYSTEM;
    
    return Parms.status;
}

ULONG  NvRmPowerManagement(
    ULONG hClient,
    ULONG hDevice,
    ULONG head,
    ULONG command, 
    VOID *paramStructPtr
)
{
    int  ret;
    NVOS22_PARAMETERS Parms;
    nv_mapping_t *nm;

    Parms.hRoot          = (NvV32) hClient;
    Parms.hDevice        = (NvV32) hDevice;
    Parms.command        = (NvV32) command;
    Parms.head           = (NvV32) head;
    Parms.params         = (NvP64) ((uintptr_t)paramStructPtr);

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS22_STATUS_ERROR_BAD_CLIENT;

    ret = doEscape(nm->fd, NV_ESC_RM_POWER_MANAGEMENT, (void *) &Parms);
    if (ret <= 0)
        return NVOS22_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}


ULONG NvRmDebugControl(NVOS20_PARAMETERS *parms)
{
    int   ret;
    NVOS20_PARAMETERS Parms;

    ret = doEscape(ctl_fd, NV_ESC_RM_DEBUG_CONTROL, (void *) &Parms);
    if (ret <= 0)
        return NVOS20_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

ULONG  NvRmAGPInit(
    ULONG hClient,
    ULONG hDevice,
    ULONG agp_config
)
{
    int  ret = 0;
    nv_mapping_t *nm;
    NVOS_AGP_PARAMS  Parms;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS_AGP_STATUS_ERROR_BAD_CLIENT;

    Parms.hDevice = (NvV32) hDevice;
    Parms.config  = (NvV32) agp_config;

    ret = doEscape(nm->fd, NV_ESC_RM_AGP_INIT, (void *) &Parms);
    if (ret <= 0)
        return NVOS_AGP_STATUS_ERROR_OPERATING_SYSTEM;

    if (Parms.status == RM_OK)
        nm->agp_enabled = 1;

    return Parms.status;
}

ULONG  NvRmAGPTeardown(
    ULONG hClient,
    ULONG hDevice
)
{
    int  ret = 0;
    nv_mapping_t *nm;
    NVOS_AGP_PARAMS  Parms;

    nm = find_mapping(hClient, hDevice);
    if (nm == 0)
        return NVOS_AGP_STATUS_ERROR_BAD_CLIENT;

    // AGP may not be enabled, which is no error
    if (!nm->agp_enabled) 
        return RM_OK;

    Parms.hDevice = (NvV32) hDevice;

    ret = doEscape(nm->fd, NV_ESC_RM_AGP_TEARDOWN, (void *) &Parms);
    if (ret <= 0)
        return NVOS_AGP_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

ULONG NvRmIoFlush(VOID)
{
    int   ret;
    NVOS20_PARAMETERS Parms;

    ret = doEscape(ctl_fd, NV_ESC_RM_IO_FLUSH, (void *) &Parms);
    if (ret <= 0)
        return NVOS20_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

ULONG NvRmConfigVersion(
    ULONG  hClient,
    ULONG  hDevice,
    ULONG *pVersion
)
{
    return NVOS12_STATUS_SUCCESS;
}

ULONG NvRmConfigUpdate(
    ULONG hClient,
    ULONG hDevice,
    ULONG flags
)
{
    return NVOS15_STATUS_SUCCESS;
}

ULONG NvWatchApiFrontEnd(
    ULONG p0,
    ULONG p1,
    ULONG p2,
    ULONG p3,
    ULONG p4,
    ULONG p5,
    ULONG p6
)
{
    NVWATCHAPI_PARAMETERS Parms;

    Parms.p0 = p0;
    Parms.p1 = p1;
    Parms.p2 = p2;
    Parms.p3 = p3;
    Parms.p4 = p4;
    Parms.p5 = p5;
    Parms.p6 = p6;
    return doEscape(ctl_fd, NV_ESC_RM_NVWATCH, (void *) &Parms);
}


/*
 * NvRmInitSwapBarrier
 */
ULONG NvRmInitSwapBarrier(
    ULONG hClient
)
{
    int  ret;
    NVOS2A_PARAMETERS Parms;

    Parms.hClient        = (NvV32) hClient;
    Parms.flags          = (NvV32) (NVOS2A_FLAGS_SWAP_BARRIER | 
                                    NVOS2A_FLAGS_ACTION_INIT);

    ret = doEscape(ctl_fd, NV_ESC_RM_SWAP_EXT, (void *) &Parms);
    if (ret <= 0)
        return NVOS2A_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvRmAddSwapGroupClient - register us as a swap group client
 */
ULONG NvRmAddSwapGroup(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG id 
)
{
    int  ret;
    NVOS2A_PARAMETERS Parms;

    Parms.hClient        = (NvV32) hClient;
    Parms.hDevice        = (NvV32) hDevice;
    Parms.hChannel       = (NvV32) hChannel;
    Parms.group_id       = (NvV32) id;
    Parms.flags          = (NvV32) (DRF_DEF(OS2A, _FLAGS, _SWAP, _GROUP)| 
                                    DRF_DEF(OS2A, _FLAGS, _ACTION, _ADD));

    ret = doEscape(ctl_fd, NV_ESC_RM_SWAP_EXT, (void *) &Parms);
    if (ret <= 0)
        return NVOS2A_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvRmRemoveSwapGroup - unregister us as a swap group client
 */
ULONG NvRmRemoveSwapGroup(
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG id
)
{
    int  ret;
    NVOS2A_PARAMETERS Parms;

    Parms.hClient        = (NvV32) hClient;
    Parms.hDevice        = (NvV32) hDevice;
    Parms.hChannel       = (NvV32) hChannel;
    Parms.group_id       = (NvV32) id;
    Parms.flags          = (NvV32) (DRF_DEF(OS2A, _FLAGS, _SWAP, _GROUP)| 
                                    DRF_DEF(OS2A, _FLAGS, _ACTION, _REMOVE));

    ret = doEscape(ctl_fd, NV_ESC_RM_SWAP_EXT, (void *) &Parms);
    if (ret <= 0)
        return NVOS2A_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvRmAddSwapBarrier - register us as a swap barrier client
 */
ULONG NvRmAddSwapBarrier(
    ULONG group_id,
    ULONG barrier_id
)
{
    int  ret;
    NVOS2A_PARAMETERS Parms;

    Parms.group_id       = (NvV32) group_id;
    Parms.barrier_id     = (NvV32) barrier_id;
    Parms.flags          = (NvV32) (DRF_DEF(OS2A, _FLAGS, _SWAP, _BARRIER)| 
                                    DRF_DEF(OS2A, _FLAGS, _ACTION, _ADD));

    ret = doEscape(ctl_fd, NV_ESC_RM_SWAP_EXT, (void *) &Parms);
    if (ret <= 0)
        return NVOS2A_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}

/*
 * NvRmRemoveSwapBarrier - unregister us as a swap barrier client
 */
ULONG NvRmRemoveSwapBarrier(
    ULONG barrier_id
)
{
    int  ret;
    NVOS2A_PARAMETERS Parms;

    Parms.barrier_id     = (NvV32) barrier_id;
    Parms.flags          = (NvV32) (DRF_DEF(OS2A, _FLAGS, _SWAP, _BARRIER)| 
                                    DRF_DEF(OS2A, _FLAGS, _ACTION, _REMOVE));

    ret = doEscape(ctl_fd, NV_ESC_RM_SWAP_EXT, (void *) &Parms);
    if (ret <= 0)
        return NVOS2A_STATUS_ERROR_OPERATING_SYSTEM;

    return Parms.status;
}


// end of nvRmApi.c
