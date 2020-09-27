VERSION 5.00
Begin VB.Form frmProp 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Properties"
   ClientHeight    =   2520
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   5010
   Icon            =   "frmProp.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   2520
   ScaleWidth      =   5010
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.CheckBox chkKiosk 
      Alignment       =   1  'Right Justify
      Caption         =   "&Kiosk Mode"
      Height          =   255
      Left            =   3720
      TabIndex        =   0
      Top             =   420
      Width           =   1155
   End
   Begin VB.CommandButton btnSave 
      Caption         =   "&Save"
      CausesValidation=   0   'False
      Default         =   -1  'True
      Height          =   375
      Left            =   2580
      TabIndex        =   3
      Top             =   2040
      Width           =   1035
   End
   Begin VB.TextBox txtPersist 
      CausesValidation=   0   'False
      Height          =   315
      Left            =   1200
      MaxLength       =   40
      TabIndex        =   2
      Top             =   1500
      Width           =   3675
   End
   Begin VB.TextBox txtCallingXBE 
      CausesValidation=   0   'False
      Height          =   315
      Left            =   1200
      MaxLength       =   40
      TabIndex        =   1
      Top             =   1080
      Width           =   3675
   End
   Begin VB.CommandButton btnCancel 
      Cancel          =   -1  'True
      Caption         =   "&Cancel"
      CausesValidation=   0   'False
      Height          =   375
      Left            =   3840
      TabIndex        =   4
      Top             =   2040
      Width           =   1035
   End
   Begin VB.Label lblCallingXBE 
      AutoSize        =   -1  'True
      Caption         =   "Calling &XBE:"
      Height          =   195
      Left            =   180
      TabIndex        =   8
      Top             =   1140
      Width           =   870
   End
   Begin VB.Label lblPersist 
      AutoSize        =   -1  'True
      Caption         =   "&Persist image:"
      Height          =   195
      Left            =   165
      TabIndex        =   7
      Top             =   1560
      Width           =   975
   End
   Begin VB.Label lblNum 
      Caption         =   "Number of demo entries:"
      Height          =   255
      Left            =   240
      TabIndex        =   6
      Top             =   420
      Width           =   3015
   End
   Begin VB.Line Line2 
      BorderColor     =   &H00E0E0E0&
      X1              =   120
      X2              =   4920
      Y1              =   840
      Y2              =   840
   End
   Begin VB.Line Line1 
      BorderColor     =   &H00000000&
      X1              =   60
      X2              =   4860
      Y1              =   780
      Y2              =   780
   End
   Begin VB.Label lblVer 
      Caption         =   "File Version: 1.0"
      Height          =   255
      Left            =   240
      TabIndex        =   5
      Top             =   120
      Width           =   3015
   End
End
Attribute VB_Name = "frmProp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Dim lHeader As XDIHEADER

Private Sub btnCancel_Click()
  Unload Me
End Sub

Private Sub btnSave_Click()
  Dim fDirty As Boolean
  
  If chkKiosk.Value = 0 And txtCallingXBE.Text = "" Then
    MsgBox "You must enter a value for the Calling XBE.", vbOKOnly
    txtCallingXBE.Text = strDEFAULTXBE
    txtCallingXBE.SetFocus
  Else
    If chkKiosk.Value = 0 Then
      tHeader.strCallingXBE = Trim(txtCallingXBE.Text)
      tHeader.strPersist = Trim(txtPersist.Text)
    Else
      tHeader.strCallingXBE = ""
      tHeader.strPersist = ""
    End If
    
    tHeader.lKiosk = chkKiosk.Value
    
    fDirty = True
    If Trim(tHeader.strPersist) = Trim(lHeader.strPersist) Then
      If Trim(tHeader.strCallingXBE) = Trim(lHeader.strCallingXBE) Then
        If tHeader.lKiosk = lHeader.lKiosk Then
          If False = g_fDirty Then
            fDirty = False
          End If
        End If
      End If
    End If
    
    g_fDirty = fDirty
    
    SetUIState
    Unload Me
  End If
End Sub

Private Sub chkKiosk_Click()
  SetPropUIState
End Sub

Private Sub Form_Activate()
  If chkKiosk.Value Then
    btnSave.SetFocus
  Else
    txtCallingXBE.SetFocus
  End If
End Sub

Private Sub Form_Load()
  Dim lVersion As Long
  Dim mHeader As XDIHEADER
  
  Line2.X1 = Line1.X1 + 10
  Line2.Y1 = Line1.Y2 + 10
  Line2.Y2 = Line2.Y1
  
  lHeader.lKiosk = tHeader.lKiosk
  lHeader.strCallingXBE = Trim(tHeader.strCallingXBE)
  lHeader.strPersist = Trim(tHeader.strPersist)
  
  lblNum.Caption = "Number of demo entries:" + Str(tHeader.lNumDemos)
  
  lVersion = XDIGetDllversion
  
  If g_fNamedFile Then
    Caption = GetFileNameOnly(g_strXDIFile) + " Properties"
    If XDIGetDemoHeader(g_strXDIFile, mHeader) Then
      lVersion = mHeader.lVerMajor * &H10000 + mHeader.lVerMinor
    End If
  Else
    Caption = "New XDI File Properties"
  End If
  
  lblVer = "File Version: " + Trim(Str((lVersion And &HFFFF0000) \ &H10000)) + "." + _
                              Trim(Str(lVersion And &HFFFF&))
  
  chkKiosk.Value = tHeader.lKiosk
  txtCallingXBE.Text = Trim(tHeader.strCallingXBE)
  txtPersist.Text = Trim(tHeader.strPersist)
  
  If 0 = chkKiosk.Value And "" = txtCallingXBE.Text Then
    txtCallingXBE.Text = strDEFAULTXBE
  End If
  
  SetPropUIState
End Sub

Private Sub SetPropUIState()
  Dim fKiosk As Boolean
  
  fKiosk = IIf(chkKiosk.Value, False, True)
  
  txtCallingXBE.Enabled = fKiosk
  txtCallingXBE.BackColor = IIf(fKiosk, vbWindowBackground, vbInactiveBorder)
  lblCallingXBE.Enabled = fKiosk
  
  txtPersist.Enabled = fKiosk
  txtPersist.BackColor = IIf(fKiosk, vbWindowBackground, vbInactiveBorder)
  lblPersist.Enabled = fKiosk
  
  If fKiosk And "" = txtCallingXBE.Text Then
    txtCallingXBE.Text = strDEFAULTXBE
  End If
End Sub
