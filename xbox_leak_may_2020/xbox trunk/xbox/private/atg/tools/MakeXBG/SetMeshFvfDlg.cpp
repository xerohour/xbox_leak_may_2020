//-----------------------------------------------------------------------------
// File: SetMeshFVFDlg.cpp
//
// Desc: Code to implement a dialog to let the user modify the FVF code of a
//       mesh.
//
// Hist: 03.01.01 - New for April XDK release
//       11.08.01 - Made checkboxes work according to FVF code restrictions
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "MakeXBG.h"
#include "SetMeshFvfDlg.h"
#include "FVF.h"




//-----------------------------------------------------------------------------
// Name: CSetMeshFvfDlg()
// Desc: Constructor
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC(CSetMeshFvfDlg, CDialog)

CSetMeshFvfDlg::CSetMeshFvfDlg( CWnd* pParent /*=NULL*/ )
          :CDialog(CSetMeshFvfDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSetMeshFvfDlg)
    //}}AFX_DATA_INIT
}




//-----------------------------------------------------------------------------
// Name: DoDataExchange()
// Desc: 
//-----------------------------------------------------------------------------
void CSetMeshFvfDlg::DoDataExchange( CDataExchange* pDX )
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSetMeshFvfDlg)
    //}}AFX_DATA_MAP

    if( pDX->m_bSaveAndValidate )
    {
    }
    else
    {
        VERTEXFORMAT fvf;
        CrackFVF( m_dwFVF, fvf );
        
        // Set controls for position, normal, diffuse, and specular
        ((CButton*)GetDlgItem(IDC_POSITION))->SetCheck( fvf.bPosition );
        ((CButton*)GetDlgItem(IDC_NORMAL))->SetCheck( fvf.bNormal );
        ((CButton*)GetDlgItem(IDC_DIFFUSE))->SetCheck( fvf.bDiffuse );
        ((CButton*)GetDlgItem(IDC_SPECULAR))->SetCheck( fvf.bSpecular );

        // Set controls for blend weight
        ((CButton*)GetDlgItem(IDC_BLENDWEIGHTS))->SetCheck( fvf.dwNumBlendWeights > 0 );
        GetDlgItem(IDC_NUMBLENDWEIGHTS)->EnableWindow( fvf.dwNumBlendWeights > 0 );
        ((CComboBox*)GetDlgItem(IDC_NUMBLENDWEIGHTS))->SetCurSel(fvf.dwNumBlendWeights-1);

        // Handle texture coordinates
        ((CButton*)GetDlgItem(IDC_TEXTURE0))->SetCheck( fvf.dwNumTexCoords[0] > 0 );
        ((CButton*)GetDlgItem(IDC_TEXTURE1))->SetCheck( fvf.dwNumTexCoords[1] > 0 );
        ((CButton*)GetDlgItem(IDC_TEXTURE2))->SetCheck( fvf.dwNumTexCoords[2] > 0 );
        ((CButton*)GetDlgItem(IDC_TEXTURE3))->SetCheck( fvf.dwNumTexCoords[3] > 0 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS0))->EnableWindow( fvf.dwNumTexCoords[0] > 0 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS1))->EnableWindow( fvf.dwNumTexCoords[1] > 0 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS2))->EnableWindow( fvf.dwNumTexCoords[2] > 0 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS3))->EnableWindow( fvf.dwNumTexCoords[3] > 0 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS0))->SetCurSel( fvf.dwNumTexCoords[0] ? fvf.dwNumTexCoords[0]-1 : 1 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS1))->SetCurSel( fvf.dwNumTexCoords[1] ? fvf.dwNumTexCoords[1]-1 : 1 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS2))->SetCurSel( fvf.dwNumTexCoords[2] ? fvf.dwNumTexCoords[2]-1 : 1 );
        ((CComboBox*)GetDlgItem(IDC_TEXCOORDS3))->SetCurSel( fvf.dwNumTexCoords[3] ? fvf.dwNumTexCoords[3]-1 : 1 );

        // Display the FVF
        TCHAR strText[20];
        _stprintf( strText, "0x%08lx", m_dwFVF );
        GetDlgItem(IDC_FVF)->SetWindowText( strText );
    }
}




BEGIN_MESSAGE_MAP(CSetMeshFvfDlg, CDialog)
    //{{AFX_MSG_MAP(CSetMeshFvfDlg)
    ON_BN_CLICKED(IDC_POSITION, OnControl)
    ON_BN_CLICKED(IDC_NORMAL,   OnControl)
    ON_BN_CLICKED(IDC_DIFFUSE,  OnControl)
    ON_BN_CLICKED(IDC_SPECULAR, OnControl)
    ON_BN_CLICKED(IDC_BLENDWEIGHTS, OnControl)
    ON_CBN_SELCHANGE(IDC_NUMBLENDWEIGHTS, OnControl)
    ON_BN_CLICKED(IDC_TEXTURE0, OnControl)
    ON_BN_CLICKED(IDC_TEXTURE1, OnControl)
    ON_BN_CLICKED(IDC_TEXTURE2, OnControl)
    ON_BN_CLICKED(IDC_TEXTURE3, OnControl)
    ON_CBN_SELCHANGE(IDC_TEXCOORDS0, OnControl)
    ON_CBN_SELCHANGE(IDC_TEXCOORDS1, OnControl)
    ON_CBN_SELCHANGE(IDC_TEXCOORDS2, OnControl)
    ON_CBN_SELCHANGE(IDC_TEXCOORDS3, OnControl)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()




//-----------------------------------------------------------------------------
// Name: OnControl()
// Desc: 
//-----------------------------------------------------------------------------
void CSetMeshFvfDlg::OnControl() 
{
    VERTEXFORMAT fvf;
    ZeroMemory( &fvf, sizeof(fvf) );

    // Handle position, normal, diffuse, and specular
    fvf.bPosition = ((CButton*)GetDlgItem(IDC_POSITION))->GetCheck();
    fvf.bNormal   = ((CButton*)GetDlgItem(IDC_NORMAL))->GetCheck();
    fvf.bDiffuse  = ((CButton*)GetDlgItem(IDC_DIFFUSE))->GetCheck();
    fvf.bSpecular = ((CButton*)GetDlgItem(IDC_SPECULAR))->GetCheck();

    // Handle blend weights
    if( ((CButton*)GetDlgItem(IDC_BLENDWEIGHTS))->GetCheck() )
    {
        GetDlgItem(IDC_NUMBLENDWEIGHTS)->EnableWindow(TRUE);

        fvf.dwNumBlendWeights = ((CComboBox*)GetDlgItem(IDC_NUMBLENDWEIGHTS))->GetCurSel() + 1;
    }
    else
        GetDlgItem(IDC_NUMBLENDWEIGHTS)->EnableWindow(FALSE);

    // Handle texture coordinates
    CButton* pTextureButtons[4];
    pTextureButtons[0] = ((CButton*)GetDlgItem(IDC_TEXTURE0));
    pTextureButtons[1] = ((CButton*)GetDlgItem(IDC_TEXTURE1));
    pTextureButtons[2] = ((CButton*)GetDlgItem(IDC_TEXTURE2));
    pTextureButtons[3] = ((CButton*)GetDlgItem(IDC_TEXTURE3));
    CComboBox* pTextureComboBoxes[4];
    pTextureComboBoxes[0] = ((CComboBox*)GetDlgItem(IDC_TEXCOORDS0));
    pTextureComboBoxes[1] = ((CComboBox*)GetDlgItem(IDC_TEXCOORDS1));
    pTextureComboBoxes[2] = ((CComboBox*)GetDlgItem(IDC_TEXCOORDS2));
    pTextureComboBoxes[3] = ((CComboBox*)GetDlgItem(IDC_TEXCOORDS3));

    for( DWORD i=0; i<4; i++ )
    {
        if( pTextureButtons[i]->GetCheck() )
        {
            pTextureComboBoxes[i]->EnableWindow(TRUE);

            fvf.dwNumTexCoords[i] = pTextureComboBoxes[i]->GetCurSel() + 1;
        }
        else
        {
            pTextureComboBoxes[i]->EnableWindow(FALSE);

            // Turn all above stages off
            for( DWORD j=i; j<4; j++ )
                pTextureButtons[j]->SetCheck( FALSE );
        }
    }

    // Compute the resulting FVF code
    m_dwFVF = EncodeFVF( fvf );

    // Display the FVF
    TCHAR strText[20];
    _stprintf( strText, "0x%08lx", m_dwFVF );
    GetDlgItem(IDC_FVF)->SetWindowText( strText );
}





