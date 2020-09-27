#ifdef XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

#include <mmsystem.h>
#include "waveutil.h"

#ifdef XBOX
#define NO_COMPRESSION_SUPPORT
#define IGNORE_ACM(format) ((format->wFormatTag == WAVE_FORMAT_PCM) || (format->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)) 
#else
#define IGNORE_ACM(format) (format->wFormatTag == WAVE_FORMAT_PCM) 
#endif // XBOX

void CopyFormat
(
	LPWAVEFORMATEX 	pwfxDst,
	LPWAVEFORMATEX 	pwfxSrc
)
{
	DWORD	cbSize;

	cbSize = SIZEOFFORMAT(pwfxSrc);

	CopyMemory(pwfxDst, pwfxSrc, cbSize);
}

void CopyFormatEx
(
	LPWAVEFORMATEX 	pwfxDst,
	LPWAVEFORMATEX 	pwfxSrc
)
{
	DWORD	cbSize;

	cbSize = SIZEOFFORMATEX(pwfxSrc);

	CopyMemory(pwfxDst, pwfxSrc, cbSize);

	if (!IGNORE_ACM(pwfxDst))
	{
		pwfxDst->cbSize = 0;
	}
}

BOOL FormatCmp
(
	LPWAVEFORMATEX 	pwfx1,
	LPWAVEFORMATEX 	pwfx2
)
{
	DWORD	cbSize;

	cbSize = SIZEOFFORMAT(pwfx1);

	if (cbSize != SIZEOFFORMAT(pwfx2))
	{
		return (FALSE);
	}
	else
	{
		LPBYTE	pb1 = (LPBYTE)pwfx1, pb2 = (LPBYTE)pwfx2;

		for (; cbSize; cbSize--)
		{
			if (pb1[cbSize - 1] != pb2[cbSize - 1])
			{
				return (FALSE);
			}
		}
	}

	return (TRUE);
}


DWORD DeinterleaveBuffers
(
	LPWAVEFORMATEX 	pwfx,
	LPBYTE 			pSrc,
	LPBYTE 			*ppbDst,
	DWORD 			cBuffers,
	DWORD 			cbSrcLength,
	DWORD 			dwOffset
)
{
	DWORD			ii;

	//	Note:  Right now, we don't deal with anything but PCM
    //  We need to handle the interleaved XBOXADPCM case, which interleaves on 36 byte boundaries.

	if (!IGNORE_ACM(pwfx))
	{
		return (dwOffset);
	}

	if (1 == cBuffers)
	{
		LPBYTE	pDst = ppbDst[0];

		pDst += dwOffset;

		CopyMemory(pDst, pSrc, cbSrcLength);

		dwOffset += cbSrcLength;
		return (dwOffset);
	}

	if (8 == pwfx->wBitsPerSample)
	{
        if (pwfx->wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
        {
		    PDWORD	*ppdwDst = (PDWORD*)ppbDst;
		    PDWORD	pdwSrc   = (PDWORD)pSrc;
		    // Convert byte index into DWORD index
		    //
            dwOffset /= sizeof(DWORD);
            // ADPCM uses 36-byte packets. It's bad news if we're
            // asked to deinterleave and do not have an integral
            // number of packets.
            DWORD dwPacketSize = 36 * cBuffers;
            if(cbSrcLength != (cbSrcLength / dwPacketSize) * dwPacketSize){
                return (0);
            }

            DWORD dwStep = sizeof(DWORD) * cBuffers;
            
		    for (DWORD i = 0; i < cbSrcLength; i += dwStep)
		    {
			    for (ii = 0; ii < cBuffers; ii++)
			    {
				    ppdwDst[ii][dwOffset] = *pdwSrc++;
			    }

			    dwOffset++;
		    }

		    // Return byte index
		    //
		    dwOffset *= sizeof(DWORD);

		    return (dwOffset);

        }
		if ( !cBuffers || (0 != (cbSrcLength % cBuffers)) )
		{
			//  Hmm... Buffer sizes don't line up!

			return (dwOffset);
		}

		for (; cbSrcLength; cbSrcLength -= cBuffers)
		{
			for (ii = 0; ii < cBuffers; ii++)
			{
				ppbDst[ii][dwOffset] = *pSrc++;
			}

			dwOffset++;
		}

		return (dwOffset);
	}
	else
	{
		PWORD	*ppwDst = (PWORD*)ppbDst;
		PWORD	pwSrc   = (PWORD)pSrc;

		//  Assuming 16-bit...

		if ( !cBuffers || (0 != (cbSrcLength % (cBuffers * 2))) || (0 != (dwOffset % sizeof(WORD))))
		{
			//	Hmm... Buffer sizes don't line up!

			return (0);
		}

		// Convert byte index into word index
		//
        dwOffset /= sizeof(WORD);            
		for (; cbSrcLength; cbSrcLength -= (2 * cBuffers))
		{
			for (ii = 0; ii < cBuffers; ii++)
			{
				ppwDst[ii][dwOffset] = *pwSrc++;
			}

			dwOffset++;
		}

		// Return byte index
		//
		dwOffset *= sizeof(WORD);

		return (dwOffset);
	}
}
