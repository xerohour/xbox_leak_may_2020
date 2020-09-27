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
* File:         vLibQuan.h
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
* $Header:   /export/phobos/pvcs/pvcsproj/voxlib.prj/vlibquan.h_v   1.16   08 Apr 1998 17:44:52   weiwang  $
******************************************************************************/
 
#ifndef __VLIBQUAN
#define __VLIBQUAN

#ifdef __cplusplus
extern "C" {
#endif

/* SQ.h */
/*******************************************************************************
* Function:  ScalarQuant()
*
* Action:    Find index into codebook of the value closest to the
*              input variable.
*
* Input:     fValue:------- Input variable
*            pfCodeBook:--- Codebook valiables
*            iLength:------ Number of valiable in the codebook
*
* Output:    none
*
* Return:    index--------- Index to codebook
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
int ScalarQuant( float fValue, const float *pfCodeBook, int iLength );

/*******************************************************************************
* Function:  ScalarLUT
*
* Action:    Dequantisation.  Return the peoper valiable according to the
*              index of the codebook
*
* Input:     pfCodeBook:------ Codebook valiables
*            iLength:--------- Number of valiable in the codebook
*            index------------ Index to codebook
*
* Output:    none
*
* Return:    pfCodeBook[index]----chosen variable in the codebook
*******************************************************************************/
float ScalarLUT( const float *pfCodeBook, int iLength, int iIndex);



/* VQ.h */
/*******************************************************************************
* FUNCTION    : VectorQuant()
*
* PURPOSE     : Quantize a vector using trained codebook
*
* INPUT
*    pfVector:    Input vector
*    pfCodeBook:  Codebook vectors
*    iRows:       Dimension of the vector
*    iCBEntries:  Number of entries in the codebook
*
* OUTPUT        None
* RETURN        entry---- Chosen index of the codebook
*******************************************************************************/
int VectorQuant( const float *pfVector, int iRows, const float *pfCodeBook,
                 int iCBEntries );


/*******************************************************************************
* FUNCTION    : VectorLUT()
*
* PURPOSE     : Dequantize a vector from index of the codebook,
*
* INPUT
*    uiIndex:     Chosen index of the codebook
*    pfCodeBook:  Codebook vectors
*                   Codebook vectors are stored in a format of (row * column)
*                   i.e. dimention of component * entries of the codebook
*    iRows:       Dimension of each vector
*    iCBEntries:  Number of bits for the codebook
*
* OUTPUT
*    pfVector:    Chosen vector according to the index
*******************************************************************************/
void VectorLUT( unsigned int uiIndex, const float *pfCodeBook, int iCBEntries,
                int iRows, float *pfVector );
 

/* VQWeight.h */
/*******************************************************************************
* FUNCTION    : WeightedVectorQuant()
*
* PURPOSE     : Weighted vector quantization using a trained codebook
*
* INPUT
*    pfWeights:   Vector of weights
*    pfVector:    Input vector
*    pfCodeBook:  Codebook vectors
*    iRows:       Dimension of the vector
*    iCBEntries:  Number of entries in the codebook
*
* OUTPUT        None
* RETURN        entry---- Chosen index of the codebook
*******************************************************************************/
int WeightedVectorQuant( const float *pfWeights, const float *pfVector,
                         int iRows, const float *pfCodeBook, int iCBEntries );


/* VQ2.h */
/*******************************************************************************
* Function:  Dim2VectorQuantize()
*
* Action:    Encode a 2-dimensional input vector using a 2-D vector quantizer
*            with the straightforward mean squared error (MSE) criterion
*
* INPUT:     pfVector:   --- 2-dimensional input vector
*            pfCodeBook: --- one-dimensional codebook array containing
*                             (2 * iEntries) elements. Codebook vectors
*                             are stored in a format of (row * column)
*                             i.e. dimention of component * entries of
*                             the codebook
*            iEntries:   --- codebook size (number of 2-D codevectors)
*
* OUTPUT:    None
*
* RETURN:    Chosen index of the codebook
*******************************************************************************/
int Dim2VectorQuantize( const float *pfVector, const float *pfCodeBook,
                        int iEntries );

/*******************************************************************************
* Function:  Dim2VectorLUT()
*
* Action:    Decode the codebook index of a 2-dimensional vector quantizer
*
* Input:     iIndex:     --- the index of the selected codevector
*            pfCodeBook: --- one-dimensional codebook array containing
*                             (2 * iEntries) elements
*            iEntries:   --- number of entries in the codebook
*
* Output:    pfVector:   --- decoded 2-dimensional VQ output vector
*
* Author/Date:  Juin-Hwey (Raymond) Chen, June 9, 1997
*
* NOTE: the function VQ2dec() works only for the case of 2-dimensional VQ
*******************************************************************************/
void Dim2VectorLUT( unsigned int iIndex, const float *pfCodeBook,
                    int iEntries, float *pfVector );


#ifdef __cplusplus
}
#endif

#endif /* __VLIBQUAN */
