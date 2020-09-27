//////////////////////////////////////////////////////////////////////
// Copyright (C) Microsoft Corporation, 1999.
//
// CStage.cpp
//
// CStage class - Contains all information for a texture blend stage
//
// History: 5/15/99 Jeff Vezina     - Created
//
//////////////////////////////////////////////////////////////////////

//#define D3D_OVERLOADS
#include "d3dlocus.h"
#include "cd3dtest.h"
#include "CStage.h"
#include "CTextureStage.h"

extern CTextureStage* g_pTextureStage;

CStage::CStage()
{
    // Set default color op and args
    m_dwColorOp=D3DTOP_DISABLE;
    m_dwColorArg1=D3DTA_TEXTURE;
    m_dwColorArg2=D3DTA_CURRENT;

    // Set default alpha op and args
    m_dwAlphaOp=D3DTOP_DISABLE;
    m_dwAlphaArg1=D3DTA_TEXTURE;
    m_dwAlphaArg2=D3DTA_CURRENT;

    m_paNext=NULL;

    ZeroMemory(m_rgdwColor,sizeof(m_rgdwColor));
    m_paTexture=NULL;
}

CStage::~CStage()
{
    if (m_paNext!=NULL) {
        delete m_paNext;
        m_paNext=NULL;
    }
    ReleaseTexture(m_paTexture);
}

CStage *CStage::Create()
{
    // If not last node of the list, move down the list
    if (m_paNext!=NULL) { return m_paNext->Create(); }

    m_paNext=new CStage;

    return m_paNext;
}

void CStage::LoadTexture()
{
//    CImageData *paImage;
//    CImageLoader Image;
//    DEVICEDESC SrcDesc=g_pTextureStage->m_pAdapter->Devices[g_pTextureStage->m_pMode->nSrcDevice].Desc;
    D3DCAPS8 d3dcaps;

    g_pTextureStage->m_pSrcDevice8->GetDeviceCaps(&d3dcaps);

    ReleaseTexture(m_paTexture);

    if (!(d3dcaps.ShadeCaps&D3DPSHADECAPS_ALPHAGOURAUDBLEND))
    {
        for (int i=1;i<4;i++)
            m_rgdwColor[i]=(m_rgdwColor[0]&0xFF000000)|(m_rgdwColor[i]&~0xFF000000);
    }

    if ((m_rgdwColor[0]!=0x00000000) || (m_rgdwColor[1]!=0x00000000) ||
        (m_rgdwColor[2]!=0x00000000) || (m_rgdwColor[3]!=0x00000000))
    {
//        paImage=Image.LoadGradient(128,128,m_rgdwColor);
//        m_paTexture=g_pTextureStage->CreateTexture(128,128,CDDS_TEXTURE_MANAGE,paImage);
        m_paTexture=(CTexture8*)CreateGradientTexture(g_pTextureStage->m_pSrcDevice8, 128, 128, m_rgdwColor);

//        RELEASE(paImage);
    }
}

void CStage::SetStates(DWORD dwStage)
{
    // Set color op and args states
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_COLOROP,m_dwColorOp);
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_COLORARG1,m_dwColorArg1);
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_COLORARG2,m_dwColorArg2);

    LogStage(TRUE, dwStage);

    // Set alpha op and args states
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_ALPHAOP,m_dwAlphaOp);
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_ALPHAARG1,m_dwAlphaArg1);
    g_pTextureStage->SetTextureStageState(dwStage,D3DTSS_ALPHAARG2,m_dwAlphaArg2);

    LogStage(FALSE, dwStage);

    g_pTextureStage->SetTexture(dwStage,m_paTexture);

#ifdef DEMO_HACK // The Win32 NV20 driver doesn't correctly handle D3DTA_SPECULAR as an alpha argument, causing verification to incorrectly report failure
    if (m_dwAlphaArg1 == D3DTA_SPECULAR || m_dwAlphaArg2 == D3DTA_SPECULAR) {
        g_pTextureStage->GetDisplay()->EnableFrameVerification(FALSE);
    }
#endif
}

void CStage::LogStage(BOOL bColor, DWORD dwStage) {

    TCHAR szLog[512], *sz;
    DWORD dwOp, dwArg[2];
    UINT  i;

    _stprintf(szLog, TEXT("Stage %d - "), dwStage);

    if (bColor) {
        _tcscat(szLog, TEXT("Color op: "));
        dwOp = m_dwColorOp;
        dwArg[0] = m_dwColorArg1;
        dwArg[1] = m_dwColorArg2;
    }
    else {
        _tcscat(szLog, TEXT("Alpha op: "));
        dwOp = m_dwAlphaOp;
        dwArg[0] = m_dwAlphaArg1;
        dwArg[1] = m_dwAlphaArg2;
    }

//    sz = szLog + _tcslen(szLog);

    switch (dwOp) {

        case D3DTOP_DISABLE:
            _tcscat(szLog, TEXT("D3DTOP_DISABLE"));
            break;
        case D3DTOP_SELECTARG1:
            _tcscat(szLog, TEXT("D3DTOP_SELECTARG1"));
            break;
        case D3DTOP_SELECTARG2:
            _tcscat(szLog, TEXT("D3DTOP_SELECTARG2"));
            break;
        case D3DTOP_MODULATE:
            _tcscat(szLog, TEXT("D3DTOP_MODULATE"));
            break;
        case D3DTOP_MODULATE2X:
            _tcscat(szLog, TEXT("D3DTOP_MODULATE2X"));
            break;
        case D3DTOP_MODULATE4X:
            _tcscat(szLog, TEXT("D3DTOP_MODULATE4X"));
            break;
        case D3DTOP_ADD:
            _tcscat(szLog, TEXT("D3DTOP_ADD"));
            break;
        case D3DTOP_ADDSIGNED:
            _tcscat(szLog, TEXT("D3DTOP_ADDSIGNED"));
            break;
        case D3DTOP_ADDSIGNED2X:
            _tcscat(szLog, TEXT("D3DTOP_ADDSIGNED2X"));
            break;
        case D3DTOP_SUBTRACT:
            _tcscat(szLog, TEXT("D3DTOP_SUBTRACT"));
            break;
        case D3DTOP_ADDSMOOTH:
            _tcscat(szLog, TEXT("D3DTOP_ADDSMOOTH"));
            break;
        case D3DTOP_BLENDDIFFUSEALPHA:
            _tcscat(szLog, TEXT("D3DTOP_BLENDDIFFUSEALPHA"));
            break;
        case D3DTOP_BLENDTEXTUREALPHA:
            _tcscat(szLog, TEXT("D3DTOP_BLENDTEXTUREALPHA"));
            break;
        case D3DTOP_BLENDFACTORALPHA:
            _tcscat(szLog, TEXT("D3DTOP_BLENDFACTORALPHA"));
            break;
        case D3DTOP_BLENDTEXTUREALPHAPM:
            _tcscat(szLog, TEXT("D3DTOP_BLENDTEXTUREALPHAPM"));
            break;
        case D3DTOP_BLENDCURRENTALPHA:
            _tcscat(szLog, TEXT("D3DTOP_BLENDCURRENTALPHA"));
            break;
        case D3DTOP_PREMODULATE:
            _tcscat(szLog, TEXT("D3DTOP_PREMODULATE"));
            break;
        case D3DTOP_MODULATEALPHA_ADDCOLOR:
            _tcscat(szLog, TEXT("D3DTOP_MODULATEALPHA_ADDCOLOR"));
            break;
        case D3DTOP_MODULATECOLOR_ADDALPHA:
            _tcscat(szLog, TEXT("D3DTOP_MODULATECOLOR_ADDALPHA"));
            break;
        case D3DTOP_MODULATEINVALPHA_ADDCOLOR:
            _tcscat(szLog, TEXT("D3DTOP_MODULATEINVALPHA_ADDCOLOR"));
            break;
        case D3DTOP_MODULATEINVCOLOR_ADDALPHA:
            _tcscat(szLog, TEXT("D3DTOP_MODULATEINVCOLOR_ADDALPHA"));
            break;
        case D3DTOP_BUMPENVMAP:
            _tcscat(szLog, TEXT("D3DTOP_BUMPENVMAP"));
            break;
        case D3DTOP_BUMPENVMAPLUMINANCE:
            _tcscat(szLog, TEXT("D3DTOP_BUMPENVMAPLUMINANCE"));
            break;
        case D3DTOP_DOTPRODUCT3:
            _tcscat(szLog, TEXT("D3DTOP_DOTPRODUCT3"));
            break;
        case D3DTOP_MULTIPLYADD:
            _tcscat(szLog, TEXT("D3DTOP_MULTIPLYADD"));
            break;
        case D3DTOP_LERP:
            _tcscat(szLog, TEXT("D3DTOP_LERP"));
            break;
    }


    for (i = 0; i < 2; i++) {

        _stprintf(szLog+_tcslen(szLog), TEXT(", Arg %d: "), i+1);

        switch (dwArg[i]) {
            case D3DTA_DIFFUSE:
                _tcscat(szLog, TEXT("D3DTA_DIFFUSE"));
                break;
            case D3DTA_TEXTURE:
                _tcscat(szLog, TEXT("D3DTA_TEXTURE"));
                break;
            case D3DTA_CURRENT:
                _tcscat(szLog, TEXT("D3DTA_CURRENT"));
                break;
            case D3DTA_TFACTOR:
                _tcscat(szLog, TEXT("D3DTA_TFACTOR"));
                break;
            case D3DTA_SPECULAR:
                _tcscat(szLog, TEXT("D3DTA_SPECULAR"));
                break;
        }
    }

    Log(LOG_COMMENT, szLog);
}

void CStage::WriteToStatus(UINT uStage)
{
    char szStage[50];
    char *rgszTextureOp[21]={   "Disabled",
                                "Arg1",
                                "Arg2",
                                "Modulate",
                                "Modulate2X",
                                "Modulate4X",
                                "Add",
                                "AddSigned",
                                "AddSigned2X",
                                "Subtract",
                                "AddSmooth",
                                "BlendDiffuseAlpha",
                                "BlendTextureAlpha",
                                "BlendFactorAlpha",
                                "BlendTextureAlphaPM",
                                "BlendCurrentAlpha",
                                "Premodulate",
                                "ModulateAlphaAddColor",
                                "ModulateColorAddAlpha",
                                "ModulateInvAlphaAddColor",
                                "ModulateInvColorAddAlpha"};
    char *rgszTextureArg[5]={   "Diffuse",
                                "Current",
                                "Texture",
                                "TFactor",
                                "Specular"};

    if ((m_dwColorOp!=D3DTOP_DISABLE) || (m_dwAlphaOp!=D3DTOP_DISABLE) || (uStage==0))
    {
        sprintf(szStage,"$gStage %u Color",uStage);
        g_pTextureStage->WriteStatus(szStage,"$yOp=$c%s $yArg1=$c%s $yArg2=$c%s",
                                   rgszTextureOp[m_dwColorOp-1],rgszTextureArg[m_dwColorArg1],
                                   rgszTextureArg[m_dwColorArg2]);
        sprintf(szStage,"$gStage %u Alpha",uStage);
        g_pTextureStage->WriteStatus(szStage,"$yOp=$c%s $yArg1=$c%s $yArg2=$c%s",
                                   rgszTextureOp[m_dwAlphaOp-1],rgszTextureArg[m_dwAlphaArg1],
                                   rgszTextureArg[m_dwAlphaArg2]);

        sprintf(szStage,"$gStage %u Texture Top",uStage);
        g_pTextureStage->WriteStatus(szStage,"");
        sprintf(szStage,"$gStage %u Texture Bottom",uStage);
        g_pTextureStage->WriteStatus(szStage,"");

        // Was a texture loaded?
        if ((m_rgdwColor[0]!=0x00000000) || (m_rgdwColor[1]!=0x00000000) ||
            (m_rgdwColor[2]!=0x00000000) || (m_rgdwColor[3]!=0x00000000))
        {
            sprintf(szStage,"$gStage %u Texture Top",uStage);
            g_pTextureStage->WriteStatus(szStage,"$yLeft=$c0x%08X $yRight=$c0x%08X",m_rgdwColor[0],m_rgdwColor[1]);
            sprintf(szStage,"$gStage %u Texture Bottom",uStage);
            g_pTextureStage->WriteStatus(szStage,"$yLeft=$c0x%08X $yRight=$c0x%08X",m_rgdwColor[2],m_rgdwColor[3]);
        }
    }
}

void CStage::WriteToLog(UINT uStage)
{
    char *rgszTextureOp[21]={   "Disabled",
                                "Arg1",
                                "Arg2",
                                "Modulate",
                                "Modulate2X",
                                "Modulate4X",
                                "Add",
                                "AddSigned",
                                "AddSigned2X",
                                "Subtract",
                                "AddSmooth",
                                "BlendDiffuseAlpha",
                                "BlendTextureAlpha",
                                "BlendFactorAlpha",
                                "BlendTextureAlphaPM",
                                "BlendCurrentAlpha",
                                "Premodulate",
                                "ModulateAlphaAddColor",
                                "ModulateColorAddAlpha",
                                "ModulateInvAlphaAddColor",
                                "ModulateInvColorAddAlpha"};
    char *rgszTextureArg[5]={   "Diffuse",
                                "Current",
                                "Texture",
                                "TFactor",
                                "Specular"};

    if ((m_dwColorOp!=D3DTOP_DISABLE) || (m_dwAlphaOp!=D3DTOP_DISABLE) || (uStage==0))
    {
        // Was a texture loaded?
        if ((m_rgdwColor[0]!=0x00000000) || (m_rgdwColor[1]!=0x00000000) ||
            (m_rgdwColor[2]!=0x00000000) || (m_rgdwColor[3]!=0x00000000))
        {
            g_pTextureStage->WriteToLog("Stage %u\n"
                                      "\t\tColorOp=%s ColorArg1=%s ColorArg2=%s\n"
                                      "\t\tAlphaOp=%s AlphaArg1=%s AlphaArg2=%s\n"
                                      "\t\tTexture colors: (0,0)=0x%08X (1,0)=0x%08X (0,1)=0x%08X (1,1)=0x%08X\n\n",
                                      uStage,rgszTextureOp[m_dwColorOp-1],
                                      rgszTextureArg[m_dwColorArg1],
                                      rgszTextureArg[m_dwColorArg2],
                                      rgszTextureOp[m_dwAlphaOp-1],
                                      rgszTextureArg[m_dwAlphaArg1],
                                      rgszTextureArg[m_dwAlphaArg2],
                                      m_rgdwColor[0],m_rgdwColor[1],m_rgdwColor[2],m_rgdwColor[3]);
        }
        else
        {
            g_pTextureStage->WriteToLog("Stage %u\n"
                                      "\t\tColorOp=%s ColorArg1=%s ColorArg2=%s\n"
                                      "\t\tAlphaOp=%s AlphaArg1=%s AlphaArg2=%s\n\n",
                                      uStage,rgszTextureOp[m_dwColorOp-1],
                                      rgszTextureArg[m_dwColorArg1],
                                      rgszTextureArg[m_dwColorArg2],
                                      rgszTextureOp[m_dwAlphaOp-1],
                                      rgszTextureArg[m_dwAlphaArg1],
                                      rgszTextureArg[m_dwAlphaArg2]);
        }
    }
}

CStage *CStage::GetStageList(int n)
{
    if ((m_paNext!=NULL) && (n>0))
        return m_paNext->GetStageList(n-1);
    else
        return this;
}

UINT CStage::Count()
{
    if (m_paNext!=NULL) { return m_paNext->Count()+1; }

    return 1;
}
