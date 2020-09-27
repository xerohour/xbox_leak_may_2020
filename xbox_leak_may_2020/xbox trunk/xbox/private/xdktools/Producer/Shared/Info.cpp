//////////////////////////////////////////////////////////////////////
//
//	Info.cpp
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "info.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
//
// CInfo::Load
//
//////////////////////////////////////////////////////////////////////
void CInfo::Load(IDMUSProdRIFFStream* pIRiffStream, MMCKINFO* pckMain)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	IStream* pIStream;
    MMCKINFO ck;
	
	pIStream = pIRiffStream->GetStream();
	
	ck.ckid = 0;
	ck.fccType = 0;
    
	while(pIRiffStream->Descend(&ck, pckMain, 0) == 0)
	{
		
		switch(ck.ckid) 
		{
			case mmioFOURCC('I','C','O','P'):
				Alloc_and_Read(m_csCopyright, pIStream, &ck);
				break;

			case mmioFOURCC('I','S','F','T'):
				Alloc_and_Read(m_csSoftware, pIStream, &ck);
				break;
		
			case mmioFOURCC('I','N','A','M'):
				Alloc_and_Read(m_csName, pIStream, &ck);
				break;
		
			case mmioFOURCC('I','C','R','D'):
				Alloc_and_Read(m_csDate, pIStream, &ck);
				break;
			
			case mmioFOURCC('I','E','N','G'):
				Alloc_and_Read(m_csEngineer, pIStream, &ck);
				break;
		
			case mmioFOURCC('I','P','R','D'):
				Alloc_and_Read(m_csName, pIStream, &ck);
				break;
		
			case mmioFOURCC('I','C','M','T'):
				Alloc_and_Read(m_csComment, pIStream, &ck);
				break;
		
			case mmioFOURCC('I','S','B','J'):
				Alloc_and_Read(m_csSubject, pIStream, &ck);
				break;

            default: // we don't know what this is - just copy the data.
                m_UnknownChunk.Load(pIRiffStream, &ck, FALSE);
                break;
		}

        pIRiffStream->Ascend(&ck, 0);
		
		ck.ckid = 0;
		ck.fccType = 0;
	}
	pIStream->Release();
}

//////////////////////////////////////////////////////////////////////
//
//	SaveString
//
//////////////////////////////////////////////////////////////////////
static DWORD SaveString(DWORD dwID, IDMUSProdRIFFStream* pIRiffStream, CString csString)
{
	HRESULT hr = S_OK; // Return code
	MMCKINFO ck;
	DWORD cb;
    
	ck.ckid = dwID;
	
	IStream *pIStream = pIRiffStream->GetStream();
    
	hr = pIRiffStream->CreateChunk(&ck, 0);
    if(FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }
    
	hr = pIStream->Write((LPCTSTR) csString, csString.GetLength()+1, &cb);
    if(FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }
    
	if(FAILED(pIRiffStream->Ascend(&ck, 0)))
    {
		pIStream->Release();
        return (E_FAIL);
    }
	
	pIStream->Release();
    return (hr);
}

//////////////////////////////////////////////////////////////////////
//
//	CInfo::Save
//
//////////////////////////////////////////////////////////////////////
DWORD CInfo::Save(IDMUSProdRIFFStream* pIRiffStream)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);
	
	HRESULT hr = S_OK; // Return code
	MMCKINFO ck;
	
	IStream* pIStream = pIRiffStream->GetStream();
    
	ck.fccType = mmioFOURCC('I','N','F','O') ;
    
	hr = pIRiffStream->CreateChunk(&ck, MMIO_CREATELIST);
    
	if (FAILED(hr))
    {
		pIStream->Release();
        return (hr);
    }

    SaveString(mmioFOURCC('I','C','M','T'), pIRiffStream, m_csComment);
    
    SaveString(mmioFOURCC('I','C','O','P'), pIRiffStream, m_csCopyright);

    if(!m_csDate.IsEmpty())
    {
        SaveString(mmioFOURCC('D','A','T','E'), pIRiffStream, m_csDate);
    }

    SaveString(mmioFOURCC('I','E','N','G'), pIRiffStream, m_csEngineer);

    if(!m_csName.IsEmpty())
    {
        SaveString(mmioFOURCC('I','N','A','M'), pIRiffStream, m_csName);
    }

    if(!m_csProduct.IsEmpty())
    {
        SaveString(mmioFOURCC('I','P','R','D'), pIRiffStream, m_csProduct);
    }

    if(!m_csSoftware.IsEmpty())
    {
        SaveString(mmioFOURCC('I','S','F','T'), pIRiffStream, m_csSoftware);
    }

    SaveString(mmioFOURCC('I','S','B','J'), pIRiffStream, m_csSubject);

    //save unknown chunks
    m_UnknownChunk.Save(pIRiffStream);

    pIRiffStream->Ascend(&ck, 0);
	pIStream->Release();
    
	return (hr);
}

bool CInfo::Load(IStream* pIStream)
{
	return true;
}

bool CInfo::Save(IStream* pIStream)
{
	return true;
}

void CInfo::Alloc_and_Read(CString & csDest,IStream* pIStream, MMCKINFO* pck)
{
	DWORD cb;
	void* pstring = NULL;

	pstring = new char[pck->cksize];
	if (pstring)
	{
		pIStream->Read(pstring, pck->cksize, &cb);
		csDest = (char*) pstring;
		delete pstring;
	}
	else
	{
		return;
	}
}
