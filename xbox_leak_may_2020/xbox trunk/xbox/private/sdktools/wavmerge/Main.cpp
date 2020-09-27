/******************************************************************************************

WavMerge

Merge NUMCHANNELS mono files into 1 5.1 file.

danhaff      7/23/01    Started
******************************************************************************************/

#include "globals.h"
#include "cwavefile.h"
#include "helpers.h"
/******************************************************************************************
******************************************************************************************/

const LPSTR szSixChannelNames[] = {"Left", "Right", "Center", "LFE", "LeftRear", "RightRear"};
const LPSTR szFourChannelNames[]= {"Left", "Right","LeftRear", "RightRear"};
const LPSTR szSilence = ".";

HRESULT CheckParams(int cArg, char *argv[], LPDWORD dwNumChannels)
{
    HRESULT hr = S_OK;
        
    if (cArg != (NUMELEMS(szSixChannelNames) + 2) && cArg!= (NUMELEMS(szFourChannelNames)+2))
    {
        Log("");
        Log("  Error: Incorrect number of parameters specified");
        Log("");
        Log("  Usage: wavmerge [%s | %s] [%s | %s] [%s | %s] [%s | %s]\n                  [%s | %s] [%s | %s] %s",


                                          szSixChannelNames[0], szSilence,
                                          szSixChannelNames[1], szSilence,
                                          szSixChannelNames[2], szSilence,
                                          szSixChannelNames[3], szSilence,
                                          szSixChannelNames[4], szSilence,
                                          szSixChannelNames[5], szSilence,
                                          "OUTPUT");
        Log("     or");
        Log("  Usage: wavmerge [%s | %s] [%s | %s] [%s | %s] [%s | %s] %s",


                                          szFourChannelNames[0], szSilence,
                                          szFourChannelNames[1], szSilence,
                                          szFourChannelNames[2], szSilence,
                                          szFourChannelNames[3], szSilence,
                                          "OUTPUT");
        Log("");
        Log("");
        Log("    WavMerge combines mono wave files into a 4- or 6-channel WaveFormatEx   ");
        Log("    interleaved PCM wave file.  The bit depths and sampling rates of the       ");
        Log("    mono wave files must be identical.  Their durations, however, may differ.  ");
        Log("    The output file's duration will equal that of the longest input file.      ") ;
        Log("    You may pass \".\" to denote silence for a channel.                        ");
        Log("                                                                               ");
        Log("    Examples:                                                                  ");
        Log("       wavmerge left.wav . . . . . result.wav                                  ");
        Log("       Creates a 6-channel wave file with the left channel audible and         ");
        Log("       silence in the other channels.                                          ");
        Log("                                                                               ");
        Log("       wavmerge . right.wav . . . . result.wav                                 ");
        Log("       Creates a 6-channel wave file with the right channel audible and        ");
        Log("       silence in the other channels.                                          ");
        Log("                                                                               ");
        Log("       wavmerge left.wav right.wav . . result.wav                              ");
        Log("       Creates a 4-channel wave file with the left and right channels audible  ");
        Log("       and silence in the other channels.                                      ");

        hr = E_FAIL;
        *dwNumChannels = 0;
    }
    else
    {
        *dwNumChannels = cArg - 2;
    }
    return hr;
};

/******************************************************************************************
Load up the files, G.
******************************************************************************************/
HRESULT LoadFiles(CWaveFile WaveFile[], char *argv[], DWORD dwNumChannels)
{
    HRESULT hr = S_OK;
    DWORD i = 0;
    BOOL bFound = 0;
    WAVEFORMATEX wfxSilence = {0};

    //Load all the non-silence files.
    for (i=0; i<dwNumChannels && SUCCEEDED(hr); i++)
    {
        if (_strcmpi(argv[1+i], szSilence) != 0)
        {
            CHECKRUN(WaveFile[i].OpenForRead(argv[1+i]));
            if (FAILED(hr))
            {
                Log("Failed to load %s", argv[1+i]);
            }
        }
    }

    //Find the format of the first non-silence file.  We'll use that for the silence files.
    bFound = FALSE;
    for (i=0; i<dwNumChannels && SUCCEEDED(hr); i++)
    {
        if (_strcmpi(argv[1+i], szSilence) != 0)
        {
            WaveFile[i].GetWaveFormatEx(&wfxSilence);
            bFound = TRUE;
            break;
        }
    }

    //If all the files are silent, set the format to 8-bit mono, 8khz.
    if (SUCCEEDED(hr))
        if (!bFound)
        {
            Log("All channels are set to silence.  You must specify an input file for at least one channel.");
            return E_FAIL;
    /*
            wfxSilence.wFormatTag = 1;
            wfxSilence.nChannels = 1;
            wfxSilence.nSamplesPerSec = 8000;
            wfxSilence.nAvgBytesPerSec = 8000;
            wfxSilence.nBlockAlign = 1;
            wfxSilence.wBitsPerSample = 8;
            wfxSilence.cbSize = 0;
    */
        }


    //Go through all the "silence" files and set them to the wave format we've found/created.
    for (i=0; i<dwNumChannels && SUCCEEDED(hr); i++)
    {
        if (_strcmpi(argv[1+i], szSilence) == 0)
            CHECKRUN(WaveFile[i].OpenForSilence(wfxSilence));
    }

    return hr;
}



/******************************************************************************************
Make sure these files are all mono and of the same bit depths and sample rate.
******************************************************************************************/
HRESULT VerifyFormats(CWaveFile WaveFile[], DWORD dwNumChannels)
{
    HRESULT hr = S_OK;
    DWORD i,j = 0;
    WAVEFORMATEX wfx[2]  = {NULL};
    CHAR szFileName[2][MAX_PATH];
    DWORD dwLength[2] = {NULL};

    //Check channels and bit-rate for non-silence files.
    for (i=0; i<dwNumChannels && SUCCEEDED(hr); i++)
    {
        WaveFile[i].GetWaveFormatEx(&wfx[0]);
        WaveFile[i].GetFileName(szFileName[0]);

        if (_strcmpi(szFileName[0], szSilence) == 0)
            continue;

        if (wfx[0].nChannels != 1)
        {
            Log("Error: %s has %u channels instead of 1.  Input files must be mono.", szFileName[0], wfx[0].nChannels);
            hr = E_FAIL;
            break;
        }

        if (wfx[0].wBitsPerSample!=8 && wfx[0].wBitsPerSample!=16)
        {
            Log("Error: %s is %u-bit; it must be 8- or 16- bit.", szFileName[0], wfx[0].wBitsPerSample);
            hr = E_FAIL;
            break;
        }
    }

    //Check
    for (i=0; i<dwNumChannels - 1 && SUCCEEDED(hr); i++)
    {
        for (j=i+1; j<dwNumChannels && SUCCEEDED(hr); j++)
        {
            WaveFile[i].GetWaveFormatEx(&wfx[0]);
            WaveFile[j].GetWaveFormatEx(&wfx[1]);
            
            WaveFile[i].GetFileName(szFileName[0]);
            WaveFile[j].GetFileName(szFileName[1]);

            if (_strcmpi(szFileName[0], szSilence) == 0)
                continue;
            if (_strcmpi(szFileName[1], szSilence) == 0)
                continue;


            if (wfx[0].nSamplesPerSec != wfx[1].nSamplesPerSec)
            {
                Log("Error: sample rate mismatch: %s's sample rate is %u: %s's sample rate is %u", szFileName[0], wfx[0].nSamplesPerSec,                                                                                                   
                                                                                                   szFileName[1], wfx[1].nSamplesPerSec);
                hr = E_FAIL;
                break;
            }

            if (wfx[0].wBitsPerSample != wfx[1].wBitsPerSample)
            {
                Log("Error: bit depth mismatch: %s's bit depth is %u: %s's bit depth is %u", szFileName[0], wfx[0].wBitsPerSample,
                                                                                             szFileName[1], wfx[1].wBitsPerSample);
                hr = E_FAIL;
                break;
            }
        }
    }

    return hr;
}


/******************************************************************************************
******************************************************************************************/
#define MAX 10
int __cdecl main(int cArg, char *argv[])
{
	HRESULT hr = S_OK;
    CWaveFile InputFiles[MAX];              //NUMCHANNELS input files
    CWaveFile OutputFile;                           //1 output file.
    DWORD dwNumChannels = 0;


//    CWaveFile temp;
//    temp.Load("Output.wav");

    //Make sure the correct number of paramters are present.
    CHECKRUN(CheckParams(cArg, argv, &dwNumChannels));
    if (MAX < dwNumChannels)
        Log("NOT ENOUGH CHANNELS IN MAIN FUNCTION!!");
    
    //Load da files
    CHECKRUN(LoadFiles(InputFiles, argv, dwNumChannels));

    //Make sure the wave files all have the same format.
    CHECKRUN(VerifyFormats(InputFiles, dwNumChannels));

    //Merge them into a single file.
    CHECKRUN(Merge(InputFiles, argv[dwNumChannels + 1], dwNumChannels));

    if (SUCCEEDED(hr))
    {
        Log("Successfully created %s",argv[dwNumChannels + 1]);
    }

    return 0;

}
