////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef AC3DECODER_H
#define AC3DECODER_H

#include "..\common\StreamTiming.h"
#include "..\audio\PCMWaveOut.h"
#include "..\audio\AudioBitStream.h"
#include "library\common\profiles.h"
#include "library\hardware\audio\generic\ac3setup.h"

#define HIGH_PRECISION_FFT	0

//#define DOLBY_TEST_MODE		0

#if HIGH_PRECISION_FFT
typedef	double	fftreal;
#else
typedef	float		fftreal;
#endif

struct complex {fftreal r; fftreal i;};

#define COMP_MODE_CUSTOM_A		0
#define COMP_MODE_CUSTOM_B		1
#define COMP_MODE_LINE			2
#define COMP_MODE_RF			3

#define	MAX_AC3_CHANNELS	6
#define	MAX_CPLSUBND	18
#define	MAX_CPLGRPS		100
#define	MAX_CHGRPS		100

#define AC3_BLOCK_TO_ZERO_ON_SWITCH	(32)

#define	M3DB	0.707106781

class AC3Decoder
	{
	protected:
		AudioBitStream	*	strm;
		PCMWaveOut     *	pcmWaveOut;
		GenericProfile *	globalProfile;
		GenericProfile	*	profile;

	private:
		fftreal mant1lt[32][4];
		fftreal mant2lt[128][4];
		fftreal mant4lt[128][2];
		char	exp5lt[128][4];

	protected:
		int		zeroBlocks;
		WORD	syncword;
		WORD	crc1;
		BYTE	fscod;
		BYTE	frmsizecod;
		BYTE	lastfscod;

		int		frameSize;
		__int64	frameStart;

		BYTE	bsid;
		BYTE	bsmod;
		BYTE	acmod;

		BYTE	nfchans;

		BYTE	cmixlev;
		BYTE	surmixlev;
		BYTE	dsurmod;
		bool	lfeon;
		BYTE	dialnorm;
		bool	compre;
		BYTE	compr;
		bool	langcode;
		BYTE	langcod;
		bool	audprodie;
		BYTE	mixlevel;
		BYTE	roomtyp;
		BYTE	dialnorm2;
		bool	compr2e;
		BYTE	compr2;
		bool	lngcod2e;
		BYTE	langcod2;
		bool	audprodi2e;
		BYTE	mixlevel2;
		BYTE	roomtyp2;
		bool	copyrightb;
		bool	origbs;
		bool	timecod1e;
		WORD	timecod1;
		bool	timecod2e;
		WORD	timecod2;

		bool	addbsie;
		BYTE	addbsil;

		bool	blksw[MAX_AC3_CHANNELS];
		bool	dithflag[MAX_AC3_CHANNELS];
		short	dithReg;

		bool	dynrnge;
		BYTE	dynrng;
		bool	dynrng2e;
		BYTE	dynrng2;

		bool	cplstre;
		bool	cplinu;

		bool	chincpl[MAX_AC3_CHANNELS];
		bool	phsflginu;
		BYTE	cplbegf;
		BYTE	cplendf;

		BYTE	ncplsubnd;

		bool	cplbndstrc[MAX_CPLSUBND];

		bool	cplcoe[MAX_AC3_CHANNELS];
		BYTE	mstrcplco[MAX_AC3_CHANNELS];

		BYTE	ncplbnd;

		BYTE	cplcoexp[MAX_AC3_CHANNELS][MAX_CPLSUBND];
		BYTE	cplcomant[MAX_AC3_CHANNELS][MAX_CPLSUBND];
		fftreal cplco[MAX_AC3_CHANNELS][MAX_CPLSUBND];

		bool	phsflg[MAX_CPLSUBND];

		bool	rematstr;

		bool	rematflg[4];

		BYTE	cplexpstr;
		bool	newcplexps;
		BYTE	chexpstr[MAX_AC3_CHANNELS];
		bool	newchexps[MAX_AC3_CHANNELS];
		BYTE	lfeexpstr;
		bool	newlfeexps;
		BYTE	chbwcod[MAX_AC3_CHANNELS];

		BYTE	cplabsexp;
		BYTE	ncplgrps;
		WORD	cplstrtmant;
		WORD	cplendmant;
		WORD	ncplmant;
		BYTE	cplgexps[MAX_CPLGRPS];
		BYTE	cplexps[256];
		WORD	strtmant[MAX_AC3_CHANNELS];
		WORD	endmant[MAX_AC3_CHANNELS];
		WORD	nchmant[MAX_AC3_CHANNELS];
		BYTE	nchgrps[MAX_AC3_CHANNELS];
		BYTE	gexps[MAX_AC3_CHANNELS][MAX_CHGRPS+1];
		BYTE	exps[MAX_AC3_CHANNELS][256];
		BYTE	gainrng[MAX_AC3_CHANNELS];
		WORD	lfestrtmant;
		WORD	lfeendmant;
		BYTE	nlfegrps;
		BYTE	lfegexps[3];
		BYTE	lfeexps[256];
		WORD	nlfemant;

		bool	baie;
		BYTE	sdcycod;
		BYTE	fdcycod;
		BYTE	sgaincod;
		BYTE	dbpbcod;
		BYTE	floorcod;

		bool	snroffste;
		BYTE	csnroffst;
		BYTE	cplfsnroffst;
		BYTE	cplfgaincod;
		bool	nosnroffst;

		BYTE	fsnroffst[MAX_AC3_CHANNELS];
		BYTE	fgaincod[MAX_AC3_CHANNELS];

		BYTE	lfefsnroffst;
		BYTE	lfefgaincod;

		bool	cplleake;
		BYTE	cplfleak;
		BYTE	cplsleak;

		bool	deltbaie;

		BYTE	cpldeltbae;
		BYTE	deltbae[MAX_AC3_CHANNELS];

		BYTE	cpldeltnseg;

		BYTE	cpldeltoffst[8];
		BYTE	cpldeltlen[8];
		BYTE	cpldeltba[8];

		BYTE	deltnseg[MAX_AC3_CHANNELS];
		BYTE	deltoffst[MAX_AC3_CHANNELS][8];
		BYTE	deltlen[MAX_AC3_CHANNELS][8];
		BYTE	deltba[MAX_AC3_CHANNELS][8];

		BYTE	cplbap[256];
		BYTE	chbap[MAX_AC3_CHANNELS][256];
		BYTE	lfebap[256];

//		fftreal	chcoeff[MAX_AC3_CHANNELS][256];

		PadOn16Byte	pad0;

		fftreal	cplcoeff[256];
		fftreal	lfecoeff[256];

		fftreal	chpostfft[MAX_AC3_CHANNELS][512];
		fftreal lfepostfft[512];

		fftreal overlap[6][256];
		fftreal ac3WindowTable2[256*2];

		short	leftRightBuffer[6*256*2];
		short	encodedBuffer[6*256*2];

		AC3DynamicRange		drc;
		AC3DualModeConfig	dualMode;
		AC3KaraokeConfig	karaoke;
		bool				ac3DigitalOutput;

		bool	skiple;
		WORD	skipl;

		int	sampleRate;
		int	outNum;

		bool	downmixActive;
		int		hdr, ldr;
		int		compressionMode;
		bool	useDialNorm;
		bool	surroundDownmix, surroundDownmixProfile, enableLFE;

		bool UnpackExponents(int ngrps, BYTE * gexp, BYTE expstr, BYTE absexp, BYTE * exp);

		void CalculateBitAllocation(
										int start,
										int end,
										int fgaincod,
										int fsnroffst,
										BYTE * exp,
										int fastleak,
										int slowleak,
										BYTE deltbae,
										WORD deltnseg,
										BYTE * deltoffst,
										BYTE * deltba,
										BYTE * deltlen,
										BYTE * bap,
										bool lfe);

		int mant1Num, mant2Num, mant4Num;

		fftreal mant1Val[3], mant2Val[3], mant4Val[2];

		void InitParseMantissa(void);

		fftreal GetDitherValue(void);
		fftreal GetDitheredMantissa(int exp);

		fftreal GetMantissa(int size, int exp, bool dither);

		void GetMantissas(int num, fftreal * dst, BYTE * bap, BYTE * exps, bool dither);

		void UncoupleChannel(int ch);

		void RematrixChannels(void);

		void TransformChannels(void);

		int leftChannelIndex, rightChannelIndex;

		fftreal leftMultiplier, rightMultiplier;
		fftreal cmix, smix;

		bool channelPremixed[MAX_AC3_CHANNELS];

		void PreDownmixChannels(void);

		fftreal CalcCompression(int cVal, int hscl, int lscl);

		void DynamicRangeCompression(void);

		void PostDownmixChannels(void);

		void InitWindow(void);
		void WindowChannels(short * leftRightBuffer);

		void WindowChannel(fftreal * src, fftreal * overlap, fftreal mul, short * dst);
		void WindowChannel(fftreal * src, fftreal * overlap, fftreal * dst);
		void WindowChannelInterleaved(fftreal * src, fftreal * overlap, fftreal * dst, fftreal mul=1.0);

		bool ParseAudblk(int blknum);

		void StartParse(void);

		bool ParseSyncInfo(void);

		bool ParseBSI(void);

		int	lastTime;

		int	currentDataTime;

		bool	hurryUp;
		int	recoverPhase;

		int	lastLeft, lastRight;

		TimingSlave	*	timingSlave;

		void MuteAudioBuffer(void);
		void UnmuteAudioBuffer(void);
		bool CheckCRCAndFrameAC3DigitalOutput(void);
		void InitLocalsNewStream(void);

	public:
		AC3Decoder(AudioBitStream	*	strm,
			        PCMWaveOut		*	pcmWaveOut,
					  TimingSlave		*	timingSlave,
					  GenericProfile			*	globalProfile,
					  GenericProfile			*	profile);

		virtual ~AC3Decoder(void);

		void ParseSyncFrame(void);

		void SkipSyncFrame(void);

		void BeginStreaming(void);

		void StartStreaming(int playbackSpeed);

		void StopStreaming(void);

		void EndStreaming(bool fullReset);

		void DetachStreaming(void);
		void AttachStreaming(void);

		void SetDigitalOut(AudioTypeConfig ac3AudioType);
		void GetDigitalOut(AudioTypeConfig & ac3AudioType);
		void SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg);
		void SetDolbyTestMode(AC3OperationalMode op, WORD hf, WORD lf, AC3DialogNorm dn);
		void GetDolbyTestMode(AC3OperationalMode &op, WORD &hf, WORD &lf, AC3DialogNorm &dn);

		void SetAC3Config(AC3DynamicRange dr, BOOL enableLFE);
		void GetAC3Config(AC3DynamicRange &dr, BOOL &enableLFE);

	};

#endif
