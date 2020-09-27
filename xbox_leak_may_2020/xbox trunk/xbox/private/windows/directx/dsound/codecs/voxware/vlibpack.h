/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*                                                                              *
*******************************************************************************/
 
/*******************************************************************************
*
* File:          vLibPack.h
*
* Purpose:
*
* Functions:
*
* Author/Date:
*******************************************************************************
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vLibPack.h_v   1.1   07 Apr 1998 18:02:36   weiwang  $
******************************************************************************/
 
#ifndef _VLIB_PACK
#define _VLIB_PACK

#ifdef __cplusplus
extern "C" {
#endif

/*
   The definition of structure:
   Note : The word Byte in below variables should really be char to reflect that 
   on some platforms a char is not necessarily 1 byte. However, for
   historical reasons, the variables will keep there names.
*/


typedef struct
{
  unsigned char  *pBuffer;       /* The circular buffer */
                                 
  unsigned short wBufferSize;     /* Size of VoxBuffer in char               */  

  unsigned short *pBufferReadByteOffset; /* The offset in chars (from the beginning 
                                            of the buffer) of the first char to    
                                            start reading from. */


  unsigned char *pBufferReadBitOffset;  /* The offset in bits (from the most sign-
                                           ificant bit) to start reading from. */

  unsigned short *pBufferWriteByteOffset;/* The offset in chars (from the beginning
                                            of the buffer) of the first char to 
                                            start writing to.*/

  unsigned char *pBufferWriteBitOffset; /* The offset in bits (from the most sign-
                                           ificant bit) to start writing to.*/
} C_BUFFER_TYPE;


#define PACK_BITS_PER_CHAR   8    /* Defines the number of bits to be writt- */
                                  /* en into each char. This should always   */
                                  /* be 8 on platfroms that define char as 8 */
                                  /* On some DSP platforms, sizeof(char) is  */
                                  /* > than 8. The routine still works but   */
                                  /* there are wasted bytes. If desired this */
                                  /* constant can be changed to sizeof(char) */
                                  /* to avoid the waste.                     */

/******************************************************************************
*
* Function:  BitPackCBuffer
*
* Action:    Packs a bitstream into a circular bitstream buffer.
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*                       The data struct is defined in BitPackC.h. This routine
*                       only writes to the buffer, thus the read offsets are 
*                       not needed. 
*            cElement -- The char containing the data
*            cDataWidth -- The number of bits to be written into the buffer.
*
* Output:    CBuffer -- The circular buffer, the write-byte-offset and write-
*                       bit-offset are updated.
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void BitPackCBuffer(C_BUFFER_TYPE *CBuffer, unsigned char cElement,  
                    short cDataWidth);


/******************************************************************************
*
* Function:  BitUnPackCBuffer
*
* Action:    Unpacks a bitstream from a circular bitstream buffer.
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*                       The data struct is defined in BitPackC.h. This routine
*                       only reads to the buffer, thus the write offsets are 
*                       not needed. 
*            cElement -- The char containing the data
*            cDataWidth -- The number of bits to be read from the buffer.
*
* Output:    CBuffer -- The read-byte-offset and read-bit-offset are updated.
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void BitUnpackCBuffer(C_BUFFER_TYPE *CBuffer, unsigned char *cElement, 
                      short cDataWidth);

/******************************************************************************
*
* Function:  copyCBuf2LBuf
*
* Action:    copy circular buffer to linear buffer
*
* Input:     CBuffer -- The circular buffer into which data is to be packed. 
*            ByteOffset -- byte offset 
*            BitOffset -- bit offset
*            BufferSize -- circular buffer size
*            bitsToCopy  -- how many bits need to copy to linear buffer
*            LBuffer -- linear buffer to copy to 
*
* Output:    ByteOffset -- updated byte offset
*            BitOffset -- updated bit offset
*            LBuffer  -- updated linear buffer
*
* Globals:   none
*
* Return:    void
******************************************************************************/
void copyCBuf2LBuf(unsigned char *CBuffer,    unsigned short *ByteOffset,  
                   unsigned char *BitOffset,  unsigned short  BufferSize,
                   unsigned long  bitsToCopy, unsigned char  *LBuffer);

#ifdef __cplusplus
}
#endif

#endif /* _VLIB_PACK */



