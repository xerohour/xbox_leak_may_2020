/*
This folder contains data files that contain sets of parameters for the reverb engine.

The format of the files is as follows:

Offset	Description
0	Host address of the base of the main delay line
1	Size-1 of host delay line measures in samples
2	Host address of the first 'long reverb' extension to the main delay line
3	Size-1 of first extension delay line measures in samples
4	Host address of the second 'long reverb' extension to the main delay line
5	Size-1 of second extension delay line measures in samples
6	Host address of the third 'long reverb' extension to the main delay line
7	Size-1 of third extension delay line measures in samples

8	Host address of the reflection delay buffer 0
9	Size-1 of reflection delay buffer 0
10	Host address of the reflection delay buffer 1
11	Size-1 of reflection delay buffer 1
12	Host address of the reflection delay buffer 2
13	Size-1 of reflection delay buffer 2
14	Host address of the reflection delay buffer 3
15	Size-1 of reflection delay buffer 3

16	Host address of the short reverb delay buffer 0
17	Size-1 of short reverb delay buffer 0
18	Host address of the short reverb delay buffer 1
19	Size-1 of short reverb delay buffer 1
20	Host address of the short reverb delay buffer 2
21	Size-1 of short reverb delay buffer 2
22	Host address of the short reverb delay buffer 3
23	Size-1 of short reverb delay buffer 3

24	Host address of the long reverb delay buffer 2
25	Size-1 of long reverb delay buffer 2
26	Host address of the long reverb delay buffer 3
27	Size-1 of long reverb delay buffer 3


28	Reflections input delay 0 measured in samples
29	Reflections input delay 1 measured in samples
30	Reflections input delay 2 measured in samples
31	Reflections input delay 3 measured in samples
32	Reflections input delay 4 measured in samples

33	Short reverb input delay measured in samples

34	Long reverb input 0a delay measured in samples
35	Long reverb input 0b delay measured in samples
36	Long reverb input 1a delay measured in samples
37	Long reverb input 1b delay measured in samples
38	Long reverb input 2a delay measured in samples
39	Long reverb input 2b delay measured in samples
40	Long reverb input 3a delay measured in samples
41	Long reverb input 3b delay measured in samples

42	Reflections delay 0 measured in samples
43	Reflections delay 1 measured in samples
44	Reflections delay 2 measured in samples
45	Reflections delay 3 measured in samples

46	Long reverb delay measured in samples

47	Short reverb channel 0a input gain * 2^23
48	Short reverb channel 0b input gain * 2^23
49	Short reverb channel 1a input gain * 2^23
50	Short reverb channel 1b input gain * 2^23
51	Short reverb channel 2a input gain * 2^23
52	Short reverb channel 2b input gain * 2^23
53	Short reverb channel 3a input gain * 2^23
54	Short reverb channel 3b input gain * 2^23

55	Long reverb input gain * 2^23

56	Long reverb crossfeed gain * 2^23

57	Reflections channel 0 output gain *2^16
58	Reflections channel 1 output gain *2^16
59	Reflections channel 2 output gain *2^16
60	Reflections channel 3 output gain *2^16

61	Short reverb output gain *2^16

62	Long reverb output gain *2^16

63	Number of channels (2 for 2 channel; 4 for 4 channels)

64	Input IIR A coefficient * 2^23
65	Input IIR B coefficient * 2^23
66	IIR Long reverb main delay extension 1 A coefficient * 2^23
67	IIR Long reverb main delay extension 1 B coefficient * 2^23
68	IIR Long reverb main delay extension 2 A coefficient * 2^23
69	IIR Long reverb main delay extension 2 B coefficient * 2^23
70	IIR Long reverb main delay extension 3 A coefficient * 2^23
71	IIR Long reverb main delay extension 3 B coefficient * 2^23

72	IIR short reverb channel 0 A coefficient * 2^23
73	IIR Short reverb channel 0 B coefficient * 2^23
74	IIR Short reverb channel 1 A coefficient * 2^23
75	IIR Short reverb channel 1 B coefficient * 2^23
76	IIR Short reverb channel 2 A coefficient * 2^23
77	IIR Short reverb channel 2 B coefficient * 2^23
78	IIR Short reverb channel 3 A coefficient * 2^23
79	IIR Short reverb channel 3 B coefficient * 2^23

80	IIR Long reverb channel 0a A coefficient * 2^23
81	IIR Long reverb channel 0b B coefficient * 2^23
82	IIR Long reverb channel 1a A coefficient * 2^23
83	IIR Long reverb channel 1b B coefficient * 2^23



All numbers are decimal integers represented in ASCII format
All delays are measured in samples, divisible by 4 and greater than 32 samples.
The maximum value for the reverb gain is 20dB (x100) so the corresponding output gain values are scaled up by 2^(23-7)=2^16 to fill the entire 24 bit data range.
Although the maximum value for the reflections gain is 10dB (x10) it is easiest to scale the gain values by 2^16 to match the reverb gain.

Other floating point values are scaled from -1 to 1 so are scaled 
by 2^23 to fill the 24 bit data range.
*/

typedef struct _REVERB_EFFECT_PARAMS {

	DWORD dwMainDelayLineBase;
	DWORD dwMainDelayLineSize;

	DWORD dwMainDelayLine1Base;
	DWORD dwMainDelayLine1Size;

	DWORD dwMainDelayLine2Base;
	DWORD dwMainDelayLine2Size;

	DWORD dwMainDelayLine3Base;
	DWORD dwMainDelayLine3Size;

	DWORD dwBufferReflection0Base;
    DWORD dwBufferReflection0Size;

	DWORD dwBufferReflection1Base;
    DWORD dwBufferReflection1Size;

	DWORD dwBufferReflection2Base;
    DWORD dwBufferReflection2Size;

	DWORD dwBufferReflection3Base;
    DWORD dwBufferReflection3Size;

	DWORD dwBufferShortReverb0Base;
    DWORD dwBufferShortReverb0Size;
	
	DWORD dwBufferShortReverb1Base;
    DWORD dwBufferShortReverb1Size;

	DWORD dwBufferShortReverb2Base;
    DWORD dwBufferShortReverb2Size;

	DWORD dwBufferShortReverb3Base;
    DWORD dwBufferShortReverb3Size;

	DWORD dwBufferLongReverb0Base;
    DWORD dwBufferLongReverb0Size;

	DWORD dwBufferLongReverb1Base;
    DWORD dwBufferLongReverb1Size;

	DWORD dwReflectionsInputDelay0;
	DWORD dwReflectionsInputDelay1;
	DWORD dwReflectionsInputDelay2;
	DWORD dwReflectionsInputDelay3;
	DWORD dwReflectionsInputDelay4;

	DWORD dwShortReverbInputDelay;

	DWORD dwLongReverbInputDelay0a;
	DWORD dwLongReverbInputDelay0b;

	DWORD dwLongReverbInputDelay1a;
	DWORD dwLongReverbInputDelay1b;

	DWORD dwLongReverbInputDelay2a;
	DWORD dwLongReverbInputDelay2b;

	DWORD dwLongReverbInputDelay3a;
	DWORD dwLongReverbInputDelay3b;

	DWORD dwReflectionDelay0;
	DWORD dwReflectionDelay1;
	DWORD dwReflectionDelay2;
	DWORD dwReflectionDelay3;

	DWORD dwLongReverbDelay;

	DWORD dwShortReverbInputGain_Channel0a;
	DWORD dwShortReverbInputGain_Channel0b;

	DWORD dwShortReverbInputGain_Channel1a;
	DWORD dwShortReverbInputGain_Channel1b;

	DWORD dwShortReverbInputGain_Channel2a;
	DWORD dwShortReverbInputGain_Channel2b;

	DWORD dwShortReverbInputGain_Channel3a;
	DWORD dwShortReverbInputGain_Channel3b;

	DWORD dwLongReverbInputGain;

	DWORD dwLongReverbCrossfeedGain;

	DWORD dwNewReflectionOutputGain_Channel0;
	DWORD dwNewReflectionOutputGain_Channel1;
	DWORD dwNewReflectionOutputGain_Channel2;
	DWORD dwNewReflectionOutputGain_Channel3;

	DWORD dwNewShortReverbOutputGain_Channel;
	DWORD dwNewLongReverbOutputGain_Channel;

    DWORD dwNumOutputChannels;
	DWORD IIR_Input_Delay;
	DWORD IIR_InputCoefficientA;
	DWORD IIR_InputCoefficientB;

	DWORD IIR_LongReverb_MainDelay0_Delay;
	DWORD IIR_LongReverb_MainDelay0_CoefficientA;
	DWORD IIR_LongReverb_MainDelay0_CoefficientB;

	DWORD IIR_LongReverb_MainDelay1_Delay;
	DWORD IIR_LongReverb_MainDelay1_CoefficientA;
	DWORD IIR_LongReverb_MainDelay1_CoefficientB;

	DWORD IIR_LongReverb_MainDelay2_Delay;
	DWORD IIR_LongReverb_MainDelay2_CoefficientA;
	DWORD IIR_LongReverb_MainDelay2_CoefficientB;

	DWORD IIR_ShortReverb_Channel0_Delay;
	DWORD IIR_ShortReverb_Channel0_CoefficientA;
	DWORD IIR_ShortReverb_Channel0_CoefficientB;

	DWORD IIR_ShortReverb_Channel1_Delay;
	DWORD IIR_ShortReverb_Channel1_CoefficientA;
	DWORD IIR_ShortReverb_Channel1_CoefficientB;

	DWORD IIR_ShortReverb_Channel2_Delay;
	DWORD IIR_ShortReverb_Channel2_CoefficientA;
	DWORD IIR_ShortReverb_Channel2_CoefficientB;

	DWORD IIR_ShortReverb_Channel3_Delay;
	DWORD IIR_ShortReverb_Channel3_CoefficientA;
	DWORD IIR_ShortReverb_Channel3_CoefficientB;

	DWORD IIR_LongReverb_Channel0_Delay;
	DWORD IIR_LongReverb_Channel0_CoefficientA;
	DWORD IIR_LongReverb_Channel0_CoefficientB;

	DWORD IIR_LongReverb_Channel1_Delay;
	DWORD IIR_LongReverb_Channel1_CoefficientA;
	DWORD IIR_LongReverb_Channel1_CoefficientB;

} REVERB_EFFECT_PARAMS, *PREVERB_EFFECT_PARAMS;


