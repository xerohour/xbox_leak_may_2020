// !!!!!!!!! DO NOT RELEASE THIS SOURCE FILE !!!!!!!

/*   Copyright (c) 1998  S3 Inc.  All Rights Reserved.
 *
 *   Module Name:  s3_intrf.c
 *
 *   Purpose:  highlevel interface of S3TC to DX surface
 *
 *   Author:        Dan Hung, Martin Hoffesommer
 *   Maintained by: Dan Hung
 *
 *   Revision History:
 *      -> 04/09/98 : found bad assertion in S3TCencode(), commented it out
 *      -> 05/15/98 : fixed block ordering to be as in dx spec.  alpha blocks
 *                      coded first, then rgb blocks.
 *      -> 05/18/98 : fixed bug in S3TCdecode().
 *      -> 06/01/98 : fixed alpha compare bug.
 *      -> 06/01/98 : fixed colorkey bug for non-RGB888 surfaces.
 *      04.03.01 - Cleaned up a bit and added to XGraphics.lib (JHarding)
 *
 *  version Beta 1.00.00-98-06-01
 */

// !! NOTE !! "//@@@" indicates uncertain assumption or future work needed...

// Highlevel interface, implementation

#include <crtdbg.h>
#include <math.h>
#include "S3_intrf.h"
#include "S3_quant.h"


#if 0
// determine number of bytes needed to compress given source image
unsigned S3TCgetEncodeSize(LPDIRECT3DTEXTURE8 lpDesc, unsigned dwEncodeType)
{
    _ASSERTE(lpDesc!=NULL);
    _ASSERTE(lpDesc->Size=sizeof(D3DSURFACE_DESC));
    _ASSERTE((lpDesc->dwFlags&(DDSD_WIDTH|DDSD_HEIGHT))==(DDSD_WIDTH|DDSD_HEIGHT));

// calculate number of blocks and multiply by the number of bytes required per block
// if encoding alpha separately, requires 16 bytes per block, else 8 bytes
    return ((lpDesc->Width+3)/4)*((lpDesc->Height+3)/4)*    
                ((dwEncodeType&_S3TC_ENCODE_ALPHA_MASK)?16:8);
}
#endif

// determine number of shifts for given bit mask
_inline unsigned getNrOfShifts(DWORD dwMask)
{
    _ASSERTE(dwMask);

    // Bit scan forward returns index of 
    // least significant set bit
    _asm { 
        bsf eax, [dwMask]
        mov dwMask, eax
    }

    return dwMask;
}

// Pixel formats for various texture formats we'd like to compress
typedef struct {
    DWORD dwRMask;
    DWORD dwGMask;
    DWORD dwBMask;
    DWORD dwAMask;
    DWORD bpp;
} FORMAT_INFO;


//-----------------------------------------------------------------------------
// Name: GetFormatInfo
// Desc: Returns information about the specified format:
//       * Mask information for each channel of data
//       TODO: Add linear formats and missing PC formats
//-----------------------------------------------------------------------------
HRESULT GetFormatInfo( D3DFORMAT fmt, FORMAT_INFO * pfi )
{
    memset( pfi, 0, sizeof( FORMAT_INFO ) );

    switch( fmt )
    {
    case D3DFMT_LIN_A8R8G8B8:
        pfi->dwAMask = 0xFF000000;
        // Fall through
    case D3DFMT_LIN_X8R8G8B8:
        pfi->dwRMask = 0x00FF0000;
        pfi->dwGMask = 0x0000FF00;
        pfi->dwBMask = 0x000000ff;
        pfi->bpp     = 4;
        break;

    case D3DFMT_LIN_R5G6B5:
        pfi->dwRMask = 0x0000F800;
        pfi->dwGMask = 0x000007E0;
        pfi->dwBMask = 0x0000001F;
        pfi->bpp     = 2;
        break;

    case D3DFMT_LIN_A1R5G5B5:
        pfi->dwAMask = 0x00008000;
        // Fall through
    case D3DFMT_LIN_X1R5G5B5:
        pfi->dwRMask = 0x00007C00;
        pfi->dwGMask = 0x000003E0;
        pfi->dwBMask = 0x0000001F;
        pfi->bpp     = 2;
        break;

    case D3DFMT_LIN_A4R4G4B4:
        pfi->dwAMask = 0x0000F000;
        pfi->dwRMask = 0x00000F00;
        pfi->dwGMask = 0x000000F0;
        pfi->dwBMask = 0x0000000F;
        pfi->bpp     = 2;
        break;
                
    case D3DFMT_LIN_R6G5B5:
        pfi->dwRMask = 0x0000FC00;
        pfi->dwGMask = 0x000003E0;
        pfi->dwBMask = 0x0000001F;
        pfi->bpp     = 2;
        break;

    case D3DFMT_LIN_A8B8G8R8:
        pfi->dwAMask = 0xFF000000;
        pfi->dwBMask = 0x00FF0000;
        pfi->dwGMask = 0x0000FF00;
        pfi->dwRMask = 0x000000ff;
        pfi->bpp     = 4;
        break;

    case D3DFMT_LIN_B8G8R8A8:
        pfi->dwBMask = 0xFF000000;
        pfi->dwGMask = 0x00FF0000;
        pfi->dwRMask = 0x0000FF00;
        pfi->dwAMask = 0x000000ff;
        pfi->bpp     = 4;
        break;

    case D3DFMT_LIN_R4G4B4A4:
        pfi->dwRMask = 0x0000F000;
        pfi->dwGMask = 0x00000F00;
        pfi->dwBMask = 0x000000F0;
        pfi->dwAMask = 0x0000000F;
        pfi->bpp     = 2;
        break;

    case D3DFMT_LIN_R5G5B5A1:
        pfi->dwRMask = 0x0000F800;
        pfi->dwGMask = 0x000007C0;
        pfi->dwBMask = 0x0000003E;
        pfi->dwAMask = 0x00000001;
        pfi->bpp     = 2;
        break;

    case D3DFMT_LIN_R8G8B8A8:
        pfi->dwRMask = 0xFF000000;
        pfi->dwGMask = 0x00FF0000;
        pfi->dwBMask = 0x0000FF00;
        pfi->dwAMask = 0x000000ff;
        pfi->bpp     = 4;
        break;

    default:
        return E_INVALIDARG;
    }

    return S_OK;
}

#if _DEBUG
BOOL IsValidCoding( D3DFORMAT format, RGBBlock b )
{
    // For DXT1, we can take a 3- or 4-color ramp.
    if( format == D3DFMT_DXT1 )
        return true;

    // For DXT2-5, it has to be a 4-color ramp, so
    // assert we either got 4 levels out, or that
    // all quantized values are identical (in that
    // case, the quantizer doesn't set the endpoints
    // to be equal).
    if( b.outLevel == 4 )
        return true;

    for( int i = 1; i < b.n; i++ )
    {
        if( b.index[ i ] != b.index[ 0 ] )
            return false;
    }

    return true;
}
#endif // _DEBUG

//-----------------------------------------------------------------------------
// Name: XGCompressRect
// Desc: Compresses a texture to the specified buffer in the given format
// This is _almost_ unchanged code from s3.  The top of the function dealing
// with argument checking, texture formats, etc. is new.  Also, some bad loops
// were changed.  This means there may be many opportunities for optimization
//-----------------------------------------------------------------------------
extern "C"
HRESULT WINAPI XGCompressRect( LPVOID pDestBuf,
                               D3DFORMAT DestFormat,
                               DWORD dwDestPitch,
                               DWORD dwWidth,
                               DWORD dwHeight,
                               LPVOID pSrcData,
                               D3DFORMAT SrcFormat,
                               DWORD dwSrcPitch,
                               FLOAT fAlphaRef,
                               DWORD dwFlags )
{
    FORMAT_INFO fi;
    int y;                      // current Y line (steps 4 pixels each)
    unsigned char *lpSrcBuf;    // current beginning of line in source buffer
    unsigned char *lpDstBuf;    // current destination buffer pointer
    int bpp;                    // byte per pixel
    unsigned rgbShift[3];       // bit shifts for given color component
    unsigned aShift;            // bit shift for alpha component
    unsigned aRef;              // adjusted alpha reference value
    DWORD *  rgbBitMask;        // pointer to rgb bit mask
    int k;
    DWORD dwEncodeType;
    BYTE * pbSrc = NULL;
    FLOAT weight[3] = { 0.3086f, 0.6094f, 0.0820f };
    DWORD dwBytesPerBlock;
    DWORD dwWidthInBytes;
    DWORD DestPitchIncrement;   // Number of BYTEs to increment dest ptr, per row
    BOOL bPreMultiply = dwFlags & XGCOMPRESS_PREMULTIPLY;

    // Check input data and destination buffer
    if( !pSrcData || !pDestBuf )
        return E_INVALIDARG;
    
    // Make sure the format makes sense
    if( DestFormat != D3DFMT_DXT1 &&
        DestFormat != D3DFMT_DXT2 &&
        DestFormat != D3DFMT_DXT4 )
        return E_INVALIDARG;

    // Must be power-of-2 dimensions
    if( dwWidth & ( dwWidth - 1 ) )
        return E_INVALIDARG;
    if( dwHeight & ( dwHeight - 1 ) )
        return E_INVALIDARG;

    // Get info on the source texture format
    if( FAILED( GetFormatInfo( SrcFormat, &fi ) ) )
        return E_INVALIDARG;

    rgbBitMask = &fi.dwRMask;

    switch( DestFormat )
    {
    case D3DFMT_DXT1:
        dwEncodeType = S3TC_ENCODE_RGB_ALPHA_COMPARE|S3TC_ENCODE_ALPHA_NONE;
        dwBytesPerBlock = 8;
        break;
    case D3DFMT_DXT2:
        dwEncodeType = S3TC_ENCODE_RGB_FULL|S3TC_ENCODE_ALPHA_EXPLICIT;
        dwBytesPerBlock = 16;
        break;
    case D3DFMT_DXT4:
        dwEncodeType = S3TC_ENCODE_RGB_FULL|S3TC_ENCODE_ALPHA_INTERPOLATED;
        dwBytesPerBlock = 16;
        break;
    }

    if( dwFlags & XGCOMPRESS_NEEDALPHA0 )
        dwEncodeType |= S3TC_ENCODE_ALPHA_NEED0;
    if( dwFlags & XGCOMPRESS_NEEDALPHA1 )
        dwEncodeType |= S3TC_ENCODE_ALPHA_NEED1;
    if( dwFlags & XGCOMPRESS_PROTECTNONZERO )
        dwEncodeType |= S3TC_ENCODE_ALPHA_PROTECTNONZERO;

    dwWidthInBytes = ( dwWidth >> 2 ) * dwBytesPerBlock;

    // Calculate default pitches, if not specified.
    if( dwSrcPitch == 0 )
        dwSrcPitch = dwWidth * fi.bpp;
    if( dwDestPitch == 0 )
        dwDestPitch = dwWidthInBytes;

    // Number of bytes to skip for each row is the
    // difference between the dest pitch and the width
    // of the compressed data.  Normally, this will be
    // zero, unless a wider pitch was specified.  If this
    // is less than zero, then we were given an invalid
    // pitch
    if( dwDestPitch < dwWidthInBytes )
        return E_INVALIDARG;

    DestPitchIncrement = dwDestPitch - dwWidthInBytes;
    
    // precompute some useful values
    // XGBytesPerPixelFromFormat only understands xbox-flavored formats
    // bpp = XGBytesPerPixelFromFormat( SrcFormat );
    bpp = fi.bpp;
    rgbShift[0] = getNrOfShifts( fi.dwRMask );
    rgbShift[1] = getNrOfShifts( fi.dwGMask );
    rgbShift[2] = getNrOfShifts( fi.dwBMask );
    aShift      = fi.dwAMask ? getNrOfShifts( fi.dwAMask ) : 0;
    if ((dwEncodeType&_S3TC_ENCODE_RGB_MASK)==S3TC_ENCODE_RGB_ALPHA_COMPARE)
    {
        aRef = DWORD( fAlphaRef * ( fi.dwAMask >> aShift ) ) << aShift;
    }

    // now encode stuff (...)
    // Note, we read from the pointer that was given, so it better not be
    // swizzled, and if you want this fast, it better not be write-combined
    // memory
    lpSrcBuf = (unsigned char *)pSrcData;
    lpDstBuf = (unsigned char *)pDestBuf;

    // main y loop, go thru each upper-left most point on all 4x4 blocks
    for ( y = 0; y < (int)dwHeight; y += 4, lpSrcBuf += dwSrcPitch * 4 )
    {
        int x;                      // current X pixel (steps 4 pixels each)
        unsigned char *lpSrcCur;    // pointer to beginning of current source block
        int blockHeight;            // height of current blocks in pixel
        
        blockHeight=min(dwHeight-y,4);
        lpSrcCur=lpSrcBuf;
        // main x loop, go thru each upper left most point on all 4x4 blocks
        for ( x = 0; x <( int)dwWidth; x += 4, lpSrcCur += bpp * 4 )
        {
            int blockWidth;             // width of current block in pixel
            RGBBlock b;                 // current source block
            int k;                      // misc loop var
            int index[16];              // index of encoded texels in RGBBlock
                                        // if index[i] < 0, texel i is not quantizable
                                        // either due to colorkey, alpha compare, or
                                        // outside image boundry
            unsigned short endPt[2];    // RGB565 encoded quantized end points
            int bSwapped;               // end points swapped?
            unsigned dwIndex=0;         // encoded index values

            for (k=0;k<3;k++) 
                    b.weight[k]=weight[k];
            
            blockWidth=min(dwWidth-x,4);
            b.n=0;

            // start by encoding alpha stuff...
            switch(dwEncodeType&_S3TC_ENCODE_ALPHA_MASK)
            {
                case S3TC_ENCODE_ALPHA_NONE:
                    break;
                case S3TC_ENCODE_ALPHA_EXPLICIT:
                    {
                        int iy;                     // inner block pixel loop variable
                        unsigned char *lpCur;       // current pixel

                        if( fi.dwAMask )
                        {
                            lpCur=lpSrcCur;

                            // go thru all pixels in the block and write out the alpha values
                            for ( iy = 0 ; iy < 4 ; iy++, lpCur += dwSrcPitch - 4 * bpp )
                            {
                                if (iy<blockHeight)         // skip if outside
                                {
                                    unsigned short val = 0; // current output value
                                    int ix;
                                    for (ix=0;ix<4;ix++,lpCur+=bpp)
                                    {
                                        DWORD dwCurTexel;
                                        switch( bpp )
                                        {
                                        case 4:dwCurTexel = *(DWORD *)lpCur;
                                            break;
                                        case 2:dwCurTexel = (DWORD)(*(WORD *)lpCur);
                                            break;
                                        case 1:dwCurTexel = (DWORD)(*(BYTE *)lpCur);
                                            break;
                                        }
                                        // pack it...
                                        if (ix<blockWidth)
                                            val=(val>>4) | ( unsigned short(floor(( ( dwCurTexel & fi.dwAMask ) >> aShift ) / float( fi.dwAMask >> aShift )* 15.0f + 0.5f )) << 12 );
                                        else
                                            val>>=4;
                                    }
                                    // write it out
                                    *(unsigned short *)lpDstBuf=val;
                                }
                                else
                                    *(unsigned short *)lpDstBuf=0;
                                lpDstBuf+=2;
                            }
                        }
                        else
                        {
                            // Set to opaque
                            memset( lpDstBuf, 0xFFFF, 8 );
                            lpDstBuf += 8;
                        }
                    } 
                    break;
                case S3TC_ENCODE_ALPHA_INTERPOLATED:
                    {
                        AlphaBlock a;               // current alpha source block
                        int ix,iy;                  // inner block pixel loop variable
                        unsigned char *lpCur;       // current pixel
                        unsigned val;               // current output value

                        if( fi.dwAMask )
                        {
                            lpCur=lpSrcCur;

                            a.n=0;
                            k=0;
                            // go thru all pixels in the block and set up the AlphaBlock structure a
                            for ( iy = 0; iy < 4; iy++, lpCur += dwSrcPitch - 4 * bpp )
                                for ( ix = 0; ix < 4; ix++, lpCur += bpp)
                                    if ( ix < blockWidth && iy < blockHeight )
                                    {
                                        DWORD dwCurTexel;
                                        switch( bpp )
                                        {
                                        case 4:dwCurTexel = *(DWORD *)lpCur;
                                            break;
                                        case 2:dwCurTexel = (DWORD)(*(WORD *)lpCur);
                                            break;
                                        case 1:dwCurTexel = (DWORD)(*(BYTE *)lpCur);
                                            break;
                                        }
                                        unsigned cur,max;

                                        cur = ( dwCurTexel & fi.dwAMask ) >> aShift;
                                        max = fi.dwAMask >> aShift;
                                        index[k++]=a.n;
                                        a.alpha[a.n++]=(cur==max)?1.f:cur/((float)max); // to ensure that 1.0 exists
                                    }
                                    else
                                        index[k++]=-1;

                            // set flags -> do we need total opaqueness and total transparecy
                            // explicitly?
                            a.need0=dwEncodeType&S3TC_ENCODE_ALPHA_NEED0;
                            a.need1=dwEncodeType&S3TC_ENCODE_ALPHA_NEED1;
                            a.protectnonzero=dwEncodeType&S3TC_ENCODE_ALPHA_PROTECTNONZERO;

                            // check to see if there are no points to be quantized...
                            if (a.n==0) {
                                // zero out endpoints (they are invalid)
                                a.endPoint[0]=a.endPoint[1]=0;
                                for (k=0;k<16;k++)
                                    _ASSERTE(index[k]==-1);
                            } else
                                // !!! ENTRY INTO S3TC QUANTIZER !!!
                                CodeAlphaBlock(&a);

                            // check if endpoints are equal, if so, default to 6 points,
                            // second endpoint invalid, so increment to indicate 6 points.
                            // (JHarding): If 2nd endpoint is 255, can't increment, so
                            //   decrement first endpoint and use second instead.
                            // force all indices to endpoint 0 (or 1).
                            if (a.endPoint[0]==a.endPoint[1]) {
                                if( a.endPoint[1] < 255 )
                                {
                                    a.endPoint[1]++;
                                    for (k=0;k<a.n;k++) {
                                        a.index[k]=0;
                                    }
                                }
                                else
                                {
                                    a.endPoint[0]--;
                                    for(k=0;k<a.n;k++) {
                                        a.index[k]=1;
                                    }
                                }
                                a.outLevel=6;
                            }

                            // need swapping?
                            if ((a.endPoint[0]>a.endPoint[1])==(a.outLevel==6))
                            {
                                int sw=a.endPoint[0];
                                a.endPoint[0]=a.endPoint[1];
                                a.endPoint[1]=sw;
                                bSwapped=1;
                            }
                            else
                                bSwapped=0;

                            // write out endpoints
                            for (k=0;k<2;k++)
                            {
                                _ASSERTE(a.endPoint[k]>=0&&a.endPoint[k]<=255);
                                *lpDstBuf++=(unsigned char)a.endPoint[k];
                            }

                            // handle indices
                            memset(lpDstBuf,0,6);
                            val=0;
                            k=0;
                            // go thru indices and pack them...
                            // note packing is done 3 bytes (24 bits) at a time
                            // which correspond to 8 texels or 2 4-texel rows at a time
                            for (iy=0;iy<4;iy++)
                            {
                                for (ix=0;ix<4;ix++,k++)
                                {
                                    val>>=3;
                                    if (index[k]>=0)
                                    {
                                        int curIndex;

                                        curIndex=a.index[index[k]];
                                        _ASSERTE(curIndex>=0&&curIndex<=7);
                                        // handle indices swapping if needed
                                        // mapping as follows if swapped: 
                                        // outLevel==8 : 0->1, 1->0, 2->7, 3->6, 4->5, 5->4, 6->3, 7->2
                                        // outLevel==6 : 0->1, 1->0, 2->5, 3->4, 4->3, 5->2, 6->6, 7->7
                                        if (bSwapped) {
                                            if (a.outLevel==8) {
                                                curIndex = (curIndex>1) ? (9-curIndex) : (curIndex==0?1:0);
                                            } else if (a.outLevel==6) {
                                                curIndex = (curIndex>5) ? curIndex : ( (curIndex>1)?(7-curIndex):(curIndex==0?1:0) );
                                            }

                                        }
                                        _ASSERTE(curIndex>=0&&curIndex<=7);
                                        // pack it...
                                        val|=curIndex<<21;
                                    }
                                }

                                // check to see if we've finished 2 texel rows...
                                // equivalent to having packed 3 bytes (24 bits)...
                                if (iy&1)
                                {
                                    // write out packed indices
                                    *(unsigned *)(lpDstBuf)|=val;
                                    // realign boundry
                                    lpDstBuf+=3;
                                    val=0;
                                }
                            }
                        }
                        else
                        {
                            *lpDstBuf++ = 0x00;
                            *lpDstBuf++ = 0xFF;
                            memset( lpDstBuf, 0xFFFF, 6 );
                            lpDstBuf += 6;
                        }
                    }
                    break;
                default:
                    _ASSERTE(0);
            }
            
            // now encode rgb stuff...
#if 0
            if (lpPal)
            {
                // palettized image
                int ix,iy;                  // inner block pixel loop variables
                unsigned char *lpCur;       // current pixel

                lpCur=lpSrcCur;
                switch(dwEncodeType&_S3TC_ENCODE_RGB_MASK)
                {
                    case S3TC_ENCODE_RGB_FULL:
                        // go thru all pixels in the block and set up the RGBBlock structure b
                        for (k=0,iy=0;iy<4;iy++,lpCur+=lpSrc->lPitch-4)
                            for (ix=0;ix<4;ix++,lpCur++)
                                if (ix<blockWidth&&iy<blockHeight)
                                {
                                    double *pChannel;
                                    PALETTEENTRY *pEntry=lpPal+*lpCur;

                                    index[k++]=b.n;
                                    pChannel=b.colorChannel[b.n++];
                                    *pChannel++=(double)pEntry->peRed/255.;
                                    *pChannel++=(double)pEntry->peGreen/255.;
                                    *pChannel++=(double)pEntry->peBlue/255.;
                                }
                                else
                                    index[k++]=-1;      // non-quantizable if outside image
                        break;
                    case S3TC_ENCODE_RGB_COLOR_KEY:
                        // go thru all pixels in the block and set up the RGBBlock structure b
                        //@@@ assumption: color key in palettized format
                        //@@@ is in XBGR8888 format
                        for (k=0,iy=0;iy<4;iy++,lpCur+=lpSrc->lPitch-4)
                            for (ix=0;ix<4;ix++,lpCur++)
                                if (ix<blockWidth&&iy<blockHeight)
                                {
                                    double *pChannel;
                                    PALETTEENTRY *pEntry=lpPal+*lpCur;
                                    // check for color key
                                    if ( ((*(unsigned *)pEntry)&0xffffff)<lpSrc->ddckCKSrcBlt.dwColorSpaceLowValue ||
                                         ((*(unsigned *)pEntry)&0xffffff)>lpSrc->ddckCKSrcBlt.dwColorSpaceHighValue )
                                    {
                                        index[k++]=b.n;
                                        pChannel=b.colorChannel[b.n++];
                                        *pChannel++=(double)pEntry->peRed/255.;
                                        *pChannel++=(double)pEntry->peGreen/255.;
                                        *pChannel++=(double)pEntry->peBlue/255.;
                                    }
                                    else
                                        index[k++]=-1;  // non-quantizable texel if colorkey
                                }
                                else
                                    index[k++]=-1;      // non-quantizable texel if outside image
                        break;
                    default:
                        _ASSERTE(0);
                }
            }
            else
#endif
            {
                // non-palettized image
                int ix,iy;                  // inner block pixel loop variables
                unsigned char *lpCur;       // current pixel

                lpCur=lpSrcCur;
                // encode RGB stuff first...
                switch(dwEncodeType&_S3TC_ENCODE_RGB_MASK)
                {
                    case S3TC_ENCODE_RGB_FULL:
                        // go thru all pixels in the block and set up the RGBBlock structure b
                        for ( k = 0, iy = 0; iy < 4; iy++, lpCur += dwSrcPitch - 4 * bpp )
                            for ( ix = 0; ix < 4; ix++, lpCur += bpp)
                                if ( ix < blockWidth && iy < blockHeight )
                                {
                                    double *pChannel;
                                    int i;

                                    index[k++]=b.n;
                                    pChannel=b.colorChannel[b.n++];

                                    DWORD dwCurTexel;
                                    switch( bpp )
                                    {
                                    case 4:dwCurTexel = *(DWORD *)lpCur;
                                        break;
                                    case 2:dwCurTexel = (DWORD)(*(WORD *)lpCur);
                                        break;
                                    case 1:dwCurTexel = (DWORD)(*(BYTE *)lpCur);
                                        break;
                                    }

                                    float fAlpha = ( bPreMultiply && fi.dwAMask ) ? ( ( dwCurTexel & fi.dwAMask ) >> aShift ) / float( fi.dwAMask >> aShift ): 1.0f;
                                    for (i=0;i<3;i++)
                                        *pChannel++ = fAlpha * 
                                                      ( ( dwCurTexel & rgbBitMask[i] ) >> rgbShift[i] ) /
                                                      ( (double)( rgbBitMask[i] >> rgbShift[i] ) );
                                } else
                                    index[k++]=-1;          // non-quantizable if outside image
                        break;
#if 0
                    case S3TC_ENCODE_RGB_COLOR_KEY:
                        // go thru all pixels in the block and set up the RGBBlock structure b
                        for (k=0,iy=0;iy<4;iy++,lpCur+=lpSrc->lPitch-4*bpp)
                            for (ix=0;ix<4;ix++,lpCur+=bpp)
                                if (ix<blockWidth&&iy<blockHeight)
                                {
                                    double *pChannel;
                                    int i;
                                    unsigned curVal=*(unsigned *)lpCur;
                                    curVal&=(rgbBitMask[0]|rgbBitMask[1]|rgbBitMask[2]);

                                    // color key testing
                                    if (curVal<lpSrc->ddckCKSrcBlt.dwColorSpaceLowValue||
                                        curVal>lpSrc->ddckCKSrcBlt.dwColorSpaceHighValue)
                                    {
                                        index[k++]=b.n;
                                        pChannel=b.colorChannel[b.n++];
                                        for (i=0;i<3;i++)
                                            *pChannel++=((curVal&rgbBitMask[i])>>rgbShift[i])/((double)(rgbBitMask[i]>>rgbShift[i]));
                                    }
                                    else
                                        index[k++]=-1;      // non-quantizable if colorkey
                                }   
                                else
                                    index[k++]=-1;          // non-quantizable if outside image
                        break;
#endif // 0
                    case S3TC_ENCODE_RGB_ALPHA_COMPARE:
                        // go thru all pixels in the block and set up the RGBBlock structure b
                        for ( k = 0, iy = 0; iy < 4; iy++, lpCur+= dwSrcPitch - 4 * bpp )
                            for ( ix = 0; ix < 4; ix++, lpCur += bpp)
                                if ( ix < blockWidth && iy < blockHeight )
                                {
                                    double *pChannel;
                                    int i;

                                    DWORD dwCurTexel;
                                    switch( bpp )
                                    {
                                    case 4:dwCurTexel = *(DWORD *)lpCur;
                                        break;
                                    case 2:dwCurTexel = (DWORD)(*(WORD *)lpCur);
                                        break;
                                    case 1:dwCurTexel = (DWORD)(*(BYTE *)lpCur);
                                        break;
                                    }

                                    // alpha compare
                                    if ( fi.dwAMask == 0 || ( dwCurTexel & fi.dwAMask ) > aRef )
                                    {
                                        index[k++]=b.n;
                                        pChannel=b.colorChannel[b.n++];
                                        for (i=0;i<3;i++)
                                            *pChannel++=((dwCurTexel&rgbBitMask[i])>>rgbShift[i])/((double)(rgbBitMask[i]>>rgbShift[i]));
                                    }
                                    else
                                        index[k++]=-1;      // non-quantizable if alpha compare
                                }
                                else
                                    index[k++]=-1;          // non-quantizable if outside image
                        break;
                    default:
                        _ASSERTE(0);
                }
            }

            _ASSERTE(k==16);

            // input quantization is set to 3 if there are non-quantizable texels
            // so a spare flag can be used to indicate such, else set to 4.
            b.inLevel=b.n<blockWidth*blockHeight?3:4;

            // check to see if there are no points to be quantized...
            if (b.n==0) {
                // zero out endpoints (they are invalid)
                for (k=0;k<2;k++)
                    b.endPoint[k][0]=b.endPoint[k][1]=b.endPoint[k][2]=0;
                for (k=0;k<16;k++)
                    _ASSERTE(index[k]==-1);
            } else
            {
                // Run the actual quantizer
                CodeRGBBlock(&b);

                _ASSERTE( IsValidCoding( DestFormat, b ) );
            }

            // retrieve endpoints
            for (k=0;k<2;k++)
            {
                _ASSERTE(b.endPoint[k][0]>=0&&b.endPoint[k][0]<=31);
                _ASSERTE(b.endPoint[k][1]>=0&&b.endPoint[k][1]<=63);
                _ASSERTE(b.endPoint[k][2]>=0&&b.endPoint[k][2]<=31);
                endPt[k]=(b.endPoint[k][0]<<11)|(b.endPoint[k][1]<<5)|b.endPoint[k][2];
            }

            // check if end-points are equal, if so, default to 3 points,
            // second endpoint invalid, so increment to indicate 3 points.
            // force all indices to endpoint 0.
            if (endPt[0]==endPt[1]) {
                endPt[1]++;
                for (k=0;k<b.n;k++) {
                    b.index[k]=0;
                }
                b.outLevel=3;
            }

            // swap needed?
            if ((endPt[0]>endPt[1])==(b.outLevel==3))
            {
                unsigned short sw=endPt[0]; endPt[0]=endPt[1]; endPt[1]=sw;
                bSwapped=1;
            }
            else
                bSwapped=0;

            // write out end-points
            for (k=0;k<2;k++,lpDstBuf+=2)
                *(unsigned short *)lpDstBuf=endPt[k];

            // go thru indices and pack them...
            for (k=15;k>=0;k--)
            {
                dwIndex<<=2;
                // check if texel is not quantizable, if so, set index to 3
                if (index[k]<0) {
                    // _ASSERTE((b.inLevel==3)&&(b.outLevel==3));  <- bad assertion, taken out 4/9/98
                    dwIndex|=3;
                } else {
                    _ASSERTE(b.index[index[k]]>=0&&b.index[index[k]]<b.outLevel);
                    // pack indices
                    dwIndex|=b.index[index[k]];
                    // if swapped, changed the index
                    if (bSwapped)
                    {
                        if (b.outLevel==4)
                            dwIndex^=1;
                        else
                            dwIndex^=((dwIndex&2)>>1)^1;
                    }
                }
            }

            // write out indices
            *(unsigned *)lpDstBuf=dwIndex;

            lpDstBuf+=4;

        }
        // Increment to next row of blocks in dest buffer
        lpDstBuf += DestPitchIncrement;
    }

    delete[] pbSrc;
    return S_OK;
}

#if 0
// determine number of bytes needed do decompress given compressed image
unsigned S3TCgetDecodeSize(LPDIRECT3DTEXTURE8 lpDesc)
{
    _ASSERTE(lpDesc!=NULL);
    _ASSERTE(lpDesc->dwSize=sizeof(D3DSURFACE_DESC));
    _ASSERTE((lpDesc->dwFlags&(DDSD_WIDTH|DDSD_HEIGHT))==(DDSD_WIDTH|DDSD_HEIGHT));
    _ASSERTE(lpDesc->ddpfPixelFormat.dwFourCC=S3TC_FOURCC);

    return lpDesc->dwWidth*lpDesc->dwHeight*4;
}

// decode single RGB block (does not handle index) -> just return 
// endpoint and interpolated colors
static void decodeRGB(unsigned short *color, unsigned *rgb)
{
    int k;
    unsigned c[2][3];

    // _ASSERTE(color[0]!=color[1]);  <- no longer used, take out 5/15/98

    // go thru two endpoints
    for (k=0;k<2;k++)
    {
        // do color expansion: 5 bit abcde -> 8 bit abcdeabc
        // do color expansion: 6 bit abcdef -> 8 bit abcdefab
        c[k][0]=(unsigned)((color[k]>>11)<<3)|(color[k]>>13);
        c[k][1]=(unsigned)(((color[k]>>5)&0x3f)<<2)|((color[k]>>9)&0x3);
        c[k][2]=(unsigned)((color[k]&0x1f)<<3)|((color[k]>>3)&0x3);

        // set endpoint colors
        rgb[k]=0xff000000|(c[k][0]<<16)|(c[k][1]<<8)|c[k][2];
    }

    // check quantization level
    if (color[0]>color[1])
    {
        // if 4, interpolate colors at one-third and two-third points
        rgb[2]=rgb[3]=0xff000000;
        for (k=0;k<3;k++)
        {
            rgb[2]|=((2*c[0][k]+c[1][k]+1)/3)<<(16-k*8);
            rgb[3]|=((c[0][k]+2*c[1][k]+1)/3)<<(16-k*8);
        }
    }
    else
    {
        // if 3, interpolate color at mid-point
        rgb[2]=0xff000000;
        rgb[3]=0x00ff00ff;          // third color undefined - possibly color key
                                    // @@@ set magenta FOR NOW...
        for (k=0;k<3;k++)
            rgb[2]|=((c[0][k]+c[1][k])/2)<<(16-k*8);
    }
}

// decode single alpha block (does not handle index) -> just return 
// endpoint and interpolated alpha values
static void decodeAlpha(unsigned char *alpha, unsigned *rgb)
{
    int k;

    _ASSERTE(alpha[0]!=alpha[1]);

    // go thru two endpoints
    for (k=0;k<2;k++)
        rgb[k]=alpha[k]?((alpha[k]<<24)|0xffffff):0;

    // check quantization level
    if (alpha[0]>alpha[1])
    {
        // if 8, interpolate values at 1/7,2/7,...6/7 points
        for (k=0;k<6;k++)
        {
            rgb[2+k]=((alpha[1]*(k+1)+alpha[0]*(6-k)+3)/7)<<24;
            if (rgb[2+k])
                rgb[2+k]|=0xffffff;
        }
    }
    else
    {
        // if 6, interpolate values at 1/5,2/5,...4/5 points
        for (k=0;k<4;k++)
        {
            rgb[2+k]=((alpha[1]*(k+1)+alpha[0]*(4-k)+2)/5)<<24;
            if (rgb[2+k])
                rgb[2+k]|=0xffffff;
        }
        // last two points are explicit totally transparent and totally opaque
        rgb[6]=0;
        rgb[7]=0xffffffff;
    }
}

// decode (decompress) from surface to surface
void S3TCdecode(LPDIRECT3DTEXTURE8 lpSrc,       // [in]
                LPDIRECT3DTEXTURE8 lpDest,      // [out]
                void *lpDestBufIn           // [in]
                )
{
    int y;                      // current Y line (steps 4 pixels each)
    unsigned char *lpSrcBuf;    // current source buffer pointer
    unsigned char *lpDstBuf;    // current beginning of line in destination buffer

    // parameter validation
    _ASSERTE(lpSrc!=NULL);
    _ASSERTE(lpSrc->dwSize=sizeof(D3DSURFACE_DESC));
    _ASSERTE(lpDest!=NULL);
    _ASSERTE(lpDest->dwSize=sizeof(D3DSURFACE_DESC));
    _ASSERTE((lpSrc->dwFlags&(DDSD_WIDTH|DDSD_HEIGHT|DDSD_LPSURFACE|DDSD_PIXELFORMAT))==
                             (DDSD_WIDTH|DDSD_HEIGHT|DDSD_LPSURFACE|DDSD_PIXELFORMAT));
    _ASSERTE(lpDestBufIn);

    // fill in lpDest data members
    lpDest->dwFlags=DDSD_WIDTH|DDSD_HEIGHT|DDSD_LPSURFACE|DDSD_PITCH|DDSD_PIXELFORMAT;
    lpDest->dwWidth=lpSrc->dwWidth;
    lpDest->dwHeight=lpSrc->dwHeight;
    lpDest->lpSurface=lpDestBufIn;
    lpDest->lPitch=lpSrc->dwWidth*4;
    lpDest->ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
    lpDest->ddpfPixelFormat.dwFlags=DDPF_RGB|DDPF_ALPHAPIXELS;
    lpDest->ddpfPixelFormat.dwRGBBitCount=32;
    lpDest->ddpfPixelFormat.dwRBitMask=0x00ff0000;
    lpDest->ddpfPixelFormat.dwGBitMask=0x0000ff00;
    lpDest->ddpfPixelFormat.dwBBitMask=0x000000ff;
    lpDest->ddpfPixelFormat.dwRGBAlphaBitMask=0xff000000;

    // now decompress...
    lpSrcBuf=(unsigned char *)lpSrc->lpSurface;
    lpDstBuf=(unsigned char *)lpDest->lpSurface;
    // main y loop, go thru each upper-left most point on all 4x4 blocks
    for (y=0;y<(int)lpSrc->dwHeight;y+=4,lpDstBuf+=4*lpDest->lPitch)
    {
        int x;                      // current X pixel (steps 4 pixels each)
        unsigned char *lpDstCur;    // pointer to beginning of current destination block
        int blockHeight;            // height of current blocks in pixel
        
        blockHeight=min(lpSrc->dwHeight-y,4);
        lpDstCur=lpDstBuf;
        // main x loop, go thru each upper-left most point on all 4x4 blocks
        for (x=0;x<(int)lpSrc->dwWidth;x+=4,lpDstCur+=16)
        {
            int blockWidth;             // width of current block in pixel
            int ix,iy;                  // misc loop var
            unsigned rgbValue[8];       // current RGB (including transparency)
            unsigned *lpCur;            // current pixel

            blockWidth=min(lpSrc->dwWidth-x,4);

            // alpha block
            switch(lpSrc->ddpfPixelFormat.dwRGBBitCount&_S3TC_ENCODE_ALPHA_MASK)
            {
                unsigned index;             // index for alpha encoding
                case S3TC_ENCODE_ALPHA_NONE:
                    break;
                case S3TC_ENCODE_ALPHA_EXPLICIT:
                    lpCur=(unsigned *)lpDstCur;
                    // go thru each texel in block and set alpha 
                    for (iy=0;iy<4;iy++,lpCur+=lpDest->dwWidth-4)
                    {
                        unsigned short index=*(unsigned short *)lpSrcBuf;
                        lpSrcBuf+=2;
                        if (iy<blockHeight)
                            for (ix=0;ix<4;ix++,index>>=4,lpCur++)
                                if (ix<blockWidth)
                                    *lpCur=(((unsigned)floor(((index&15)/15.f)*255.f))<<24)|0xffffff;
                    }
                    break;
                case S3TC_ENCODE_ALPHA_INTERPOLATED:
                    // decode reference alpha values
                    decodeAlpha(lpSrcBuf,rgbValue);
                    lpCur=(unsigned *)lpDstCur;
                    lpSrcBuf+=2;
                    index=*(unsigned *)(lpSrcBuf);
                    // go thru each texel in block and set alpha values as referenced via index
                    for (iy=0;iy<4;iy++,lpCur+=lpDest->dwWidth-4)
                    {
                        // if finished two texel rows, reset source byte boundry
                        if (iy==2)
                            index=*(unsigned *)(lpSrcBuf+3);
                        if (iy<blockHeight)
                            for (ix=0;ix<4;ix++,index>>=3,lpCur++)
                                if (ix<blockWidth)
                                    *lpCur=rgbValue[index&7];
                    }
                    lpSrcBuf+=6;
                    break;
            }

            // RGB block
            // decode referenced colors
            decodeRGB((unsigned short *)lpSrcBuf,rgbValue);
            lpSrcBuf+=4;

            lpCur=(unsigned *)lpDstCur;
            // go thru each texel in block and set color as referenced via index
            for (iy=0;iy<4;iy++,lpCur+=lpDest->dwWidth-4)
            {
                unsigned char index=*lpSrcBuf++;
                if (iy<blockHeight)
                    for (ix=0;ix<4;ix++,index>>=2,lpCur++)
                        if (ix<blockWidth) {
                            *lpCur&=0xff000000;
                            rgbValue[index&3]&=0x00ffffff;
                            *lpCur|=rgbValue[index&3];
                        }
            }
        }
    }
}
#endif // 0
