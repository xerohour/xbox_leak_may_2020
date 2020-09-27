//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
//-------------------------------------------------------------
#include "std.h"

#include "Locale.h"
#include "FileUtil.h"
#include "globals.h"
#include "utilities.h"
#include "SmartObject.h"
#include "Translator.h"
#include "IniFile.h"
#include "TGL.h"
#include "Font.h"
#include "XOConst.h"

#define TEXT_SCALE 40


D3DXMATRIX  CText::s_scaleMatrix;


void CText::VerticalFade(TEXTVERTEX* verts, int nVertexCount, float nTop, float nBottom, float nScroll)
{
	for (int i = 0; i < nVertexCount; i += 1)
	{
		float a = 1.0f;
		float y = verts[i].y + nScroll;

		if (y > nTop || y < nBottom)
		{
			a = 0.0f;
		}
		else if (y > nTop - 1.0f)
		{
			a = 1.0f - (y - (nTop - 1.0f));
		}
		else if (y < nBottom + 1.0f)
		{
			a = 1.0f - ((nBottom + 1.0f) - y);
		}
		else //if (y >= nTop + 1.0f && y <= nBottom - 1.0f)
		{
			a = 1.0f;
		}

		ASSERT(a >= 0.0f && a <= 1.0f);

		a = smoothstep(0.0f, 1.0f, a);
//		float a0 = (float)(verts[i].color >> 24);
		verts[i].color = (verts[i].color & 0x00ffffff) | (((DWORD)(255.0f * a)) << 24);
	}
}


////////////////////////////////////////////////////////////////////////////

struct CFontTableEntry
{
	CFontTableEntry()
	{
		m_szFaceName = NULL;
		m_szFileName = NULL;
	}

	CFontTableEntry(const TCHAR* szFaceName, const TCHAR* szFileName)
	{
		m_szFaceName = szFaceName;
		m_szFileName = szFileName;
	}

	const TCHAR* m_szFaceName;
	const TCHAR* m_szFileName;
	CFont m_font;
};

#define MAX_FONT_COUNT 10

CFontTableEntry g_fonts [MAX_FONT_COUNT] =
{
	// NOTE: The first font is used as the default when a specified font is not found!
	CFontTableEntry(_T("HGGothicM"), _T("HGGothicM.xtf")),
};

int g_nFontCount = 1;

void InitFontTable()
{
	TCHAR szWild [MAX_PATH];
	WIN32_FIND_DATA fd;
	HANDLE h;


	TCHAR szFontPath [MAX_PATH];
	_tcscpy(szFontPath, XTF_PATH);

	_tcscpy(szWild, szFontPath);
	_tcscat(szWild, _T("*.xtf"));

#if defined(_XBOX)
	char szWildA [MAX_PATH];
	Ansi(szWildA, szWild, MAX_PATH);
	h = FindFirstFile(szWildA, &fd);
#else
	h = FindFirstFile(szWild, &fd);
#endif

	if (h == INVALID_HANDLE_VALUE)
	{
		DbgPrint("\001InitFontTable: No fonts!\n");
		return;
	}

	do
	{
		TCHAR szFileName [MAX_PATH];

		_tcscpy(szFileName, szFontPath);


#if defined(_XBOX)
		Unicode(szFileName + _tcslen(szFileName), fd.cFileName, countof(szFileName));
#else
		_tcscpy(szFileName + cchAppDir, fd.cFileName);
#endif

		if (g_nFontCount >= MAX_FONT_COUNT)
		{
			DbgPrint("\001InitFontTable: too many fonts!\n");
			continue;
		}

		HANDLE hFont = XAppCreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFont != INVALID_HANDLE_VALUE)
		{
			DWORD dwRead;
			DWORD cbGlyphSet;
			if(!ReadFile(hFont, &cbGlyphSet, 4, &dwRead, NULL))
			{
				CloseHandle(hFont);
				DbgPrint("InitFontTable() - fail to read from font file\n");
				continue;
			}

			if (cbGlyphSet == 0x30465458)
			{
				DWORD dwHeaderLen;
				ReadFile(hFont, &dwHeaderLen, 4, &dwRead, NULL);

				ASSERT(dwHeaderLen == LF_FACESIZE);

				char szFaceA [LF_FACESIZE];
				ReadFile(hFont, szFaceA, LF_FACESIZE, &dwRead, NULL);

				TCHAR* szFace = new TCHAR [strlen(szFaceA) + 1];
#ifdef _UNICODE
				Unicode(szFace, szFaceA, LF_FACESIZE);
#else
				_tcscpy(szFace, szFaceA);
#endif

				// Don't add the default font twice!
				if (_tcscmp(szFace, g_fonts[0].m_szFaceName) == 0)
				{
					delete [] szFace;
				}
				else
				{
					TCHAR* szFile = new TCHAR [_tcslen(szFileName) + 1];
					if(!szFile)
					{
						// we are out of memory nothing to do
						CloseHandle(hFont);
						FindClose(h);
						return;
					}
					_tcscpy(szFile, szFileName);

					g_fonts[g_nFontCount].m_szFaceName = szFace;
					g_fonts[g_nFontCount].m_szFileName = szFile;
					g_nFontCount += 1;

					DbgPrint("Adding font: %s (%s)\n", szFace, szFile);
				}
			}

			CloseHandle(hFont);
		}
	}
	while (FindNextFile(h, &fd));

	FindClose(h);
}

CFont* GetFont(const TCHAR* szFaceName)
{
	if (g_nFontCount == 1)
		InitFontTable();

	for (int i = 0; i < g_nFontCount; i += 1)
	{
		if (_tcsicmp(szFaceName, g_fonts[i].m_szFaceName) == 0)
			break;
	}

	if (i == g_nFontCount)
	{
		DbgPrint("Substituting font '%s' for '%s'\n", g_fonts[0].m_szFaceName, szFaceName);
		i = 0;
	}

	if (g_fonts[i].m_font.m_hFile == INVALID_HANDLE_VALUE)
	{
		TCHAR szFontPath [MAX_PATH];

		if (_tcschr(g_fonts[i].m_szFileName, ':') == NULL)
			_stprintf(szFontPath, _T("%s%s"), g_szAppDir, g_fonts[i].m_szFileName);
		else
			_tcscpy(szFontPath, g_fonts[i].m_szFileName);

		if (!g_fonts[i].m_font.Open(szFontPath))
		{
			DbgPrint("\001Cannot load font: %s\n", szFaceName);

			if (g_fonts[0].m_font.m_hFile == INVALID_HANDLE_VALUE)
			{
				if (!g_fonts[0].m_font.Open(g_fonts[0].m_szFileName))
				{
					ASSERT(FALSE);
					return NULL;
				}
			}
		}
	}

	return &g_fonts[i].m_font;
}

void Text_Exit()
{
	for (int i = 0; i < g_nFontCount; i += 1)
	{
		if (i > 0)
		{
			delete [] (TCHAR*)g_fonts[i].m_szFaceName;
			delete [] (TCHAR*)g_fonts[i].m_szFileName;
			g_fonts[i].m_szFaceName = NULL;
			g_fonts[i].m_szFileName = NULL;
		}
		g_fonts[i].m_font.Close();
	}
	g_nFontCount = 1;
}

////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////

static const float nScrollSpace = 1.0f;


CText::CText() :
	m_szTextId(NULL),
	m_szText(NULL),
	m_szFont(NULL),
	m_adjust('L'),

	m_bTranslate(true),
	m_width(0.0f),
	m_height(0.0f),
	m_scroll(0.0f),
	m_scrollRate(0.0f),
	m_scrollDelay(0.0f),
	m_bboxMin(0.0f,0.0f,0.0f), 
	m_bboxMax(0.0f,0.0f,0.0f),
	m_bSingleLine(false),
	m_pMesh(NULL),
	m_timeToScroll(0.0f),
	m_nCursorPosition(-1),
	m_fLastRedraw (0.0f),
	m_nCurLanguage (XC_LANGUAGE_UNKNOWN),
    m_bDraw( 1 )
	

	{
	}
	

void CText::CopyObj(const CText& OtherObj)
{
	if(OtherObj.m_szFont)
	{
		m_szFont = OtherObj.m_szFont; 
		ASSERT(m_szFont);
	}

	if(OtherObj.m_szText)
	{
		m_szText = new TCHAR[_tcslen(OtherObj.m_szText)+1];
		ASSERT(m_szText);
		if(m_szText)
		{
			_tcscpy(m_szText, OtherObj.m_szText);
		}
	}
	else
	{
		m_szText = NULL;
	}
	
	if(OtherObj.m_szTextId)
	{
		m_szTextId = new TCHAR[_tcslen(OtherObj.m_szTextId)+1];
		ASSERT(m_szTextId);
		if(m_szTextId)
		{
			_tcscpy(m_szTextId, OtherObj.m_szTextId);
		}
	}
	else
	{
		m_szTextId = NULL;
	}

	m_pMesh = NULL;

	m_adjust = OtherObj.m_adjust;

	m_bTranslate = OtherObj.m_bTranslate;
	m_width  = OtherObj.m_width;
	m_height = OtherObj.m_height;
	m_scroll = OtherObj.m_scroll;
	m_scrollRate = OtherObj.m_scrollRate;
	m_scrollDelay = OtherObj.m_scrollDelay;
	m_fScaleX = OtherObj.m_fScaleX;
    m_fScaleY = OtherObj.m_fScaleY;

	m_materialID = OtherObj.m_materialID;

	m_bboxMin = OtherObj.m_bboxMin;
	m_bboxMax = OtherObj.m_bboxMax;
	m_bSingleLine = OtherObj.m_bSingleLine;
	m_bIsHelpText = OtherObj.m_bIsHelpText;
	m_nCursorPosition = -1;
	m_fLastRedraw = OtherObj.m_fLastRedraw;
	
    m_pAppearance = new TG_Shape(*(OtherObj.m_pAppearance));

}

CText::CText(const CText& OtherObj)
{
	CopyObj(OtherObj);
}


CText& CText::operator = (const CText& OtherObj)
{
	if(&OtherObj != this)
	{
		Cleanup();
		CopyObj(OtherObj);
	}
	return *this;
}
void CText::Cleanup()
{

	delete [] m_szTextId;
	m_szTextId = NULL;

	delete [] m_szText;
	m_szText = NULL;

	if (m_pMesh != NULL)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	}
	SmartObject::Cleanup();
}
CText::~CText()
{
	Cleanup();
}

HRESULT CText::Initialize()
{
    m_materialID = g_MaterialLib.GetButtonTextMaterial();
	return S_OK;
}


HRESULT CText::FrameMove(float nSeconds)
{
	float now;
	if (m_scrollRate > 0.0f && m_pMesh != NULL)
	{
		now = XAppGetNow();

		if (m_timeToScroll == 0.0f)
			m_timeToScroll = now + m_scrollDelay;

		if (now >= m_timeToScroll )
		{
			float nContentHeight = m_bboxMax.y - m_bboxMin.y;
			if (nContentHeight > m_height)
			{
				m_scroll += m_height * nSeconds * m_scrollRate;
				if (m_scroll >= nContentHeight + nScrollSpace)
					m_scroll -= nContentHeight + nScrollSpace;
			}
		}
		
	}
	
	// make cursor blinking, recreate Mesh once in a while
	m_fLastRedraw += nSeconds;
	if(m_nCursorPosition >= 0 && m_pMesh != NULL && m_fLastRedraw > 0.3f)
	{
		m_fLastRedraw = 0.0f;
		m_pMesh->Release();
		m_pMesh = NULL;
	}
	return S_OK;
}

const TCHAR* FaceFromFont(long font)
{
	const TCHAR* szFace = _T("XBox Book"); //default to something real
	{
		if (font == HEADING_FONT)
			szFace = _T("Xbox");
	}

	return szFace;
}


HRESULT CText::Render( DWORD flags /*= 0*/) 
{
    if ( !m_bDraw ) // just invisible, don't sweat
        return S_OK;

	bool bInvalidMesh = false;
    HRESULT hr =S_OK;
	if (m_nCurLanguage != CTranslator::Instance()->GetCurLanguage())
	{
		// invalidate the mesh only for localized texts
		bInvalidMesh = m_bTranslate;
		m_nCurLanguage = CTranslator::Instance()->GetCurLanguage();
	}

	if (m_pMesh != NULL && bInvalidMesh)
	{
		m_pMesh->Release();
		m_pMesh = NULL;
	
		delete [] m_szText;
		m_szText = NULL;

	}

	if (m_pMesh == NULL)
	{
		if (m_bTranslate && !m_szText)
		{
			// for localized text, textId should be not null
			ASSERT(m_szTextId && m_szTextId[0]!= 0);

			TCHAR* pRes = NULL;
			if (m_szTextId && m_szTextId[0])
			{
				pRes = const_cast <TCHAR*> (CTranslator::Instance()->Translate(m_szTextId));
			}

            if ( !pRes )
            {
                m_szText = new TCHAR[_tcslen(_T("Missing Text"))+1];
			    _tcscpy(m_szText, _T("Missing Text"));
            }
            else
            {
			    m_szText = new TCHAR[_tcslen(pRes)+1];
			    _tcscpy(m_szText, pRes);
            }

			ASSERT(m_szText);
			if (m_szText == NULL)
			{
				DbgPrint("CText::Render - Translation is not found, text is empty");
				return hr;
			}
		} 
		
			// allow empty text string
		if(m_szText && m_szText[0] == 0 && m_nCursorPosition < 0)
		{
			return hr; 
		}


	    CFont* pFont = GetFont(m_szFont);
	
	    hr = pFont->CreateTextMesh(m_szText, -1, &m_pMesh, &m_bboxMin, &m_bboxMax, fabsf(m_width/m_fScaleX),m_bSingleLine, m_nCursorPosition);

	    ASSERT(m_pMesh && SUCCEEDED(hr));
	    if (m_pMesh == NULL || FAILED(hr))
		{
			if(m_pMesh)
			{
				m_pMesh->Release();
				m_pMesh = NULL;
			}
			DbgPrint("CText::Render - fail to create the mesh, hr = 0x%x",hr);
			return hr;
		}
	}
	
	float nContentHeight = (m_bboxMax.y - m_bboxMin.y)*m_fScaleY;

	float xOffset = 0.0f;
    float yOffset = 0.f;

    {
		float nWidth = (m_bboxMax.x - m_bboxMin.x)*m_fScaleX;
		if (m_width != 0.0f)
		{
			float widthLimit = fabsf(m_width);
			if (nWidth > widthLimit)
				nWidth = widthLimit;
		}

        if ( m_bSingleLine ) // center vertically
        {
            yOffset = nContentHeight/2.f;
            
        }
		else
		{
			yOffset = (-m_height/ 2.0f) + nScrollSpace * m_fScaleY;
		}

        if (m_adjust == 'L')
        {
   			xOffset = -m_width / 2.0f;
        }
		else if (m_adjust == 'C')
		{
			xOffset = -nWidth / 2.0f;
			if (m_width < 0.0f)
			{
				// TODO: Fade both sides
			}
		}
		else if (m_adjust == 'R')
		{
			xOffset = -nWidth + m_width/2.f;
			if (m_width < 0.0f)
			{
				// TODO: Fade left side
			}
		}
		else
		{
			 ASSERT(false);
		}
	}

	for (int i = 0; i < 2; i += 1)
	{
		if (m_height != 0.0f && nContentHeight > m_height)
		{
			TEXTVERTEX* verts;
			m_pMesh->LockVertexBuffer(0, (BYTE**)&verts);
			float yTop = -m_scroll + 1.0f;
			float yBottom = yTop - m_height - 2.0f;
			if (m_scroll == 0.0f)
				yTop += 1.0f; // don't fade at top when not scrolled...
			CText::VerticalFade(verts, m_pMesh->GetNumVertices(), yTop/* + 0.5f*/, yBottom, i == 0 ? 0 : -(nContentHeight + nScrollSpace));
			m_pMesh->UnlockVertexBuffer();
		}
		else if (i == 1)
		{
			// Don't need second pass if not scrolling...
			break;
		}

		// Don't need second pass unless part of it is visible...
		if (i == 1 && m_scroll + m_height < nContentHeight)
			break;

		hr = g_MaterialLib.Setup( m_materialID, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE );
		ASSERT(SUCCEEDED(hr) );
     	if(FAILED(hr))
		{
			DbgPrint("CText::Render - fail to get material");
		}


        // need to get the transform    
        D3DXMATRIX worldTransform;
        m_pAppearance->GetWorldTransform( worldTransform  );

      
        float fScale = TEXT_SCALE;

        g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, D3DZB_FALSE ); // draw on top regardless...

        D3DXMATRIX oldWorldTransform;
        g_pd3dDevice->GetTransform( D3DTS_WORLD, &oldWorldTransform  );


	    D3DXMATRIX finalWorld;
        D3DXMATRIX flipX;
        // need to move to left edge
        D3DXMATRIX offset;
        D3DXMatrixTranslation( &offset, xOffset, -yOffset, 0.f );
        D3DXMatrixRotationX( &flipX, D3DX_PI/2.f );
        D3DXMatrixMultiply( &worldTransform, &oldWorldTransform, &worldTransform );
        D3DXMatrixMultiply( &worldTransform, &flipX, &worldTransform );
        D3DXMatrixMultiply( &worldTransform, &offset, &worldTransform );
        D3DXMatrixScaling(&s_scaleMatrix, m_fScaleX, m_fScaleY, m_fScaleY);
        D3DXMatrixMultiply( &worldTransform, &s_scaleMatrix, &worldTransform );

        g_pd3dDevice->SetTransform( D3DTS_WORLD, &worldTransform );

		VERIFYHR(g_pd3dDevice ->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));

		hr = m_pMesh->DrawSubset(0);
		VERIFYHR(g_pd3dDevice ->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW));

        g_pd3dDevice->SetTransform( D3DTS_WORLD, &oldWorldTransform  );

	}
	return hr;
}

#ifndef NODE_NAME_LEN
#define NODE_NAME_LEN 64
#endif

HRESULT CText::LoadFromXBG( File* pFile, TG_Shape* pRoot )
{

   long oldPos = pFile->getLogicalPosition();
    long objSize = pFile->readLong( );
   char shapeID[NODE_NAME_LEN];

   pFile->read( (BYTE*)shapeID, NODE_NAME_LEN * sizeof( char ) );

   m_pAppearance = pRoot->FindObject( shapeID );
   ASSERT(m_pAppearance);
   m_pAppearance->setVisible( false );


   char vertAdjust =    pFile->readByte(  );
   m_adjust =           pFile->readByte(  );
   m_bSingleLine =      pFile->readByte( ) ? 1 : 0;
   long fontType =      pFile->readLong();

   m_height =            pFile->readFloat( );
   m_width =           pFile->readFloat( );
   m_bIsHelpText =      pFile->readByte(  ) ? 1 : 0;
   m_fScaleX =          pFile->readFloat(  );
   m_fScaleY =          pFile->readFloat(  );
   m_scrollRate =       pFile->readFloat(  );
   m_scrollDelay =      pFile->readFloat( );
   m_timeToScroll =     pFile->readFloat();   
   m_bTranslate =       pFile->readByte( ) ? 1 : 0;

   long newPos =        pFile->getLogicalPosition();
   long count = (objSize - (newPos - oldPos))/2;
   if ( count )
   {
       m_szTextId = new TCHAR[count+1];
       pFile->read( (BYTE*)m_szTextId, count * sizeof( TCHAR ) );
       m_szTextId[count] = 0;
   }
// TBD REMOVE
   if ( !m_bTranslate ) // need placeholder text of some kind so we don't crash
   {
        m_szText = new TCHAR[3];
        _tcscpy( m_szText, _T("A") );
   }
   m_szFont = FaceFromFont( fontType );

    return S_OK;
}


void    CText::SetMaterial( long resourceMgrID )
{
    m_materialID = resourceMgrID;
}


void CText::Dump(bool bAllData)
{
	if(bAllData)
	{
		char szBuf[256];
		if(m_szTextId)
		{
			ZeroMemory(szBuf,countof(szBuf)); 
			Ansi(szBuf, m_szTextId, min(countof(szBuf)-1, _tcslen(m_szTextId)) );
		}
		else 
		{
			strcpy(szBuf, "[null]");
		}

		DbgPrint("TextId:[%s]\n", szBuf);
		DbgPrint("Translate:[%s]", m_bTranslate? "TRUE":"FALSE");

		if(m_szText!=NULL)
		{
			ZeroMemory(szBuf,countof(szBuf)); 
			Ansi(szBuf, m_szText, countof(szBuf));
			DbgPrint(" Text:[%s]", szBuf);
		}
		else
		{
			DbgPrint(" Text:[null]");
		}		
		
		Ansi(szBuf, m_szFont, countof(szBuf));
		DbgPrint(" Font:[%s]", szBuf);

		DbgPrint(" Adjust:[%c]", m_adjust);
		DbgPrint(" SingleLine:[%s]\n", m_bSingleLine?"TRUE":"FALSE");

		int nLen = sprintf(szBuf,"Height:[%f] Width:[%f] ScrollRate:[%f] ScrollDelay: [%f] \n", m_height, m_width, m_scrollRate, m_scrollDelay );
		ASSERT(nLen < countof(szBuf));
		DbgPrint("%s", szBuf);
		
		nLen = sprintf(szBuf,"ScaleX:[%f] ScaleY:[%f]", m_fScaleX, m_fScaleY);
		ASSERT(nLen < countof(szBuf));
		DbgPrint("%s\n", szBuf);

		DbgPrint("HelpText:[%s]", m_bIsHelpText? "TRUE":"FALSE");
	}

}



void CText::SetTextId(const TCHAR* pNewTextId)
{
	ASSERT(pNewTextId && pNewTextId[0]);
	TG_Shape* pCurShape = m_pAppearance;
	Cleanup();

	// safer to call Cleanup and restore the TG_Shape ptr
	m_pAppearance = pCurShape;
	if (pNewTextId)
	{
		m_szTextId = new TCHAR[_tcslen(pNewTextId)+1];
		if (m_szTextId)
		{
			_tcscpy(m_szTextId, pNewTextId);
		}
	}
}


void CText::SetText(const TCHAR* pNewText)
{
	ASSERT(pNewText);
	TG_Shape* pCurShape = m_pAppearance;
	Cleanup();

	m_pAppearance = pCurShape;
	if (pNewText)
	{
		m_szText = new TCHAR[_tcslen(pNewText)+1];
		if (m_szText)
		{
			_tcscpy(m_szText, pNewText);
		}
	}

    // this won't reset the text properly if its translatable
    m_bTranslate = 0;
}

int CText::MoveCursorLeft(int nPos) 
{
	if(m_nCursorPosition-nPos >= 0)
	{
		m_nCursorPosition = m_nCursorPosition-nPos; 
		if (m_pMesh)
		{
			m_pMesh->Release();
			m_pMesh = NULL;
		}
	}
	return m_nCursorPosition;
}

int CText::MoveCursorRight(int nPos) 
{
	bool bInvalidateMesh = false;
	// allow to draw cursor only
	if(!m_szText || m_szText && !m_szText[0] && nPos == 1)
	{
		bInvalidateMesh = true;
	}

	if(m_szText && (m_nCursorPosition + nPos <=(int) _tcslen(m_szText) )  ) 
	{
		bInvalidateMesh = true;
		m_nCursorPosition = m_nCursorPosition + nPos;
	}
	
	if (bInvalidateMesh && m_pMesh)
	{
			m_pMesh->Release();
			m_pMesh = NULL;
	}
	
	return m_nCursorPosition;
}

int CText::MoveCursorHome()
{
	if(m_szText && !m_szText[0])
	{
		MoveCursorRight();
	}
	MoveCursorLeft(m_nCursorPosition);
	return 	m_nCursorPosition;
}

int CText::MoveCursorEnd()
{
	int nNewPos, nPos =0;
	if(m_szText && m_szText[0])
	{
		nNewPos = _tcslen(m_szText);
	}
	else
	{
		return MoveCursorHome();
	}
	if(m_nCursorPosition>=0)
	{
		nPos = nNewPos - nPos;
	}
	else
	{
		nPos = nNewPos+1;
	}
	MoveCursorRight(nPos);
	return 	m_nCursorPosition;			
}
void CText::AutoTest()
{
	char szBuf[256];
	if(m_szTextId)
	{
		ZeroMemory(szBuf,countof(szBuf)); 
		Ansi(szBuf, m_szTextId, min(countof(szBuf)-1, _tcslen(m_szTextId)) );
	}
	else 
	{
		strcpy(szBuf, "[null]");
	}

	DbgPrint("\nAUTO:TEXT=TEXT_ID[%s]\n", szBuf);

}

void CText::Show( bool bDrawThisObject )
{
    m_bDraw = bDrawThisObject;
  
}
