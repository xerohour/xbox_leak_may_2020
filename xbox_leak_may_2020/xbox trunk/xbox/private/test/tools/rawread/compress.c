#include "compress.h"
#include "defs.h"
#include "mrcicode.h"


// Compressed data format is as follows:  2 bytes indicating the size of
// the compressed data that when uncompressed will equal 1 cluster, followed
// by the MRCI 1 compresed data itself.


// Compresses a given data buffer.  On entry, lpvInput points to a buffer
// containing the uncompressed data, wInputSize is the size of the buffer,
// lpvOutput points to a buffer to receive the compressed data, lpwOutputSize
// points to the size of the output buffer.  On exit, lpwOutputSize points
// to the size of the compressed data.
BOOL CompressData(LPVOID lpvInput, WORD wInputSize, LPVOID lpvOutput, LPWORD lpwOutputSize)
{
    LPVOID                  lpvOffsetOutput;
    WORD                    wOffsetInputSize;
    WORD                    wCompressedSize;
    
    // Compress the data.  Note that the output buffer needs to point
    // to 2 bytes after the actual beginning and the size of the buffer
    // needs to be 2 less than the actual because we'll be writing the
    // compressed data size to the first 2 bytes.
    lpvOffsetOutput = (LPBYTE)lpvOutput + sizeof(WORD);
    wOffsetInputSize = wInputSize - sizeof(WORD);

    wCompressedSize = Mrci1MaxCompress(lpvInput, wInputSize, lpvOffsetOutput, wOffsetInputSize);
    
    // Make sure the compressor didn't return failure
    if(wCompressedSize == (WORD)-1)
    {
        return FALSE;
    }
    
    // Did we actually save anything?
    if(wCompressedSize >= wOffsetInputSize)
    {
        return FALSE;
    }
    
    // Write the compressed data size to the first two bytes.  This is
    // the size of the actual compressed data, not including the first
    // two bytes.
    *(LPWORD)lpvOutput = wCompressedSize;
    
    // Return the size of the data that will need to be written to the file
    *lpwOutputSize = wCompressedSize + sizeof(WORD);
    
    return TRUE;
}


// Uncompress a given data buffer.  On entry, lpvInput points to a buffer
// containing the compressed data, wInputSize is the size of the buffer to
// uncompress, lpvOutput points to a buffer to receive the uncompressed data,
// lpwOutputSize points to the expected size of compressed data.  On exit, 
// lpwOutputSize points to the size of the uncompressed data.
BOOL UncompressData(LPVOID lpvInput, LPWORD lpwInputSize, LPVOID lpvOutput, WORD wOutputSize)
{
    WORD                    wCompressed;
    WORD                    wUncompressed;
    
    // The actual size of the compressed data is in the first 2 bytes of
    // the input buffer
    wCompressed = *(LPWORD)lpvInput;
    lpvInput = (LPWORD)lpvInput + 1;

    // Uncompress the data to the output buffer
    wUncompressed = Mrci1Decompress((LPBYTE)lpvInput, wCompressed, lpvOutput, wOutputSize);
    
    // Did we uncompress properly?
    if(wUncompressed != wOutputSize)
    {
        return FALSE;
    }
    
    // Return success
    *lpwInputSize = wCompressed + sizeof(WORD);

    return TRUE;
}


