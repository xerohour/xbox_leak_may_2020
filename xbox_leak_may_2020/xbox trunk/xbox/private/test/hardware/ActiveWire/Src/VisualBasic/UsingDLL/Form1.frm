VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3240
   ClientLeft      =   84
   ClientTop       =   408
   ClientWidth     =   3672
   LinkTopic       =   "Form1"
   ScaleHeight     =   3240
   ScaleWidth      =   3672
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Pulse 
      Caption         =   "Make Pulse"
      Height          =   492
      Left            =   1320
      TabIndex        =   8
      Top             =   1920
      Width           =   1092
   End
   Begin VB.CommandButton Open 
      Caption         =   "Open"
      Height          =   492
      Left            =   120
      TabIndex        =   6
      Top             =   120
      Width           =   1092
   End
   Begin VB.TextBox Message 
      Height          =   288
      Left            =   120
      TabIndex        =   5
      Text            =   "Message"
      Top             =   2760
      Width           =   3372
   End
   Begin VB.CommandButton Close 
      Caption         =   "Close"
      Height          =   492
      Left            =   120
      TabIndex        =   4
      Top             =   720
      Width           =   1092
   End
   Begin VB.CommandButton Out 
      Caption         =   "Out"
      Height          =   492
      Left            =   1320
      TabIndex        =   3
      Top             =   720
      Width           =   1092
   End
   Begin VB.CommandButton In 
      Caption         =   "In"
      Height          =   492
      Left            =   1320
      TabIndex        =   2
      Top             =   1320
      Width           =   1092
   End
   Begin VB.CommandButton Enable 
      Caption         =   "Enable"
      Height          =   492
      Left            =   1320
      TabIndex        =   1
      Top             =   120
      Width           =   1092
   End
   Begin VB.TextBox DataText 
      Height          =   288
      Left            =   2520
      TabIndex        =   0
      Text            =   "0"
      Top             =   840
      Width           =   972
   End
   Begin VB.Label Label1 
      Caption         =   "Hex Data"
      Height          =   252
      Left            =   2520
      TabIndex        =   7
      Top             =   600
      Width           =   972
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Declare Function AwusbOpen Lib "awusbvb.dll" (ByVal devnum As Long) As Long
Private Declare Function AwusbClose Lib "awusbvb.dll" () As Long
Private Declare Function AwusbEnablePort Lib "awusbvb.dll" (ByRef Data As Long, ByVal count As Long) As Long
Private Declare Function AwusbOutPort Lib "awusbvb.dll" (ByRef Data As Long, ByVal count As Long) As Long
Private Declare Function AwusbInPort Lib "awusbvb.dll" (ByRef Data As Long, ByVal count As Long) As Long
Private Declare Function AwusbErrorMessage Lib "awusbvb.dll" (ByVal errorcode As Long) As String
Dim Data As Long

Private Sub DataText_Change()
    Data = CLng(DataText.Text)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call Close_Click
End Sub

Private Sub Open_Click()
    Dim ret As Long
    Message.Text = "Open #0 "
    ret = AwusbOpen(0)
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
        
End Sub

Private Sub Close_Click()
    Dim ret As Long
    Message.Text = "Close #0 "
    ret = AwusbClose()
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
End Sub

Private Sub Enable_Click()
    Dim ret As Long
    Message.Text = "Enable " + Str(Data) + " "
    ret = AwusbEnablePort(Data, 2)
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
End Sub

Private Sub Out_Click()
    Dim ret As Long
    Message.Text = "Out " + Str(Data) + " "
    ret = AwusbOutPort(Data, 2)
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
End Sub

Private Sub In_Click()
    Dim ret As Long
    Message.Text = "In "
    ret = AwusbInPort(Data, 2)
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
    DataText.Text = Str(Data)
End Sub

Private Sub Pulse_Click()
    Dim ret As Long
    Dim PulseData(100) As Integer
    Message.Text = "Making Pulse "
    For i = 1 To 100 Step 2
        PulseData(i) = -32768
        PulseData(i + 1) = 0
    Next
    ret = AwusbOutPort(Pulse, 200)
    If (ret <> 0) Then
        Message.Text = Message.Text + AwusbErrorMessage(ret)
    Else
        Message.Text = Message.Text + "Success"
    End If
End Sub
