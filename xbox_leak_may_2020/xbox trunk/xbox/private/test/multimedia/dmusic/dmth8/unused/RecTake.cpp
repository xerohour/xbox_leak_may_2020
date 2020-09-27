// RecTake.cpp: implementation of the CRecTake class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4786)

#include "RecTake.h"
#include <assert.h>
#include "diffInfo.h"


static structDiff::Info PMsgInfo[] =
{
	DIFF_INFO (DMUS_PMSG, dwSize, "%08Xh"),
//	  DIFF_INFO (DMUS_PMSG, rtTime, "%I64"),			// This will change in each run, so is not a valid comparason
//	  DIFF_INFO (DMUS_PMSG, mtTime, "%I64"),			// This will change in each run, so is not a valid comparason
	DIFF_INFO (DMUS_PMSG, dwFlags, "%08Xh"),
	DIFF_INFO (DMUS_PMSG, dwPChannel, "%08Xh"),
	DIFF_INFO (DMUS_PMSG, dwVirtualTrackID, "%08Xh"),
//	  DIFF_INFO (DMUS_PMSG, pTool, "%08Xh"),			// This will change in each run, so is not a valid comparason
//	  DIFF_INFO (DMUS_PMSG, pGraph, "%08Xh"),			// This will change in each run, so is not a valid comparason
	DIFF_INFO (DMUS_PMSG, dwType, "%08Xh"),
	DIFF_INFO (DMUS_PMSG, dwVoiceID, "%08Xh"),
	DIFF_INFO (DMUS_PMSG, dwGroupID, "%08Xh"),
	DIFF_INFO (DMUS_PMSG, punkUser, "%08Xh"),
};

// DMUS_NOTE_PMSG
static structDiff::Info notePMsgInfo[] =
{
	DIFF_INFO (DMUS_NOTE_PMSG, mtDuration, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, wMusicValue, "%I64"),
	DIFF_INFO (DMUS_NOTE_PMSG, wMeasure, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, nOffset, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bBeat, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bGrid, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bVelocity, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bFlags, "%08Xh"),
	DIFF_INFO (DMUS_NOTE_PMSG, bTimeRange, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bDurRange, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bVelRange, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bPlayModeFlags, "%08Xh"),
	DIFF_INFO (DMUS_NOTE_PMSG, bSubChordLevel, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, bMidiValue, "%d"),
	DIFF_INFO (DMUS_NOTE_PMSG, cTranspose, "%d"),
};

// DMUS_CURVE_PMSG
static structDiff::Info curvePMsgInfo[] =
{
	DIFF_INFO (DMUS_CURVE_PMSG, mtDuration, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, mtOriginalStart, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, mtResetDuration, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, nStartValue, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, nEndValue, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, nResetValue, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, wMeasure, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, nOffset, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bBeat, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bGrid, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bType, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bCurveShape, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bCCData, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, bFlags, "%08Xh"),
	DIFF_INFO (DMUS_CURVE_PMSG, wParamType, "%d"),
	DIFF_INFO (DMUS_CURVE_PMSG, wMergeIndex, "%d"),
};

// DMUS_MIDI_PMSG				DMUS_PMSGT_MIDI:
static structDiff::Info midiPMsgInfo[] =
{
	DIFF_INFO (DMUS_MIDI_PMSG, bStatus, "%d"),
	DIFF_INFO (DMUS_MIDI_PMSG, bByte1, "%d"),
	DIFF_INFO (DMUS_MIDI_PMSG, bByte2, "%d"),
//BUGBUG
//	  DIFF_INFO (DMUS_MIDI_PMSG, bPad[1], "%d"),
};

// DMUS_NOTIFICATION_PMSG		DMUS_PMSGT_NOTIFICATION:
static structDiff::Info notificationPMsgInfo[] =
{
//BUGBUG
//	  GUID	  guidNotificationType;
	DIFF_INFO (DMUS_NOTIFICATION_PMSG, dwNotificationOption, "%08Xh"),
	DIFF_INFO (DMUS_NOTIFICATION_PMSG, dwField1, "%08Xh"),
	DIFF_INFO (DMUS_NOTIFICATION_PMSG, dwField2, "%08Xh"),
};

// DMUS_PATCH_PMSG				DMUS_PMSGT_PATCH:
static structDiff::Info patchPMsgInfo[] =
{
	DIFF_INFO (DMUS_PATCH_PMSG, byInstrument, "%d"),
	DIFF_INFO (DMUS_PATCH_PMSG, byMSB, "%d"),
	DIFF_INFO (DMUS_PATCH_PMSG, byLSB, "%d"),
//BUGBUG
//	  DIFF_INFO (DMUS_PATCH_PMSG, byPad[1], "%d"),
};

// DMUS_SYSEX_PMSG				DMUS_PMSGT_SYSEX:
static structDiff::Info sysexPMsgInfo[] =
{
	DIFF_INFO (DMUS_SYSEX_PMSG, dwLen, "%08Xh"),
//BUGBUG
//	  DIFF_INFO (DMUS_PATCH_PMSG, abData[1], "%d"),
};

// DMUS_TEMPO_PMSG				DMUS_PMSGT_TEMPO:
static structDiff::Info tempoPMsgInfo[] =
{
	DIFF_INFO (DMUS_TEMPO_PMSG, dblTempo, "%f"),
};

// DMUS_TIMESIG_PMSG			DMUS_PMSGT_TIMESIG:
static structDiff::Info timesigPMsgInfo[] =
{
	DIFF_INFO (DMUS_TIMESIG_PMSG, bBeatsPerMeasure, "%d"),
	DIFF_INFO (DMUS_TIMESIG_PMSG, bBeat, "%d"),
	DIFF_INFO (DMUS_TIMESIG_PMSG, wGridsPerBeat, "%d"),
};

// DMUS_TRANSPOSE_PMSG			DMUS_PMSGT_TRANSPOSE:
static structDiff::Info transposePMsgInfo[] =
{
	DIFF_INFO (DMUS_TRANSPOSE_PMSG, nTranspose, "%d"),
	DIFF_INFO (DMUS_TRANSPOSE_PMSG, wMergeIndex, "%d"),
};

// DMUS_CHANNEL_PRIORITY_PMSG	DMUS_PMSGT_CHANNEL_PRIORITY:
static structDiff::Info channelpriorityPMsgInfo[] =
{
	DIFF_INFO (DMUS_CHANNEL_PRIORITY_PMSG, dwChannelPriority, "%08Xh"),
};


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRecTake::CRecTake() : m_Count (0)
{

}

CRecTake::~CRecTake()
{
	Erase ();
}

CRecTake* CRecTake::Create()
{
	return (new CRecTake);
}

void CRecTake::AddMsg(DMUS_PMSG const* pPMsg)
{
	AddMsg (m_Count, pPMsg);
}

CRecTake::list CRecTake::GetMessages() const
{
	return (m_MsgList);
}

CRecTake::list CRecTake::GetMessages(DWORD dwType) const
{
	list retv;

	for (const_iterator itt = m_MsgList.begin ();itt != m_MsgList.end ();itt++)
	{
		if (itt->pPMsg->dwType == dwType)
		{
			retv.push_back (*itt);
		}
	}

	return (retv);
}

int CRecTake::GetMessageCount() const
{
	assert (m_MsgList.size () == m_Count);
	return (m_Count);
}

void CRecTake::Erase()
{
	for (iterator itt = m_MsgList.begin ();itt != m_MsgList.end ();itt++)
	{
		// Since this was allocated as a char array, it must be deleted as one
		// See AddMsg
		delete [] reinterpret_cast <char*> (itt->pPMsg);
	}

	m_MsgList.clear ();
	m_Count = 0;
}

HRESULT  CRecTake::Compare(const CRecTake *pTake, ComparePMsgsFn compare) const
{
	if (m_Count != pTake->m_Count)
	{
		return (S_FALSE);
	}

	// Check for internal consistency
	if (m_MsgList.size () != pTake->m_MsgList.size ())
	{
		// BUGBUG: if we EVER get here, we have an internal bug
		return (S_FALSE);
	}

	const_iterator lhs (m_MsgList.begin ());
	const_iterator rhs (pTake->m_MsgList.begin ());
	for (;lhs != m_MsgList.end ();lhs++, rhs++)
	{
		HRESULT hr = compare (lhs->pPMsg, rhs->pPMsg);
		if (hr != S_OK)
		{
			return (hr);
		}
	}

	return (S_OK);
}

HRESULT CRecTake::CompareTwoPMsgs(DMUS_PMSG *pPMsg1, DMUS_PMSG *pPMsg2)
{
	HRESULT hRes = S_FALSE;

	if (pPMsg1->dwType != pPMsg2->dwType)
	{
		return (hRes);
	}

	structDiff::Compare (pPMsg1, pPMsg2, PMsgInfo, sizeof (PMsgInfo) /
					sizeof (PMsgInfo[0]));

	if (!hRes)
	{
		return (hRes);
	}

	switch (pPMsg1->dwType)
	{
		case DMUS_PMSGT_NOTE :
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_NOTE_PMSG* pLhs = reinterpret_cast<DMUS_NOTE_PMSG*> (pPMsg1);
				DMUS_NOTE_PMSG* pRhs = reinterpret_cast<DMUS_NOTE_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, notePMsgInfo, sizeof (notePMsgInfo) /
					sizeof (notePMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_CURVE :
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_CURVE_PMSG* pLhs = reinterpret_cast<DMUS_CURVE_PMSG*> (pPMsg1);
				DMUS_CURVE_PMSG* pRhs = reinterpret_cast<DMUS_CURVE_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, curvePMsgInfo, sizeof (curvePMsgInfo) /
					sizeof (curvePMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_MIDI:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_MIDI_PMSG* pLhs = reinterpret_cast<DMUS_MIDI_PMSG*> (pPMsg1);
				DMUS_MIDI_PMSG* pRhs = reinterpret_cast<DMUS_MIDI_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, midiPMsgInfo, sizeof (midiPMsgInfo) /
					sizeof (midiPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_NOTIFICATION:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_NOTIFICATION_PMSG* pLhs = reinterpret_cast<DMUS_NOTIFICATION_PMSG*> (pPMsg1);
				DMUS_NOTIFICATION_PMSG* pRhs = reinterpret_cast<DMUS_NOTIFICATION_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, notificationPMsgInfo, sizeof (notificationPMsgInfo) /
					sizeof (notificationPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_PATCH:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_PATCH_PMSG* pLhs = reinterpret_cast<DMUS_PATCH_PMSG*> (pPMsg1);
				DMUS_PATCH_PMSG* pRhs = reinterpret_cast<DMUS_PATCH_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, patchPMsgInfo, sizeof (patchPMsgInfo) /
					sizeof (patchPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_SYSEX:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_SYSEX_PMSG* pLhs = reinterpret_cast<DMUS_SYSEX_PMSG*> (pPMsg1);
				DMUS_SYSEX_PMSG* pRhs = reinterpret_cast<DMUS_SYSEX_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, sysexPMsgInfo, sizeof (sysexPMsgInfo) /
					sizeof (sysexPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_TEMPO:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TEMPO_PMSG* pLhs = reinterpret_cast<DMUS_TEMPO_PMSG*> (pPMsg1);
				DMUS_TEMPO_PMSG* pRhs = reinterpret_cast<DMUS_TEMPO_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, tempoPMsgInfo, sizeof (tempoPMsgInfo) /
					sizeof (tempoPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_TIMESIG:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TIMESIG_PMSG* pLhs = reinterpret_cast<DMUS_TIMESIG_PMSG*> (pPMsg1);
				DMUS_TIMESIG_PMSG* pRhs = reinterpret_cast<DMUS_TIMESIG_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, timesigPMsgInfo, sizeof (timesigPMsgInfo) /
					sizeof (timesigPMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_TRANSPOSE:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TRANSPOSE_PMSG* pLhs = reinterpret_cast<DMUS_TRANSPOSE_PMSG*> (pPMsg1);
				DMUS_TRANSPOSE_PMSG* pRhs = reinterpret_cast<DMUS_TRANSPOSE_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, transposePMsgInfo, sizeof (transposePMsgInfo) /
					sizeof (transposePMsgInfo[0])));
			}
			break;

		case DMUS_PMSGT_CHANNEL_PRIORITY:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_CHANNEL_PRIORITY_PMSG* pLhs = reinterpret_cast<DMUS_CHANNEL_PRIORITY_PMSG*> (pPMsg1);
				DMUS_CHANNEL_PRIORITY_PMSG* pRhs = reinterpret_cast<DMUS_CHANNEL_PRIORITY_PMSG*> (pPMsg2);

				return (structDiff::Compare (pLhs, pRhs, channelpriorityPMsgInfo, sizeof (channelpriorityPMsgInfo) /
					sizeof (channelpriorityPMsgInfo[0])));
			}
			break;

		//default:
		//	unsuppored at the moment but don't fail
		 // return (S_FALSE);
	}

	return (E_INVALIDARG);
}

HRESULT CRecTake::ReadTake(int resource)
{
	Erase ();

	HRSRC hResInfo = FindResource (NULL, MAKEINTRESOURCE (resource), "TAKE");
	if (!hResInfo)
	{
		return (E_INVALIDARG);
	}

	HGLOBAL hRes = LoadResource (NULL, hResInfo);
	if (!hRes)
	{
		return (E_INVALIDARG);
	}

	LPVOID pRes = LockResource (hRes);
	if (!pRes)
	{
		return (E_INVALIDARG);
	}

	// Note the extensive use of reinterpret_cast is simply because
	// we are converting raw memory into data
	int nElements = reinterpret_cast<DWORD*> (pRes)[0];
	pRes = reinterpret_cast <char*> (pRes) + sizeof (nElements);

	for (int i=0;i<nElements;i++)
	{
		DWORD dwIndex  = reinterpret_cast<DWORD*> (pRes)[0];
		pRes = reinterpret_cast <char*> (pRes) + sizeof (dwIndex);
		DMUS_PMSG const* pPMsg = reinterpret_cast <DMUS_PMSG*> (pRes);
		DWORD size = pPMsg->dwSize;

		HRESULT hr = AddMsg (dwIndex, pPMsg);

		if (FAILED (hr))
		{
			return (hr);
		}

		pRes = ((char*)pRes) + size;
	}

	return (S_OK);
}

HRESULT CRecTake::AddMsg(DWORD dwIndex, DMUS_PMSG const* pPMsg)
{
	if (!pPMsg)
	{
		return (E_INVALIDARG);
	}

	RECTOOL_PMSG msg;
	msg.dwIndex = dwIndex;

	assert (pPMsg->dwSize >= sizeof (DMUS_PMSG));

	// use reinterpret_cast to allow us to allocate a variable size
	// DMUS_PMSG object; see Erase
	msg.pPMsg = reinterpret_cast <DMUS_PMSG*> (new char[pPMsg->dwSize]);

	// Fail gracefully
	if (!msg.pPMsg)
	{
		return (E_OUTOFMEMORY);
	}

	memcpy (msg.pPMsg, pPMsg, pPMsg->dwSize);

	try
	{
		m_MsgList.push_back (msg);
		m_Count ++;
	}
	catch (...)
	{
		return (E_OUTOFMEMORY);
	}


	return (S_OK);
}

static void PrintMsg (LPVOID pPMsg1, structDiff::Info* pDiffInfo, int nFields, int size)
{
	LPCSTR pLhs = ((LPCSTR) pPMsg1);

	for (int i=0;i < size;i++, pLhs++)
	{
		for (int j=0;j<nFields;j++)
		{
			char		value1[16];
			LONGLONG	iValue1;

			if (i >= pDiffInfo[j].baseOffset && (i - pDiffInfo[j].baseOffset) < pDiffInfo[j].size)
			{
				iValue1 = 0;

				assert (pDiffInfo[j].size <= sizeof (iValue1));

				memcpy (&iValue1, ((LPCSTR) pPMsg1) + pDiffInfo[j].baseOffset, pDiffInfo[j].size);

				sprintf (value1, pDiffInfo[j].pszFieldFormat, iValue1);

				printf ("PMsg->%s = %s\n", pDiffInfo[j].pszFieldName, value1);

				// Offset both sides so we don't find this element again
				int elementRemain = pDiffInfo[j].size - (i - pDiffInfo[j].baseOffset);
				pLhs += elementRemain - 1;
				i += elementRemain - 1;
				break;
			}
		}
	}
}

template <typename T>
static void PrintMsg (T* pPMsg, structDiff::Info* pDiffInfo, int nFields)
{
	PrintMsg (static_cast <void*> (pPMsg), pDiffInfo, nFields, sizeof (*pPMsg));
}


void CRecTake::PrintPMsg (DMUS_PMSG *pPMsg1)
{
	HRESULT hRes = S_FALSE;

	PrintMsg (pPMsg1, PMsgInfo, sizeof (PMsgInfo) / sizeof (PMsgInfo[0]));

	switch (pPMsg1->dwType)
	{
		case DMUS_PMSGT_NOTE :
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_NOTE_PMSG* pLhs = reinterpret_cast<DMUS_NOTE_PMSG*> (pPMsg1);

				PrintMsg (pLhs, notePMsgInfo, sizeof (notePMsgInfo) /
					sizeof (notePMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_CURVE :
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_CURVE_PMSG* pLhs = reinterpret_cast<DMUS_CURVE_PMSG*> (pPMsg1);

				PrintMsg (pLhs, curvePMsgInfo, sizeof (curvePMsgInfo) /
					sizeof (curvePMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_MIDI:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_MIDI_PMSG* pLhs = reinterpret_cast<DMUS_MIDI_PMSG*> (pPMsg1);

				PrintMsg (pLhs, midiPMsgInfo, sizeof (midiPMsgInfo) /
					sizeof (midiPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_NOTIFICATION:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_NOTIFICATION_PMSG* pLhs = reinterpret_cast<DMUS_NOTIFICATION_PMSG*> (pPMsg1);

				PrintMsg (pLhs, notificationPMsgInfo, sizeof (notificationPMsgInfo) /
					sizeof (notificationPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_PATCH:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_PATCH_PMSG* pLhs = reinterpret_cast<DMUS_PATCH_PMSG*> (pPMsg1);

				PrintMsg (pLhs, patchPMsgInfo, sizeof (patchPMsgInfo) /
					sizeof (patchPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_SYSEX:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_SYSEX_PMSG* pLhs = reinterpret_cast<DMUS_SYSEX_PMSG*> (pPMsg1);

				PrintMsg (pLhs, sysexPMsgInfo, sizeof (sysexPMsgInfo) /
					sizeof (sysexPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_TEMPO:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TEMPO_PMSG* pLhs = reinterpret_cast<DMUS_TEMPO_PMSG*> (pPMsg1);

				PrintMsg (pLhs, tempoPMsgInfo, sizeof (tempoPMsgInfo) /
					sizeof (tempoPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_TIMESIG:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TIMESIG_PMSG* pLhs = reinterpret_cast<DMUS_TIMESIG_PMSG*> (pPMsg1);

				PrintMsg (pLhs, timesigPMsgInfo, sizeof (timesigPMsgInfo) /
					sizeof (timesigPMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_TRANSPOSE:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_TRANSPOSE_PMSG* pLhs = reinterpret_cast<DMUS_TRANSPOSE_PMSG*> (pPMsg1);

				PrintMsg (pLhs, transposePMsgInfo, sizeof (transposePMsgInfo) /
					sizeof (transposePMsgInfo[0]));
			}
			break;

		case DMUS_PMSGT_CHANNEL_PRIORITY:
			{
				// Use reinterpret_cast because a static cast would fail
				DMUS_CHANNEL_PRIORITY_PMSG* pLhs = reinterpret_cast<DMUS_CHANNEL_PRIORITY_PMSG*> (pPMsg1);

				PrintMsg (pLhs, channelpriorityPMsgInfo, sizeof (channelpriorityPMsgInfo) /
					sizeof (channelpriorityPMsgInfo[0]));
			}
			break;

		//default:
		//	unsuppored at the moment but don't fail
		 // return (S_FALSE);
	}
}
