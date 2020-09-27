/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciSC3.h
*
* Purpose:         Defines the interface functions for the Voxware SC Quality codec
*
* Functions:   vciGetInfoSC3, vciEncodeSC3, vciDecodeSC3, 
*              vciInitEncodeSC3, vciInitDecodeSC3, vciFreeEncodeSC3,
*              vciFreeDecodeSC3, vciGetExtCompatibilitySC3
*
* Author/Date:     Ilan Berci, Epiphany Vera 97/05/19
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/vci/vciSC3.h_v   1.2   16 Mar 1998 17:26:40   weiwang  $
******************************************************************************/
#ifndef _VCISC3
#define _VCISC3

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef _VCI_RETCODE_
#define _VCI_RETCODE_
typedef unsigned short VCI_RETCODE; /* Voxware Core Interface error type.    */
                                    /* This variable is also defined in vci.h*/
#endif  /* _VCI_RETCODE_ */

/******************************************************************************
*
* Function:  vciGetInfoSC3()        
*
* Action:    Fill up information block for SC3
*
* Input:    pvciCodecInfoBlk -- pointer to the info. block
*
* Output:   pvciCodecInfoBlk -- filled up info. block
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciGetInfoSC3(VCI_CODEC_INFO_BLOCK *pvciCodecInfoBlk);

/******************************************************************************
*
* Function:  vciEncodeSC3()     
*
* Action:    Encodes one frame of speech using SC3.
*
* Input:    pEncodeMemBlk -- encoder structure
*           pvciCodecIOBlk -- codec I/O structure
*
* Output:   pEncodeMemBlk -- updated strcture
*           pvciCodecIOBlk -- updated strcture
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciEncodeSC3(void *pEncodeMemBlk, 
                         VCI_CODEC_IO_BLOCK *pvciCodecIOBlk);

/******************************************************************************
*
* Function:  vciDecodeSC3()
*
* Action:    Decodes one frame of speech using SC3.
*
* Input:    pDecodeMemBlk -- decoder structure
*           pvciCodecIOBlk -- codec I/O structure
*
* Output:   pDecodeMemBlk -- updated strcture
*           pvciCodecIOBlk -- updated strcture
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciDecodeSC3(void *pDecodeMemBlk, 
                         VCI_CODEC_IO_BLOCK *pvciCodecIOBlk);

/******************************************************************************
*
* Function:  vciInitEncodeSC3()
*
* Action:    To allocate memory for a Codec Memory Block and to initialize
*            it as an encoder.  
*
* Input:    hEncodeMemBlk -- the pointer to the encoder structure
*
* Output:   hEncodeMemBlk -- memory allocated and initialized encoder structure
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciInitEncodeSC3(void **hEncodeMemBlk);

/******************************************************************************
*
* Function:  vciInitDecodeSC3()
*
* Action:    To allocate memory for a Codec Memory Block and to initialize
*            it as a decoder.
*
* Input:    hDecodeMemBlk -- the pointer to the decoder structure
*
* Output:   hDecodeMemBlk -- allocated  and initilized structure
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciInitDecodeSC3(void **hDecodeMemBlk);

/******************************************************************************
*
* Function:  vciFreeEncodeSC3()
*
* Action:    Frees memory assigned to the encoder.
*
* Input:    hEncodeMemBlk -- the pointer to the encoder structure
*
* Output:   hEncodeMemBlk -- freed memory 
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciFreeEncodeSC3(void **hEncodeMemBlk);


/******************************************************************************
*
* Function:  vciFreeDecodeSC3()
*
* Action:    Frees memory assigned to the decoder.
*
* Input:    hDecodeMemBlk -- the pointer to the decoder structure
*
* Output:   hDecodeMemBlk -- freed memory 
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/

VCI_RETCODE vciFreeDecodeSC3(void **hDecodeMemBlk);

/******************************************************************************
*
* Function:  vciGetExtCompatibilitySC3()  
*
* Action:    Returns compatibility function 
*
* Input:    pCompatFunc -- the function pointer to the compatibility function
*
* Output:   pCompatFunc -- the compatibility function
*
* Globals:   none
*
* Return:    VCI return error
*******************************************************************************/

VCI_RETCODE vciGetExtCompatibilitySC3(void **pCompatFunc);

#define END_OF_FRAME     0
#define LONGER_FRAME     1

#ifdef __cplusplus
}
#endif

#endif /* _VCISC3 */



