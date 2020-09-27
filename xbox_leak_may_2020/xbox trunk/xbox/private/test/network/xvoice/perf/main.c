/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.c

Abstract:

  This module tests XMO Voice performance

Author:

  Steven Kehrli (steveke) 23-Oct-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include "precomp.h"



#define MAX_MICROPHONE       4
#define MAX_HEADPHONE        4
#define MAX_ENCODER          4
#define MAX_DECODER          12
#define MAX_QUEUE            12

#define MAX_PACKETS          10
#define MAX_THRESHOLD        25

#define VOICE_SAMPLING_RATE  8000
#define VOICE_BYTES_PER_MS   (VOICE_SAMPLING_RATE / 1000) * 2



VOID
GetDevices(
    IN OUT DWORD  *dwMicrophoneMask,
    IN OUT DWORD  *dwHeadphoneMask
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Gets all devices

Arguments:

  dwMicrophoneMask - Pointer to the bitmask of microphone devices
  dwHeadphoneMask - Pointer to the bitmask of headphone devices

Return Value:

  None

------------------------------------------------------------------------------*/
{
    // dwInsertions is the device mask of insertions
    DWORD  dwInsertions;
    // dwRemovals is the device mask of removals
    DWORD  dwRemovals;
    // dwCurrentDevice is a counter to enumerate each device
    DWORD  dwCurrentDevice;
    // dwObject is a counter to enumerate device changes
    DWORD  dwObject;



    // Get the microphones
    *dwMicrophoneMask = XGetDevices(XDEVICE_TYPE_VOICE_MICROPHONE);

    // Get the headphones
    *dwHeadphoneMask = XGetDevices(XDEVICE_TYPE_VOICE_HEADPHONE);

    for (dwObject = 0; dwObject < 40 && (((XDEVICE_PORT0_MASK | XDEVICE_PORT1_MASK | XDEVICE_PORT2_MASK | XDEVICE_PORT3_MASK) != *dwMicrophoneMask) || ((XDEVICE_PORT0_MASK | XDEVICE_PORT1_MASK | XDEVICE_PORT2_MASK | XDEVICE_PORT3_MASK) != *dwHeadphoneMask)); dwObject++) {
        Sleep(200);

        if (FALSE == XGetDeviceChanges(XDEVICE_TYPE_VOICE_MICROPHONE, &dwInsertions, &dwRemovals)) {
            dwInsertions = 0;
            dwRemovals = 0;
        }

        for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
            // Remove the device if it has been removed
            if ((1 << dwCurrentDevice) & dwRemovals) {
                *dwMicrophoneMask &= ~(1 << dwCurrentDevice);
            }

            // Add the device if it has been inserted
            if ((1 << dwCurrentDevice) & dwInsertions) {
                *dwMicrophoneMask |= (1 << dwCurrentDevice);
            }
        }

        if (FALSE == XGetDeviceChanges(XDEVICE_TYPE_VOICE_HEADPHONE, &dwInsertions, &dwRemovals)) {
            dwInsertions = 0;
            dwRemovals = 0;
        }

        for (dwCurrentDevice = 0; dwCurrentDevice < XGetPortCount(); dwCurrentDevice++) {
            // Remove the device if it has been removed
            if ((1 << dwCurrentDevice) & dwRemovals) {
                *dwHeadphoneMask &= ~(1 << dwCurrentDevice);
            }

            // Add the device if it has been inserted
            if ((1 << dwCurrentDevice) & dwInsertions) {
                *dwHeadphoneMask |= (1 << dwCurrentDevice);
            }
        }
    };
}



BOOL
DeviceInit(
    IN HANDLE  hLog
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Initializes all devices

Arguments:

  hLog - Handle to the log object

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // dwMicrophoneMask is the microphone device mask
    DWORD  dwMicrophoneMask;
    // dwHeadphoneMask is the headphone device mask
    DWORD  dwHeadphoneMask;



    Log_va(hLog, TRUE, "**********\r\nDeviceInit\r\n**********\r\n");
    Log_va(hLog, TRUE, "Please ensure four (4) Xbox Communicators are connected.\r\n");
    Log_va(hLog, TRUE, "Enumerating devices...\r\n");

    // Initialize the devices
    XInitDevices(0, NULL);

    // Get the devices
    GetDevices(&dwMicrophoneMask, &dwHeadphoneMask);

    if (0 == (XDEVICE_PORT0_MASK & dwMicrophoneMask)) {
        Log_va(hLog, TRUE, "    Microphone missing in Port 0\r\n");
    }

    if (0 == (XDEVICE_PORT1_MASK & dwMicrophoneMask)) {
        Log_va(hLog, TRUE, "    Microphone missing in Port 1\r\n");
    }

    if (0 == (XDEVICE_PORT2_MASK & dwMicrophoneMask)) {
        Log_va(hLog, TRUE, "    Microphone missing in Port 2\r\n");
    }

    if (0 == (XDEVICE_PORT3_MASK & dwMicrophoneMask)) {
        Log_va(hLog, TRUE, "    Microphone missing in Port 3\r\n");
    }

    if (0 == (XDEVICE_PORT0_MASK & dwHeadphoneMask)) {
        Log_va(hLog, TRUE, "    Headphone missing in Port 0\r\n");
    }

    if (0 == (XDEVICE_PORT1_MASK & dwHeadphoneMask)) {
        Log_va(hLog, TRUE, "    Headphone missing in Port 1\r\n");
    }

    if (0 == (XDEVICE_PORT2_MASK & dwHeadphoneMask)) {
        Log_va(hLog, TRUE, "    Headphone missing in Port 2\r\n");
    }

    if (0 == (XDEVICE_PORT3_MASK & dwHeadphoneMask)) {
        Log_va(hLog, TRUE, "    Headphone missing in Port 3\r\n");
    }

    Log_va(hLog, TRUE, "**********\r\n\r\n");

    return (((XDEVICE_PORT0_MASK | XDEVICE_PORT1_MASK | XDEVICE_PORT2_MASK | XDEVICE_PORT3_MASK) == dwMicrophoneMask) || ((XDEVICE_PORT0_MASK | XDEVICE_PORT1_MASK | XDEVICE_PORT2_MASK | XDEVICE_PORT3_MASK) == dwHeadphoneMask));
}



BOOL
DeviceCreate(
    IN     HANDLE  hLog,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumMicrophoneFailures,
    IN OUT DWORD   *dwNumHeadphoneFailures,
    IN     DWORD   dwNumMicrophones,
    IN     DWORD   dwNumHeadphones
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates devices

Arguments:

  hLog - Handle to the log object
  lpszTestName - Pointer to the test name
  dwTestIteration - Test iteration
  KernelTimeDiff - Diff kernel time
  InterruptTimeDiff - Diff interrupt time
  DpcTimeDiff - Diff dpc time
  IdleTimeDiff - Diff idle time
  dwNumMicrophoneFailures - Pointer to the number of microphone creation failures
  dwNumHeadphoneFailures - Pointer to the number of headphone creation failures
  dwNumMicrophones - Number of microphone devices to initialize
  dwNumHeadphones - Number of headphone devices to initialize

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // KernelTimeInitial is the initial kernel time
    ULONG          KernelTimeInitial;
    // KernelTimeFinal is the final kernel time
    ULONG          KernelTimeFinal;
    // InterruptTimeInitial is the initial interrupt time
    ULONG          InterruptTimeInitial;
    // InterruptTimeFinal is the final interrupt time
    ULONG          InterruptTimeFinal;
    // DpcTimeInitial is the initial dpc time
    ULONG          DpcTimeInitial;
    // DpcTimeFinal is the final dpc time
    ULONG          DpcTimeFinal;
    // IdleTimeInitial is the initial idle time
    ULONG          IdleTimeInitial;
    // IdleTimeFinal is the final idle time
    ULONG          IdleTimeFinal;

    // wfxFormat is the wave format for the device
    WAVEFORMATEX   wfxFormat;
    // pMicrophoneObject is the array of microphone devices
    XMediaObject   *pMicrophoneObject[MAX_MICROPHONE];
    // pHeadphoneObject is the array of headphone devices
    XMediaObject   *pHeadphoneObject[MAX_HEADPHONE];
    // hMicrophoneResult is the result of the microphone device creation
    HRESULT        hMicrophoneResult[MAX_MICROPHONE];
    // hHeadphoneResult is the result of the headphone device creation
    HRESULT        hHeadphoneResult[MAX_HEADPHONE];
    // dwMicrophoneAttempts is the number of microphone device creation attempts
    DWORD          dwMicrophoneAttempts[MAX_MICROPHONE];
    // dwHeadphoneAttempts is the number of headphone device creation attempts
    DWORD          dwHeadphoneAttempts[MAX_HEADPHONE];
    // dwTotalMicrophoneFailures is the number of total microphone device creation failures
    DWORD          dwTotalMicrophoneFailures = 0;
    // dwTotalHeadphoneFailures is the number of total headphone device creation failures
    DWORD          dwTotalHeadphoneFailures = 0;
    // dwObject is a counter to enumerate each device
    DWORD          dwObject;

    // bPassed is the result of this test
    BOOL           bPassed = TRUE;



    // Set the numbers
    dwNumMicrophones = __min(MAX_MICROPHONE, dwNumMicrophones);
    dwNumHeadphones = __min(MAX_HEADPHONE, dwNumHeadphones);



    // Initialize the variables
    ZeroMemory(pMicrophoneObject, sizeof(pMicrophoneObject));
    ZeroMemory(pHeadphoneObject, sizeof(pHeadphoneObject));



    // Initialize the wave format
    ZeroMemory(&wfxFormat, sizeof(wfxFormat));
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    // Query the CPU times
    GetCpuTimes(&KernelTimeInitial, &InterruptTimeInitial, &DpcTimeInitial, &IdleTimeInitial);



    // Create the devices

    // Microphone
    for (dwObject = 0; dwObject < dwNumMicrophones; dwObject++) {
        dwMicrophoneAttempts[dwObject] = 0;
        do {
            hMicrophoneResult[dwObject] = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, dwObject, MAX_PACKETS, &wfxFormat, &pMicrophoneObject[dwObject]);
            dwMicrophoneAttempts[dwObject]++;

            if (FAILED(hMicrophoneResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hMicrophoneResult[dwObject]));
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumHeadphones; dwObject++) {
        dwHeadphoneAttempts[dwObject] = 0;
        do {
            hHeadphoneResult[dwObject] = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, dwObject, MAX_PACKETS, &wfxFormat, &pHeadphoneObject[dwObject]);
            dwHeadphoneAttempts[dwObject]++;

            if (FAILED(hHeadphoneResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hHeadphoneResult[dwObject]));
    }



    // Query the CPU times
    GetCpuTimes(&KernelTimeFinal, &InterruptTimeFinal, &DpcTimeFinal, &IdleTimeFinal);



    // Free the devices

    // Microphone
    for (dwObject = 0; dwObject < dwNumMicrophones; dwObject++) {
        dwTotalMicrophoneFailures += dwMicrophoneAttempts[dwObject] - 1;
        *dwNumMicrophoneFailures += dwMicrophoneAttempts[dwObject] - 1;

        // Close the device
        pMicrophoneObject[dwObject]->Release();
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumHeadphones; dwObject++) {
        dwTotalHeadphoneFailures += dwHeadphoneAttempts[dwObject] - 1;
        *dwNumHeadphoneFailures += dwHeadphoneAttempts[dwObject] - 1;

        // Close the device
        pHeadphoneObject[dwObject]->Release();
    }



    if (TRUE == bPassed) {
        // Log the stats
        LogStats(hLog, lpszTestName, dwTestIteration, KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, &dwTotalMicrophoneFailures, &dwTotalHeadphoneFailures, NULL, NULL, NULL);
        CalculateAndAddStatsDiff(KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, KernelTimeDiff, InterruptTimeDiff, DpcTimeDiff, IdleTimeDiff);
    }

    return (TRUE == bPassed);
}



BOOL
CodecCreate(
    IN     HANDLE  hLog,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumEncoderFailures,
    IN OUT DWORD   *dwNumDecoderFailures,
    IN OUT DWORD   *dwNumQueueFailures,
    IN     DWORD   dwNumEncoder,
    IN     DWORD   dwNumDecoder,
    IN     DWORD   dwNumQueue,
    IN     DWORD   dwCodecTag,
    IN     WORD    wMsOfDataPerPacket
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Creates codecs

Arguments:

  hLog - Handle to the log object
  lpszTestName - Pointer to the test name
  dwTestIteration - Test iteration
  KernelTimeDiff - Diff kernel time
  InterruptTimeDiff - Diff interrupt time
  DpcTimeDiff - Diff dpc time
  IdleTimeDiff - Diff idle time
  dwNumEncoderFailures - Pointer to the number of encoder creation failures
  dwNumDecoderFailures - Pointer to the number of decoder creation failures
  dwNumQueueFailures - Pointer to the number of queue creation failures
  dwCodecTag - Specifies the codec
  dwNumEncoder - Number of encoder objects to create
  dwNumDecoder - Number of decoder objects to create
  dwNumQueue - Number of queue objects to create
  wMsOfDataPerPacket - Milliseconds of voice data per packet

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // KernelTimeInitial is the initial kernel time
    ULONG                   KernelTimeInitial;
    // KernelTimeFinal is the final kernel time
    ULONG                   KernelTimeFinal;
    // InterruptTimeInitial is the initial interrupt time
    ULONG                   InterruptTimeInitial;
    // InterruptTimeFinal is the final interrupt time
    ULONG                   InterruptTimeFinal;
    // DpcTimeInitial is the initial dpc time
    ULONG                   DpcTimeInitial;
    // DpcTimeFinal is the final dpc time
    ULONG                   DpcTimeFinal;
    // IdleTimeInitial is the initial idle time
    ULONG                   IdleTimeInitial;
    // IdleTimeFinal is the final idle time
    ULONG                   IdleTimeFinal;

    // wfxFormat is the wave format for the object
    WAVEFORMATEX            wfxFormat;
    // QueueConfig is the queue config for the object
    QUEUE_XMO_CONFIG        QueueConfig;
    // pEncoderObject is the array of encoder objects
    XMediaObject            *pEncoderObject[MAX_ENCODER];
    // pDecoderObject is the array of decoder objects
    XMediaObject            *pDecoderObject[MAX_DECODER];
    // pQueueObject is the array of queue objects
    XVoiceQueueMediaObject  *pQueueObject[MAX_QUEUE];
    // hEncoderResult is the result of the encoder object creation
    HRESULT                 hEncoderResult[MAX_ENCODER];
    // hDecoderResult is the result of the decoder object creation
    HRESULT                 hDecoderResult[MAX_DECODER];
    // hQueueResult is the result of the queue object creation
    HRESULT                 hQueueResult[MAX_QUEUE];
    // dwEncoderAttempts is the number of encoder object creation attempts
    DWORD                   dwEncoderAttempts[MAX_ENCODER];
    // dwDecoderAttempts is the number of decoder object creation attempts
    DWORD                   dwDecoderAttempts[MAX_DECODER];
    // dwQueueAttempts is the number of queue object creation attempts
    DWORD                   dwQueueAttempts[MAX_QUEUE];
    // dwTotalEncoderFailures is the number of total encoder object creation failures
    DWORD                   dwTotalEncoderFailures = 0;
    // dwTotalDecoderFailures is the number of total decoder object creation failures
    DWORD                   dwTotalDecoderFailures = 0;
    // dwTotalQueueFailures is the number of total queue object creation failures
    DWORD                   dwTotalQueueFailures = 0;
    // dwObject is a counter to enumerate each object
    DWORD                   dwObject;

    // bPassed is the result of this test
    BOOL                    bPassed = TRUE;



    // Set the numbers
    dwNumEncoder = __min(MAX_ENCODER, dwNumEncoder);
    dwNumDecoder = __min(MAX_DECODER, dwNumDecoder);
    dwNumQueue = __min(MAX_QUEUE, dwNumQueue);



    // Initialize the variables
    ZeroMemory(pEncoderObject, sizeof(pEncoderObject));
    ZeroMemory(pDecoderObject, sizeof(pDecoderObject));
    ZeroMemory(pQueueObject, sizeof(pQueueObject));



    // Initialize the wave format
    ZeroMemory(&wfxFormat, sizeof(wfxFormat));
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    // Initialize the queue config
    ZeroMemory(&QueueConfig, sizeof(QueueConfig));
    QueueConfig.cbSize = sizeof(QueueConfig);
    QueueConfig.wVoiceSamplingRate = VOICE_SAMPLING_RATE;
    QueueConfig.dwCodecTag = dwCodecTag;
    QueueConfig.wMsOfDataPerPacket = wMsOfDataPerPacket;

    // Query the CPU times
    GetCpuTimes(&KernelTimeInitial, &InterruptTimeInitial, &DpcTimeInitial, &IdleTimeInitial);



    // Create the objects

    // Encoder
    for (dwObject = 0; dwObject < dwNumEncoder; dwObject++) {
        dwEncoderAttempts[dwObject] = 0;

        do {
            hEncoderResult[dwObject] = XVoiceEncoderCreateMediaObject(FALSE, dwCodecTag, &wfxFormat, MAX_THRESHOLD, &pEncoderObject[dwObject]);
            dwEncoderAttempts[dwObject]++;

            if (FAILED(hEncoderResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hEncoderResult[dwObject]));
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumDecoder; dwObject++) {
        dwDecoderAttempts[dwObject] = 0;

        do {
            hDecoderResult[dwObject] = XVoiceDecoderCreateMediaObject(dwCodecTag, &wfxFormat, &pDecoderObject[dwObject]);
            dwDecoderAttempts[dwObject]++;

            if (FAILED(hDecoderResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hDecoderResult[dwObject]));
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumQueue; dwObject++) {
        dwQueueAttempts[dwObject] = 0;
        do {
            hQueueResult[dwObject] = XVoiceQueueCreateMediaObject(&QueueConfig, &pQueueObject[dwObject]);
            dwQueueAttempts[dwObject]++;

            if (FAILED(hQueueResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hQueueResult[dwObject]));
    }



    // Query the CPU times
    GetCpuTimes(&KernelTimeFinal, &InterruptTimeFinal, &DpcTimeFinal, &IdleTimeFinal);



    // Free the objects

    // Encoder
    for (dwObject = 0; dwObject < dwNumEncoder; dwObject++) {
        dwTotalEncoderFailures += dwEncoderAttempts[dwObject] - 1;
        *dwNumEncoderFailures += dwEncoderAttempts[dwObject] - 1;

        // Close the device
        pEncoderObject[dwObject]->Release();
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumDecoder; dwObject++) {
        dwTotalDecoderFailures += dwDecoderAttempts[dwObject] - 1;
        *dwNumDecoderFailures += dwDecoderAttempts[dwObject] - 1;

        // Close the device
        pDecoderObject[dwObject]->Release();
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumQueue; dwObject++) {
        dwTotalQueueFailures += dwQueueAttempts[dwObject] - 1;
        *dwNumQueueFailures += dwQueueAttempts[dwObject] - 1;

        // Close the device
        pQueueObject[dwObject]->Release();
    }



    if (TRUE == bPassed) {
        // Log the stats
        LogStats(hLog, lpszTestName, dwTestIteration, KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, NULL, NULL, &dwTotalEncoderFailures, &dwTotalDecoderFailures, &dwTotalQueueFailures);
        CalculateAndAddStatsDiff(KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, KernelTimeDiff, InterruptTimeDiff, DpcTimeDiff, IdleTimeDiff);
    }

    return (TRUE == bPassed);
}



BOOL
MediaProcess(
    IN     HANDLE  hLog,
    IN     HANDLE  hMemObject,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumMicrophoneFailures,
    IN OUT DWORD   *dwNumHeadphoneFailures,
    IN OUT DWORD   *dwNumEncoderFailures,
    IN OUT DWORD   *dwNumDecoderFailures,
    IN OUT DWORD   *dwNumQueueFailures,
    IN     DWORD   dwNumMicrophones,
    IN     DWORD   dwNumHeadphones,
    IN     DWORD   dwNumEncoder,
    IN     DWORD   dwNumDecoder,
    IN     DWORD   dwNumQueue,
    IN     BOOL    bMix,
    IN     DWORD   dwCodecTag,
    IN     WORD    wNumPackets,
    IN     WORD    wMsOfDataPerPacket
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Processes media

Arguments:

  hLog - Handle to the log object
  hMemObject - Handle to the memory objet
  lpszTestName - Pointer to the test name
  dwTestIteration - Test iteration
  KernelTimeDiff - Diff kernel time
  InterruptTimeDiff - Diff interrupt time
  DpcTimeDiff - Diff dpc time
  IdleTimeDiff - Diff idle time
  dwNumMicrophoneFailures - Pointer to the number of microphone creation failures
  dwNumHeadphoneFailures - Pointer to the number of headphone creation failures
  dwNumEncoderFailures - Pointer to the number of encoder creation failures
  dwNumDecoderFailures - Pointer to the number of decoder creation failures
  dwNumQueueFailures - Pointer to the number of queue creation failures
  dwNumMicrophones - Specifies the number of microphone objects to create
  dwNumHeadphones - Specifies the number of headphone objects to create
  dwNumEncoder - Specifies the number of encoder objects to create
  dwNumDecoder - Specifies the number of decoder objects to create
  dwNumQueue - Specifies the number of queue objects to create
  bMix - Specifies if the output is mixed to a single buffer
  dwCodecTag - Specifies the codec
  dwNumPackets - Specifies the number of packets
  wMsOfDataPerPacket - Milliseconds of voice data per packet

Return Value:

  TRUE on success

------------------------------------------------------------------------------*/
{
    // KernelTimeInitial is the initial kernel time
    ULONG                   KernelTimeInitial;
    // KernelTimeFinal is the final kernel time
    ULONG                   KernelTimeFinal;
    // InterruptTimeInitial is the initial interrupt time
    ULONG                   InterruptTimeInitial;
    // InterruptTimeFinal is the final interrupt time
    ULONG                   InterruptTimeFinal;
    // DpcTimeInitial is the initial dpc time
    ULONG                   DpcTimeInitial;
    // DpcTimeFinal is the final dpc time
    ULONG                   DpcTimeFinal;
    // IdleTimeInitial is the initial idle time
    ULONG                   IdleTimeInitial;
    // IdleTimeFinal is the final idle time
    ULONG                   IdleTimeFinal;

    // pMicrophonePacket is the array of microphone packets
    XMEDIAPACKET            *pMicrophonePacket[MAX_MICROPHONE];
    // pHeadphonePacket is the array of headphone packets
    XMEDIAPACKET            *pHeadphonePacket[MAX_HEADPHONE];
    // pEncoderPacket is the encoder packet
    XMEDIAPACKET            *pEncoderPacket[MAX_ENCODER];
    // pDecoderPacket is the decoder packet
    XMEDIAPACKET            *pDecoderPacket[MAX_DECODER];
    // pQueuePacket is the queue packet
    XMEDIAPACKET            *pQueuePacket[MAX_QUEUE];
    // pQueuePacketInfo is the queue packet info
    PCM_INFO                *pQueuePacketInfo[MAX_QUEUE];
    // pMixPacket is the mixing packet
    XMEDIAPACKET            *pMixPacket = NULL;
    // wNumMixPackets is the number of packets in the mixing buffer
    WORD                    wNumMixPackets;
    // wValue is a counter to enumerate each value in the buffer
    WORD                    wValue;
    // fMixBuffer is a pointer to the mixing buffer
    FLOAT                   *fMixBuffer = NULL;
    // wEncoderSize is the size of the encoder packet buffer
    WORD                    wEncoderSize = 0;

    // hMicrophoneEvent is a handle to the microphone packet event
    HANDLE                  *hMicrophoneEvent[MAX_MICROPHONE];
    // hHeadphoneEvent is a handle to the headphone packet event
    HANDLE                  *hHeadphoneEvent[MAX_HEADPHONE];

    // wfxFormat is the wave format for the device
    WAVEFORMATEX            wfxFormat;
    // QueueConfig is the queue config for the object
    QUEUE_XMO_CONFIG        QueueConfig;
    // pMicrophoneObject is the array of microphone devices
    XMediaObject            *pMicrophoneObject[MAX_MICROPHONE];
    // pHeadphoneObject is the array of headphone devices
    XMediaObject            *pHeadphoneObject[MAX_HEADPHONE];
    // pEncoderObject is the encoder object
    XMediaObject            *pEncoderObject[MAX_ENCODER];
    // pDecoderObject is the decoder object
    XMediaObject            *pDecoderObject[MAX_DECODER];
    // pQueueObject is the array of queue objects
    XVoiceQueueMediaObject  *pQueueObject[MAX_QUEUE];

    // hMicrophoneResult is the result of the microphone device creation
    HRESULT                 hMicrophoneResult[MAX_MICROPHONE];
    // hHeadphoneResult is the result of the headphone device creation
    HRESULT                 hHeadphoneResult[MAX_HEADPHONE];
    // hEncoderResult is the result of the encoder object creation
    HRESULT                 hEncoderResult[MAX_ENCODER];
    // hDecoderResult is the result of the decoder object creation
    HRESULT                 hDecoderResult[MAX_DECODER];
    // hQueueResult is the result of the queue object creation
    HRESULT                 hQueueResult[MAX_QUEUE];
    // hResult is the result of an object operation
    HRESULT                 hResult;

    // dwMicrophoneAttempts is the number of microphone device creation attempts
    DWORD                   dwMicrophoneAttempts[MAX_MICROPHONE];
    // dwHeadphoneAttempts is the number of headphone device creation attempts
    DWORD                   dwHeadphoneAttempts[MAX_HEADPHONE];
    // dwEncoderAttempts is the number of encoder object creation attempts
    DWORD                   dwEncoderAttempts[MAX_ENCODER];
    // dwDecoderAttempts is the number of decoder object creation attempts
    DWORD                   dwDecoderAttempts[MAX_DECODER];
    // dwQueueAttempts is the number of queue object creation attempts
    DWORD                   dwQueueAttempts[MAX_QUEUE];
    // dwTotalMicrophoneFailures is the number of total microphone device creation failures
    DWORD                   dwTotalMicrophoneFailures = 0;
    // dwTotalHeadphoneFailures is the number of total headphone device creation failures
    DWORD                   dwTotalHeadphoneFailures = 0;
    // dwTotalEncoderFailures is the number of total encoder object creation failures
    DWORD                   dwTotalEncoderFailures = 0;
    // dwTotalDecoderFailures is the number of total decoder object creation failures
    DWORD                   dwTotalDecoderFailures = 0;
    // dwTotalQueueFailures is the number of total queue object creation failures
    DWORD                   dwTotalQueueFailures = 0;

    // dwObject is a counter to enumerate each object
    DWORD                   dwObject;
    // dwPacket is a counter to enumerate each packet
    DWORD                   dwPacket;

    // bPassed is the result of this test
    BOOL                    bPassed = TRUE;



    // Set the numbers
    dwNumMicrophones = __min(MAX_MICROPHONE, dwNumMicrophones);
    dwNumHeadphones = __min(MAX_HEADPHONE, dwNumHeadphones);
    dwNumEncoder = __min(MAX_ENCODER, dwNumEncoder);
    dwNumDecoder = __min(MAX_DECODER, dwNumDecoder);
    dwNumQueue = __min(MAX_QUEUE, dwNumQueue);



    // Initialize the variables
    ZeroMemory(pMicrophonePacket, sizeof(pMicrophonePacket));
    ZeroMemory(pHeadphonePacket, sizeof(pHeadphonePacket));
    ZeroMemory(pEncoderPacket, sizeof(pEncoderPacket));
    ZeroMemory(pDecoderPacket, sizeof(pDecoderPacket));    
    ZeroMemory(pQueuePacket, sizeof(pQueuePacket));
    ZeroMemory(pQueuePacketInfo, sizeof(pQueuePacketInfo));

    ZeroMemory(hMicrophoneEvent, sizeof(hMicrophoneEvent));
    ZeroMemory(hHeadphoneEvent, sizeof(hHeadphoneEvent));

    ZeroMemory(pMicrophoneObject, sizeof(pMicrophoneObject));
    ZeroMemory(pHeadphoneObject, sizeof(pHeadphoneObject));
    ZeroMemory(pEncoderObject, sizeof(pEncoderObject));
    ZeroMemory(pDecoderObject, sizeof(pDecoderObject));
    ZeroMemory(pQueueObject, sizeof(pQueueObject));



    // Initialize the wave format
    ZeroMemory(&wfxFormat, sizeof(wfxFormat));
    wfxFormat.wFormatTag = WAVE_FORMAT_PCM;
    wfxFormat.nChannels = 1;
    wfxFormat.nSamplesPerSec = VOICE_SAMPLING_RATE;
    wfxFormat.nAvgBytesPerSec = 2 * VOICE_SAMPLING_RATE;
    wfxFormat.nBlockAlign = 2;
    wfxFormat.wBitsPerSample = 16;
    wfxFormat.cbSize = 0;

    // Initialize the queue config
    ZeroMemory(&QueueConfig, sizeof(QueueConfig));
    QueueConfig.cbSize = sizeof(QueueConfig);
    QueueConfig.wVoiceSamplingRate = VOICE_SAMPLING_RATE;
    QueueConfig.dwCodecTag = dwCodecTag;
    QueueConfig.wMsOfDataPerPacket = wMsOfDataPerPacket;
    QueueConfig.wMaxDelay = wNumPackets * wMsOfDataPerPacket;



    // Create the devices and objects

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        dwMicrophoneAttempts[dwObject] = 0;

        do {
            hMicrophoneResult[dwObject] = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, dwObject, wNumPackets, &wfxFormat, &pMicrophoneObject[dwObject]);
            dwMicrophoneAttempts[dwObject]++;

            if (FAILED(hMicrophoneResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hMicrophoneResult[dwObject]));
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        dwHeadphoneAttempts[dwObject] = 0;

        do {
            hHeadphoneResult[dwObject] = XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, dwObject, wNumPackets, &wfxFormat, &pHeadphoneObject[dwObject]);
            dwHeadphoneAttempts[dwObject]++;

            if (FAILED(hHeadphoneResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hHeadphoneResult[dwObject]));
    }

    // Encoder
    for (dwObject = 0; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
        dwEncoderAttempts[dwObject] = 0;

        do {
            hEncoderResult[dwObject] = XVoiceEncoderCreateMediaObject(FALSE, dwCodecTag, &wfxFormat, MAX_THRESHOLD, &pEncoderObject[dwObject]);
            dwEncoderAttempts[dwObject]++;

            if (FAILED(hEncoderResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hEncoderResult[dwObject]));
    }

    // Decoder
    for (dwObject = 0; dwObject < (sizeof(pDecoderObject) / sizeof(pDecoderObject[0])); dwObject++) {
        dwDecoderAttempts[dwObject] = 0;

        do {
            hDecoderResult[dwObject] = XVoiceDecoderCreateMediaObject(dwCodecTag, &wfxFormat, &pDecoderObject[dwObject]);
            dwDecoderAttempts[dwObject]++;

            if (FAILED(hDecoderResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hDecoderResult[dwObject]));
    }


    // Queue
    for (dwObject = 0; dwObject < (sizeof(pQueueObject) / sizeof(pQueueObject[0])); dwObject++) {
        dwQueueAttempts[dwObject] = 0;

        do {
            hQueueResult[dwObject] = XVoiceQueueCreateMediaObject(&QueueConfig, &pQueueObject[dwObject]);
            dwQueueAttempts[dwObject]++;

            if (FAILED(hQueueResult[dwObject])) {
                Sleep(200);
            }
        } while (FAILED(hQueueResult[dwObject]));
    }



    // Determine the codec buffer size
    XVoiceGetCodecBufferSize(pEncoderObject[0], VOICE_BYTES_PER_MS * wMsOfDataPerPacket, &wEncoderSize);



    // Create the packets

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        pMicrophonePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
        if (NULL == pMicrophonePacket[dwObject]) {
            Log_va(hLog, TRUE, "%s: pMicrophonePacket [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        pHeadphonePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
        if (NULL == pHeadphonePacket[dwObject]) {
            Log_va(hLog, TRUE, "%s: pHeadphonePacket [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
        pEncoderPacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + wEncoderSize) * wNumPackets);
        if (NULL == pEncoderPacket[dwObject]) {
            Log_va(hLog, TRUE, "%s: pEncoderPacket [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }
    }

    // Decoder
    for (dwObject = 0; dwObject < (sizeof(pDecoderObject) / sizeof(pDecoderObject[0])); dwObject++) {
        pDecoderPacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
        if (NULL == pDecoderPacket[dwObject]) {
            Log_va(hLog, TRUE, "%s: pDecoderPacket [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }
    }

    // Queue
    for (dwObject = 0; dwObject < (sizeof(pQueueObject) / sizeof(pQueueObject[0])); dwObject++) {
        pQueuePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
        if (NULL == pQueuePacket[dwObject]) {
            Log_va(hLog, TRUE, "%s: pQueuePacket [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }

        pQueuePacketInfo[dwObject] = (PCM_INFO *) xMemAlloc(hMemObject, sizeof(PCM_INFO) * wNumPackets);
        if (NULL == pQueuePacketInfo[dwObject]) {
            Log_va(hLog, TRUE, "%s: pQueuePacketInfo [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }
    }

    // Mixing packet
    pMixPacket = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
    if (NULL == pMixPacket) {
        Log_va(hLog, TRUE, "%s: pMixPacket memory allocation failed, ec = 0x%08x\r\n", lpszTestName, GetLastError());

        bPassed = FALSE;

        goto ExitTest;
    }

    // Mixing buffer
    fMixBuffer = (FLOAT *) xMemAlloc(hMemObject, sizeof(FLOAT) * VOICE_BYTES_PER_MS * wMsOfDataPerPacket / sizeof(SHORT));
    if (NULL == fMixBuffer) {
        Log_va(hLog, TRUE, "%s: fMixBuffer memory allocation failed, ec = 0x%08x\r\n", lpszTestName, GetLastError());

        bPassed = FALSE;

        goto ExitTest;
    }



    // Initialize the events

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        hMicrophoneEvent[dwObject] = (HANDLE *) xMemAlloc(hMemObject, sizeof(HANDLE) * wNumPackets);
        if (NULL == hMicrophoneEvent[dwObject]) {
            Log_va(hLog, TRUE, "%s: hMicrophoneEvent [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            hMicrophoneEvent[dwObject][dwPacket] = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (NULL == hMicrophoneEvent[dwObject][dwPacket]) {
                Log_va(hLog, TRUE, "%s: hMicrophoneEvent [%d][%d] event failed, ec = 0x%08x\r\n", lpszTestName, dwObject, dwPacket, GetLastError());

                bPassed = FALSE;

                goto ExitTest;
            }
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        hHeadphoneEvent[dwObject] = (HANDLE *) xMemAlloc(hMemObject, sizeof(HANDLE) * wNumPackets);
        if (NULL == hHeadphoneEvent[dwObject]) {
            Log_va(hLog, TRUE, "%s: hHeadphoneEvent [%d] memory allocation failed, ec = 0x%08x\r\n", lpszTestName, dwObject, GetLastError());

            bPassed = FALSE;

            goto ExitTest;
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            hHeadphoneEvent[dwObject][dwPacket] = CreateEvent(NULL, FALSE, FALSE, NULL);
            if (NULL == hHeadphoneEvent[dwObject][dwPacket]) {
                Log_va(hLog, TRUE, "%s: hHeadphoneEvent [%d][%d] event failed, ec = 0x%08x\r\n", lpszTestName, dwObject, dwPacket, GetLastError());

                bPassed = FALSE;

                goto ExitTest;
            }
        }
    }



    // Initialize the packets

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pMicrophonePacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pMicrophonePacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pMicrophonePacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pMicrophonePacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pMicrophonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pMicrophonePacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pMicrophonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
            pMicrophonePacket[dwObject][dwPacket].hCompletionEvent = hMicrophoneEvent[dwObject][dwPacket];
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pHeadphonePacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pHeadphonePacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pHeadphonePacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pHeadphonePacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pHeadphonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pHeadphonePacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pHeadphonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
            pHeadphonePacket[dwObject][dwPacket].hCompletionEvent = hHeadphoneEvent[dwObject][dwPacket];
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pEncoderPacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pEncoderPacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + wEncoderSize * dwPacket);
            pEncoderPacket[dwObject][dwPacket].dwMaxSize = wEncoderSize;
            pEncoderPacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pEncoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + wEncoderSize) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pEncoderPacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pEncoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + wEncoderSize) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
        }
    }

    // Decoder
    for (dwObject = 0; dwObject < (sizeof(pDecoderObject) / sizeof(pDecoderObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pDecoderPacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pDecoderPacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pDecoderPacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pDecoderPacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pDecoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pDecoderPacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pDecoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
        }
    }

    // Queue
    for (dwObject = 0; dwObject < (sizeof(pQueueObject) / sizeof(pQueueObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pQueuePacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pQueuePacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pQueuePacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pQueuePacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pQueuePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pQueuePacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pQueuePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));

            pQueuePacketInfo[dwObject][dwPacket].cbSize = sizeof(pQueuePacketInfo[dwObject][dwPacket]);
        }
    }

    // Mixing packet
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        pMixPacket[dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pMixPacket + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
        pMixPacket[dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
        pMixPacket[dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pMixPacket + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
        pMixPacket[dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pMixPacket + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
    }



    // Seed the microphone packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        pMicrophoneObject[0]->Process(NULL, &pMicrophonePacket[0][dwPacket]);
    }

    // Wait for the microphone packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        WaitForSingleObject(hMicrophoneEvent[0][dwPacket], INFINITE);

        for (dwObject = 1; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
            CopyMemory(pMicrophonePacket[dwObject][dwPacket].pvBuffer, pMicrophonePacket[0][dwPacket].pvBuffer, pMicrophonePacket[dwObject][dwPacket].dwMaxSize);
        }
    }

    // Seed the encoder packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        pEncoderObject[0]->Process(&pMicrophonePacket[0][dwPacket], &pEncoderPacket[0][dwPacket]);

        for (dwObject = 1; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
            CopyMemory(pEncoderPacket[dwObject][dwPacket].pvBuffer, pEncoderPacket[0][dwPacket].pvBuffer, pEncoderPacket[dwObject][dwPacket].dwMaxSize);
        }
    }

    // Seed the headphone packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
            CopyMemory(pHeadphonePacket[dwObject][dwPacket].pvBuffer, pMicrophonePacket[0][dwPacket].pvBuffer, pHeadphonePacket[dwObject][dwPacket].dwMaxSize);
        }
    }



    // Query the CPU times
    GetCpuTimes(&KernelTimeInitial, &InterruptTimeInitial, &DpcTimeInitial, &IdleTimeInitial);



    // Microphone
    if (0 != dwNumMicrophones) {
        // Submit the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumMicrophones; dwObject++) {
                pMicrophoneObject[dwObject]->Process(NULL, &pMicrophonePacket[dwObject][dwPacket]);
            }
        }

        // Wait for the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumMicrophones; dwObject++) {
                WaitForSingleObject(hMicrophoneEvent[dwObject][dwPacket], INFINITE);
            }
        }
    }

    // Encoder
    if (0 != dwNumEncoder) {
        // Submit the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumEncoder; dwObject++) {
                pEncoderObject[dwObject]->Process(&pMicrophonePacket[dwObject % (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0]))][dwPacket], &pEncoderPacket[dwObject][dwPacket]);
            }
        }
    }

    // Decoder
    if (0 != dwNumDecoder) {
        // Submit the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumDecoder; dwObject++) {
                pDecoderObject[dwObject]->Process(&pEncoderPacket[dwObject % (sizeof(pEncoderObject) / sizeof(pEncoderObject[0]))][dwPacket], &pDecoderPacket[dwObject][dwPacket]);
            }

            // Mix the packets
            if (TRUE == bMix) {
                wNumMixPackets = 0;
                ZeroMemory(fMixBuffer, sizeof(FLOAT) * VOICE_BYTES_PER_MS * wMsOfDataPerPacket / sizeof(SHORT));

                for (dwObject = 0; dwObject < dwNumDecoder; dwObject++) {
                    wNumMixPackets++;

                    for (wValue = 0; wValue < (pDecoderPacket[dwObject][dwPacket].dwMaxSize / sizeof(SHORT)); wValue++) {
                        fMixBuffer[wValue] += (FLOAT) ((SHORT *) pDecoderPacket[dwObject][dwPacket].pvBuffer)[wValue];
                    }
                }

                for (wValue = 0; wValue < (pMixPacket[dwPacket].dwMaxSize / sizeof(SHORT)); wValue++) {
                    ((SHORT *) pMixPacket[dwPacket].pvBuffer)[wValue] = (SHORT) (fMixBuffer[wValue] / wNumMixPackets);
                }
            }
        }
    }

    // Queue
    if (0 != dwNumQueue) {
        // Submit the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumQueue; dwObject++) {
                pQueueObject[dwObject]->Process(&pEncoderPacket[dwObject % (sizeof(pEncoderObject) / sizeof(pEncoderObject[0]))][dwPacket], NULL);
            }
        }

        // Wait for the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumQueue; dwObject++) {
                do {
                    hResult = pQueueObject[dwObject]->Process(NULL, &pQueuePacket[dwObject][dwPacket]);

                    if (FAILED(hResult)) {
                        Sleep(wMsOfDataPerPacket);
                    }
                } while (FAILED(hResult));
            }

            // Mix the packets
            if (TRUE == bMix) {
                wNumMixPackets = 0;
                ZeroMemory(fMixBuffer, sizeof(FLOAT) * VOICE_BYTES_PER_MS * wMsOfDataPerPacket / sizeof(SHORT));

                for (dwObject = 0; dwObject < dwNumQueue; dwObject++) {
                    pQueueObject[dwObject]->GetLastOutputInfo(&pQueuePacketInfo[dwObject][dwPacket]);

                    if (0 < pQueuePacketInfo[dwObject][dwPacket].nMaxPower) {
                        wNumMixPackets++;

                        for (wValue = 0; wValue < (pQueuePacket[dwObject][dwPacket].dwMaxSize / sizeof(SHORT)); wValue++) {
                            fMixBuffer[wValue] += (FLOAT) ((SHORT *) pQueuePacket[dwObject][dwPacket].pvBuffer)[wValue];
                        }
                    }
                }

                for (wValue = 0; wValue < (pMixPacket[dwPacket].dwMaxSize / sizeof(SHORT)); wValue++) {
                    ((SHORT *) pMixPacket[dwPacket].pvBuffer)[wValue] = (SHORT) (fMixBuffer[wValue] / wNumMixPackets);
                }
            }
        }
    }

    // Headphone
    if (0 != dwNumHeadphones) {
        // Submit the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumHeadphones; dwObject++) {
                pHeadphoneObject[dwObject]->Process(&pHeadphonePacket[dwObject][dwPacket], NULL);
            }
        }

        // Wait for the packets
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumHeadphones; dwObject++) {
                // Wait for a packet
                WaitForSingleObject(hHeadphoneEvent[dwObject][dwPacket], INFINITE);
            }
        }
    }



    // Query the CPU times
    GetCpuTimes(&KernelTimeFinal, &InterruptTimeFinal, &DpcTimeFinal, &IdleTimeFinal);

ExitTest:
    // Free the devices and objects

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        if (NULL != pMicrophoneObject[dwObject]) {
            dwTotalMicrophoneFailures += dwMicrophoneAttempts[dwObject] - 1;
            *dwNumMicrophoneFailures += dwMicrophoneAttempts[dwObject] - 1;

            // Close the device
            pMicrophoneObject[dwObject]->Release();
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        if (NULL != pHeadphoneObject[dwObject]) {
            dwTotalHeadphoneFailures += dwHeadphoneAttempts[dwObject] - 1;
            *dwNumHeadphoneFailures += dwHeadphoneAttempts[dwObject] - 1;

            // Close the device
            pHeadphoneObject[dwObject]->Release();
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
        if (NULL != pEncoderObject[dwObject]) {
            dwTotalEncoderFailures += dwEncoderAttempts[dwObject] - 1;
            *dwNumEncoderFailures += dwEncoderAttempts[dwObject] - 1;

            // Close the device
            pEncoderObject[dwObject]->Release();
        }
    }

    // Decoder
    for (dwObject = 0; dwObject < (sizeof(pDecoderObject) / sizeof(pDecoderObject[0])); dwObject++) {
        if (NULL != pDecoderObject[dwObject]) {
            dwTotalDecoderFailures += dwDecoderAttempts[dwObject] - 1;
            *dwNumDecoderFailures += dwDecoderAttempts[dwObject] - 1;

            // Close the device
            pDecoderObject[dwObject]->Release();
        }
    }

    // Queue
    for (dwObject = 0; dwObject < (sizeof(pQueueObject) / sizeof(pQueueObject[0])); dwObject++) {
        if (NULL != pQueueObject[dwObject]) {
            dwTotalQueueFailures += dwQueueAttempts[dwObject] - 1;
            *dwNumQueueFailures += dwQueueAttempts[dwObject] - 1;

            // Close the device
            pQueueObject[dwObject]->Release();
        }
    }



    // Free the events

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            if ((NULL != hMicrophoneEvent[dwObject]) && (NULL != hMicrophoneEvent[dwObject][dwPacket])) {
                CloseHandle(hMicrophoneEvent[dwObject][dwPacket]);
            }
        }

        if (NULL != hMicrophoneEvent[dwObject]) {
            xMemFree(hMemObject, hMicrophoneEvent[dwObject]);
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            if ((NULL != hHeadphoneEvent[dwObject]) && (NULL != hHeadphoneEvent[dwObject][dwPacket])) {
                CloseHandle(hHeadphoneEvent[dwObject][dwPacket]);
            }
        }

        if (NULL != hHeadphoneEvent[dwObject]) {
            xMemFree(hMemObject, hHeadphoneEvent[dwObject]);
        }
    }



    // Free the packets

    // Microphone
    for (dwObject = 0; dwObject < (sizeof(pMicrophoneObject) / sizeof(pMicrophoneObject[0])); dwObject++) {
        if (NULL != pMicrophonePacket[dwObject]) {
            xMemFree(hMemObject, pMicrophonePacket[dwObject]);
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < (sizeof(pHeadphoneObject) / sizeof(pHeadphoneObject[0])); dwObject++) {
        if (NULL != pHeadphonePacket[dwObject]) {
            xMemFree(hMemObject, pHeadphonePacket[dwObject]);
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < (sizeof(pEncoderObject) / sizeof(pEncoderObject[0])); dwObject++) {
        if (NULL != pEncoderPacket[dwObject]) {
            xMemFree(hMemObject, pEncoderPacket[dwObject]);
        }
    }

    // Decoder
    for (dwObject = 0; dwObject < (sizeof(pDecoderObject) / sizeof(pDecoderObject[0])); dwObject++) {
        if (NULL != pDecoderPacket[dwObject]) {
            xMemFree(hMemObject, pDecoderPacket[dwObject]);
        }
    }

    // Queue
    for (dwObject = 0; dwObject < (sizeof(pQueueObject) / sizeof(pQueueObject[0])); dwObject++) {
        if (NULL != pQueuePacket[dwObject]) {
            xMemFree(hMemObject, pQueuePacket[dwObject]);
            xMemFree(hMemObject, pQueuePacketInfo[dwObject]);
        }
    }

    // Mixing packet
    if (NULL != pMixPacket){
        xMemFree(hMemObject, pMixPacket);
    }

    // Mixing buffer
    if (NULL != fMixBuffer) {
        xMemFree(hMemObject, fMixBuffer);
    }



    if (TRUE == bPassed) {
        // Log the stats
        LogStats(hLog, lpszTestName, dwTestIteration, KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, &dwTotalMicrophoneFailures, &dwTotalHeadphoneFailures, &dwTotalEncoderFailures, &dwTotalDecoderFailures, &dwTotalQueueFailures);
        CalculateAndAddStatsDiff(KernelTimeInitial, KernelTimeFinal, InterruptTimeInitial, InterruptTimeFinal, DpcTimeInitial, DpcTimeFinal, IdleTimeInitial, IdleTimeFinal, KernelTimeDiff, InterruptTimeDiff, DpcTimeDiff, IdleTimeDiff);
    }

    return (TRUE == bPassed);
}



typedef BOOL (*PDEVICE_CREATE_FUNCTION) (
    IN     HANDLE  hLog,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumMicrophoneFailures,
    IN OUT DWORD   *dwNumHeadphoneFailures,
    IN     DWORD   dwNumMicrophones,
    IN     DWORD   dwNumHeadphones
);

typedef struct _DEVICE_CREATE_FUNCTION_TABLE {
    PDEVICE_CREATE_FUNCTION  pFunction;
    LPSTR                    lpszTestName;
    DWORD                    dwNumMicrophones;
    DWORD                    dwNumHeadphones;
} DEVICE_CREATE_FUNCTION_TABLE, *PDEVICE_CREATE_FUNCTION_TABLE;

static DEVICE_CREATE_FUNCTION_TABLE DeviceCreateFunctionTable[] = {
    { DeviceCreate, "Device Create, Microphone, 1",             1, 0 },
    { DeviceCreate, "Device Create, Microphone, 4",             4, 0 },
    { DeviceCreate, "Device Create, Headphone, 1",              0, 1 },
    { DeviceCreate, "Device Create, Headphone, 4",              0, 4 },
    { DeviceCreate, "Device Create, Microphone & Headphone, 1", 1, 1 },
    { DeviceCreate, "Device Create, Microphone & Headphone, 4", 4, 4 }
};

#define DeviceCreateFunctionTableCount (sizeof(DeviceCreateFunctionTable) / sizeof(DEVICE_CREATE_FUNCTION_TABLE))



typedef BOOL (*PCODEC_CREATE_FUNCTION) (
    IN     HANDLE  hLog,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumEncoderFailures,
    IN OUT DWORD   *dwNumDecoderFailures,
    IN OUT DWORD   *dwNumQueueFailures,
    IN     DWORD   dwNumEncoder,
    IN     DWORD   dwNumDecoder,
    IN     DWORD   dwNumQueue,
    IN     DWORD   dwCodecTag,
    IN     WORD    wMsOfDataPerPacket
);

typedef struct _CODEC_CREATE_FUNCTION_TABLE {
    PCODEC_CREATE_FUNCTION  pFunction;
    LPSTR                   lpszTestName;
    DWORD                   dwNumEncoder;
    DWORD                   dwNumDecoder;
    DWORD                   dwNumQueue;
    DWORD                   dwCodecTag;
    WORD                    wMsOfDataPerPacket;
} CODEC_CREATE_FUNCTION_TABLE, *PCODEC_CREATE_FUNCTION_TABLE;

static CODEC_CREATE_FUNCTION_TABLE CodecCreateFunctionTable[] = {
    { CodecCreate, "Codec Create, Encoder, Voxware SC03, 1",                1, 0,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Encoder, Voxware SC03, 4",                4, 0,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC03, 1",                0, 1,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC03, 4",                0, 4,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC03, 12",               0, 12, 0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC03, 1",             0, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC03, 4",             0, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC03, 12",            0, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC03, 1",             0, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC03, 4",             0, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC03, 12",            0, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC03, 1",            0, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC03, 4",            0, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC03, 12",           0, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC03, 1",      1, 1,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC03, 4",      4, 4,  0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC03, 12",     4, 12, 0,  WAVE_FORMAT_VOXWARE_SC03, 0   },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC03, 1",   1, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC03, 4",   4, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC03, 12",  4, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC03, 1",   1, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC03, 4",   4, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC03, 12",  4, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC03, 1",  1, 0,  1,  WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC03, 4",  4, 0,  4,  WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC03, 12", 4, 0,  12, WAVE_FORMAT_VOXWARE_SC03, 100 },
    { CodecCreate, "Codec Create, Encoder, Voxware SC06, 1",                1, 0,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Encoder, Voxware SC06, 4",                4, 0,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC06, 1",                0, 1,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC06, 4",                0, 4,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Decoder, Voxware SC06, 12",               0, 12, 0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC06, 1",             0, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC06, 4",             0, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Queue 20ms, Voxware SC06, 12",            0, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC06, 1",             0, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC06, 4",             0, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Queue 40ms, Voxware SC06, 12",            0, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC06, 1",            0, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 100 },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC06, 4",            0, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 100 },
    { CodecCreate, "Codec Create, Queue 100ms, Voxware SC06, 12",           0, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 100 },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC06, 1",      1, 1,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC06, 4",      4, 4,  0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Encoder & Decoder, Voxware SC06, 12",     4, 12, 0,  WAVE_FORMAT_VOXWARE_SC06, 0   },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC06, 1",   1, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC06, 4",   4, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 20ms, Voxware SC06, 12",  4, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 20  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC06, 1",   1, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC06, 4",   4, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 40ms, Voxware SC06, 12",  4, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 40  },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC06, 1",  1, 0,  1,  WAVE_FORMAT_VOXWARE_SC06, 100 },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC06, 4",  4, 0,  4,  WAVE_FORMAT_VOXWARE_SC06, 100 },
    { CodecCreate, "Codec Create, Encoder & Queue 100ms, Voxware SC06, 12", 4, 0,  12, WAVE_FORMAT_VOXWARE_SC06, 100 }
};

#define CodecCreateFunctionTableCount (sizeof(CodecCreateFunctionTable) / sizeof(CODEC_CREATE_FUNCTION_TABLE))



typedef BOOL (*PMEDIA_PROCESS_FUNCTION) (
    IN     HANDLE  hLog,
    IN     HANDLE  hMemObject,
    IN     LPSTR   lpszTestName,
    IN     DWORD   dwTestIteration,
    IN OUT ULONG   *KernelTimeDiff,
    IN OUT ULONG   *InterruptTimeDiff,
    IN OUT ULONG   *DpcTimeDiff,
    IN OUT ULONG   *IdleTimeDiff,
    IN OUT DWORD   *dwNumMicrophoneFailures,
    IN OUT DWORD   *dwNumHeadphoneFailures,
    IN OUT DWORD   *dwNumEncoderFailures,
    IN OUT DWORD   *dwNumDecoderFailures,
    IN OUT DWORD   *dwNumQueueFailures,
    IN     DWORD   dwNumMicrophones,
    IN     DWORD   dwNumHeadphones,
    IN     DWORD   dwNumEncoder,
    IN     DWORD   dwNumDecoder,
    IN     DWORD   dwNumQueue,
    IN     BOOL    bMix,
    IN     DWORD   dwCodecTag,
    IN     WORD    wNumPackets,
    IN     WORD    wMsOfDataPerPacket
);

typedef struct _MEDIA_PROCESS_FUNCTION_TABLE {
    PMEDIA_PROCESS_FUNCTION  pFunction;
    LPSTR                    lpszTestName;
    DWORD                    dwNumMicrophones;
    DWORD                    dwNumHeadphones;
    DWORD                    dwNumEncoder;
    DWORD                    dwNumDecoder;
    DWORD                    dwNumQueue;
    BOOL                     bMix;
    DWORD                    dwCodecTag;
    WORD                     wNumPackets;
    WORD                     wMsOfDataPerPacket;
} MEDIA_PROCESS_FUNCTION_TABLE, *PMEDIA_PROCESS_FUNCTION_TABLE;

static MEDIA_PROCESS_FUNCTION_TABLE MediaProcessFunctionTable[] = {
    { MediaProcess, "Process, Microphone, 10 Buffers, 20ms, Voxware SC03, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Headphone, 10 Buffers, 20ms, Voxware SC03, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Encoder, 10 Buffers, 20ms, Voxware SC03, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC03, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC03, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC03, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC03, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Microphone, 10 Buffers, 20ms, Voxware SC03, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Headphone, 10 Buffers, 20ms, Voxware SC03, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Encoder, 10 Buffers, 20ms, Voxware SC03, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC03, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 20ms, Voxware SC03, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC03, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 20ms, Voxware SC03, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC03, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 20ms, Voxware SC03, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC03, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 20ms, Voxware SC03, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC03, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 20ms, Voxware SC03, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC03, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 20ms, Voxware SC03, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC03, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 20ms, Voxware SC03, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC03, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 20ms, Voxware SC03, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  20  },
    { MediaProcess, "Process, Microphone, 50 Buffers, 20ms, Voxware SC03, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Headphone, 50 Buffers, 20ms, Voxware SC03, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Encoder, 50 Buffers, 20ms, Voxware SC03, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC03, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC03, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC03, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC03, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Microphone, 50 Buffers, 20ms, Voxware SC03, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Headphone, 50 Buffers, 20ms, Voxware SC03, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Encoder, 50 Buffers, 20ms, Voxware SC03, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC03, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Decoder Mix, 50 Buffers, 20ms, Voxware SC03, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC03, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Queue Mix, 50 Buffers, 20ms, Voxware SC03, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC03, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 50 Buffers, 20ms, Voxware SC03, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC03, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Queue Mix), 50 Buffers, 20ms, Voxware SC03, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC03, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Decoder Mix, 50 Buffers, 20ms, Voxware SC03, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC03, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Queue Mix, 50 Buffers, 20ms, Voxware SC03, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC03, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 50 Buffers, 20ms, Voxware SC03, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC03, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, All (Queue Mix), 50 Buffers, 20ms, Voxware SC03, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 50,  20  },
    { MediaProcess, "Process, Microphone, 5 Buffers, 40ms, Voxware SC03, 1",           1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Headphone, 5 Buffers, 40ms, Voxware SC03, 1",            0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Encoder, 5 Buffers, 40ms, Voxware SC03, 1",              0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC03, 1",              0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC03, 1",                0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC03, 1",        1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC03, 1",          1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Microphone, 5 Buffers, 40ms, Voxware SC03, 4",           4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Headphone, 5 Buffers, 40ms, Voxware SC03, 4",            0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Encoder, 5 Buffers, 40ms, Voxware SC03, 4",              0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC03, 4",              0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Decoder Mix, 5 Buffers, 40ms, Voxware SC03, 4",          0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC03, 4",                0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Queue Mix, 5 Buffers, 40ms, Voxware SC03, 4",            0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC03, 4",        4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Decoder Mix), 5 Buffers, 40ms, Voxware SC03, 4",    4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC03, 4",          4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Queue Mix), 5 Buffers, 40ms, Voxware SC03, 4",      4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC03, 12",             0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Decoder Mix, 5 Buffers, 40ms, Voxware SC03, 12",         0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC03, 12",               0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Queue Mix, 5 Buffers, 40ms, Voxware SC03, 12",           0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC03, 12",       4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Decoder Mix), 5 Buffers, 40ms, Voxware SC03, 12",   4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC03, 12",         4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, All (Queue Mix), 5 Buffers, 40ms, Voxware SC03, 12",     4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 5,   40  },
    { MediaProcess, "Process, Microphone, 25 Buffers, 40ms, Voxware SC03, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Headphone, 25 Buffers, 40ms, Voxware SC03, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Encoder, 25 Buffers, 40ms, Voxware SC03, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC03, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC03, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC03, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC03, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Microphone, 25 Buffers, 40ms, Voxware SC03, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Headphone, 25 Buffers, 40ms, Voxware SC03, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Encoder, 25 Buffers, 40ms, Voxware SC03, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC03, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Decoder Mix, 25 Buffers, 40ms, Voxware SC03, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC03, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Queue Mix, 25 Buffers, 40ms, Voxware SC03, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC03, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Decoder Mix), 25 Buffers, 40ms, Voxware SC03, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC03, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Queue Mix), 25 Buffers, 40ms, Voxware SC03, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC03, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Decoder Mix, 25 Buffers, 40ms, Voxware SC03, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC03, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Queue Mix, 25 Buffers, 40ms, Voxware SC03, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC03, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Decoder Mix), 25 Buffers, 40ms, Voxware SC03, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC03, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, All (Queue Mix), 25 Buffers, 40ms, Voxware SC03, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 25,  40  },
    { MediaProcess, "Process, Microphone, 2 Buffers, 100ms, Voxware SC03, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Headphone, 2 Buffers, 100ms, Voxware SC03, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Encoder, 2 Buffers, 100ms, Voxware SC03, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC03, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC03, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC03, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC03, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Microphone, 2 Buffers, 100ms, Voxware SC03, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Headphone, 2 Buffers, 100ms, Voxware SC03, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Encoder, 2 Buffers, 100ms, Voxware SC03, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC03, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Decoder Mix, 2 Buffers, 100ms, Voxware SC03, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC03, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Queue Mix, 2 Buffers, 100ms, Voxware SC03, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC03, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Decoder Mix), 2 Buffers, 100ms, Voxware SC03, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC03, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Queue Mix), 2 Buffers, 100ms, Voxware SC03, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC03, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Decoder Mix, 2 Buffers, 100ms, Voxware SC03, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC03, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Queue Mix, 2 Buffers, 100ms, Voxware SC03, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC03, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Decoder Mix), 2 Buffers, 100ms, Voxware SC03, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC03, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, All (Queue Mix), 2 Buffers, 100ms, Voxware SC03, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 2,   100 },
    { MediaProcess, "Process, Microphone, 10 Buffers, 100ms, Voxware SC03, 1",         1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Headphone, 10 Buffers, 100ms, Voxware SC03, 1",          0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Encoder, 10 Buffers, 100ms, Voxware SC03, 1",            0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC03, 1",            0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC03, 1",              0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC03, 1",      1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC03, 1",        1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Microphone, 10 Buffers, 100ms, Voxware SC03, 4",         4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Headphone, 10 Buffers, 100ms, Voxware SC03, 4",          0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Encoder, 10 Buffers, 100ms, Voxware SC03, 4",            0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC03, 4",            0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 100ms, Voxware SC03, 4",        0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC03, 4",              0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 100ms, Voxware SC03, 4",          0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC03, 4",      4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 100ms, Voxware SC03, 4",  4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC03, 4",        4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 100ms, Voxware SC03, 4",    4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC03, 12",           0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 100ms, Voxware SC03, 12",       0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC03, 12",             0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 100ms, Voxware SC03, 12",         0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC03, 12",     4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 100ms, Voxware SC03, 12", 4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC03, 12",       4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 100ms, Voxware SC03, 12",   4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC03, 10,  100 },
    { MediaProcess, "Process, Microphone, 10 Buffers, 20ms, Voxware SC06, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Headphone, 10 Buffers, 20ms, Voxware SC06, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Encoder, 10 Buffers, 20ms, Voxware SC06, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC06, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC06, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC06, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC06, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Microphone, 10 Buffers, 20ms, Voxware SC06, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Headphone, 10 Buffers, 20ms, Voxware SC06, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Encoder, 10 Buffers, 20ms, Voxware SC06, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC06, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 20ms, Voxware SC06, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC06, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 20ms, Voxware SC06, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC06, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 20ms, Voxware SC06, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC06, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 20ms, Voxware SC06, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Decoder, 10 Buffers, 20ms, Voxware SC06, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 20ms, Voxware SC06, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Queue, 10 Buffers, 20ms, Voxware SC06, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 20ms, Voxware SC06, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 20ms, Voxware SC06, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 20ms, Voxware SC06, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 20ms, Voxware SC06, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 20ms, Voxware SC06, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  20  },
    { MediaProcess, "Process, Microphone, 50 Buffers, 20ms, Voxware SC06, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Headphone, 50 Buffers, 20ms, Voxware SC06, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Encoder, 50 Buffers, 20ms, Voxware SC06, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC06, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC06, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC06, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC06, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Microphone, 50 Buffers, 20ms, Voxware SC06, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Headphone, 50 Buffers, 20ms, Voxware SC06, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Encoder, 50 Buffers, 20ms, Voxware SC06, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC06, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Decoder Mix, 50 Buffers, 20ms, Voxware SC06, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC06, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Queue Mix, 50 Buffers, 20ms, Voxware SC06, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC06, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 50 Buffers, 20ms, Voxware SC06, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC06, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Queue Mix), 50 Buffers, 20ms, Voxware SC06, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Decoder, 50 Buffers, 20ms, Voxware SC06, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Decoder Mix, 50 Buffers, 20ms, Voxware SC06, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Queue, 50 Buffers, 20ms, Voxware SC06, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Queue Mix, 50 Buffers, 20ms, Voxware SC06, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Decoder), 50 Buffers, 20ms, Voxware SC06, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Decoder Mix), 50 Buffers, 20ms, Voxware SC06, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Queue), 50 Buffers, 20ms, Voxware SC06, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, All (Queue Mix), 50 Buffers, 20ms, Voxware SC06, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 50,  20  },
    { MediaProcess, "Process, Microphone, 5 Buffers, 40ms, Voxware SC06, 1",           1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Headphone, 5 Buffers, 40ms, Voxware SC06, 1",            0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Encoder, 5 Buffers, 40ms, Voxware SC06, 1",              0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC06, 1",              0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC06, 1",                0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC06, 1",        1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC06, 1",          1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Microphone, 5 Buffers, 40ms, Voxware SC06, 4",           4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Headphone, 5 Buffers, 40ms, Voxware SC06, 4",            0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Encoder, 5 Buffers, 40ms, Voxware SC06, 4",              0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC06, 4",              0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Decoder Mix, 5 Buffers, 40ms, Voxware SC06, 4",          0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC06, 4",                0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Queue Mix, 5 Buffers, 40ms, Voxware SC06, 4",            0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC06, 4",        4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Decoder Mix), 5 Buffers, 40ms, Voxware SC06, 4",    4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC06, 4",          4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Queue Mix), 5 Buffers, 40ms, Voxware SC06, 4",      4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Decoder, 5 Buffers, 40ms, Voxware SC06, 12",             0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Decoder Mix, 5 Buffers, 40ms, Voxware SC06, 12",         0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Queue, 5 Buffers, 40ms, Voxware SC06, 12",               0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Queue Mix, 5 Buffers, 40ms, Voxware SC06, 12",           0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Decoder), 5 Buffers, 40ms, Voxware SC06, 12",       4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Decoder Mix), 5 Buffers, 40ms, Voxware SC06, 12",   4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Queue), 5 Buffers, 40ms, Voxware SC06, 12",         4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, All (Queue Mix), 5 Buffers, 40ms, Voxware SC06, 12",     4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 5,   40  },
    { MediaProcess, "Process, Microphone, 25 Buffers, 40ms, Voxware SC06, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Headphone, 25 Buffers, 40ms, Voxware SC06, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Encoder, 25 Buffers, 40ms, Voxware SC06, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC06, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC06, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC06, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC06, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Microphone, 25 Buffers, 40ms, Voxware SC06, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Headphone, 25 Buffers, 40ms, Voxware SC06, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Encoder, 25 Buffers, 40ms, Voxware SC06, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC06, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Decoder Mix, 25 Buffers, 40ms, Voxware SC06, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC06, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Queue Mix, 25 Buffers, 40ms, Voxware SC06, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC06, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Decoder Mix), 25 Buffers, 40ms, Voxware SC06, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC06, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Queue Mix), 25 Buffers, 40ms, Voxware SC06, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Decoder, 25 Buffers, 40ms, Voxware SC06, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Decoder Mix, 25 Buffers, 40ms, Voxware SC06, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Queue, 25 Buffers, 40ms, Voxware SC06, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Queue Mix, 25 Buffers, 40ms, Voxware SC06, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Decoder), 25 Buffers, 40ms, Voxware SC06, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Decoder Mix), 25 Buffers, 40ms, Voxware SC06, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Queue), 25 Buffers, 40ms, Voxware SC06, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, All (Queue Mix), 25 Buffers, 40ms, Voxware SC06, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 25,  40  },
    { MediaProcess, "Process, Microphone, 2 Buffers, 100ms, Voxware SC06, 1",          1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Headphone, 2 Buffers, 100ms, Voxware SC06, 1",           0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Encoder, 2 Buffers, 100ms, Voxware SC06, 1",             0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC06, 1",             0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC06, 1",               0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC06, 1",       1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC06, 1",         1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Microphone, 2 Buffers, 100ms, Voxware SC06, 4",          4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Headphone, 2 Buffers, 100ms, Voxware SC06, 4",           0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Encoder, 2 Buffers, 100ms, Voxware SC06, 4",             0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC06, 4",             0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Decoder Mix, 2 Buffers, 100ms, Voxware SC06, 4",         0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC06, 4",               0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Queue Mix, 2 Buffers, 100ms, Voxware SC06, 4",           0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC06, 4",       4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Decoder Mix), 2 Buffers, 100ms, Voxware SC06, 4",   4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC06, 4",         4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Queue Mix), 2 Buffers, 100ms, Voxware SC06, 4",     4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Decoder, 2 Buffers, 100ms, Voxware SC06, 12",            0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Decoder Mix, 2 Buffers, 100ms, Voxware SC06, 12",        0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Queue, 2 Buffers, 100ms, Voxware SC06, 12",              0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Queue Mix, 2 Buffers, 100ms, Voxware SC06, 12",          0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Decoder), 2 Buffers, 100ms, Voxware SC06, 12",      4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Decoder Mix), 2 Buffers, 100ms, Voxware SC06, 12",  4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Queue), 2 Buffers, 100ms, Voxware SC06, 12",        4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, All (Queue Mix), 2 Buffers, 100ms, Voxware SC06, 12",    4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 2,   100 },
    { MediaProcess, "Process, Microphone, 10 Buffers, 100ms, Voxware SC06, 1",         1, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Headphone, 10 Buffers, 100ms, Voxware SC06, 1",          0, 1, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Encoder, 10 Buffers, 100ms, Voxware SC06, 1",            0, 0, 1, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC06, 1",            0, 0, 0, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC06, 1",              0, 0, 0, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC06, 1",      1, 1, 1, 1,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC06, 1",        1, 1, 1, 0,  1,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Microphone, 10 Buffers, 100ms, Voxware SC06, 4",         4, 0, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Headphone, 10 Buffers, 100ms, Voxware SC06, 4",          0, 4, 0, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Encoder, 10 Buffers, 100ms, Voxware SC06, 4",            0, 0, 4, 0,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC06, 4",            0, 0, 0, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 100ms, Voxware SC06, 4",        0, 0, 0, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC06, 4",              0, 0, 0, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 100ms, Voxware SC06, 4",          0, 0, 0, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC06, 4",      4, 4, 4, 4,  0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 100ms, Voxware SC06, 4",  4, 4, 4, 4,  0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC06, 4",        4, 4, 4, 0,  4,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 100ms, Voxware SC06, 4",    4, 4, 4, 0,  4,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Decoder, 10 Buffers, 100ms, Voxware SC06, 12",           0, 0, 0, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Decoder Mix, 10 Buffers, 100ms, Voxware SC06, 12",       0, 0, 0, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Queue, 10 Buffers, 100ms, Voxware SC06, 12",             0, 0, 0, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, Queue Mix, 10 Buffers, 100ms, Voxware SC06, 12",         0, 0, 0, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Decoder), 10 Buffers, 100ms, Voxware SC06, 12",     4, 4, 4, 12, 0,  FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Decoder Mix), 10 Buffers, 100ms, Voxware SC06, 12", 4, 4, 4, 12, 0,  TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Queue), 10 Buffers, 100ms, Voxware SC06, 12",       4, 4, 4, 0,  12, FALSE, WAVE_FORMAT_VOXWARE_SC06, 10,  100 },
    { MediaProcess, "Process, All (Queue Mix), 10 Buffers, 100ms, Voxware SC06, 12",   4, 4, 4, 0,  12, TRUE,  WAVE_FORMAT_VOXWARE_SC06, 10,  100 }
};

#define MediaProcessFunctionTableCount (sizeof(MediaProcessFunctionTable) / sizeof(MEDIA_PROCESS_FUNCTION_TABLE))

#define AllFunctionTestCount   DeviceCreateFunctionTableCount + CodecCreateFunctionTableCount + MediaProcessFunctionTableCount
#define AllFunctionTableCount  3



void __cdecl main()
{
    // hMemObject is the mem object
    HANDLE         hMemObject = INVALID_HANDLE_VALUE;

    // LaunchData is the title launch data
    LAUNCH_DATA    LaunchData;
    // szCmdLine is the title command line
    CHAR           *szCmdLine;
    // szParameter is a title command line parameter
    CHAR           *szParameter;
    // dwDataType is the type of title launch data
    DWORD          dwDataType;

    // dwTableIndex is a counter to enumerate each table
    DWORD          dwTableIndex = 0;
    // dwNumTables is the number of tables
    DWORD          dwNumTables = AllFunctionTableCount;
    // dwTestIndex is a counter to enumerate each test
    DWORD          dwTestIndex = 0;
    // dwTestIteration is a counter to enumerate each test iteration
    DWORD          dwTestIteration = 0;
    // dwNumIterations is the number of test iterations
    DWORD          dwNumIterations = 10;

    // hLogAll is a handle to the log file
    HANDLE         hLogAll = INVALID_HANDLE_VALUE;
    // lpszLogFileNameAll is the name of the log file
    LPSTR          lpszLogFileNameAll = "T:\\xvoiceperf_all.log";
    // hLogSummary is a handle to the log file
    HANDLE         hLogSummary = INVALID_HANDLE_VALUE;
    // lpszLogFileNameSummary is the name of the log file
    LPSTR          lpszLogFileNameSummary = "T:\\xvoiceperf_summary.log";
    // hLogDeviceCreateCsv is a handle to the device create csv log file
    HANDLE         hLogDeviceCreateCsv = INVALID_HANDLE_VALUE;
    // lpszLogFileNameDeviceCreateCsv is the name of the device create csv log file
    LPSTR          lpszLogFileNameDeviceCreateCsv = "T:\\xvoiceperf_devicecreate.csv";
    // hLogCodecCreateCsv is a handle to the codec create csv log file
    HANDLE         hLogCodecCreateCsv = INVALID_HANDLE_VALUE;
    // lpszLogFileNameCodecCreateCsv is the name of the device create csv log file
    LPSTR          lpszLogFileNameCodecCreateCsv = "T:\\xvoiceperf_codeccreate.csv";
    // hLogProcessCsv is a handle to the process csv log file
    HANDLE         hLogProcessCsv = INVALID_HANDLE_VALUE;
    // lpszLogFileNameProcessCsv is the name of the process csv log file
    LPSTR          lpszLogFileNameProcessCsv = "T:\\xvoiceperf_process.csv";

    // KernelTime is the kernel time
    ULONG          KernelTime;
    // InterruptTime is the interrupt time
    ULONG          InterruptTime;
    // DpcTime is the dpc time
    ULONG          DpcTime;
    // IdleTime is the idle time
    ULONG          IdleTime;

    // dwNumMicrophoneFailures is the number of microphone creation failures
    DWORD          dwNumMicrophoneFailures;
    // dwNumTotalMicrophoneFailures is the number total of microphone creation failures
    DWORD          dwNumTotalMicrophoneFailures = 0;
    // dwNumHeadphoneFailures is the number of headphone creation failures
    DWORD          dwNumHeadphoneFailures;
    // dwNumTotalHeadphoneFailures is the number total of headphone creation failures
    DWORD          dwNumTotalHeadphoneFailures = 0;
    // dwNumEncoderFailures is the number of encoder creation failures
    DWORD          dwNumEncoderFailures;
    // dwNumTotalEncoderFailures is the number total of encoder creation failures
    DWORD          dwNumTotalEncoderFailures = 0;
    // dwNumDecoderFailures is the number of decoder creation failures
    DWORD          dwNumDecoderFailures;
    // dwNumTotalDecoderFailures is the number total of decoder creation failures
    DWORD          dwNumTotalDecoderFailures = 0;
    // dwNumQueueFailures is the number of queue creation failures
    DWORD          dwNumQueueFailures;
    // dwNumTotalQueueFailures is the number total of queue creation failures
    DWORD          dwNumTotalQueueFailures = 0;



    // Create the private heap
    hMemObject = xMemCreate();
    if (INVALID_HANDLE_VALUE == hMemObject) {
        XLaunchNewImage(NULL, NULL);
    }

    // Get the launch info
    XGetLaunchInfo(&dwDataType, &LaunchData);

    if (LDT_FROM_DEBUGGER_CMDLINE == dwDataType) {
        szCmdLine = ((LD_FROM_DEBUGGER_CMDLINE *) &LaunchData)->szCmdLine;

        szParameter = strstr(szCmdLine, "/S:");
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "/s:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-S:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-s:");
        }

        if (NULL != szParameter) {
            dwTableIndex = atoi(szParameter + 3);
        }

        szParameter = strstr(szCmdLine, "/E:");
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "/e:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-E:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-e:");
        }

        if (NULL != szParameter) {
            dwNumTables = __max(atoi(szParameter + 3), AllFunctionTableCount);
        }

        szParameter = strstr(szCmdLine, "/X:");
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "/x:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-X:");
        }
        if (NULL == szParameter) {
            szParameter = strstr(szCmdLine, "-x:");
        }

        if (NULL != szParameter) {
            dwNumIterations = atoi(szParameter + 3);
        }
    }

    // Open the log file
    hLogAll = CreateFile(lpszLogFileNameAll, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    hLogSummary = CreateFile(lpszLogFileNameSummary, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    hLogDeviceCreateCsv = CreateFile(lpszLogFileNameDeviceCreateCsv, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    LogDeviceCreateBanner(hLogDeviceCreateCsv);

    hLogCodecCreateCsv = CreateFile(lpszLogFileNameCodecCreateCsv, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    LogCodecCreateBanner(hLogCodecCreateCsv);

    hLogProcessCsv = CreateFile(lpszLogFileNameProcessCsv, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    LogProcessBanner(hLogProcessCsv);

    // Flush the file buffers to prevent corruption
    FlushFileBuffers(hLogAll);
    FlushFileBuffers(hLogSummary);
    FlushFileBuffers(hLogDeviceCreateCsv);
    FlushFileBuffers(hLogCodecCreateCsv);
    FlushFileBuffers(hLogProcessCsv);

    // Initialize
    if (FALSE == DeviceInit(hLogAll)) {
        goto ExitTest;
    }

    // Device Create Tests
    if ((dwNumTables >= dwTableIndex) && (0 == dwTableIndex)) {
        for (dwTestIndex = 0; dwTestIndex < DeviceCreateFunctionTableCount; dwTestIndex++) {
            ZeroMemory(&KernelTime, sizeof(KernelTime));
            ZeroMemory(&InterruptTime, sizeof(InterruptTime));
            ZeroMemory(&DpcTime, sizeof(DpcTime));
            ZeroMemory(&IdleTime, sizeof(IdleTime));

            dwNumMicrophoneFailures = 0;
            dwNumHeadphoneFailures = 0;

            for (dwTestIteration = 0; dwTestIteration < dwNumIterations; dwTestIteration++) {
                if (FALSE == DeviceCreateFunctionTable[dwTestIndex].pFunction(hLogAll, DeviceCreateFunctionTable[dwTestIndex].lpszTestName, dwTestIteration, &KernelTime, &InterruptTime, &DpcTime, &IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, DeviceCreateFunctionTable[dwTestIndex].dwNumMicrophones, DeviceCreateFunctionTable[dwTestIndex].dwNumHeadphones)) {
                    goto ExitTest;
                }
            }

            LogStatsSummary(hLogSummary, DeviceCreateFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, NULL, NULL, NULL, dwNumIterations);
            LogStatsCsv(hLogDeviceCreateCsv, DeviceCreateFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, NULL, NULL, NULL, dwNumIterations);

            dwNumTotalMicrophoneFailures += dwNumMicrophoneFailures;
            dwNumTotalHeadphoneFailures += dwNumHeadphoneFailures;
        }

        dwTableIndex = 1;
    }

    if ((dwNumTables >= dwTableIndex) && (1 == dwTableIndex)) {
        for (dwTestIndex = 0; dwTestIndex < CodecCreateFunctionTableCount; dwTestIndex++) {
            ZeroMemory(&KernelTime, sizeof(KernelTime));
            ZeroMemory(&InterruptTime, sizeof(InterruptTime));
            ZeroMemory(&DpcTime, sizeof(DpcTime));
            ZeroMemory(&IdleTime, sizeof(IdleTime));

            dwNumEncoderFailures = 0;
            dwNumDecoderFailures = 0;
            dwNumQueueFailures = 0;

            for (dwTestIteration = 0; dwTestIteration < dwNumIterations; dwTestIteration++) {
                if (FALSE == CodecCreateFunctionTable[dwTestIndex].pFunction(hLogAll, CodecCreateFunctionTable[dwTestIndex].lpszTestName, dwTestIteration, &KernelTime, &InterruptTime, &DpcTime, &IdleTime, &dwNumEncoderFailures, &dwNumDecoderFailures, &dwNumQueueFailures, CodecCreateFunctionTable[dwTestIndex].dwNumEncoder, CodecCreateFunctionTable[dwTestIndex].dwNumDecoder, CodecCreateFunctionTable[dwTestIndex].dwNumQueue, CodecCreateFunctionTable[dwTestIndex].dwCodecTag, CodecCreateFunctionTable[dwTestIndex].wMsOfDataPerPacket)) {
                    goto ExitTest;
                }
            }

            LogStatsSummary(hLogSummary, CodecCreateFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, NULL, NULL, NULL, dwNumIterations);
            LogStatsCsv(hLogCodecCreateCsv, CodecCreateFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, NULL, NULL, NULL, dwNumIterations);

            dwNumTotalEncoderFailures += dwNumEncoderFailures;
            dwNumTotalDecoderFailures += dwNumDecoderFailures;
            dwNumTotalQueueFailures += dwNumQueueFailures;
        }

        dwTableIndex = 2;
    }

    if ((dwNumTables >= dwTableIndex) && (2 == dwTableIndex)) {
        for (dwTestIndex = 0; dwTestIndex < MediaProcessFunctionTableCount; dwTestIndex++) {
            ZeroMemory(&KernelTime, sizeof(KernelTime));
            ZeroMemory(&InterruptTime, sizeof(InterruptTime));
            ZeroMemory(&DpcTime, sizeof(DpcTime));
            ZeroMemory(&IdleTime, sizeof(IdleTime));

            dwNumMicrophoneFailures = 0;
            dwNumHeadphoneFailures = 0;
            dwNumEncoderFailures = 0;
            dwNumDecoderFailures = 0;
            dwNumQueueFailures = 0;

            for (dwTestIteration = 0; dwTestIteration < dwNumIterations; dwTestIteration++) {
                if (FALSE == MediaProcessFunctionTable[dwTestIndex].pFunction(hLogAll, hMemObject, MediaProcessFunctionTable[dwTestIndex].lpszTestName, dwTestIteration, &KernelTime, &InterruptTime, &DpcTime, &IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, &dwNumEncoderFailures, &dwNumDecoderFailures, &dwNumQueueFailures, MediaProcessFunctionTable[dwTestIndex].dwNumMicrophones, MediaProcessFunctionTable[dwTestIndex].dwNumHeadphones, MediaProcessFunctionTable[dwTestIndex].dwNumEncoder, MediaProcessFunctionTable[dwTestIndex].dwNumDecoder, MediaProcessFunctionTable[dwTestIndex].dwNumQueue, MediaProcessFunctionTable[dwTestIndex].bMix, MediaProcessFunctionTable[dwTestIndex].dwCodecTag, MediaProcessFunctionTable[dwTestIndex].wNumPackets, MediaProcessFunctionTable[dwTestIndex].wMsOfDataPerPacket)) {
                    goto ExitTest;
                }
            }

            LogStatsSummary(hLogSummary, MediaProcessFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, &dwNumEncoderFailures, &dwNumDecoderFailures, &dwNumQueueFailures, dwNumIterations);
            LogStatsCsv(hLogProcessCsv, MediaProcessFunctionTable[dwTestIndex].lpszTestName, KernelTime, InterruptTime, DpcTime, IdleTime, &dwNumMicrophoneFailures, &dwNumHeadphoneFailures, &dwNumEncoderFailures, &dwNumDecoderFailures, &dwNumQueueFailures, dwNumIterations);

            dwNumTotalMicrophoneFailures += dwNumMicrophoneFailures;
            dwNumTotalHeadphoneFailures += dwNumHeadphoneFailures;
            dwNumTotalEncoderFailures += dwNumEncoderFailures;
            dwNumTotalDecoderFailures += dwNumDecoderFailures;
            dwNumTotalQueueFailures += dwNumQueueFailures;
        }

        dwTableIndex = 3;
    }

ExitTest:
    // Log failures
    if (dwNumTables <= dwTableIndex) {
        LogStatsFailures(hLogAll, &dwNumTotalMicrophoneFailures, &dwNumTotalHeadphoneFailures, &dwNumTotalEncoderFailures, &dwNumTotalDecoderFailures, &dwNumTotalQueueFailures, AllFunctionTestCount * dwNumIterations);
        LogStatsFailures(hLogSummary, &dwNumTotalMicrophoneFailures, &dwNumTotalHeadphoneFailures, &dwNumTotalEncoderFailures, &dwNumTotalDecoderFailures, &dwNumTotalQueueFailures, AllFunctionTestCount * dwNumIterations);
    }

    // Flush the file buffers to prevent corruption
    FlushFileBuffers(hLogAll);
    FlushFileBuffers(hLogSummary);
    FlushFileBuffers(hLogDeviceCreateCsv);
    FlushFileBuffers(hLogCodecCreateCsv);
    FlushFileBuffers(hLogProcessCsv);

    // Close the log file
    CloseHandle(hLogAll);
    CloseHandle(hLogSummary);
    CloseHandle(hLogDeviceCreateCsv);
    CloseHandle(hLogCodecCreateCsv);
    CloseHandle(hLogProcessCsv);

    // Free the private heap
    xMemClose(hMemObject);

    XLaunchNewImage(NULL, NULL);

    __asm int 3;
}
