// !!!!!!!!! DO NOT RELEASE THIS SOURCE FILE !!!!!!!

/*
 *   Copyright (c) 1997-8  S3 Inc.  All Rights Reserved.
 *
 *   Module Name:  s3_quant.h
 *
 *   Purpose:   Headr file for <quantizer.c> - 
 *				S3 linear interpolation encoding and decoding
 *              mechanism for texture map compression.
 *
 *   Author:   Konstantine Iourcha, Dan Hung
 *
 *   Revision History:
 *      04.03.01 - Cleaned up a bit and added to XGraphics.lib (JHarding)
 *	version Beta 1.00.00-98-03-26
 */
#ifndef _S3_QUANT_H_

#define _S3_QUANT_H_


#define	MAX_PIXEL_PER_BLOCK		16

    /* RGB block structure, contains array of texels with colors in RGB888 
       format as well as variables for coded results			    */

typedef struct _RGBBlock {
    /* INPUTS */
    int n;			/* number of texels for  this block	    */
    double colorChannel[MAX_PIXEL_PER_BLOCK][3];	
				/* array of RGB channel values		
				   assumed to be  normalized to [0.,1.]	    */
    double weight[3];		/* quantization weights on each channel	    */
    int inLevel;		/* quantization level (type)	
				   3 - quantized to line with two endpoints
				       and single interpolant inbetween 
				       plus trancparency index if needed;
				   4 - quantized to line with two endpoints
				       and two interpolants inbetween	    */

    /* OUTPUTS */
    int outLevel;		/* actual number of quantized points	    */
    int endPoint[2][3];		/* quantized end points in RGB565 format,
				   unordered				    */
    int index[MAX_PIXEL_PER_BLOCK];		
				/* texel indices to quantized points	    */
} RGBBlock;

    /* Alpha block structure, contains array of texels with alpha in 
       8-bit format plus various variables for coded results		    */
typedef struct _AlphaBlock {
    /* INPUTS */
	int n;			/* number of alpha values for this block    */
	float alpha[MAX_PIXEL_PER_BLOCK];	
				/* array of alpha normalized to [0.,1.]	    */
	int need0;		/* !=0 if exact 0.0 is needed in compressed
				   format ==0 otherwise			    */
	int need1;		/* !=0 if exact 1.0 is needed in compressed
				    format==0 otherwise			    */
    int protectnonzero; /* != 0 if we need to ensure non-zero values
                        don't get quantized to 0 */

    /* OUTPUTS */
	int outLevel;		/* actual number of quantized points	    */
	int endPoint[2];	/* quantized end points in 8-bit alpha,
				   unordered				    */
	int index[MAX_PIXEL_PER_BLOCK];		
				/* alpha texel indices to quantized 
				   points				    */
} AlphaBlock;

    /* for details on the mathematical basis of this, see easys3tc.doc 
       "S3TC made easy"							*/

    /* function which encodes a block					*/
void CodeRGBBlock(RGBBlock *b);

void CodeAlphaBlock(AlphaBlock *b);
			   
#endif