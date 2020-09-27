VERSION 5.00
Begin VB.Form Stress 
   Caption         =   "Xbox Stress"
   ClientHeight    =   4065
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   4530
   LinkTopic       =   "Xbox Stress"
   ScaleHeight     =   4065
   ScaleWidth      =   4530
   StartUpPosition =   3  'Windows Default
   Begin VB.ComboBox Testini 
      Height          =   315
      ItemData        =   "stress_ui.frx":0000
      Left            =   2160
      List            =   "stress_ui.frx":0010
      TabIndex        =   7
      Text            =   "Random"
      Top             =   2760
      Width           =   1815
   End
   Begin VB.CommandButton StopStress 
      Caption         =   "Stop Stress"
      Height          =   495
      Left            =   1598
      TabIndex        =   9
      Top             =   3360
      Width           =   1335
   End
   Begin VB.ComboBox Source 
      Height          =   315
      ItemData        =   "stress_ui.frx":0031
      Left            =   2160
      List            =   "stress_ui.frx":003E
      TabIndex        =   3
      Text            =   "\\xbuilds\release\usa"
      Top             =   1320
      Width           =   1815
   End
   Begin VB.CommandButton xskd 
      Caption         =   "Only Open KD"
      Height          =   495
      Left            =   3038
      TabIndex        =   10
      Top             =   3360
      Width           =   1335
   End
   Begin VB.CheckBox CopyMedia 
      Caption         =   "yes"
      Height          =   315
      Left            =   2160
      TabIndex        =   6
      Top             =   2400
      Value           =   1  'Checked
      Width           =   1815
   End
   Begin VB.ComboBox Recovery 
      Height          =   315
      ItemData        =   "stress_ui.frx":0089
      Left            =   2160
      List            =   "stress_ui.frx":0093
      TabIndex        =   2
      Text            =   "internal"
      Top             =   960
      Width           =   1815
   End
   Begin VB.CommandButton Start 
      Caption         =   "Start Stress"
      Height          =   495
      Left            =   158
      TabIndex        =   8
      Top             =   3360
      Width           =   1335
   End
   Begin VB.ComboBox BuildVer 
      Height          =   315
      ItemData        =   "stress_ui.frx":00AB
      Left            =   2160
      List            =   "stress_ui.frx":00BB
      TabIndex        =   4
      Text            =   "latest"
      Top             =   1680
      Width           =   1815
   End
   Begin VB.ComboBox K_Htype 
      Height          =   315
      ItemData        =   "stress_ui.frx":00D9
      Left            =   2160
      List            =   "stress_ui.frx":00E9
      TabIndex        =   5
      Text            =   "chk_chk"
      Top             =   2040
      Width           =   1815
   End
   Begin VB.ComboBox Port 
      Height          =   315
      ItemData        =   "stress_ui.frx":0111
      Left            =   2160
      List            =   "stress_ui.frx":0121
      TabIndex        =   1
      Text            =   "Com1"
      Top             =   600
      Width           =   1815
   End
   Begin VB.TextBox Xbox 
      Height          =   315
      Left            =   2160
      TabIndex        =   0
      Text            =   "xbox"
      Top             =   240
      Width           =   1815
   End
   Begin VB.Label Label9 
      Caption         =   "Testini"
      Height          =   255
      Left            =   480
      TabIndex        =   18
      Top             =   2760
      Width           =   1215
   End
   Begin VB.Label Label8 
      Caption         =   "Source"
      Height          =   255
      Left            =   480
      TabIndex        =   17
      Top             =   1320
      Width           =   1215
   End
   Begin VB.Label Label7 
      Caption         =   "Copy Media ?"
      Height          =   255
      Left            =   480
      TabIndex        =   16
      Top             =   2400
      Width           =   1215
   End
   Begin VB.Label Label1 
      Caption         =   "Recovery Type"
      Height          =   255
      Left            =   480
      TabIndex        =   13
      Top             =   960
      Width           =   1215
   End
   Begin VB.Label Label6 
      Caption         =   "Build Version"
      Height          =   255
      Left            =   480
      TabIndex        =   14
      Top             =   1680
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "Kernel_Harness"
      Height          =   255
      Left            =   480
      TabIndex        =   15
      Top             =   2040
      Width           =   1215
   End
   Begin VB.Label Label4 
      Caption         =   "Xbox Name:"
      Height          =   255
      Left            =   480
      TabIndex        =   11
      Top             =   240
      Width           =   1215
   End
   Begin VB.Label Label3 
      Caption         =   "Com Port:"
      Height          =   255
      Left            =   480
      TabIndex        =   12
      Top             =   600
      Width           =   1215
   End
End
Attribute VB_Name = "Stress"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Form_Load()
    Dim o As Object
    Set o = CreateObject("WScript.Shell")
    
    On Error Resume Next
    Xbox.Text = o.RegRead("HKCU\Software\Microsoft\XboxSdk\XboxName")
    If (Err.Number <> 0) Then
        Xbox.Text = ""
    End If
        
End Sub

Private Sub Start_Click()

If Xbox.Text <> "" And Port.Text <> "" And BuildVer.Text <> "" And K_Htype.Text <> "" Then
    Dim tempdir
    tempdir = Environ("TEMP")
    Set fs = CreateObject("Scripting.FileSystemObject")
    Set opts = fs.CreateTextFile(tempdir & "\xsopts.cmd", True)
    opts.writeline ("@echo off")
    opts.writeline ("set _xsconfig=1")
    opts.writeline ("set XboxName=" & Xbox.Text)
    opts.writeline ("set ComPort=" & Port.Text)
    opts.writeline ("set recovery=" & Recovery.Text)
    opts.writeline ("set source=" & Source.Text)
    opts.writeline ("set buildver=" & BuildVer.Text)
    opts.writeline ("set k_htype=" & K_Htype.Text)
    opts.writeline ("set testini=" & Testini.Text)
    opts.writeline ("set copymedia=" & CopyMedia.Value)
    opts.writeline ("call \\xstress\stress\stress.cmd")
    Rem opts.writeline ("call C:\stress_ui\stress.cmd")
    opts.Close
    Shell tempdir & "\xsopts.cmd", vbNormalFocus

End If
End Sub

Private Sub xskd_Click()

If Xbox.Text <> "" And Port.Text <> "" And BuildVer.Text <> "" And K_Htype.Text <> "" Then
    Shell "\\xstress\stress\xskd.cmd " & Xbox.Text & " " & Port.Text & " " & Source.Text & " " & BuildVer.Text & " " & K_Htype.Text, vbNormalFocus

End If
End Sub

Private Sub StopStress_Click()

Shell "cscript \\xstress\stress\StopStress.wsf /box=" & Xbox.Text, vbNormalFocus
End Sub

