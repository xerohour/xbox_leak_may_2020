// AddRemoveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "bandeditordll.h"
#include "Band.h"
#include "BandDlg.h"
#include "AddRemoveDlg.h"
#include "PChannelName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD dwaPriorities[];

/////////////////////////////////////////////////////////////////////////////
// CAddRemoveDlg dialog


CAddRemoveDlg::CAddRemoveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddRemoveDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAddRemoveDlg)
	//}}AFX_DATA_INIT
}


void CAddRemoveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddRemoveDlg)
	DDX_Control(pDX, IDC_PCHANNEL_SPIN, m_ChannelSpin);
	DDX_Control(pDX, IDC_CHANNEL_LIST, m_ChanListBox);
	DDX_Control(pDX, IDC_PCHANNEL_EDIT, m_ChannelEdit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAddRemoveDlg, CDialog)
	//{{AFX_MSG_MAP(CAddRemoveDlg)
	ON_BN_CLICKED(IDC_ADD_BTN, OnAddBtn)
	ON_BN_CLICKED(IDC_REMOVE_BTN, OnRemoveBtn)
	ON_BN_CLICKED(IDCANCEL, OnCancel)
	ON_NOTIFY(UDN_DELTAPOS, IDC_PCHANNEL_SPIN, OnDeltaposPchannelSpin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAddRemoveDlg::UpdateData
//
void CAddRemoveDlg::OnAddBtn() 
{
	// Do a sorted add to the list
	DWORD dwChannel = m_ChannelSpin.GetPos()-1;
	
	if(AddToChannels((int)dwChannel) == TRUE)
		PopulateChannelList();

	m_ChanListBox.SetSel(-1, FALSE);
	CString sChannelString;
	sChannelString.Format("%d", dwChannel + 1);
	int nChannelIndex = m_ChanListBox.FindStringExact(0, sChannelString);
	m_ChanListBox.SetSel(nChannelIndex);

	int nNextChannel = GetNextAvailablePChannel(dwChannel + 1);
	m_ChannelSpin.SetPos(nNextChannel);
}

void CAddRemoveDlg::OnRemoveBtn() 
{
	ASSERT(m_pBandDlg != NULL);

	if (m_ChanListBox.GetSelCount() <= 0)
		return;

	// Bring up a message box warning the user.
	if (m_ChanListBox.GetSelCount() > 0)
	{
		CString strWarning;
		strWarning.LoadString(IDS_REMOVEWARNING);
		CString strConfirm;
		strConfirm.LoadString(IDS_CONFIRM);
		if (MessageBox(strWarning, strConfirm, MB_YESNO | MB_ICONINFORMATION) == IDNO)
			return;
	}

	// Remove channels selected in the list
	for (int nCount = m_ChanListBox.GetCount() - 1; nCount >= 0; nCount--) 
	{
		if (m_ChanListBox.GetSel(nCount)) 
		{
			int nChannel = m_ChanListBox.GetItemData(nCount);			
			
			POSITION position = m_Channels.Find(nChannel);
			m_Channels.RemoveAt(position);

			m_ChanListBox.DeleteString(nCount);
		}
	}

}

BOOL CAddRemoveDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_ChannelEdit.SetLimitText(3);
	
	CloneChannels();
	// Fill dialog with data
	PopulateChannelList();

	CBand* pBand = m_pBandDlg->GetBand();
	ASSERT(pBand);
	DWORD dwfirstAvailableChannel = pBand->GetNextAvailablePChannel();

	m_ChannelSpin.SetRange(1, 999);
	m_ChannelSpin.SetPos(dwfirstAvailableChannel + 1);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAddRemoveDlg::PopulateChannelList()
{
	m_ChanListBox.ResetContent();

	POSITION position = m_Channels.GetHeadPosition();
	while(position)
	{
		int nChannel = m_Channels.GetNext(position);

		CString sChannel;
		sChannel.Format("%d", nChannel + 1);
		
		int nIndex = m_ChanListBox.AddString(sChannel);
		if(nIndex != LB_ERR)
			m_ChanListBox.SetItemData(nIndex, nChannel);
	}
}


void CAddRemoveDlg::OnOK() 
{
	if (m_Channels.IsEmpty()) 
	{
		CString str;
		str.LoadString(IDS_ZERO_CHANNELS);
		MessageBox(str);
	}
	else 
	{
		m_pBandDlg->SaveStateForUndo("Add/Remove PChannel");

		POSITION position = m_Channels.GetHeadPosition();
		while(position)
		{
			int nChannel = m_Channels.GetNext(position);
			if(m_pBandDlg->GetBand()->GetInstrumentItemFromList(nChannel) == NULL)
			{
				AddPChannel(nChannel);
			}
		}
		
		position = m_pInstrumentList->GetHeadPosition();
		while(position)
		{
			CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*) m_pInstrumentList->GetNext(position);
			ASSERT(pInstrumentItem);
			
			CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
			ASSERT(pInstrument);

			if(pInstrument)
			{
				if(m_Channels.Find(pInstrument->dwPChannel) == NULL)
				{
					RemovePChannel(pInstrument->dwPChannel);
				}
			}

		}

		CBand* pBand = m_pBandDlg->GetBand();
		ASSERT(pBand);

		if(pBand)
		{
			m_pBandDlg->RefreshDisplay();
			pBand->SetModifiedFlag(TRUE);
			
			pBand->SendBand();
		}

		CDialog::OnOK();
	}
}


void CAddRemoveDlg::SetBandDialog(CBandDlg* pBandDlg)
{
	ASSERT(pBandDlg);

	m_pBandDlg = pBandDlg;
}

void CAddRemoveDlg::SetInstrumentList(CPtrList* pList)
{
	ASSERT(pList);

	m_pInstrumentList = pList;
	CloneChannels();
}

void CAddRemoveDlg::AddPChannel(int nChannel)
{
	CDMInstrument* pInstrument = new CDMInstrument();
	
	pInstrument->dwPChannel = nChannel;
	pInstrument->dwPatch = MAKE_PATCH(pInstrument->IsDrums(), 0, 0, 0);
	memset(pInstrument->dwNoteRanges,0,4 * sizeof(DWORD));
	pInstrument->dwFlags = (DMUS_IO_INST_GS | DMUS_IO_INST_GM);
	int nPriorityIndex = pInstrument->dwPChannel%16;
	pInstrument->dwChannelPriority = dwaPriorities[nPriorityIndex];
	pInstrument->nPitchBendRange = 2;
	pInstrument->SetComponent(m_pBandDlg->GetBand()->m_pComponent);
	pInstrument->SetBand(m_pBandDlg->GetBand());
	
	CInstrumentListItem* pNewInstrumentItem = new CInstrumentListItem(pInstrument);
	m_pBandDlg->GetBand()->InsertIntoInstrumentList(pNewInstrumentItem);

	// Every tenth channel in the group of 16 instruments is the drumkit
	if(pInstrument->dwPChannel != 9 && (pInstrument->dwPChannel)%16 != 9)
	{
		m_pBandDlg->m_MixGrid.AddButton((LPUNKNOWN)pNewInstrumentItem);
		m_pBandDlg->m_MixGrid.SetEnabled(pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
	}
	else
	{
			// Drums
		HBITMAP hBitmap = LoadBitmap( AfxGetResourceHandle(), MAKEINTRESOURCE( IDB_UDRUMS ) );
		m_pBandDlg->m_MixGrid.AddButton((LPUNKNOWN)pNewInstrumentItem);
		m_pBandDlg->m_MixGrid.SetEnabled(pInstrument->dwPChannel, (pInstrument->dwFlags & DMUS_IO_INST_PATCH));
		DeleteObject( hBitmap );
	}
	CBand* pBand = m_pBandDlg->GetBand();
	ASSERT(pBand);
	if(pBand)
	{
		pInstrument->Send(pBand, DM_PATCH_CHANGE);
		pInstrument->SendBandUpdate(pBand);
	}
}

void CAddRemoveDlg::OnCancel()
{
	m_Channels.RemoveAll();
	CDialog::OnCancel();
}


void CAddRemoveDlg::RemovePChannel(int nChannel)
{
	m_pBandDlg->m_MixGrid.RemoveButton(nChannel);

	CInstrumentListItem* pInstrumentItem = m_pBandDlg->GetBand()->GetInstrumentItemFromList(nChannel);
	ASSERT(pInstrumentItem);

	if(pInstrumentItem)
	{
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		CBand* pBand = m_pBandDlg->GetBand();
		ASSERT(pBand);
		if(pBand)
		{
			pInstrument->Send(pBand, DM_PATCH_CHANGE);
			pBand->DeleteFromInstrumentList(pInstrumentItem);
			pInstrument->SendBandUpdate(pBand);
		}
	}
}

void CAddRemoveDlg::CloneChannels()
{
	POSITION position = m_pInstrumentList->GetHeadPosition();

	while(position)
	{
		CInstrumentListItem* pInstrumentItem = (CInstrumentListItem*)m_pInstrumentList->GetNext(position);
		ASSERT(pInstrumentItem);
		
		CDMInstrument* pInstrument = pInstrumentItem->GetInstrument();
		ASSERT(pInstrument);

		AddToChannels(pInstrument->dwPChannel);
	}
}

// Always sorted!
BOOL CAddRemoveDlg::AddToChannels(int nChannel)
{
	if(m_Channels.Find(nChannel) != NULL)
		return FALSE;

	POSITION position = m_Channels.GetHeadPosition();
	while(position)
	{
		int nListChannel = m_Channels.GetNext(position);

		if(nListChannel >= nChannel + 1)
		{
			POSITION matchPosition = m_Channels.Find(nListChannel);
			m_Channels.InsertBefore(matchPosition, nChannel);
			return TRUE;
		}
	}

	m_Channels.AddTail(nChannel);
	return TRUE;
}

void CAddRemoveDlg::OnDeltaposPchannelSpin(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
	
	int iCurrentPosition = pNMUpDown->iPos;
	int iDelta = pNMUpDown->iDelta;

	CBand* pBand = m_pBandDlg->GetBand();
	ASSERT(pBand);

	int nNextChannel = -1;

	if(iDelta > 0)
	{
		nNextChannel = GetNextAvailablePChannel(iCurrentPosition);
	}
	else if(iDelta < 0)
	{
		nNextChannel = GetLastAvailablePChannel(iCurrentPosition);
	}
	
	if(nNextChannel >= 1)
		m_ChannelSpin.SetPos(nNextChannel);
	
	*pResult = 1;
}


int CAddRemoveDlg::GetNextAvailablePChannel(int nStartIndex)
{
	int  nHighestPChannelInList = 0;

	POSITION position = m_Channels.GetHeadPosition();
	int nLastPChannel = nStartIndex;
	
	while(position)
	{
		int nPChannelInList = m_Channels.GetNext(position) + 1;
		
		if(nLastPChannel < nPChannelInList - 1)		// If a vacant position
			return nLastPChannel + 1;						// Return the last position + 1

		if(nLastPChannel < nPChannelInList)
			nLastPChannel = nPChannelInList;

		if(nStartIndex <= nPChannelInList)
			nLastPChannel = nPChannelInList;

		if(nHighestPChannelInList < nLastPChannel)
			nHighestPChannelInList = nLastPChannel;
	}
	
	if(nLastPChannel == nStartIndex)
			nHighestPChannelInList = nLastPChannel;
	
	return (nHighestPChannelInList + 1);	
}

int	CAddRemoveDlg::GetLastAvailablePChannel(int nStartIndex)
{
	int  nHighestPChannelInList = 0;

	if((int)nStartIndex == 0)
		return 0;

	POSITION position = m_Channels.GetTailPosition();
	
	int nLastPChannel = (int)nStartIndex;

	// If there are no channels in use then we can go the whole range
	if(m_Channels.IsEmpty())
	{
		nLastPChannel--;
		nLastPChannel = nLastPChannel < 0 ? 0 : nLastPChannel;
		nLastPChannel = nLastPChannel > 998 ? 998 : nLastPChannel;
		return nLastPChannel;
	}

	while(position)
	{
		int nPChannelInList = m_Channels.GetPrev(position) + 1;

		if(nLastPChannel > nPChannelInList + 1) // If a vacant position
		{
			// The difference could be more than one pchannel
			// in which case return the closest to the highest on the list
			if(abs(nLastPChannel - nPChannelInList + 1) > 1)
				return nLastPChannel - 1;
			else
				return nPChannelInList + 1;
		}
		
		else if(nLastPChannel > nPChannelInList)
			nLastPChannel = nPChannelInList;
	}

	// None before were found so return the start index
	return -1;	
}