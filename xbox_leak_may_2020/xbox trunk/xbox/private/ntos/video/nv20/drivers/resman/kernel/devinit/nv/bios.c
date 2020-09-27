 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/**************************************************************************************************************
*
*   Module: bios.c
*
*   Description:
*       generic bios read/write routines
*
*   Revision History:
*       Original -- 2/99 Jeff Westerinen
*
**************************************************************************************************************/

#include <nvrm.h>
#include <nvhw.h>
#include <nv4_ref.h>
#include <os.h>

#include <bios.h>

RM_STATUS BiosReadBytes(
    PHWINFO pDev,
    U008    buffer[],
    U032    offset,
    U032    size
)
{
    U032 i;

    for (i = 0; i < size; i++)
    {
        buffer[i] = BIOS_RD08(pDev->biosOffset + offset + i);
    }

    return RM_OK;
}

RM_STATUS BiosCalculateSizes(
    const char *format,
    U032       *pPackedSize,
    U032       *pPaddedSize
)
{
    U032 packed_size = 0;
    U032 padded_size = 0;
    char fmt;

    while ((fmt = *format++))
    {
        U032 count;

        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0) count = 1;

        switch (fmt)
        {
            case 'b':
                packed_size += count * 1;
                padded_size += count * sizeof(bios_U008);
                break;
            case 'w':
                packed_size += count * 2;
                padded_size += count * sizeof(bios_U016);
                break;
            case 'd':
                packed_size += count * 4;
                padded_size += count * sizeof(bios_U032);
                break;
        }
    }

    if (pPackedSize) *pPackedSize = packed_size;
    if (pPaddedSize) *pPaddedSize = padded_size;

    return RM_OK;
}


/*
  Unpack little-endian byte stream data, and expand to an array of 32-bit values.
*/
RM_STATUS BiosUnpackLittleEndianStructure(
    U008 *packedLEData,
    U032 *unpackedData,
    const char *format,
    U032 *fields,
    U032 *unpacked_bytes
)
{
    U032 count;
    U032 unpacked;
    char fmt;

    *fields = 0;
    *unpacked_bytes = 0;

    for ( ; (fmt = *format++); )
    {
        count = 0;
        while ((fmt >= '0') && (fmt <= '9'))
        {
            count *= 10;
            count += fmt - '0';
            fmt = *format++;
        }
        if (count == 0) count = 1;

        while (count--)
        {
            switch (fmt)
            {
                case 'b':
                    unpacked = *packedLEData++;
                    *unpacked_bytes += 1;
                    break;

                case 'w':
                    unpacked =  *packedLEData++;
                    unpacked |= *packedLEData++ << 8;
                    *unpacked_bytes += 2;
                    break;

                case 'd':
                    unpacked =  *packedLEData++;
                    unpacked |= *packedLEData++ << 8;
                    unpacked |= *packedLEData++ << 16;
                    unpacked |= *packedLEData++ << 24;
                    *unpacked_bytes += 4;
                    break;

                default:
                    return RM_ERROR;
            }
            *unpackedData++ = unpacked;
            (*fields)++;
        }
    }

    return RM_OK;
}

RM_STATUS BiosReadStructure(
    PHWINFO     pDev,
    void       *structure,
    U032        offset,
    U032       *ppacked_size,
    const char *format
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 packed_size;
    void *packed_data = 0;
    U032 fields;
    U032 unpacked_bytes;
    U008 small_buffer[256];

    // calculate the size of the data as indicated by its packed format.
    BiosCalculateSizes(format, &packed_size, (U032 *) 0);
    if (ppacked_size)
        *ppacked_size = packed_size;

    if (packed_size <= sizeof(small_buffer))
        packed_data = small_buffer;
    else
    {
        rmStatus = osAllocMem((VOID **) &packed_data, packed_size);
        if (rmStatus != RM_OK)
            goto done;
    }

    rmStatus = BiosReadBytes(pDev, (U008*) packed_data, offset, packed_size);
    if (rmStatus != RM_OK)
        goto done;

    rmStatus = BiosUnpackLittleEndianStructure(packed_data, structure, format, &fields, &unpacked_bytes);

 done:
    if (packed_data && (packed_data != small_buffer))
        osFreeMem(packed_data);

    return rmStatus;
}

RM_STATUS BiosReadArray(
    PHWINFO     pDev,
    void       *buffer,
    U032        offset,
    U032        count,
    U032       *ppacked_size,
    const char *format
)
{
    U032 packed_size, padded_size;
    U032 i;
    
    BiosCalculateSizes(format, &packed_size, &padded_size);

    for (i=0; i<count; i++)
    {
        if (RM_OK != BiosReadStructure(pDev,
                                       (U008*)buffer + (i * padded_size),
                                       offset + (i * packed_size),
                                       (U032 *) 0,
                                       format))
            return RM_ERROR;
    }

    // packed_size will be size of 1 element, what is total?
    if (ppacked_size) *ppacked_size = count * packed_size;

    return RM_OK;
}

U008 BiosRead8(
    PHWINFO  pDev,
    U032     offset
)
{
    bios_U008 data;     // BiosReadStructure expects 'bios' types

    (void) BiosReadStructure(pDev, &data, offset, (U032 *) 0, "b");

    return (U008) data;
}


U016 BiosRead16(
    PHWINFO  pDev,
    U032     offset
)
{
    bios_U032 data;     // BiosReadStructure expects 'bios' types

    (void) BiosReadStructure(pDev, &data, offset, (U032 *) 0, "w");
    return (U016) data;
}

U032 BiosRead32(
    PHWINFO  pDev,
    U032     offset
)
{
    bios_U032 data;     // BiosReadStructure expects 'bios' types

    (void) BiosReadStructure(pDev, &data, offset, (U032 *) 0, "d");

    return (U032) data;
}


RM_STATUS BiosFindString(
    PHWINFO pDev,
    U008 string[],
    U032 *offset,
    U032 string_size
)
{
    int i, j;
    int bios_size;

    bios_size = BIOS_ROM_CODE_SIZE - string_size + 1;
    for (i = 0; i < bios_size; i++)   /* i is offset into bios */
    {
        j = 0;  /* j is offset into search string */

        /* while the bios substring matches the desired string.. */
        while (string[j] == BIOS_RD08(i + j)) j++;

        /* if we're on the null character, we found the string.. */
        if (string[j] == NULL)
        {
            *offset = i;
            return RM_OK;
        }
    }
    return RM_ERROR;
}


RM_STATUS BiosGetMaxExternalPCLKFreq
(
    PHWINFO pDev,
    U032 head,
    U032* freq
)
{
    // Try to find the frequency of an external DAC for the given head
    // The frequency return is in freq(Mz)*100
    RM_STATUS status = RM_OK;
    DEV_HEAD dev_head;
    U032 dev_offset = 0; 
    U032 offset = 0;
    U032 size = 0;

    // Display_Devices offset is found at offset 0x0036
    offset = BiosRead16( pDev, 0x0036 );

    if ( offset == 0 )
    {
        status = RM_ERROR;
    }
    else
    {
        // Read header to check version
        status = BiosReadStructure( pDev, &dev_head, offset, &size, DEV_HEAD_FMT );
        dev_offset = offset + size;

        if ( status == RM_OK )
        {
            if ( dev_head.version >= 0x13 )
            {
                // Start reading DEVICE structures 1 at a time until we hit the end
                BOOL bDone = FALSE;
                int i = 0;

                while ( !bDone )
                {
                    DEV_REC_STRUC dev_rec_struc;

                    status = BiosReadStructure( pDev, &dev_rec_struc, dev_offset, &size, DEV_REC_STRUC_FMT );
                    dev_offset = dev_offset + size;
                    i++;

                    // Check for end 
                    if ( dev_rec_struc.dev_type == DEV_REC_UNUSED_DEV )
                    {
                        status = RM_ERROR;
                        bDone = TRUE;
                    }
                    else if ( ( dev_rec_struc.dev_type & DEV_REC_CRT_DEV_MASK ) == 
                        ( DEV_REC_CRT_DEV_EXTERNAL_DAC | ( head << DEV_REC_HEAD_SHIFT ) ) )
                    {
                        // We found an external DAC
                        *freq = dev_rec_struc.data_rsvr;
                        status = RM_OK;
                        bDone = TRUE;
                    }
                    else if ( i > 128 )
                    {
                        // This is a safety check for a bad BIOS
                        // We should never ever hit this!
                        status = RM_ERROR;
                        bDone = TRUE;
                    }
                }
            }
            else
            {
                status = RM_ERROR;
            }
        }
    }

    return status;
}

