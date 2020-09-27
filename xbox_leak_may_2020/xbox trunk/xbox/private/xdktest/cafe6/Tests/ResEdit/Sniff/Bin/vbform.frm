VERSION 4.00
Begin VB.Form Form1 
   Caption         =   "VRES Sniff VB Form Record Editor Sample"
   ClientHeight    =   5385
   ClientLeft      =   975
   ClientTop       =   1515
   ClientWidth     =   6375
   ClipControls    =   0   'False
   BeginProperty Font 
      name            =   "MS Sans Serif"
      charset         =   1
      weight          =   700
      size            =   8.25
      underline       =   0   'False
      italic          =   0   'False
      strikethrough   =   0   'False
   EndProperty
   Height          =   5790
   Left            =   915
   LinkTopic       =   "Form1"
   ScaleHeight     =   5385
   ScaleWidth      =   6375
   Top             =   1170
   Width           =   6495
   Begin VB.TextBox FieldBoxes 
      Height          =   2415
      Index           =   6
      Left            =   210
      MaxLength       =   50
      MultiLine       =   -1  'True
      ScrollBars      =   2  'Vertical
      TabIndex        =   6
      Top             =   2730
      Width           =   3735
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   5
      Left            =   2280
      TabIndex        =   5
      Top             =   1890
      Width           =   1695
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   4
      Left            =   210
      TabIndex        =   4
      Top             =   1890
      Width           =   1695
   End
   Begin VB.CommandButton ExitButton 
      Caption         =   "Exit"
      Height          =   495
      Left            =   4440
      TabIndex        =   12
      Top             =   4320
      Width           =   1695
   End
   Begin VB.CommandButton DeleteRecord 
      Caption         =   "Delete Record"
      Height          =   495
      Left            =   4440
      TabIndex        =   8
      Top             =   960
      Width           =   1695
   End
   Begin VB.CommandButton AddRecord 
      Caption         =   "Add Record"
      Height          =   495
      Left            =   4440
      TabIndex        =   7
      Top             =   240
      Width           =   1695
   End
   Begin VB.CommandButton OpenFile 
      Caption         =   "Open File"
      Height          =   495
      Left            =   4440
      TabIndex        =   11
      Top             =   3600
      Width           =   1695
   End
   Begin VB.CommandButton PreviousRecord 
      Caption         =   "Previous Record"
      Height          =   495
      Left            =   4440
      TabIndex        =   9
      Top             =   1920
      Width           =   1695
   End
   Begin VB.CommandButton NextRecord 
      Caption         =   "Next Record"
      Height          =   495
      Left            =   4440
      TabIndex        =   10
      Top             =   2625
      Width           =   1695
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   3
      Left            =   2280
      MaxLength       =   15
      TabIndex        =   3
      Top             =   1155
      Width           =   1695
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   2
      Left            =   225
      TabIndex        =   2
      Top             =   1155
      Width           =   1695
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   1
      Left            =   2280
      MaxLength       =   15
      TabIndex        =   1
      Top             =   420
      Width           =   1695
   End
   Begin VB.TextBox FieldBoxes 
      Height          =   375
      Index           =   0
      Left            =   210
      MaxLength       =   15
      TabIndex        =   0
      Top             =   420
      Width           =   1695
   End
   Begin VB.Label Label9 
      Caption         =   "Last Review Comments"
      Height          =   255
      Left            =   210
      TabIndex        =   19
      Top             =   2520
      Width           =   2055
   End
   Begin VB.Label Label8 
      Caption         =   "Last Review Date"
      Height          =   255
      Left            =   2280
      TabIndex        =   18
      Top             =   1680
      Width           =   1575
   End
   Begin VB.Label Label7 
      Caption         =   "Monthly Salary"
      Height          =   255
      Left            =   210
      TabIndex        =   17
      Top             =   1680
      Width           =   1335
   End
   Begin VB.Label Label4 
      Caption         =   "Title"
      Height          =   255
      Left            =   2310
      TabIndex        =   16
      Top             =   945
      Width           =   375
   End
   Begin VB.Label Label3 
      Caption         =   "ID #"
      Height          =   255
      Left            =   210
      TabIndex        =   15
      Top             =   945
      Width           =   495
   End
   Begin VB.Label Label2 
      Caption         =   "Last Name"
      Height          =   255
      Left            =   2310
      TabIndex        =   14
      Top             =   210
      Width           =   975
   End
   Begin VB.Label Label1 
      Caption         =   "First Name"
      Height          =   255
      Left            =   210
      TabIndex        =   13
      Top             =   210
      Width           =   975
   End
End
Attribute VB_Name = "Form1"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Option Explicit
Dim Employee As Person
Dim OldContents As Person
Dim Position As Long         ' Position describes presentation order.
Dim LastRecord As Long
Dim FileName As String
Dim FileNum As Integer

Private Sub AddRecord_Click()
    Dim Ind As Integer
    SaveRecordChanges
    For Ind = 0 To 6
        Form1.FieldBoxes(Ind).Text = ""
    Next Ind
    GetFields
    LastRecord = LastRecord + 1
    Put #FileNum, LastRecord, Employee
    Position = LastRecord
    ShowRecord
End Sub

Private Sub CleanUpFile()
    Dim CleanFileNum As Integer
    Dim Ind As Long
    Dim Confirm As Integer
    Confirm = False
    CleanFileNum = FileOpener("~~Tmp~~.Tmp", conRandomFile, LenB(Employee), Confirm)
    For Ind = 1 To LastRecord
        Get #FileNum, Ind, Employee
        Put #CleanFileNum, Ind, Employee
    Next Ind
    Close       ' Close all files.
    FileCopy "~~Tmp~~.Tmp", FileName
    FileNum = FileOpener(FileName, conRandomFile, LenB(Employee), Confirm)
    Kill "~~Tmp~~.Tmp"
End Sub

Private Sub DeleteRecord_Click()
    Dim TempVar As Person
    Dim Ind As Integer
    Dim Msg As String
    If LastRecord = 1 Then
        Msg = "This is the last record in the file. Deleting it will delete"
        Msg = Msg + " the entire file."
        Msg = Msg + " Record Editor will also be closed."
        Msg = Msg + " Choose OK to delete the file."
        If MsgBox(Msg, 65, "About to delete file!") = 1 Then
            Close (FileNum)
            Kill FileName
        End If
        End
    End If
    For Ind = Position To LastRecord - 1
        Get #FileNum, Ind + 1, TempVar
        Put #FileNum, Ind, TempVar
    Next Ind
    LastRecord = LastRecord - 1
    If Position > LastRecord Then
        Position = LastRecord
    End If
    CleanUpFile
    ShowRecord                          ' This displays the record that follows the deleted record.
End Sub

Private Sub ExitButton_Click()
    CleanUpFile
    End
End Sub

Private Sub FieldBoxes_GotFocus(Index As Integer)
    FieldBoxes(Index).SelStart = 0
    FieldBoxes(Index).SelLength = Len(FieldBoxes(Index).Text)
End Sub

Private Sub FieldBoxes_LostFocus(Index As Integer)
  If Val(FieldBoxes(2).Text) > 32767 Then
    MsgBox "Enter a number less than 32,768"
    FieldBoxes(2).SetFocus
  End If
End Sub

Private Sub Form_Load()
    Dim BoxCaption As String
    Dim NL As String
    Dim Msg As String
    ChDrive App.Path
    ChDir App.Path
    Form1.Show
    OpenFile_Click
End Sub

Private Sub Form_Unload(Cancel As Integer)
    End
End Sub

Private Sub GetFields()
    Employee.FirstName = Form1.FieldBoxes(0).Text
    Employee.LastName = Form1.FieldBoxes(1).Text
    If IsNumeric(Form1.FieldBoxes(2).Text) Then
        Employee.ID = CInt(Form1.FieldBoxes(2).Text)
    Else
        Employee.ID = 0
    End If
    Employee.Title = Form1.FieldBoxes(3).Text
    If IsNumeric(Form1.FieldBoxes(4).Text) Then
        Employee.MonthlySalary = CDbl(CCur(Form1.FieldBoxes(4).Text))
    Else
        Employee.MonthlySalary = CDbl(CCur(0))
    End If
    If IsDate(Form1.FieldBoxes(5).Text) Then
        Employee.LastReviewDate = CLng(DateValue(Form1.FieldBoxes(5).Text))
    Else
        Employee.LastReviewDate = CLng(DateValue("1/1/1753"))
    End If
    Employee.ReviewComments = Form1.FieldBoxes(6).Text
End Sub

Private Sub Initialize()
    LastRecord = LOF(FileNum) \ LenB(Employee)
    Position = 1
    If LastRecord < 1 Then
        GetFields
        OldContents = Employee
        AddRecord_Click
    Else
        ShowRecord
    End If
End Sub

Private Sub NextRecord_Click()
    Dim Msg As String
    SaveRecordChanges
    If Position = LastRecord Then
        Msg = "There are no records greater than " + Str(LastRecord) + "."
        MsgBox (Msg)
    Else
        Position = Position + 1
    End If
    ShowRecord
End Sub

Private Sub OpenFile_Click()
    Dim Confirm As Integer
    Confirm = True
    If LastRecord > 0 Then
        SaveRecordChanges
        CleanUpFile
    End If
    FileNum = 0
    Do While FileNum = 0
        FileName = GetFileName("Enter the name of a file to create or open.")
        If FileName = "" Then
            If LastRecord > 0 Then
                Exit Sub
            Else
                End
            End If
        Else
            FileNum = FileOpener(FileName, conRandomFile, LenB(Employee), Confirm)
        End If
    Loop
    Initialize
End Sub

Private Sub PreviousRecord_Click()
    SaveRecordChanges
    If Position = 1 Then
        MsgBox ("There are no records less than 1.")
    Else
        Position = Position - 1
    End If
    ShowRecord
End Sub

Private Sub SaveRecordChanges()
    Dim ConvertVariant As Variant
    Dim Equal As Integer
    Equal = True
    GetFields
    If Employee.FirstName <> OldContents.FirstName Then Equal = False
    If Employee.LastName <> OldContents.LastName Then Equal = False
    If Employee.ID <> OldContents.ID Then Equal = False
    If Employee.Title <> OldContents.Title Then Equal = False
    If Employee.MonthlySalary <> OldContents.MonthlySalary Then Equal = False
    If Employee.LastReviewDate <> OldContents.LastReviewDate Then Equal = False
    If Employee.ReviewComments <> OldContents.ReviewComments Then Equal = False
    If Not Equal Then
        Put #FileNum, Position, Employee
    End If
End Sub

Private Sub ShowRecord()
    Get #FileNum, Position, Employee
    Dim ConvertVariant As Variant
    Form1.FieldBoxes(0).Text = Trim(Employee.FirstName)
    Form1.FieldBoxes(1).Text = Trim(Employee.LastName)
    If Employee.ID > 0 Then
        Form1.FieldBoxes(2).Text = LTrim(Str(Employee.ID))
    Else
        Form1.FieldBoxes(2).Text = ""
    End If
    Form1.FieldBoxes(3) = Trim(Employee.Title)
    ConvertVariant = Employee.MonthlySalary
    ConvertVariant = CCur(ConvertVariant)
    If ConvertVariant > 0 Then
        Form1.FieldBoxes(4) = Format(ConvertVariant, "$#,##0.00;(#,##0.00)")
    Else
        Form1.FieldBoxes(4) = ""
    End If
    ConvertVariant = CDate(Employee.LastReviewDate)
    If ConvertVariant <> DateValue("1/1/1753") Then
        Form1.FieldBoxes(5).Text = ConvertVariant
    Else
        FieldBoxes(5) = ""
    End If
    Form1.FieldBoxes(6) = Trim(Employee.ReviewComments)
    GetFields
    OldContents = Employee
    UpdateCaption
    FieldBoxes(0).SetFocus
End Sub

Private Sub UpdateCaption()
    Dim Caption As String
    Caption = FileName + ": Record " + Str$(Position)
    Caption = Caption + " of " + Str$(LastRecord)
    Form1.Caption = Caption
End Sub

