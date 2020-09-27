/***************************************************************************
 *
 *  Copyright (C) 2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       driver.cpp
 *  Content:    XBox ADPCM ACM driver.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  04/29/01    dereks  Created.
 *  06/12/01    jharding Adapted for command-line encode
 *
 ****************************************************************************/

#include "imaadpcm.h"
#include "resource.h"
#include "stdio.h"
#include "wavparse.h"

#define XBOX_ADPCM_SAMPLES_PER_BLOCK    64


//-----------------------------------------------------------------------------
// Name: GetSourceInfo
// Desc: Parses source wav file and gets sample data and format information
//-----------------------------------------------------------------------------
HRESULT
GetSourceInfo( const char * szInput, BYTE ** ppbSampleData, DWORD *pdwDuration, DWORD * pdwPadding, WAVEFORMATEX * pwfx )
{
    CWaveFile wfSrc;                // Source file

    *ppbSampleData = NULL;
    *pdwDuration = 0;
    *pdwPadding = 0;
    ZeroMemory( pwfx, sizeof( WAVEFORMATEX ) );

    // Open the source wav file
    if( FAILED( wfSrc.Open( szInput ) ) )
    {
        printf( "Couldn't open %s.\n", szInput );
        return E_FAIL;
    }

    // Get the format and size of the source data
    wfSrc.GetFormat( pwfx, sizeof( WAVEFORMATEX ) );
    wfSrc.GetDuration( pdwDuration );

    // Don't compress 8-bit sounds - the codec can't reliably do it, and
    // it would sound bad, anyway.
    if( pwfx->wBitsPerSample != 16 )
    {
        printf( "You can only compress 16-bit sources - 8 bits would sound bad, anyway.\n" );
        return E_FAIL;
    }

    // See if we need to pad the end to a multiple of 64 samples
    DWORD dwSamples = *pdwDuration / pwfx->nBlockAlign;
    if( dwSamples % 64 )
    {
        printf( "Source is not a multiple of 64 samples.  Padding end with silence.\n" );

        // We need enough memory to pad out to a 64-sample boundary
        *pdwPadding = ( 64 - ( dwSamples % 64 ) ) * pwfx->nBlockAlign;
    }

    // and allocate a buffer for it
    *ppbSampleData = new BYTE[ *pdwDuration + *pdwPadding ];
    if( !*ppbSampleData )
    {
        printf( "Couldn't allocate %d bytes to read sample data.n", *pdwDuration + *pdwPadding );
        return E_FAIL;
    }

    // Read the source data
    wfSrc.ReadSample( 0, *ppbSampleData, *pdwDuration, pdwDuration );
    wfSrc.Close();

    // Fill remainder with silence
    ZeroMemory( *ppbSampleData + *pdwDuration, *pdwPadding );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: WriteCompressedFile
// Desc: Writes out the compressed ADPCM wave file, optionally copying over
//       all RIFF chunks.
//-----------------------------------------------------------------------------
HRESULT
WriteCompressedFile( const char * szInput, 
                     const char * szOutput, 
                     DWORD dwOriginalLength,
                     BYTE * pbEncodedData,
                     DWORD dwNewLength,
                     IMAADPCMWAVEFORMAT * pwfx,
                     BOOL bCopyAllChunks )
{
    HANDLE hSrc;                    // Source file handle
    HANDLE hDest;                   // Destination file handle
    RIFFHEADER rh;                  // RIFF chunk header

    // Open the source file
    hSrc = CreateFile( szInput, GENERIC_READ, FILE_SHARE_READ, NULL, 
                          OPEN_EXISTING, 0L, NULL );
    if( INVALID_HANDLE_VALUE == hSrc )
    {
        printf( "Couldn't open %s", szInput );
        return E_FAIL;
    }

    // Create the destination file
    hDest = CreateFile( szOutput, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
    if( INVALID_HANDLE_VALUE == hDest )
    {
        printf( "Couldn't open output file %s\n", szOutput );
        return E_FAIL;
    }

    //
    // Loop over each chunk in the source file, and write the appropriate data
    // out to the destination file:
    // * For the RIFF chunk, we have to correct the size field of the RIFF
    // header
    // * For the FMT chunk, we have to correct the size, and write out the new
    // wave format structure
    // * For the DATA chunk, we have to correct the size, and write out the 
    // compressed sample data
    // * For all other chunks, we copy verbatim if requested
    //
    for( ; ; )
    {
        BOOL fReadOK;
        DWORD cb;

        // Read the chunk header
        fReadOK = ReadFile( hSrc, &rh, sizeof( RIFFHEADER ), &cb, NULL );
        if( !fReadOK )
        {
            printf( "Error reading from %s.\n", szInput );
            return E_FAIL;
        }

        // Check to see if we hit the end of the file
        if( cb == 0 )
            break;

        switch( rh.fccChunkId )
        {
        case FOURCC_RIFF:
            // Correct the data size to account for compression
            if( bCopyAllChunks )
            {
                // If we're copying everything over, just subtract the
                // compression amount, and account for the difference in
                // wave format structures
                rh.dwDataSize -= ( dwOriginalLength - dwNewLength ) + 
                                 sizeof( WAVEFORMATEX ) - sizeof( IMAADPCMWAVEFORMAT );
            }
            else
            {
                // If we're not copying everything over, then just
                // calculate the new data size
                rh.dwDataSize = 2 * sizeof( RIFFHEADER ) + 
                                sizeof( FOURCC ) + 
                                sizeof( IMAADPCMWAVEFORMAT ) +
                                dwNewLength;
            }

            // Write out the correct RIFF header
            WriteFile( hDest, &rh, sizeof( RIFFHEADER ), &cb, NULL );

            // Copy the form type over
            DWORD dwFormType;
            ReadFile( hSrc, &dwFormType, sizeof( DWORD ), &cb, NULL );
            WriteFile( hDest, &dwFormType, sizeof( DWORD ), &cb, NULL );

            break;
        case FOURCC_FORMAT:
            // Skip past the original format
            SetFilePointer( hSrc, rh.dwDataSize, NULL, FILE_CURRENT );

            // Correct the data size for the format struct
            rh.dwDataSize = sizeof( IMAADPCMWAVEFORMAT );

            // Write out the correct FMT header
            WriteFile( hDest, &rh, sizeof( RIFFHEADER ), &cb, NULL );

            // Write out the new format struct
            WriteFile( hDest, pwfx, sizeof( IMAADPCMWAVEFORMAT ), &cb, NULL );

            break;
        case FOURCC_DATA:
            // Skip past the original data
            SetFilePointer( hSrc, rh.dwDataSize, NULL, FILE_CURRENT );

            // Correct the data size for compressed sample data
            rh.dwDataSize = dwNewLength;

            // Write out the correct DATA header
            WriteFile( hDest, &rh, sizeof( RIFFHEADER ), &cb, NULL );

            // Write out the compressed sample data
            WriteFile( hDest, pbEncodedData, dwNewLength, &cb, NULL );

            break;
        default:
            if( bCopyAllChunks )
            {
                // Allocate a block of memory for the chunk data
                BYTE * pbChunkData = new BYTE[ rh.dwDataSize ];
                if( !pbChunkData )
                {
                    printf( "Couldn't allocate enough memory to copy data.\n" );
                    return E_FAIL;
                }

                // Copy the chunk header over
                WriteFile( hDest, &rh, sizeof( RIFFHEADER ), &cb, NULL );

                // Copy the chunk data over
                ReadFile( hSrc, pbChunkData, rh.dwDataSize, &cb, NULL );
                WriteFile( hDest, pbChunkData, rh.dwDataSize, &cb, NULL );

                // Release the memory
                delete[] pbChunkData;
            }
            else
            {
                // Seek to the next chunk
                SetFilePointer( hSrc, rh.dwDataSize, NULL, FILE_CURRENT );
            }

            break;
        }
    }

    // Clean up
    CloseHandle( hSrc );
    CloseHandle( hDest );

    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: main
// Desc: Main control of program.  Parses arguments and converts wav file
//-----------------------------------------------------------------------------
void
main( int argc, char * argv[])
{
    const char * szInput = NULL;    // Source file name
    const char * szOutput = NULL;   // Destination file name
    DWORD dwDuration;               // Original length
    DWORD dwPadding;                // Additional padding at end
    BYTE * pbSampleData = NULL;     // Source data
    BYTE * pbEncodedData = NULL;    // Compressed data
    WAVEFORMATEX wfx;               // Source format
    IMAADPCMWAVEFORMAT wfxEncode;   // Destination format
    CImaAdpcmCodec codec;           // Codec
    BOOL bCopyAllChunks = FALSE;    // TRUE to copy all chunks to encoded file

    // Parse arguments
    for( int i = 1; i < argc; i++ )
    {
        const char * szArg = argv[i];

        // Check for options
        if( szArg[0] == '-' || szArg[0] == '/' )
        {
            switch( szArg[1] )
            {
            case 'c':
            case 'C':
                bCopyAllChunks = TRUE;
                break;
            default:
                printf( "Unrecognized option %c%c.\n", szArg[0], szArg[1] );
                return;
            }
        }
        else if( !szInput )
            szInput = szArg;
        else if( !szOutput )
            szOutput = szArg;
    }

    // Check to make sure we had valid arguments
    if( !szInput || !szOutput )
    {
        printf( "Usage: xbadpcmencode Input.wav Output.wav [options]\n" );
        printf( "Options:\n\t/C\tCopy all chunks to encoded file.\n" );
        return;
    }

    printf( "Converting %s to %s...\n", szInput, szOutput );

    // Get the source data and wave format
    if( FAILED( GetSourceInfo( szInput, &pbSampleData, &dwDuration, &dwPadding, &wfx ) ) )
        return;

    if( wfx.wFormatTag != WAVE_FORMAT_PCM ||
        wfx.nChannels > 2 )
    {
        printf( "Invalid source format.\nSource must be mono or stereo 8- or 16-bit PCM.\n" );
        return;
    }

    // Create an APDCM format structure based off the source format
    codec.CreateImaAdpcmFormat( wfx.nChannels, wfx.nSamplesPerSec, XBOX_ADPCM_SAMPLES_PER_BLOCK, &wfxEncode );

    // Calculate number of ADPCM blocks and length of ADPCM data
    DWORD dwDestBlocks  = ( dwDuration + dwPadding ) / wfx.nBlockAlign / XBOX_ADPCM_SAMPLES_PER_BLOCK;
    DWORD dwDestLength  = dwDestBlocks * wfxEncode.wfx.nBlockAlign;

    // Allocate a buffer for encoded data
    pbEncodedData = new BYTE[ dwDestLength ];

    // Initialize the codec
    if( FALSE == codec.Initialize( &wfxEncode, TRUE ) )
    {
        printf( "Couldn't initialize codec.\n" );
        return;
    }

    // Convert the data
    if( FALSE == codec.Convert( pbSampleData, pbEncodedData, dwDestBlocks ) )
    {
        printf( "Codec failed.\n" );
        return;
    }

    // Write out the encoded file
    WriteCompressedFile( szInput, 
                         szOutput, 
                         dwDuration, 
                         pbEncodedData, 
                         dwDestLength, 
                         &wfxEncode, 
                         bCopyAllChunks );

    // Clean up
    delete[] pbSampleData;
    delete[] pbEncodedData;

    return;
}