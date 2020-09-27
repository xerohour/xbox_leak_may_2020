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


// FILE:      library\common\tags.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   24.03.95
//
// PURPOSE: The tag processing module.
//
// HISTORY:

#ifndef TAGS_H
#define TAGS_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"



// In order to be flexible, we use tags for setting and getting data and query
// module capabilities. Tag pairs consist of the tag itself and a data item.
// The tag tells which variable shall be set or retrieved. The data item is
// the value written to the variable or a pointer to the location where the
// retrieved value is to be stored.
// Tags have 32 bits and are built the following way:
//
// iccc uuuu uuuu uuuu rsss ssss ssss ssss
//
// "cc" is the tag command get, set or query. The tagging unit follows. The
// tag specifier itself is contained in the lowest 15 bits.
// Internal tags are marked with an i=1, external tags marked with an i=0.
// The difference between internal and external tagunits is, that external
// units have a type bit each, internal units only have numbers.
// Tags are type checked. Tags are constructed by the MKTAG_* macros and
// terminated by TAGDONE. The inline functions below do the type checking and
// are optimized to NOPs.  The r bit is used for reference tags.  These tags
// do not carry a value, but a reference to a more extendet structure;
//

#define TAG_VAL  0x00000000
#define TAG_GET  0x10000000
#define TAG_SET  0x20000000
#define TAG_QRY  0x30000000

#define TAG_REF  0x00008000

#define TAG_TYPE(x)	(x & 0x30000000)

#define ANYUNIT	0x4fff0000
#define MAIN_UNIT	0x00000000

#define MKTAGUNIT(unit, id)	\
	static const WORD unit##_ID	=	id;	\
	static const DWORD unit =  MKFLAG(16 + id);

#define GETTAGUNITS(tag)   XTBF(16, 12, tag)
#define USESTAGUNIT(tag, id) XTBF(16+id, tag)

#define MKITAGUNIT(unit, id)	\
	static const DWORD unit = 0x80000000L | (id << 16);

#if WDM_VERSION
struct __far TAG {
	DWORD	id;
	DWORD_PTR	data;
	TAG (DWORD _id, DWORD_PTR _data) { data = _data; id = _id; };
	TAG() {};
	~TAG() {}
	};
#else
struct __far TAG {
	DWORD	id;
	DWORD	data;
	TAG (DWORD _id, DWORD _data) { data = _data; id = _id; };
	TAG() {};
#if !__EDG__ || __EDG_VERSION__<240
	~TAG() {}	// This is mainly used as a workaround for a certain C++ frontend compiler bug.
#endif
	};
#endif

inline BOOL __far & QRY_TAG(TAG __far * tag) {return *((BOOL __far *)(tag->data));}

	//
	// TAG terminator
	//
#define TAGDONE TAG(0,0)

#if WDM_VERSION
DWORD_PTR FilterTags(TAG __far * tags, DWORD id, DWORD_PTR def);
#else
DWORD FilterTags(TAG __far * tags, DWORD id, DWORD def);
#endif

#if WDM_VERSION

#define MKTAG(name, unit, val, type)	\
	inline TAG SET_##name(type x) {return TAG(val | unit | TAG_SET, (DWORD_PTR)(x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET, (DWORD_PTR)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY, (DWORD_PTR)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return (type)(tag->data);}	\
	inline type FVAL_##name(TAG __far * tags, type def) {return (type)(FilterTags(tags, val | unit | TAG_GET, (DWORD_PTR)def));}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET;	\
	static const DWORD CGET_##name = val | unit | TAG_GET;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY;

#define MKRTG(name, unit, val, type)	\
	inline TAG SET_##name(type __far &x) {return TAG(val | unit | TAG_SET | TAG_REF, (DWORD_PTR)(FPTR)(&x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET | TAG_REF, (DWORD_PTR)(FPTR)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY | TAG_REF, (DWORD_PTR)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET | TAG_REF;	\
	static const DWORD CGET_##name = val | unit | TAG_GET | TAG_REF;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY | TAG_REF;

#else

#define MKTAG(name, unit, val, type)	\
	inline TAG SET_##name(type x) {return TAG(val | unit | TAG_SET, (DWORD)(x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET, (DWORD)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY, (DWORD)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return (type)(tag->data);}	\
	inline type FVAL_##name(TAG __far * tags, type def) {return (type)(FilterTags(tags, val | unit | TAG_GET, (DWORD)def));}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET;	\
	static const DWORD CGET_##name = val | unit | TAG_GET;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY;

#define MKRTG(name, unit, val, type)	\
	inline TAG SET_##name(type __far &x) {return TAG(val | unit | TAG_SET | TAG_REF, (DWORD)(FPTR)(&x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET | TAG_REF, (DWORD)(FPTR)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY | TAG_REF, (DWORD)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET | TAG_REF;	\
	static const DWORD CGET_##name = val | unit | TAG_GET | TAG_REF;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY | TAG_REF;

#endif

#if UNICODE

struct BoardVersionInfo
	{
	char	boardName[100];
	DWORD	boardVersion;
	};

struct UnicodeBoardVersionInfo
	{
	TCHAR boardName[100];
	DWORD boardVersion;
	};

MKRTG(BOARD_VERSION_UNICODE, MAIN_UNIT, 2, UnicodeBoardVersionInfo)

#else

struct BoardVersionInfo
	{
	char	boardName[100];
	DWORD	boardVersion;
	};

#endif

MKRTG(BOARD_VERSION,						MAIN_UNIT, 1,		BoardVersionInfo)
// 2 is used above!

MKRTG(BOARD_REGION_CODE,				MAIN_UNIT,	0x10,	BYTE)
MKRTG(BOARD_REGION_COUNT,				MAIN_UNIT,	0x11,	WORD)

MKRTG(BOARD_SPLIT_STREAM_SUPPORT,	MAIN_UNIT,	0x20, BOOL)

#if BOARD_HAS_AUTOMOTIVE_DRIVE
MKRTG(DRIVE_HAS_HIGH_TEMPERATURE,	MAIN_UNIT,	0x21, BOOL)
#endif

struct SoftwareVersionInfo
	{
	DWORD softwareVersion, softwareRevision;
	DWORD buildNumber;
	};

MKRTG(SOFTWARE_VERSION,		MAIN_UNIT,	0x30, SoftwareVersionInfo)

//
// Macros for easy access to TAG lists during parsing
//
// GETSET     : Simple get and set of a variable
// GETSETV    : Calls a function when the variable is changed
// GETSETVC   : Like GETSETV, but only calls on real changes
// GETSETC    : Sets a variable "changed" to TRUE when a variable is changed
// GETSETP    : Allows changing only when the virtual unit is passivated
// GETSETCF   : Ors a variable "changed" with a flag when the variable is changed
// GETONLY    : Allows only reading of a value
// GETINQUIRE : Calls an inquire function, the result is to be placed
//              in the pseudo variable "data".
//
// A typical TAG parsing routine will look like this
//
//	Error VirtualMPEGDecoder::Configure(TAG *tags)
//		{
//		PARSE_TAGS_START(tags)
//			GETSETP(MPEG_AUDIO_BITRATE,  params.audioBitRate);
//			GETSETP(MPEG_AUDIO_STREAMID, params.audioStreamID);
//			GETSETP(MPEG_VIDEO_STREAMID, params.videoStreamID);
//			GETSETP(MPEG_VIDEO_WIDTH,    params.videoWidth);
//			GETSETP(MPEG_VIDEO_HEIGHT,   params.videoHeight);
//			GETSETP(MPEG_VIDEO_FPS,      params.videoFPS);
//			GETSETP(MPEG_STREAM_TYPE,    params.streamType);
//
//			GETSET(MPEG_STREAM_HOOK, streamHook);
//
//			GETSETV(MPEG_LEFT_VOLUME,  leftVolume,  GNREASSERT(decoder->SetLeftVolume(leftVolume)));
//			GETSETV(MPEG_RIGHT_VOLUME, rightVolume, GNREASSERT(decoder->SetRightVolume(rightVolume)));
//			GETSETV(MPEG_AUDIO_MUTE,   audioMute,   GNREASSERT(decoder->MuteAudio(audioMute)));
//		PARSE_TAGS_END
//
//		GNRAISE_OK;
//		}
//

#ifndef ONLY_EXTERNAL_VISIBLE

#define PARSE_TAGS_START(tags) \
	TAG __far * tp = tags; \
	while (tp->id) { \
		switch (tp->id) {

#define PARSE_TAGS_END } tp++; }


#define GETSET(tagid, variable)	\
	case CSET_##tagid:	\
		variable = VAL_##tagid(tp);	\
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETV(tagid, variable, call) \
	case CSET_##tagid:	\
		variable = VAL_##tagid(tp);	\
		if (GetState() == current) {call;} \
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETVD(tagid, variable1, variable2, call) \
	case CSET_##tagid:	\
		variable1 = VAL_##tagid(tp);	\
		variable2 = VAL_##tagid(tp);	\
		if (GetState() == current) {call;} \
		break;	\
	case CGET_##tagid:	\
		GNRAISE(GNR_OBJECT_WRITE_ONLY); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define SETD(tagid, call) \
	case CSET_##tagid:	\
		GNREASSERT(call(VAL_##tagid(tp))); \
		break;	\
	case CGET_##tagid:	\
		GNRAISE(GNR_OBJECT_WRITE_ONLY); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETD(tagid, call) \
	case CSET_##tagid:	\
		GNRAISE(GNR_OBJECT_READ_ONLY); \
		break;	\
	case CGET_##tagid:	\
		GNREASSERT(call(REF_##tagid(tp))); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETDSETD(tagid, getcall, setcall) \
	case CSET_##tagid:	\
		setcall(VAL_##tagid(tp)); \
		break;	\
	case CGET_##tagid:	\
		getcall(REF_##tagid(tp)); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break


#define GETSETDV(tagid, variable, call) \
	case CSET_##tagid:	\
		variable = VAL_##tagid(tp);	\
		call; \
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define SETDVGETDP(tagid, variable, call, puVar) \
	case CSET_##tagid:	\
		variable = VAL_##tagid(tp);	\
		GNREASSERT(call); \
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = puVar; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define SETDV(tagid, variable, call) \
	case CSET_##tagid:	\
		variable = VAL_##tagid(tp);    \
		call; \
		break;	\
	case CGET_##tagid:	\
		GNRAISE(GNR_OBJECT_WRITE_ONLY); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define SETDDV(tagid, variable1, variable2, call) \
	case CSET_##tagid:	\
		variable1 = VAL_##tagid(tp);    \
		variable2 = VAL_##tagid(tp);    \
		call; \
		break;	\
	case CGET_##tagid:	\
		GNRAISE(GNR_OBJECT_WRITE_ONLY); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETVC(tagid, variable, call) \
	case CSET_##tagid:	\
		if (variable != VAL_##tagid(tp)) {	\
			variable = VAL_##tagid(tp);	\
			if (GetState() == current) {call;} }	\
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETC(tagid, variable) \
	case CSET_##tagid:	\
		if (VAL_##tagid(tp) != variable) { \
			variable = VAL_##tagid(tp);	\
			changed = TRUE; } \
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETCF(tagid, variable, flag) \
	case CSET_##tagid:	\
		if (VAL_##tagid(tp) != variable) { \
			variable = VAL_##tagid(tp);	\
			changed |= flag; } \
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETSETP(tagid, variable)	\
	case CSET_##tagid:	\
		if (GetState() != passive) GNRAISE(GNR_INVALID_CONFIGURE_STATE); \
		variable = VAL_##tagid(tp);	\
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable; \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETONLY(tagid, variable) \
	case CSET_##tagid:	\
		GNRAISE(GNR_OBJECT_READ_ONLY);	\
		break;	\
	case CGET_##tagid:	\
		REF_##tagid(tp) = variable;	\
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETC(tagid, call) \
	case CSET_##tagid:	\
		GNRAISE(GNR_OBJECT_READ_ONLY);	\
		break;	\
	case CGET_##tagid:	\
		if (GetState() == current) \
			GNREASSERT(call(REF_##tagid(tp))); \
		else	\
			GNRAISE(GNR_INVALID_CONFIGURE_STATE); \
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define GETINQUIRE(tagid, inquire)	\
	case CSET_##tagid:	\
		GNRAISE(GNR_OBJECT_READ_ONLY);	\
	case CGET_##tagid:	\
		{	\
		TTYPE_##tagid __far &data = REF_##tagid(tp);	\
		inquire	\
		}	\
		break;	\
	case CQRY_##tagid:	\
		QRY_TAG(tp) = TRUE;	\
		break

#define BUILD_TAGS_START(num)	\
	{TAG list[num]; TAG __far * ntp = list;

#define BUILD_TAGS_ADD(tag)	\
	*ntp++ = tag;

#define BUILD_TAGS_END(callee)	\
	*ntp++ = TAGDONE;	\
	GNREASSERT(callee->Configure(list));}

#define FILTER_TAGS_ADD(tagid)	\
	case CSET_##tagid:	\
	case CGET_##tagid:	\
	case CQRY_##tagid:	\
		*ntp++ = *tp;	\
		break;

#define FILTER_TAGS_IF(condition, tagid)	\
	case CSET_##tagid:	\
	case CGET_##tagid:	\
	case CQRY_##tagid:	\
		if (condition)	\
			*ntp++ = *tp;	\
		break;

// This macro short-cuts the typical case of setting and getting a simple
// variable via tags in the implementation of Configure() functions. Note
// that it sets a BOOL variable "changed" when a parameter is set.

#define TAG_CASE(name, var)  \
	case CSET_##name:  var = VAL_##name(tp);  changed = TRUE;  break;  \
	case CGET_##name:  REF_##name(tp) = var;  break;  \
	case CQRY_##name:  QRY_TAG(tp) = TRUE;    break;

#endif

#ifndef ONLY_EXTERNAL_VISIBLE

class TagClass {
	public:
		virtual Error Configure(TAG __far * tags) {GNRAISE_OK;}
		Error __cdecl ConfigureTags (TAG tag, ...);
	};

class TagFilter {
	protected:
		TAG	*	list;
		TAG	*  ntp;

		int 		size;
		int		num;

		virtual Error InternalStart(int size)
			{
			if (!list)
				{
				this->size = size;
				list = new TAG[size];
				}
			else if (this->size < size)
				{
				delete[] list;
				this->size = size;
				list = new TAG[size];
				}

			this->num = 0;
			ntp = list;

			GNRAISE_OK;
			}

		virtual Error InternalAdd(TAG tag) {*ntp++ = tag; GNRAISE_OK;}
		virtual Error InternalDone(void) {return InternalAdd(TAGDONE);}
	public:
		virtual Error Start(int size = 100) {return InternalStart(size);}
		virtual Error Add(TAG tag) {return InternalAdd(tag);}
		virtual Error Done(void) {return InternalDone();}

		operator TAG * (void) {return list;}
	};

#endif

#endif
