VERSION 5.00
Object = "{5E9E78A0-531B-11CF-91F6-C2863C385E30}#1.0#0"; "MSFLXGRD.OCX"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "COMDLG32.OCX"
Begin VB.Form AppMain 
   Caption         =   "XDI File Generator"
   ClientHeight    =   3165
   ClientLeft      =   60
   ClientTop       =   735
   ClientWidth     =   11775
   Icon            =   "AppMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   3165
   ScaleWidth      =   11775
   StartUpPosition =   2  'CenterScreen
   Begin MSComctlLib.ImageList imgList 
      Left            =   6600
      Top             =   0
      _ExtentX        =   1005
      _ExtentY        =   1005
      BackColor       =   -2147483643
      ImageWidth      =   16
      ImageHeight     =   16
      MaskColor       =   13555670
      _Version        =   393216
      BeginProperty Images {2C247F25-8591-11D1-B16A-00C0F0283628} 
         NumListImages   =   8
         BeginProperty ListImage1 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":08CA
            Key             =   "save"
         EndProperty
         BeginProperty ListImage2 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":0C1C
            Key             =   "del"
         EndProperty
         BeginProperty ListImage3 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":0F6E
            Key             =   "edit"
         EndProperty
         BeginProperty ListImage4 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":12C0
            Key             =   "new"
         EndProperty
         BeginProperty ListImage5 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":1612
            Key             =   "add"
         EndProperty
         BeginProperty ListImage6 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":1964
            Key             =   "open"
         EndProperty
         BeginProperty ListImage7 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":1CB6
            Key             =   "prop"
         EndProperty
         BeginProperty ListImage8 {2C247F27-8591-11D1-B16A-00C0F0283628} 
            Picture         =   "AppMain.frx":2208
            Key             =   "reset"
         EndProperty
      EndProperty
   End
   Begin MSComDlg.CommonDialog cDlg 
      Left            =   7740
      Top             =   0
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
      CancelError     =   -1  'True
      DefaultExt      =   "xdi"
      Filter          =   "XDI Demo File (*.xdi)|*.xdi|All Files (*.*)|*.*"
      Orientation     =   2
   End
   Begin MSComctlLib.StatusBar barStatus 
      Align           =   2  'Align Bottom
      Height          =   315
      Left            =   0
      TabIndex        =   0
      Top             =   2850
      Width           =   11775
      _ExtentX        =   20770
      _ExtentY        =   556
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   6
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Bevel           =   0
            Object.Width           =   12347
            MinWidth        =   12347
            Text            =   "Ready"
            TextSave        =   "Ready"
         EndProperty
         BeginProperty Panel2 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   1
            Alignment       =   1
            Enabled         =   0   'False
            Object.Width           =   1058
            MinWidth        =   1058
            TextSave        =   "CAPS"
         EndProperty
         BeginProperty Panel3 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   2
            Alignment       =   1
            Object.Width           =   1058
            MinWidth        =   1058
            TextSave        =   "NUM"
         EndProperty
         BeginProperty Panel4 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   3
            Alignment       =   1
            Enabled         =   0   'False
            Object.Width           =   1058
            MinWidth        =   1058
            TextSave        =   "INS"
         EndProperty
         BeginProperty Panel5 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   6
            Alignment       =   1
            TextSave        =   "8/6/2001"
         EndProperty
         BeginProperty Panel6 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            Style           =   5
            Alignment       =   1
            TextSave        =   "1:03 PM"
         EndProperty
      EndProperty
   End
   Begin MSComctlLib.Toolbar barTool 
      Align           =   1  'Align Top
      Height          =   420
      Left            =   0
      TabIndex        =   2
      Top             =   0
      Width           =   11775
      _ExtentX        =   20770
      _ExtentY        =   741
      ButtonWidth     =   609
      ButtonHeight    =   582
      AllowCustomize  =   0   'False
      Appearance      =   1
      ImageList       =   "imgList"
      _Version        =   393216
      BeginProperty Buttons {66833FE8-8583-11D1-B16A-00C0F0283628} 
         NumButtons      =   10
         BeginProperty Button1 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "new"
            Description     =   "New XDI File"
            Object.ToolTipText     =   "New XDI File"
            ImageKey        =   "new"
         EndProperty
         BeginProperty Button2 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "open"
            Description     =   "Open XDI File"
            Object.ToolTipText     =   "Open XDI File"
            ImageKey        =   "open"
         EndProperty
         BeginProperty Button3 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "save"
            Description     =   "Save XDI File"
            Object.ToolTipText     =   "Save XDI File"
            ImageKey        =   "save"
         EndProperty
         BeginProperty Button4 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button5 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "add"
            Description     =   "Add Item"
            Object.ToolTipText     =   "Add Item"
            ImageKey        =   "add"
         EndProperty
         BeginProperty Button6 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "edit"
            Description     =   "Edit Item"
            Object.ToolTipText     =   "Edit Item"
            ImageKey        =   "edit"
         EndProperty
         BeginProperty Button7 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "del"
            Description     =   "Delete Item"
            Object.ToolTipText     =   "Delete Item"
            ImageKey        =   "del"
         EndProperty
         BeginProperty Button8 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Style           =   3
         EndProperty
         BeginProperty Button9 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "reset"
            Object.ToolTipText     =   "Reset Column Widths"
            ImageKey        =   "reset"
         EndProperty
         BeginProperty Button10 {66833FEA-8583-11D1-B16A-00C0F0283628} 
            Key             =   "prop"
            Object.ToolTipText     =   "File Properties"
            ImageKey        =   "prop"
         EndProperty
      EndProperty
   End
   Begin MSFlexGridLib.MSFlexGrid XDIGrid 
      CausesValidation=   0   'False
      Height          =   2295
      Left            =   -60
      TabIndex        =   1
      Tag             =   "1"
      Top             =   480
      Width           =   6450
      _ExtentX        =   11377
      _ExtentY        =   4048
      _Version        =   393216
      Rows            =   55
      Cols            =   11
      RowHeightMin    =   32
      AllowBigSelection=   -1  'True
      ScrollTrack     =   -1  'True
      TextStyleFixed  =   3
      FocusRect       =   2
      SelectionMode   =   1
      AllowUserResizing=   1
      FormatString    =   "|<Title|<Priority|<Type|<XBE|<Teaser Type|<Teaser|<Kiosk?|<Persist Image|<Author|<Additional Information"
   End
   Begin VB.Menu MFile 
      Caption         =   "&File"
      Index           =   0
      Begin VB.Menu MNew 
         Caption         =   "&New..."
      End
      Begin VB.Menu MOpen 
         Caption         =   "&Open..."
      End
      Begin VB.Menu MClose 
         Caption         =   "&Close"
      End
      Begin VB.Menu MSep1 
         Caption         =   "-"
      End
      Begin VB.Menu MProp 
         Caption         =   "&Properties..."
      End
      Begin VB.Menu MSave 
         Caption         =   "&Save"
      End
      Begin VB.Menu MSaveAs 
         Caption         =   "Save &as..."
      End
      Begin VB.Menu MSep2 
         Caption         =   "-"
      End
      Begin VB.Menu MExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu MView 
      Caption         =   "&View"
      Index           =   2
      Begin VB.Menu MToolbar 
         Caption         =   "Toolbar"
         Checked         =   -1  'True
      End
      Begin VB.Menu MStatusBar 
         Caption         =   "Status bar"
         Checked         =   -1  'True
      End
      Begin VB.Menu MReset 
         Caption         =   "Reset Column Widths"
      End
   End
   Begin VB.Menu MHelp 
      Caption         =   "&Help"
      Index           =   4
      Begin VB.Menu MAbout 
         Caption         =   "About XDIGen..."
      End
   End
End
Attribute VB_Name = "AppMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim g_GridKey As Integer

Private Sub barTool_ButtonClick(ByVal Button As MSComctlLib.Button)
  Select Case Button.Key
  Case "open"
    OpenXDIFile
  Case "save"
    SaveThisFile
  Case "new"
    NewXDIFile
  Case "add"
    AddEditGrid True
    AppEntry.Show 1, Me
  Case "edit"
    If XDIGrid.TextMatrix(Row, 1) <> "" Then
      AddEditGrid False
      AppEntry.Show 1, Me
    End If
  Case "del"
    DeleteGridItem
  Case "reset"
    DefaultColumnWidths
  Case "prop"
    frmProp.Show 1, Me
  Case Else
    Debug.Print Button.Index
  End Select
End Sub

Private Sub Form_Activate()
  Dim nIdx As Integer
  
  If g_fPromptCallingXBE Then
    frmProp.Show 1, AppMain
    g_Grid.SetFocus
    
    g_fPromptCallingXBE = False
  End If
  
  If tDIB.lSize = 1 Then
    With XDIGrid
      .Row = .Tag
      .Col = 1
      .ColSel = 1
      
      .TextMatrix(.Row, nTITLE) = tDIB.strTitle
      .TextMatrix(.Row, nPRIORITY) = Trim(Str(tDIB.lPriority))
      .TextMatrix(.Row, nXBE) = tDIB.strXBE
      .TextMatrix(.Row, nTEASER) = tDIB.strTeaser
      .TextMatrix(.Row, nPERSIST) = tDIB.strPersist
      .TextMatrix(.Row, nAUTHOR) = tDIB.strAuthor
      .TextMatrix(.Row, nADDI) = tDIB.strAddi
      
      If tDIB.lFlags And &H1& Then
        .TextMatrix(.Row, nTYPE) = "Non-Interactive Movie"
      Else
        .TextMatrix(.Row, nTYPE) = "Interactive Demo"
      End If
      
      If tDIB.lFlags And &H100& Then
        .TextMatrix(.Row, nTEASERTYPE) = "Movie Teaser"
      Else
        .TextMatrix(.Row, nTEASERTYPE) = "Image Teaser"
      End If
      
      If tDIB.lFlags And &H10000 Then
        .TextMatrix(.Row, nKIOSK) = "Y"
      Else
        .TextMatrix(.Row, nKIOSK) = "N"
      End If
      
      If .RowIsVisible(.Row) = False Then
        .TopRow = .Row
      End If
            
      Refresh
    
      'Count Number of Demos we have
      tHeader.lNumDemos = 0
      For nIdx = 1 To .Rows - 1
        If .TextMatrix(nIdx, nTITLE) <> "" Then
          tHeader.lNumDemos = tHeader.lNumDemos + 1
        End If
      Next
    End With
    
    g_fDirty = True
    SetUIState
  End If
End Sub

Private Sub Form_Load()
  Dim strUIOptions As String
  Dim nIdx As Integer
  
  Set g_Grid = XDIGrid
  Set g_Dlg = cDlg
  
  'Read settings from registry
  vbReadRegString "LastXDIFile", g_strXDIFile
  vbReadRegString "UIOptions", strUIOptions
  
  g_fNamedFile = True
  g_fToolbar = True
  g_fStatusbar = True
  g_fResizing = False
  g_fOnLoad = True
  g_fPromptCallingXBE = False
  
  If Left$(strUIOptions, 1) <> "Y" Then
    g_fToolbar = False
  End If
     
  If Len(strUIOptions) > 1 Then
    If Mid$(strUIOptions, 2, 1) <> "Y" Then
      g_fStatusbar = False
    End If
  End If
    
  barTool.Visible = g_fToolbar
  barStatus.Visible = g_fStatusbar
  MToolbar.Checked = g_fToolbar
  MStatusBar.Checked = g_fStatusbar
  
  tHeader.lNumDemos = 0
  tHeader.lKiosk = 0
  
  If DoesFileExist(g_strXDIFile) Then
    GetGridFromXDIFile g_strXDIFile
  Else
      ClearGridData
      SetUIState
      g_fPromptCallingXBE = True
  End If
  
  'Flag that no Grid data has just changed
  tDIB.lSize = 0
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
  If False = CheckSave() Then
    Cancel = 1
  End If
End Sub

Private Sub Form_Resize()
  ResizeGrid
End Sub

Private Sub Form_Unload(Cancel As Integer)
  Dim strUIOptions As String
  
  If g_fToolbar Then
    strUIOptions = "Y"
  Else
    strUIOptions = "N"
  End If
  
  If g_fStatusbar Then
    strUIOptions = strUIOptions + "Y"
  Else
    strUIOptions = strUIOptions + "N"
  End If
  
  'Write settings to registry
  vbWriteRegString "LastXDIFile", g_strXDIFile
  vbWriteRegString "UIOptions", strUIOptions
  WritePosInfo
  
End Sub

Private Sub MAbout_Click()
  frmAbout.Show 1, Me
End Sub

Private Sub MClose_Click()
  If CheckSave() Then
    ClearGridData
    
    g_Grid.Visible = False
    g_Grid.Tag = 0
    g_fNamedFile = False
    
    SetUIState
  End If
End Sub
Private Sub MExit_Click()
  If CheckSave() Then
    g_fDirty = False  'Force dirty to False so Query_Unload doesn't ask again
    Unload Me
  End If
End Sub

Private Sub MNew_Click()
  NewXDIFile
End Sub

Private Sub MOpen_Click()
  OpenXDIFile
End Sub

Private Sub MProp_Click()
  frmProp.Show 1, Me
End Sub

Private Sub MReset_Click()
  DefaultColumnWidths
End Sub

Private Sub MSave_Click()
  SaveThisFile
End Sub

Private Sub MSaveAs_Click()
  On Error GoTo MSaveAsErr

  cDlg.InitDir = g_strXDIFile
  cDlg.DialogTitle = "Save As..."
  cDlg.Flags = cdlOFNHideReadOnly Or cdlOFNLongNames
  cDlg.ShowSave
  
  SaveXDIFile cDlg.FileName, True
    
MSaveAsErr:
End Sub

Private Sub MStatusBar_Click()
  If g_fStatusbar Then
    g_fStatusbar = False
  Else
    g_fStatusbar = True
  End If
  
  barStatus.Visible = g_fStatusbar
  MStatusBar.Checked = g_fStatusbar
  
  ResizeGrid
End Sub

Private Sub MToolbar_Click()
  If g_fToolbar Then
    g_fToolbar = False
  Else
    g_fToolbar = True
  End If
  
  barTool.Visible = g_fToolbar
  MToolbar.Checked = g_fToolbar
  
  ResizeGrid
End Sub

Private Sub XDIGrid_DblClick()
  AddEditGrid False
  AppEntry.Show 1, Me
End Sub

Private Sub XDIGrid_KeyDown(KeyCode As Integer, Shift As Integer)
  g_GridKey = KeyCode
End Sub

Private Sub XDIGrid_KeyUp(KeyCode As Integer, Shift As Integer)
  Dim nIdx As Integer
  
  If g_GridKey = KeyCode Then
    g_GridKey = 0
    
    Select Case KeyCode
    Case 46
      DeleteGridItem
    Case 13
      AddEditGrid False
      AppEntry.Show 1, Me
    End Select
  End If
End Sub
