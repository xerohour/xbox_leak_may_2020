VERSION 5.00
Begin VB.Form AppEntry 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Demo Entry Add/Modify"
   ClientHeight    =   6210
   ClientLeft      =   45
   ClientTop       =   330
   ClientWidth     =   9405
   Icon            =   "AppEntry.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   6210
   ScaleWidth      =   9405
   ShowInTaskbar   =   0   'False
   StartUpPosition =   1  'CenterOwner
   Begin VB.CheckBox chkKiosk 
      Alignment       =   1  'Right Justify
      Caption         =   "&Kiosk Run"
      CausesValidation=   0   'False
      Height          =   255
      Left            =   3840
      TabIndex        =   6
      Top             =   1260
      Width           =   1095
   End
   Begin VB.VScrollBar VScroll 
      Height          =   315
      Left            =   2520
      Max             =   99
      Min             =   1
      TabIndex        =   21
      TabStop         =   0   'False
      Top             =   1200
      Value           =   1
      Width           =   315
   End
   Begin VB.TextBox txtPriority 
      Height          =   315
      Left            =   2160
      MaxLength       =   2
      TabIndex        =   5
      Top             =   1200
      Width           =   375
   End
   Begin VB.TextBox txtPersist 
      Height          =   375
      Left            =   2160
      MaxLength       =   40
      TabIndex        =   16
      Top             =   3780
      Width           =   7095
   End
   Begin VB.TextBox txtAddi 
      Height          =   1035
      Left            =   2160
      MaxLength       =   255
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   18
      Top             =   4320
      Width           =   7095
   End
   Begin VB.TextBox txtAuthor 
      Height          =   375
      Left            =   2160
      MaxLength       =   40
      TabIndex        =   14
      Top             =   3240
      Width           =   7095
   End
   Begin VB.TextBox txtTeaser 
      Height          =   375
      Left            =   2160
      MaxLength       =   40
      TabIndex        =   12
      Top             =   2700
      Width           =   7095
   End
   Begin VB.ComboBox cboTeaser 
      Height          =   315
      ItemData        =   "AppEntry.frx":08CA
      Left            =   2160
      List            =   "AppEntry.frx":08D1
      Style           =   2  'Dropdown List
      TabIndex        =   10
      Top             =   2220
      Width           =   2835
   End
   Begin VB.TextBox txtXBE 
      Height          =   375
      Left            =   2160
      MaxLength       =   40
      TabIndex        =   8
      Top             =   1680
      Width           =   7095
   End
   Begin VB.ComboBox cboType 
      Height          =   315
      ItemData        =   "AppEntry.frx":08DC
      Left            =   2160
      List            =   "AppEntry.frx":08E6
      Style           =   2  'Dropdown List
      TabIndex        =   3
      Top             =   720
      Width           =   2835
   End
   Begin VB.TextBox txtTitle 
      Height          =   375
      Left            =   2160
      MaxLength       =   40
      TabIndex        =   1
      Top             =   180
      Width           =   7095
   End
   Begin VB.CommandButton btnCancel 
      Caption         =   "Cancel"
      Height          =   495
      Left            =   8040
      TabIndex        =   20
      Top             =   5580
      Width           =   1215
   End
   Begin VB.CommandButton btnSave 
      Caption         =   "&Save"
      Height          =   495
      Left            =   6360
      TabIndex        =   19
      Top             =   5580
      Width           =   1215
   End
   Begin VB.Label lblLabel 
      Caption         =   "Demo Pri&ority:"
      Height          =   255
      Index           =   9
      Left            =   240
      TabIndex        =   4
      Top             =   1260
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "&Persist Image:"
      Height          =   255
      Index           =   8
      Left            =   240
      TabIndex        =   15
      Top             =   3840
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "&Author:"
      Height          =   255
      Index           =   7
      Left            =   240
      TabIndex        =   13
      Top             =   3300
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "Additional &Information:"
      Height          =   255
      Index           =   6
      Left            =   240
      TabIndex        =   17
      Top             =   4320
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "Teaser &Name:"
      Height          =   255
      Index           =   5
      Left            =   240
      TabIndex        =   11
      Top             =   2760
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "Teaser T&ype:"
      Height          =   255
      Index           =   4
      Left            =   240
      TabIndex        =   9
      Top             =   2160
      Width           =   975
   End
   Begin VB.Label lblLabel 
      Caption         =   "Name of &XBE:"
      Height          =   255
      Index           =   3
      Left            =   240
      TabIndex        =   7
      Top             =   1740
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "&Demo Type:"
      Height          =   255
      Index           =   1
      Left            =   240
      TabIndex        =   2
      Top             =   720
      Width           =   1815
   End
   Begin VB.Label lblLabel 
      Caption         =   "&Title:"
      Height          =   255
      Index           =   0
      Left            =   240
      TabIndex        =   0
      Top             =   240
      Width           =   1815
   End
End
Attribute VB_Name = "AppEntry"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Dim g_nOrigVal As Integer

Private Sub btnCancel_Click()
  tDIB.lSize = 0
  Unload Me
End Sub

Private Sub btnSave_Click()
  With tDIB
    .strTitle = Trim(txtTitle.Text)
    .strXBE = Trim(txtXBE.Text)
    .strTeaser = Trim(txtTeaser.Text)
    .strPersist = Trim(txtPersist.Text)
    .strAddi = Trim(txtAddi.Text)
    .strAuthor = Trim(txtAuthor.Text)
    .lPriority = Val(txtPriority.Text)
    
    If cboType.ListIndex = 0 Then
      .lFlags = 0   'Interactive Demo
    Else
      .lFlags = &H1&   'Non-interactive Movie
    End If
    
    If cboTeaser.ListIndex = 0 Then
      .lFlags = .lFlags   'Image Teaser
    Else
      .lFlags = .lFlags + &H100&   'Movie Teaser
    End If
    
    If chkKiosk.Value = 1 Then
      .lFlags = .lFlags + &H10000   'Run in Kiosk mode
    End If
    
    .lSize = 1
  End With
  
  If txtTitle.Text = "" Then
    MsgBox "You must enter a value for the Demo Title.", vbOKOnly
    txtTitle.SetFocus
  ElseIf txtXBE.Text = "" Then
    MsgBox "You must enter a value for the Demo XBE.", vbOKOnly
    txtXBE.SetFocus
  ElseIf txtTeaser.Text = "" Then
    MsgBox "You must enter a value for the Demo Teaser.", vbOKOnly
    txtTeaser.SetFocus
  ElseIf txtAuthor.Text = "" Then
    MsgBox "You must enter a value for the Demo Author.", vbOKOnly
    txtAuthor.SetFocus
  Else
    Unload Me
  End If
End Sub

Private Sub Form_Load()
  With tDIB
    If .lSize = 1 Then
      txtTitle.Text = .strTitle
      VScroll.Value = 100 - .lPriority
      txtPriority.Text = Trim(Str(100 - VScroll.Value))
      txtXBE.Text = .strXBE
      txtTeaser.Text = .strTeaser
      txtPersist.Text = .strPersist
      txtAddi.Text = .strAddi
      txtAuthor.Text = .strAuthor
      
      If .lFlags And &H1& Then
        cboType.ListIndex = 1
      Else
        cboType.ListIndex = 0
      End If
      
      If .lFlags And &H100& Then
        cboTeaser.ListIndex = 1
      Else
        cboTeaser.ListIndex = 0
      End If
      
      If .lFlags And &H10000 Then
        chkKiosk.Value = 1  'Run in Kiosk mode
      Else
        chkKiosk.Value = 0
      End If
    Else
      VScroll.Value = 99
      txtPriority.Text = Trim(Str(100 - VScroll.Value))
      cboType.ListIndex = 0
      cboTeaser.ListIndex = 0
    End If
  End With
  
  g_nOrigVal = 100 - VScroll.Value
End Sub

Private Sub txtPriority_Validate(Cancel As Boolean)
  Dim nVal As Integer
  nVal = Val(txtPriority.Text)
  
  If nVal < 1 Or nVal > 99 Then
    nVal = g_nOrigVal
  End If
  
  g_nOrigVal = nVal
  VScroll.Value = 100 - nVal
  txtPriority.Text = Trim(Str(100 - VScroll.Value))
End Sub
Private Sub txtPriority_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Sub txtAddi_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Sub txtAuthor_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Sub txtTeaser_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub
Private Sub txtPersist_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Sub txtTitle_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Sub txtXBE_KeyPress(KeyAscii As Integer)
  KeyAscii = CheckDlgKeys(KeyAscii)
End Sub

Private Function CheckDlgKeys(KeyAscii As Integer) As Integer
  Select Case KeyAscii
  Case 13   'ENTER
    CheckDlgKeys = 0
    btnSave.SetFocus
    SendKeys "{enter}"
  Case 27   'ESC
    CheckDlgKeys = 0
    btnCancel.SetFocus
    SendKeys "{enter}"
  Case Else
    CheckDlgKeys = KeyAscii
  End Select
End Function

Private Sub VScroll_Change()
  txtPriority.Text = Trim(Str(100 - VScroll.Value))
End Sub
