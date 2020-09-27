/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/
 
/******************************************************************************
* Filename:      PackSC.h
*
* Purpose:       Bit packing for SC36 codecs
*
* Functions:     VoxPackSC3, VoxPackSC6, VoxUnPackSC3, VoxUnPackSC6
*
* Author/Date:   Bob Dunn (scarfed code from Zopf/Wang)
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
*    WARNING: This code is currently hard wired and will not work with 
*               various multiple quan_files!
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pack/PackSC.h_v   1.6   06 Apr 1998 17:32:44   weiwang  $
*
******************************************************************************/
#ifndef _PACKSC_H
#define _PACKSC_H

/******************************************************************************
*
* Function:  VoxPackSC3
*
* Action:    Pack quantization indices for SC3
*
* Input:     piQuanIndex -- quantization indices
*            pchPacketBuffer -- start pointer to the packet buffer
*            psByteOffset -- the pointer to the write byte offset
*            pchBitOffset -- the pointer to the write bit offset
*            sBufferSize -- packet buffer size
*            sLongerFrameFlag -- continuous flag

* Output:    pchPacketBuffer -- bit packed buffer
*            psByteOffset -- updated write byte offset
*            pchBitOffset -- updated write bit offset
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxPackSC3(int *piQuanIndex, unsigned char  *pchPacketBuffer,
                unsigned short *psByteOffset, unsigned char  *pchBitOffset,
                unsigned short  sBufferSize, unsigned short  sLongerFrameFlag);


/******************************************************************************
*
* Function:  VoxPackSC6
*
* Action:    Pack quantization indices for SC6
*
* Input:     piQuanIndex -- quantization indices
*            pchPacketBuffer -- start pointer to the packet buffer
*            psByteOffset -- the pointer to the write byte offset
*            pchBitOffset -- the pointer to the write bit offset
*            sBufferSize -- packet buffer size
*            sLongerFrameFlag -- continuous flag

* Output:    pchPacketBuffer -- bit packed buffer
*            psByteOffset -- updated write byte offset
*            pchBitOffset -- updated write bit offset
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxPackSC6(int *piQuanIndex, unsigned char *pchPacketBuffer,
                unsigned short *psByteOffset, unsigned char  *pchBitOffset,
                unsigned short  sBufferSize, unsigned short  sLongerFrameFlag);

/******************************************************************************
*
* Function:  VoxUnPackSC3
*
* Action:    Unpack quantization indices for SC3
*
* Input:     piQuanIndex -- pointer to the quantization indices
*            pchPacketBuffer -- packet buffer
*            psByteOffset -- the pointer to the read byte offset
*            pchBitOffset -- the pointer to the read bit offset
*            sBufferSize -- packet buffer size
*            psLongerFrameFlag -- pointer to the continuous flag
*
* Output:    piQuanIndex -- unpacked quantization indices
*            psByteOffset -- updated read byte offset
*            pchBitOffset -- updated read bit offset
*            psLongerFrameFlag -- continuous flag
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxUnPackSC3(int *piQuanIndex, unsigned char *pchPacketBuffer,
                  unsigned short *psByteOffset, unsigned char *pchBitOffset,
                  unsigned short psBufferSize, unsigned short *psLongerFrameFlag);


/******************************************************************************
*
* Function:  VoxUnPackSC6
*
* Action:    Unpack quantization indices for SC6
*
* Input:     piQuanIndex -- pointer to the quantization indices
*            pchPacketBuffer -- packet buffer
*            psByteOffset -- the pointer to the read byte offset
*            pchBitOffset -- the pointer to the read bit offset
*            sBufferSize -- packet buffer size
*            psLongerFrameFlag -- pointer to the continuous flag
*
* Output:    piQuanIndex -- unpacked quantization indices
*            psByteOffset -- updated read byte offset
*            pchBitOffset -- updated read bit offset
*            psLongerFrameFlag -- continuous flag
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxUnPackSC6(int *piQuanIndex, unsigned char *pchPacketBuffer,
                  unsigned short *psByteOffset, unsigned char *pchBitOffset,
                  unsigned short sBufferSize, unsigned short *psLongerFrameFlag);

#endif /* _PACKSC_H */
