/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  main.c

Abstract:

  This module tests XMO Voice memory usage

Author:

  Steven Kehrli (steveke) 20-Dec-2001

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#include "precomp.h"



#define MAX_MICROPHONE       4
#define MAX_HEADPHONE        4
#define MAX_ENCODER          4
#define MAX_DECODER          4
#define MAX_QUEUE            4

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



VOID
MediaProcess(
    IN  HANDLE  hLog,
    IN  LPSTR   lpszTestName,
    IN  HANDLE  hMemObject,
    IN  DWORD   dwNumObjects,
    IN  DWORD   dwCodecTag,
    IN  WORD    wNumPackets,
    IN  WORD    wMsOfDataPerPacket
)
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Routine Description:

  Processes media

Arguments:

  hMemObject - Handle to the memory objet
  dwNumObjects - Number of objects
  dwCodecTag - Specifies the codec
  wNumPackets - Specifies the number of packets
  wMsOfDataPerPacket - Milliseconds of voice data per packet

Return Value:

  None

------------------------------------------------------------------------------*/
{
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
    // pQueueInfoPacket is the queue packet
    XMEDIAPACKET            *pQueueInfoPacket[MAX_QUEUE];
    // pQueueInfo is the queue packet info
    PCM_INFO                *pQueueInfo[MAX_QUEUE];
    // wEncoderSize is the size of the encoder packet buffer
    WORD                    wEncoderSize = 0;

    // hMicrophoneEvent is a handle to the microphone packet event
    HANDLE                  *hMicrophoneEvent = NULL;
    HANDLE                  *hMicrophoneEventArray[MAX_MICROPHONE];
    // hHeadphoneEvent is a handle to the headphone packet event
    HANDLE                  *hHeadphoneEvent = NULL;
    HANDLE                  *hHeadphoneEventArray[MAX_MICROPHONE];

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
    // pQueueInfoObject is the array of queue objects
    XVoiceQueueMediaObject  *pQueueInfoObject[MAX_QUEUE];

    // dwObject is a counter to enumerate each object
    DWORD                   dwObject;
    // dwPacket is a counter to enumerate each packet
    DWORD                   dwPacket;
    // dwTime is the time of the test
    DWORD                   dwTime;

    // mmStats are the memory manager stats
    MM_STATISTICS           mmStatsInitial;
    MM_STATISTICS           mmStatsFinal;
    // psStats are the process manager stats
    PS_STATISTICS           psStatsInitial;
    PS_STATISTICS           psStatsFinal;



    // Initialize the variables
    ZeroMemory(pMicrophonePacket, sizeof(pMicrophonePacket));
    ZeroMemory(pHeadphonePacket, sizeof(pHeadphonePacket));
    ZeroMemory(pEncoderPacket, sizeof(pEncoderPacket));
    ZeroMemory(pDecoderPacket, sizeof(pDecoderPacket));    
    ZeroMemory(pQueuePacket, sizeof(pQueuePacket));
    ZeroMemory(pQueueInfoPacket, sizeof(pQueueInfoPacket));
    ZeroMemory(pQueueInfo, sizeof(pQueueInfo));

    ZeroMemory(hMicrophoneEventArray, sizeof(hMicrophoneEventArray));
    ZeroMemory(hHeadphoneEventArray, sizeof(hHeadphoneEventArray));

    ZeroMemory(pMicrophoneObject, sizeof(pMicrophoneObject));
    ZeroMemory(pHeadphoneObject, sizeof(pHeadphoneObject));
    ZeroMemory(pEncoderObject, sizeof(pEncoderObject));
    ZeroMemory(pDecoderObject, sizeof(pDecoderObject));
    ZeroMemory(pQueueObject, sizeof(pQueueObject));
    ZeroMemory(pQueueInfoObject, sizeof(pQueueInfoObject));



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
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_MICROPHONE, dwObject, wNumPackets, &wfxFormat, &pMicrophoneObject[dwObject]))) {
            Sleep(200);
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceCreateMediaObject(XDEVICE_TYPE_VOICE_HEADPHONE, dwObject, wNumPackets, &wfxFormat, &pHeadphoneObject[dwObject]))) {
            Sleep(200);
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceEncoderCreateMediaObject(FALSE, dwCodecTag, &wfxFormat, MAX_THRESHOLD, &pEncoderObject[dwObject]))) {
            Sleep(200);
        }
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceDecoderCreateMediaObject(dwCodecTag, &wfxFormat, &pDecoderObject[dwObject]))) {
            Sleep(200);
        }
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceQueueCreateMediaObject(&QueueConfig, &pQueueObject[dwObject]))) {
            Sleep(200);
        }
    }

    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        while (FAILED(XVoiceQueueCreateMediaObject(&QueueConfig, &pQueueInfoObject[dwObject]))) {
            Sleep(200);
        }
    }



    // Determine the codec buffer size
    XVoiceGetCodecBufferSize(pEncoderObject[0], VOICE_BYTES_PER_MS * wMsOfDataPerPacket, &wEncoderSize);



    // Create the packets

    // Microphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pMicrophonePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pHeadphonePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
    }

    // Encoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pEncoderPacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + wEncoderSize) * wNumPackets);
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pDecoderPacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pQueuePacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);
    }

    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        pQueueInfoPacket[dwObject] = (XMEDIAPACKET *) xMemAlloc(hMemObject, (sizeof(XMEDIAPACKET) + sizeof(DWORD) * 2 + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets);

        pQueueInfo[dwObject] = (PCM_INFO *) xMemAlloc(hMemObject, sizeof(PCM_INFO) * wNumPackets);
    }



    // Initialize the events

    // Microphone
    hMicrophoneEvent = (HANDLE *) xMemAlloc(hMemObject, sizeof(HANDLE) * dwNumObjects * wNumPackets);
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        hMicrophoneEventArray[dwObject] = (HANDLE *) ((UINT_PTR) hMicrophoneEvent + (sizeof(HANDLE) * dwObject * wNumPackets));

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            hMicrophoneEventArray[dwObject][dwPacket] = CreateEvent(NULL, FALSE, FALSE, NULL);
        }
    }

    // Headphone
    hHeadphoneEvent = (HANDLE *) xMemAlloc(hMemObject, sizeof(HANDLE) * dwNumObjects * wNumPackets);
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        hHeadphoneEventArray[dwObject] = (HANDLE *) ((UINT_PTR) hHeadphoneEvent + (sizeof(HANDLE) * dwObject * wNumPackets));

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            hHeadphoneEventArray[dwObject][dwPacket] = CreateEvent(NULL, FALSE, FALSE, NULL);
        }
    }



    // Initialize the packets

    // Microphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pMicrophonePacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pMicrophonePacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pMicrophonePacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pMicrophonePacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pMicrophonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pMicrophonePacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pMicrophonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
            pMicrophonePacket[dwObject][dwPacket].hCompletionEvent = hMicrophoneEventArray[dwObject][dwPacket];
        }
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pHeadphonePacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pHeadphonePacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + VOICE_BYTES_PER_MS * wMsOfDataPerPacket * dwPacket);
            pHeadphonePacket[dwObject][dwPacket].dwMaxSize = VOICE_BYTES_PER_MS * wMsOfDataPerPacket;
            pHeadphonePacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pHeadphonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pHeadphonePacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pHeadphonePacket[dwObject] + (sizeof(XMEDIAPACKET) + VOICE_BYTES_PER_MS * wMsOfDataPerPacket) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
            pHeadphonePacket[dwObject][dwPacket].hCompletionEvent = hHeadphoneEventArray[dwObject][dwPacket];
        }
    }

    // Encoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pEncoderPacket[dwObject][dwPacket].pvBuffer = (XMEDIAPACKET *) ((BYTE *) pEncoderPacket[dwObject] + sizeof(XMEDIAPACKET) * wNumPackets + wEncoderSize * dwPacket);
            pEncoderPacket[dwObject][dwPacket].dwMaxSize = wEncoderSize;
            pEncoderPacket[dwObject][dwPacket].pdwCompletedSize = (LPDWORD) ((BYTE *) pEncoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + wEncoderSize) * wNumPackets + sizeof(DWORD) * 2 * dwPacket);
            pEncoderPacket[dwObject][dwPacket].pdwStatus = (LPDWORD) ((BYTE *) pEncoderPacket[dwObject] + (sizeof(XMEDIAPACKET) + wEncoderSize) * wNumPackets + sizeof(DWORD) * ( 2 * dwPacket + 1));
        }
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            pQueueInfo[dwObject][dwPacket].cbSize = sizeof(pQueueInfo[dwObject][dwPacket]);
        }
    }



    // Microphone

    Log_va(hLog, TRUE, "%s, Microphone\n", lpszTestName);

    // Submit the packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
            pMicrophoneObject[dwObject]->Process(NULL, &pMicrophonePacket[dwObject][dwPacket]);
        }
    }

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                // Wait for a packet
                WaitForSingleObject(hMicrophoneEventArray[dwObject][dwPacket], INFINITE);

                // Resubmit the packet
                pMicrophoneObject[dwObject]->Process(NULL, &pMicrophonePacket[dwObject][dwPacket]);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);

    // Wait for the packets
    WaitForMultipleObjects(wNumPackets * dwNumObjects, (HANDLE *) hMicrophoneEvent, TRUE, INFINITE);



    // Encoder

    Log_va(hLog, TRUE, "%s, Encoder\n", lpszTestName);

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                pEncoderObject[dwObject]->Process(&pMicrophonePacket[dwObject][dwPacket], &pEncoderPacket[dwObject][dwPacket]);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);



    // Decoder

    Log_va(hLog, TRUE, "%s, Decoder\n", lpszTestName);

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                pDecoderObject[dwObject]->Process(&pEncoderPacket[dwObject][dwPacket], &pHeadphonePacket[dwObject][dwPacket]);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);



    // Queue

    Log_va(hLog, TRUE, "%s, Queue\n", lpszTestName);

    // Submit the packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
            pQueueObject[dwObject]->Process(&pEncoderPacket[dwObject][dwPacket], NULL);
        }
    }

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                // Wait for a packet
                while (FAILED(pQueueObject[dwObject]->Process(NULL, &pHeadphonePacket[dwObject][dwPacket]))) {
                    Sleep(wMsOfDataPerPacket);
                }

                // Resubmit the packet
                pQueueObject[dwObject]->Process(&pEncoderPacket[dwObject][dwPacket], NULL);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);



    // Queue & OutputInfo

    Log_va(hLog, TRUE, "%s, Queue & OutputInfo\n", lpszTestName);

    // Submit the packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
            pQueueInfoObject[dwObject]->Process(&pEncoderPacket[dwObject][dwPacket], NULL);
        }
    }

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                // Wait for a packet
                while (FAILED(pQueueInfoObject[dwObject]->Process(NULL, &pHeadphonePacket[dwObject][dwPacket]))) {
                    Sleep(wMsOfDataPerPacket);
                }

                // Get the last info
                pQueueInfoObject[dwObject]->GetLastOutputInfo(&pQueueInfo[dwObject][dwPacket]);

                // Resubmit the packet
                pQueueInfoObject[dwObject]->Process(&pEncoderPacket[dwObject][dwPacket], NULL);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);



    // Headphone

    Log_va(hLog, TRUE, "%s, Headphone\n", lpszTestName);

    // Submit the packets
    for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
        for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
            pHeadphoneObject[dwObject]->Process(&pHeadphonePacket[dwObject][dwPacket], NULL);
        }
    }

    for (dwTime = 0; dwTime < 70000; dwTime += (wMsOfDataPerPacket * wNumPackets)) {
        if (10000 > dwTime) {
            // Get the stats
            GetStats(&mmStatsInitial, &psStatsInitial);
        }

        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
                // Wait for a packet
                WaitForSingleObject(hHeadphoneEventArray[dwObject][dwPacket], INFINITE);

                // Resubmit the packet
                pHeadphoneObject[dwObject]->Process(&pHeadphonePacket[dwObject][dwPacket], NULL);
            }
        }
    }

    // Get the stats
    GetStats(&mmStatsFinal, &psStatsFinal);

    // Check the stats
    CheckStats(INVALID_HANDLE_VALUE, &mmStatsInitial, &mmStatsFinal, &psStatsInitial, &psStatsFinal);

    // Wait for the packets
    WaitForMultipleObjects(wNumPackets * dwNumObjects, (HANDLE *) hHeadphoneEvent, TRUE, INFINITE);



    // Free the devices and objects

    // Microphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pMicrophoneObject[dwObject]->Release();
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pHeadphoneObject[dwObject]->Release();
    }

    // Encoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pEncoderObject[dwObject]->Release();
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pDecoderObject[dwObject]->Release();
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pQueueObject[dwObject]->Release();
    }

    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        // Close the device
        pQueueInfoObject[dwObject]->Release();
    }



    // Free the events

    // Microphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            CloseHandle(hMicrophoneEventArray[dwObject][dwPacket]);
        }
    }
    xMemFree(hMemObject, hMicrophoneEvent);

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        for (dwPacket = 0; dwPacket < wNumPackets; dwPacket++) {
            CloseHandle(hHeadphoneEventArray[dwObject][dwPacket]);
        }
    }
    xMemFree(hMemObject, hHeadphoneEvent);



    // Free the packets

    // Microphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pMicrophonePacket[dwObject]);
    }

    // Headphone
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pHeadphonePacket[dwObject]);
    }

    // Encoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pEncoderPacket[dwObject]);
    }

    // Decoder
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pDecoderPacket[dwObject]);
    }

    // Queue
    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pQueuePacket[dwObject]);
    }

    for (dwObject = 0; dwObject < dwNumObjects; dwObject++) {
        xMemFree(hMemObject, pQueueInfoPacket[dwObject]);
        xMemFree(hMemObject, pQueueInfo[dwObject]);
    }
}



typedef VOID (*PMEDIA_PROCESS_FUNCTION) (
    IN  HANDLE  hLog,
    IN  LPSTR   lpszTestName,
    IN  HANDLE  hMemObject,
    IN  DWORD   dwNumObjects,
    IN  DWORD   dwCodecTag,
    IN  WORD    wNumPackets,
    IN  WORD    wMsOfDataPerPacket
);

typedef struct _MEDIA_PROCESS_FUNCTION_TABLE {
    PMEDIA_PROCESS_FUNCTION  pFunction;
    LPSTR                    lpszTestName;
    DWORD                    dwNumObjects;
    DWORD                    dwCodecTag;
    WORD                     wNumPackets;
    WORD                     wMsOfDataPerPacket;
} MEDIA_PROCESS_FUNCTION_TABLE, *PMEDIA_PROCESS_FUNCTION_TABLE;

static MEDIA_PROCESS_FUNCTION_TABLE MediaProcessFunctionTable[] = {
    { MediaProcess, "Process, 10 Buffers, 20ms, Voxware SC03, 1",  1, WAVE_FORMAT_VOXWARE_SC03, 10, 20  },
    { MediaProcess, "Process, 10 Buffers, 20ms, Voxware SC03, 4",  4, WAVE_FORMAT_VOXWARE_SC03, 10, 20  },
    { MediaProcess, "Process, 50 Buffers, 20ms, Voxware SC03, 1",  1, WAVE_FORMAT_VOXWARE_SC03, 50, 20  },
    { MediaProcess, "Process, 50 Buffers, 20ms, Voxware SC03, 4",  4, WAVE_FORMAT_VOXWARE_SC03, 50, 20  },
    { MediaProcess, "Process, 5 Buffers, 40ms, Voxware SC03, 1",   1, WAVE_FORMAT_VOXWARE_SC03, 5,  40  },
    { MediaProcess, "Process, 5 Buffers, 40ms, Voxware SC03, 4",   4, WAVE_FORMAT_VOXWARE_SC03, 5,  40  },
    { MediaProcess, "Process, 25 Buffers, 40ms, Voxware SC03, 1",  1, WAVE_FORMAT_VOXWARE_SC03, 25, 40  },
    { MediaProcess, "Process, 25 Buffers, 40ms, Voxware SC03, 4",  4, WAVE_FORMAT_VOXWARE_SC03, 25, 40  },
    { MediaProcess, "Process, 2 Buffers, 100ms, Voxware SC03, 1",  1, WAVE_FORMAT_VOXWARE_SC03, 2,  100 },
    { MediaProcess, "Process, 2 Buffers, 100ms, Voxware SC03, 4",  4, WAVE_FORMAT_VOXWARE_SC03, 2,  100 },
    { MediaProcess, "Process, 10 Buffers, 100ms, Voxware SC03, 1", 1, WAVE_FORMAT_VOXWARE_SC03, 10, 100 },
    { MediaProcess, "Process, 10 Buffers, 100ms, Voxware SC03, 4", 4, WAVE_FORMAT_VOXWARE_SC03, 10, 100 },
    { MediaProcess, "Process, 10 Buffers, 20ms, Voxware SC06, 1",  1, WAVE_FORMAT_VOXWARE_SC06, 10, 20  },
    { MediaProcess, "Process, 10 Buffers, 20ms, Voxware SC06, 4",  4, WAVE_FORMAT_VOXWARE_SC06, 10, 20  },
    { MediaProcess, "Process, 50 Buffers, 20ms, Voxware SC06, 1",  1, WAVE_FORMAT_VOXWARE_SC06, 50, 20  },
    { MediaProcess, "Process, 50 Buffers, 20ms, Voxware SC06, 4",  4, WAVE_FORMAT_VOXWARE_SC06, 50, 20  },
    { MediaProcess, "Process, 5 Buffers, 40ms, Voxware SC06, 1",   1, WAVE_FORMAT_VOXWARE_SC06, 5,  40  },
    { MediaProcess, "Process, 5 Buffers, 40ms, Voxware SC06, 4",   4, WAVE_FORMAT_VOXWARE_SC06, 5,  40  },
    { MediaProcess, "Process, 25 Buffers, 40ms, Voxware SC06, 1",  1, WAVE_FORMAT_VOXWARE_SC06, 25, 40  },
    { MediaProcess, "Process, 25 Buffers, 40ms, Voxware SC06, 4",  4, WAVE_FORMAT_VOXWARE_SC06, 25, 40  },
    { MediaProcess, "Process, 2 Buffers, 100ms, Voxware SC06, 1",  1, WAVE_FORMAT_VOXWARE_SC06, 2,  100 },
    { MediaProcess, "Process, 2 Buffers, 100ms, Voxware SC06, 4",  4, WAVE_FORMAT_VOXWARE_SC06, 2,  100 },
    { MediaProcess, "Process, 10 Buffers, 100ms, Voxware SC06, 1", 1, WAVE_FORMAT_VOXWARE_SC06, 10, 100 },
    { MediaProcess, "Process, 10 Buffers, 100ms, Voxware SC06, 4", 4, WAVE_FORMAT_VOXWARE_SC06, 10, 100 }
};

#define MediaProcessFunctionTableCount (sizeof(MediaProcessFunctionTable) / sizeof(MEDIA_PROCESS_FUNCTION_TABLE))



void __cdecl main()
{
    // hMemObject is the mem object
    HANDLE  hMemObject = INVALID_HANDLE_VALUE;
    // hLogAll is the handle to the log object
    HANDLE  hLogAll = INVALID_HANDLE_VALUE;
    // lpszLogFileNameAll is the name of the log file
    LPSTR   lpszLogFileNameAll = "T:\\xvoicemem_all.log";
    // dwTestIndex is a counter to enumerate each test
    DWORD   dwTestIndex = 0;



    // Create the private heap
    hMemObject = xMemCreate();

    // Open the log file
    hLogAll = CreateFile(lpszLogFileNameAll, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    // Flush the file buffers to prevent corruption
    FlushFileBuffers(hLogAll);

    // Initialize
    if (FALSE == DeviceInit(hLogAll)) {
        goto ExitTest;
    }

    for (dwTestIndex = 0; dwTestIndex < MediaProcessFunctionTableCount; dwTestIndex++) {
        MediaProcessFunctionTable[dwTestIndex].pFunction(hLogAll, MediaProcessFunctionTable[dwTestIndex].lpszTestName, hMemObject, MediaProcessFunctionTable[dwTestIndex].dwNumObjects, MediaProcessFunctionTable[dwTestIndex].dwCodecTag, MediaProcessFunctionTable[dwTestIndex].wNumPackets, MediaProcessFunctionTable[dwTestIndex].wMsOfDataPerPacket);
    }

ExitTest:
    // Flush the file buffers to prevent corruption
    FlushFileBuffers(hLogAll);

    // Close the log file
    CloseHandle(hLogAll);

    // Free the private heap
    xMemClose(hMemObject);

    XLaunchNewImage(NULL, NULL);

    __asm int 3;
}
