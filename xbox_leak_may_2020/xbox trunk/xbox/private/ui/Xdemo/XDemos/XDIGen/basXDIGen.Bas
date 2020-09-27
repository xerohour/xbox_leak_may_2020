Attribute VB_Name = "basXDIGen"
Option Explicit
Option Compare Text

Type DIB
    lSize As Long
    lFlags As Long
    lPriority As Long
    strTitle As String
    strFolder As String
    strXBE As String
    strTeaser As String
    strPersist As String
    strAddi As String
    strAuthor As String
    strDemoType As String
End Type

Type XDIHEADER
    bSignature(1 To 4) As Byte
    lVerMajor As Long
    lVerMinor As Long
    lNumDemos As Long
    lCheckSum As Long
    lKiosk As Long
    strCallingXBE As String * 128
    strPersist As String * 128
End Type

Declare Function XDIGetDllversion Lib "XDIDll" Alias "_XDIGetDllVersion@0" () As Long
Declare Function XDIGetDemoHeader Lib "XDIDll" Alias "_XDIGetDemoHeader@8" _
  (ByVal strXDIFile As String, pxdi As XDIHEADER) As Long
Declare Function XDIGetDemoInfo Lib "XDIDll" Alias "_XDIGetDemoInfo@12" _
  (ByVal strXDIFile As String, pdib As DIB, ByVal lIdx As Long) As Long
Declare Function XDIWriteHeader Lib "XDIDll" Alias "_XDIWriteHeader@20" _
  (ByVal strXDIFile As String, ByVal lNumDemos As Long, ByVal lKiosk As Long, _
   ByVal strCallingXBE As String, ByVal strPersist As String) As Long
Declare Function XDIWriteDemoInfo Lib "XDIDll" Alias "_XDIWriteDemoInfo@8" _
  (ByVal strXDIFile As String, pdib As DIB) As Long

Public Const strHEADERS As String = "|<Title|<Priority|<Type|<XBE|<Teaser Type|<Teaser|<Kiosk?|<Persist Image|<Author|<Additional Information"
Public Const strDEFAULTXBE As String = "d:\default.xbe"
Public Const nTITLE As Integer = 1
Public Const nPRIORITY As Integer = 2
Public Const nTYPE As Integer = 3
Public Const nXBE As Integer = 4
Public Const nTEASERTYPE As Integer = 5
Public Const nTEASER As Integer = 6
Public Const nKIOSK As Integer = 7
Public Const nPERSIST As Integer = 8
Public Const nAUTHOR As Integer = 9
Public Const nADDI As Integer = 10
Public Const nLASTCOL As Integer = 10
Public Const nMINROWS As Integer = 60

Public g_rgdCol(nLASTCOL + 1) As Double
Public g_Grid As Object
Public g_Dlg As Object
Public tDIB As DIB
Public tHeader As XDIHEADER
Public g_fDirty As Boolean
Public g_strXDIFile As String
Public g_fNamedFile As Boolean
Public g_fToolbar As Boolean
Public g_fStatusbar As Boolean
Public g_fResizing As Boolean
Public g_fOnLoad As Boolean
Public g_fPromptCallingXBE As Boolean

Public Function GetGridFromXDIFile(ByVal strXDIFile As String) As Boolean
  Dim nIdx As Integer
  Dim fRc As Boolean
  Dim lNumDemos As Long
  
  ClearGridData
  lNumDemos = 0
  
  fRc = XDIGetDemoHeader(strXDIFile, tHeader)
  If fRc Then
    fRc = CheckFileVersion(strXDIFile)
  End If
    
  If fRc Then
    tHeader.strCallingXBE = FixupString(tHeader.strCallingXBE)
    tHeader.strPersist = FixupString(tHeader.strPersist)
    
    For nIdx = 0 To tHeader.lNumDemos - 1
      tDIB.strAddi = Space(255)
      tDIB.strAuthor = Space(255)
      tDIB.strTeaser = Space(255)
      tDIB.strTitle = Space(255)
      tDIB.strXBE = Space(255)
      tDIB.strFolder = Space(255)
      
      fRc = XDIGetDemoInfo(strXDIFile, tDIB, nIdx + 1)
      If fRc Then
        With g_Grid
          'Need to make sure there are enough rows in the Grid first
          If .Rows = nIdx + 1 Then
            .Rows = .Rows + 1
          End If
          
          .Row = nIdx + 1
          
          .TextMatrix(.Row, 0) = Trim(Str(.Row))
          .TextMatrix(.Row, nTITLE) = Trim(tDIB.strTitle)
          .TextMatrix(.Row, nPRIORITY) = Trim(Str(tDIB.lPriority))
          .TextMatrix(.Row, nXBE) = Trim(tDIB.strXBE)
          .TextMatrix(.Row, nTEASER) = Trim(tDIB.strTeaser)
          .TextMatrix(.Row, nPERSIST) = Trim(tDIB.strPersist)
          .TextMatrix(.Row, nAUTHOR) = Trim(tDIB.strAuthor)
          .TextMatrix(.Row, nADDI) = Trim(tDIB.strAddi)
          
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
            .TextMatrix(.Row, nKIOSK) = "Y"   'Run in kiosk mode
          Else
            .TextMatrix(.Row, nKIOSK) = "N"
          End If
        End With
        
        g_fNamedFile = True
        lNumDemos = lNumDemos + 1
      Else
        Exit For
      End If
    Next
    
    g_fNamedFile = True
    g_strXDIFile = strXDIFile
    tHeader.lNumDemos = lNumDemos
  End If
  
  If False = fRc Then
    MsgBox "Error reading XDI File." + Chr(13) + Chr(13) + strXDIFile, vbOKOnly, "XDI Read Error"
    
    g_fNamedFile = False
    tHeader.lNumDemos = False
    tHeader.lKiosk = 0
    tHeader.strCallingXBE = ""
    tHeader.strPersist = ""
  End If
    
  SetUIState
  GetGridFromXDIFile = g_fNamedFile
End Function

Public Sub SaveXDIFile(ByVal strXDIFile As String, ByVal fSaveAs As Boolean)
  Dim lRc As Long
  Dim strCallingXBE As String
  Dim strPersist As String
  
  strCallingXBE = Trim(tHeader.strCallingXBE)
  strPersist = Trim(tHeader.strPersist)
  
  lRc = XDIWriteHeader(strXDIFile, tHeader.lNumDemos, tHeader.lKiosk, strCallingXBE, strPersist)
   
  If lRc Then
     g_strXDIFile = strXDIFile
     g_fNamedFile = True
     
     lRc = WriteGridToXDI(strXDIFile, tHeader.lNumDemos)
  End If
  
  If False = lRc Then
    MsgBox ("Error saving XDI File.")
  End If
End Sub

Public Function CheckSave() As Boolean
  Dim nRC As Integer
  
  nRC = vbOK
  
  On Error GoTo CSErr
  
  If g_fDirty Then
    nRC = MsgBox("The current demo information has changed." + _
          Chr(13) + Chr(13) + "Do you want to save the changes?", _
           vbYesNoCancel, "XDIGen")
           
    If vbYes = nRC Then
      nRC = vbCancel
      
      If False = g_fNamedFile Or False = DoesFileExist(g_strXDIFile) Then
        g_Dlg.InitDir = g_strXDIFile
        g_Dlg.DialogTitle = "Save XDI Demo File"
        g_Dlg.Flags = cdlOFNHideReadOnly Or cdlOFNLongNames
        g_Dlg.ShowSave
      
        g_strXDIFile = g_Dlg.FileName
      End If
    
      SaveXDIFile g_strXDIFile, False
      nRC = vbYes
    End If
  End If
    
CSErr:
  If vbCancel = nRC Then
    CheckSave = False
  Else
    CheckSave = True
  End If
End Function

Public Function WriteGridToXDI(strXDIFile As String, lNumDemos As Long) As Boolean
  Dim nIdx As Integer
  Dim fOK As Boolean
  
  fOK = True
  
  With g_Grid
    For nIdx = 1 To lNumDemos
      tDIB.strTitle = .TextMatrix(nIdx, nTITLE)
      tDIB.lPriority = Val(.TextMatrix(nIdx, nPRIORITY))
      tDIB.strXBE = .TextMatrix(nIdx, nXBE)
      tDIB.strTeaser = .TextMatrix(nIdx, nTEASER)
      tDIB.strPersist = .TextMatrix(nIdx, nPERSIST)
      tDIB.strAuthor = .TextMatrix(nIdx, nAUTHOR)
      tDIB.strAddi = .TextMatrix(nIdx, nADDI)
      tDIB.strFolder = ""
      
      If Left$(.TextMatrix(nIdx, nTYPE), 1) = "I" Then
        tDIB.lFlags = 0     'Interactive demo
      Else
        tDIB.lFlags = &H1&     'Non-interactive Movie
      End If
      
      If Left$(.TextMatrix(nIdx, nTEASERTYPE), 1) = "I" Then
        tDIB.lFlags = tDIB.lFlags   'Image teaser
      Else
        tDIB.lFlags = tDIB.lFlags + &H100&   'Movie teaser
      End If
      
      If .TextMatrix(nIdx, nKIOSK) = "Y" Then
        tDIB.lFlags = tDIB.lFlags + &H10000   'Run in Kiosk mode
      End If
      
      If False = XDIWriteDemoInfo(strXDIFile, tDIB) Then
        MsgBox "Error writing XDI file.", vbOKOnly, "XDI Write Error"
        fOK = False
        Exit For
      End If
    Next
  End With
    
  If fOK Then
    g_fDirty = False
  End If
      
  SetUIState
  WriteGridToXDI = fOK
End Function
Public Function GetFileNameOnly(strFilename As String) As String
  Dim nIdx As Integer
  
  nIdx = Len(strFilename)
  While (nIdx And (Mid(strFilename, nIdx, 1) <> "\"))
    nIdx = nIdx - 1
  Wend
  
  If (nIdx > 1) Then
    GetFileNameOnly = Mid(strFilename, nIdx + 1)
  Else
    GetFileNameOnly = strFilename
  End If
  
End Function

Public Sub GetPathOnly(strFilename As String)
  Dim nIdx As Integer
  
  nIdx = Len(strFilename)
  While (nIdx And (Mid(strFilename, nIdx, 1) <> "\"))
    nIdx = nIdx - 1
  Wend
  
  If (nIdx > 1) Then
    strFilename = Mid(strFilename, 1, nIdx - 1)
  End If
  
End Sub

Public Function DoesFileExist(strFilename As String) As Boolean
    If strFilename = "" Or Dir(strFilename) = "" Then
        DoesFileExist = False
    Else
        DoesFileExist = True
    End If
End Function

Public Sub ClearGridData()
  Dim nIdx As Integer
  
  SaveColumnWidths
  
  With g_Grid
    .Clear
    .Rows = nMINROWS
    .Cols = nLASTCOL + 1
    .Row = 1
    .FormatString = strHEADERS
    
    For nIdx = 1 To .Rows - 1
      .TextMatrix(nIdx, 0) = nIdx
    Next
    
    .Refresh
  End With
  
  tHeader.strCallingXBE = ""
  tHeader.strPersist = ""
  tHeader.lNumDemos = 0
  tHeader.lKiosk = 0
  tDIB.lSize = 0
  g_fDirty = False
  g_fNamedFile = False
End Sub

Public Sub AddEditGrid(fAddOnly As Boolean)
  tDIB.lSize = 0
  
  With g_Grid
    If False = fAddOnly And .TextMatrix(.Row, nTITLE) <> "" Then
      tDIB.strTitle = .TextMatrix(.Row, nTITLE)
      tDIB.lPriority = Val(.TextMatrix(.Row, nPRIORITY))
      tDIB.strXBE = .TextMatrix(.Row, nXBE)
      tDIB.strTeaser = .TextMatrix(.Row, nTEASER)
      tDIB.strPersist = .TextMatrix(.Row, nPERSIST)
      tDIB.strAuthor = .TextMatrix(.Row, nAUTHOR)
      tDIB.strAddi = .TextMatrix(.Row, nADDI)
      
      If Left$(.TextMatrix(.Row, nTYPE), 1) = "I" Then
        tDIB.lFlags = 0     'Interactive demo
      Else
        tDIB.lFlags = &H1&     'Non-interactive Movie
      End If
      
      If Left$(.TextMatrix(.Row, nTEASERTYPE), 1) = "I" Then
        tDIB.lFlags = tDIB.lFlags   'Image teaser
      Else
        tDIB.lFlags = tDIB.lFlags + &H100&   'Movie teaser
      End If
      
      If .TextMatrix(.Row, nKIOSK) = "Y" Then
        tDIB.lFlags = tDIB.lFlags + &H10000     'Run in kiosk mode
      End If
      
      tDIB.lSize = 1
    Else
      'Find first empty row
      Dim nIdx As Integer
      For nIdx = 1 To .Rows - 1
        If .TextMatrix(nIdx, nTITLE) = "" Then
          .Row = nIdx
          Exit For
        End If
      Next
      
      If nIdx = .Rows Then
        g_Grid.AddItem (Str(nIdx))
        .Row = .Rows - 1
      End If
    End If
  
    'Save row number so we have it upon return
    .Tag = .Row
  End With
End Sub

Public Sub OpenXDIFile()
  On Error GoTo OXDIFErr

  If CheckSave() Then
    g_Dlg.InitDir = g_strXDIFile
    g_Dlg.DialogTitle = "Open XDI Demo File"
    g_Dlg.Flags = cdlOFNFileMustExist Or cdlOFNHideReadOnly Or cdlOFNLongNames
    g_Dlg.ShowOpen
  
    If GetGridFromXDIFile(g_Dlg.FileName) Then
      ResizeGrid
      
      g_Grid.Visible = True
      g_Grid.Tag = 1
      g_Grid.SetFocus
      
      SetUIState
    End If
  End If
  
OXDIFErr:
End Sub

Public Sub NewXDIFile()
  If CheckSave() Then
    ClearGridData
    
    g_fNamedFile = False
    g_Grid.Visible = True
    g_Grid.Tag = 1
    
    ResizeGrid
    SetUIState
    frmProp.Show 1, AppMain
    
    g_Grid.SetFocus
  End If
End Sub

Public Sub SaveThisFile()
  On Error GoTo STFErr
  
  If 0 = tHeader.lKiosk And Trim(tHeader.strCallingXBE) = "" Then
    frmProp.Show 1, AppMain
    If Trim(tHeader.strCallingXBE) = "" Then
      Exit Sub
    End If
  End If
  
  If False = g_fNamedFile Or False = DoesFileExist(g_strXDIFile) Then
    g_Dlg.InitDir = g_strXDIFile
    g_Dlg.DialogTitle = "Save XDI Demo File"
    g_Dlg.Flags = cdlOFNHideReadOnly Or cdlOFNLongNames
    g_Dlg.ShowSave
    
    SaveXDIFile g_Dlg.FileName, False
  Else
    SaveXDIFile g_strXDIFile, False
  End If
  
STFErr:
End Sub

Public Sub DeleteGridItem()
  Dim nIdx As Integer
  Dim nRC As Integer
  Dim nFirst As Integer
  Dim nLast As Integer

  With g_Grid
    nRC = vbNo
    
    If .Row = .RowSel Then
      If .TextMatrix(.Row, nTITLE) <> "" Then
        nRC = MsgBox(Trim(.TextMatrix(.Row, nTITLE)) + String(2, 13) + _
                   "Are you sure you want to delete this demo?", _
                   vbYesNo, "Confirm Delete")
      End If
    Else
      If .Row > .RowSel Then
        nIdx = .Row
        .Row = .RowSel
        .RowSel = nIdx
      End If
      
      nFirst = 0
      nLast = 0
      
      For nIdx = .Row To .RowSel
        If .TextMatrix(nIdx, nTITLE) <> "" Then
          .TextMatrix(nIdx, 0) = "*" + Trim(Str(nIdx))
          
          If nFirst = 0 Then
            nFirst = nIdx
          Else
            nLast = nIdx
          End If
        End If
      Next
      
      If nFirst Then
        nRC = MsgBox("Are you sure you want to delete the selected demos (marked with a '*') ?", _
                     vbYesNo, "Confirm Delete")
                     
        If vbNo = nRC Then
          For nIdx = nFirst To nLast
            .TextMatrix(nIdx, 0) = nIdx
          Next
        End If
      End If
    End If
    
    If vbYes = nRC Then
      nFirst = .Row
      nLast = .RowSel
      
      For nIdx = nLast To nFirst Step -1
        If .TextMatrix(nIdx, nTITLE) <> "" Then
          .RemoveItem (nIdx)
          tHeader.lNumDemos = tHeader.lNumDemos - 1
        End If
      Next
      
      If .Rows < nMINROWS Then
        .Rows = nMINROWS
      End If
      
      For nIdx = 1 To .Rows - 1
        .TextMatrix(nIdx, 0) = nIdx
      Next
      
      g_fDirty = True
      SetUIState
      
      .Refresh
    End If
  End With
End Sub

Public Sub ResizeGrid()
  Dim nIdx As Integer
  Dim nVal As Integer
  Dim pRect As RECT
  Dim fLoadedPosInfo As Boolean
  
  If g_fResizing Or 1 = AppMain.WindowState Then
    Exit Sub
  End If
  
  g_fResizing = True
  
  If g_fOnLoad Then
    fLoadedPosInfo = GetPosInfo
  Else
    fLoadedPosInfo = False
  End If
  
  If AppMain.Width < 5400 Then
    AppMain.Width = 5400
  End If
  
  If AppMain.Height <= 2200 Then
    AppMain.Height = 2200
    
    If False = g_fToolbar Then
      AppMain.Height = AppMain.Height - AppMain.barTool.Height
    End If
    
    If False = g_fStatusbar Then
      AppMain.Height = AppMain.Height - AppMain.barStatus.Height
    End If
  End If
    
  GetClientRect AppMain.hWnd, pRect

  With g_Grid
    .Left = 15
    
    .Width = (pRect.Right - pRect.Left) * Screen.TwipsPerPixelX
    .Height = (pRect.Bottom - pRect.Top) * Screen.TwipsPerPixelY
    
    If g_fToolbar Then
      .Top = AppMain.barTool.Height + 60
      .Height = .Height - .Top
    Else
      .Top = 0
    End If
    
    If g_fStatusbar Then
      .Height = .Height - AppMain.barStatus.Height
    End If
     
    .ColWidth(0) = 300
    
    If Not fLoadedPosInfo And g_fOnLoad Then
      DefaultColumnWidths
    Else
      For nIdx = 1 To nLASTCOL
        .ColWidth(nIdx) = .Width * g_rgdCol(nIdx)
      Next
      
      nVal = 0
      For nIdx = 0 To nLASTCOL
        nVal = nVal + .ColWidth(nIdx)
      Next
      
      If .Width > nVal Then
        .Width = nVal
      End If
    End If
    
    .Refresh
  End With
  
  With AppMain.barStatus
    nVal = 0
    For nIdx = 2 To 6
      nVal = nVal + .Panels(nIdx).MinWidth
    Next
    nVal = nVal + 30
    
    If AppMain.Width - nVal > 0 Then
      .Panels(1).Width = AppMain.Width - nVal
    End If
  End With
  
  g_fResizing = False
  g_fOnLoad = False
End Sub

Public Sub SetUIState()
  Dim strCaption As String
  Dim strStatus
  
  strCaption = "XDI File Generator - "
  
  If g_Grid.Tag Then
    If Trim(tHeader.strCallingXBE) <> "" Then
      strCaption = strCaption + Trim(tHeader.strCallingXBE)
    Else
      strCaption = strCaption + "No Calling XBE"
    End If
    
    If g_fDirty Then
      strCaption = strCaption + " *"
      strStatus = strStatus + "[unsaved] "
    Else
      strStatus = strStatus + "[Ready] "
    End If
    
    strStatus = strStatus + IIf(tHeader.lKiosk, "Kiosk Mode", "Cross-sell Mode ")
    
    If g_fNamedFile Then
      strStatus = strStatus + " - " + g_strXDIFile
    End If
  Else
    strStatus = "[Click New or Open to begin]"
    strCaption = strCaption + "no file"
  End If
  
  AppMain.barStatus.Panels(1).Text = strStatus
  
  AppMain.Caption = strCaption
  AppMain.Refresh
  
  AppMain.MSave.Enabled = g_fDirty
  AppMain.MSaveAs.Enabled = g_fDirty Or tHeader.lNumDemos > 0
  AppMain.MClose.Enabled = g_Grid.Tag
  AppMain.MProp.Enabled = g_Grid.Tag
  AppMain.MReset.Enabled = g_Grid.Tag
  AppMain.barTool.Buttons(3).Enabled = g_fDirty
  AppMain.barTool.Buttons(10).Enabled = g_Grid.Tag
  AppMain.barTool.Buttons(9).Enabled = g_Grid.Tag
End Sub

Public Function GetPosInfo() As Boolean
  Dim strPosInfo As String
  Dim nStart As Integer
  Dim nEnd As Integer
  Dim nField As Integer
  Dim lVal As Long
  
  GetPosInfo = False
  vbReadRegString "XDIGenPos", strPosInfo

  nEnd = InStr(strPosInfo, "^")
  
  'If no checksum marker, bail
  If nEnd = 0 Then
    Exit Function
  End If
  
  For nStart = 1 To nEnd - 1
    lVal = lVal + Asc(Mid(strPosInfo, nStart, 1)) + nStart
  Next
  
  'If checksum doesn't match, bail
  If lVal <> Val(Mid(strPosInfo, nEnd + 1)) Then
    Exit Function
  End If
  
  If strPosInfo <> "" Then
    nEnd = 0
    nField = 1
    
    Do While nField < nLASTCOL + 3
      nStart = nEnd + 1
      nEnd = InStr(nStart, strPosInfo, "|")
      lVal = Val(Mid(strPosInfo, nStart, nEnd - nStart))
  
      Select Case nField
      Case 1
        AppMain.Top = lVal
      Case 2
        AppMain.Left = lVal
      Case 3
        AppMain.Width = lVal
      Case 4
        AppMain.Height = lVal
      Case 5 To nLASTCOL + 2
        If lVal Then
          g_rgdCol(nField - 4) = lVal / 100000000
        Else
          Exit Function   'return false for GetPosInfo
        End If
      End Select
      
      nField = nField + 1
    Loop
  End If
  
  GetPosInfo = True
End Function

Public Sub WritePosInfo()
  Dim strPosInfo As String
  Dim nField As Integer
  Dim nIdx As Integer
  Dim lVal As Long
  
  SaveColumnWidths
  
  strPosInfo = ""
  nField = 1
  
  While nField < nLASTCOL + 3
    Select Case nField
    Case 1
      lVal = AppMain.Top
    Case 2
      lVal = AppMain.Left
    Case 3
      lVal = AppMain.Width
    Case 4
      lVal = AppMain.Height
    Case 5 To nLASTCOL + 2
      lVal = g_rgdCol(nField - 4) * 100000000
    End Select
    
    strPosInfo = strPosInfo + Str(lVal) + "|"
    nField = nField + 1
  Wend
  
  lVal = 0
  For nIdx = 1 To Len(strPosInfo)
    lVal = lVal + Asc(Mid(strPosInfo, nIdx, 1)) + nIdx
  Next
  strPosInfo = strPosInfo + "^" + Str(lVal)
  
  vbWriteRegString "XDIGenPos", strPosInfo
End Sub

Public Sub SaveColumnWidths()
  Dim nVal As Integer
  Dim nIdx As Integer
  
  With g_Grid
    'Total current widths of all columns
    nVal = 0
    For nIdx = 1 To nLASTCOL
      nVal = nVal + .ColWidth(nIdx)
    Next
    
    'Record percentage of total width for each column, we'll apply this when resizing
    For nIdx = 1 To nLASTCOL
      g_rgdCol(nIdx) = .ColWidth(nIdx) / nVal
    Next
  End With
End Sub

Public Sub DefaultColumnWidths()
  Dim nVal As Integer
  Dim nIdx As Integer
  
  With g_Grid
    .ColWidth(nTITLE) = 0.16 * .Width
    .ColWidth(nPRIORITY) = 0.08 * .Width
    .ColWidth(nTYPE) = 0.09 * .Width
    .ColWidth(nXBE) = 0.1 * .Width
    .ColWidth(nTEASERTYPE) = 0.09 * .Width
    .ColWidth(nTEASER) = 0.1 * .Width
    .ColWidth(nKIOSK) = 0.04 * .Width
    .ColWidth(nPERSIST) = 0.09 * .Width
    .ColWidth(nAUTHOR) = 0.09 * .Width
    .ColWidth(nADDI) = 0.15 * .Width
  
    nVal = 0
    For nIdx = 0 To nLASTCOL
      nVal = nVal + .ColWidth(nIdx)
    Next
    
    If .Width > nVal + 105 Then
      .Width = nVal
    End If
  End With
  
  SaveColumnWidths
End Sub

Public Function FixupString(ByVal strString As String) As String
  Dim nPos As Integer
  nPos = InStr(strString, Chr(0))
  
  If nPos Then
    FixupString = Left(strString, nPos - 1)
  Else
    FixupString = strString
  End If
End Function

Public Function CheckFileVersion(ByVal strFilename As String) As Boolean
  Dim lVersion As Long
  Dim strError As String
  
  lVersion = XDIGetDllversion

  If tHeader.lVerMajor * &H10000 + tHeader.lVerMinor <> lVersion Then
    strError = "Cannot load file, incompatible version." + Chr(13) + Chr(13) + _
               GetFileNameOnly(strFilename) + " file format: " + _
               Trim(Str(tHeader.lVerMajor)) + "." + Trim(Str(tHeader.lVerMinor)) + _
               Chr(13) + "App file format: " + _
               Trim(Str((lVersion And &HFFFF0000) \ &H10000)) + "." + _
               Trim(Str(lVersion And &HFFFF&))
    MsgBox strError, vbOKOnly, "Incompatible File Version"
  
    CheckFileVersion = False
  Else
    CheckFileVersion = True
  End If
End Function
