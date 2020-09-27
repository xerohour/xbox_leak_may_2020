VERSION 5.00
Object = "{95438942-F98A-11D2-AC25-00104B306BEE}#1.0#0"; "AWUSBIO.OCX"
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   4332
   ClientLeft      =   84
   ClientTop       =   408
   ClientWidth     =   3672
   LinkTopic       =   "Form1"
   ScaleHeight     =   4332
   ScaleWidth      =   3672
   StartUpPosition =   3  'Windows Default
   Begin AWUSBIOLib.AwusbIO AwusbIO1 
      Height          =   852
      Left            =   720
      TabIndex        =   9
      Top             =   3360
      Width           =   972
      _Version        =   65536
      _ExtentX        =   1714
      _ExtentY        =   1503
      _StockProps     =   0
   End
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
Dim Data As Long

Private Sub DataText_Change()
    Data = CLng(DataText.Text)
End Sub

Private Sub Form_Unload(Cancel As Integer)
    Call AwusbIO1.Close
End Sub

Private Sub Open_Click()
    Message.Text = "Open #0 " + AwusbIO1.Open(0)
End Sub

Private Sub Close_Click()
    Message.Text = "Close #0 " + AwusbIO1.Close()
End Sub

Private Sub Enable_Click()
    Message.Text = "Enable " + Str(Data) + " " + AwusbIO1.EnablePort(Data)
End Sub

Private Sub Out_Click()
    Message.Text = "Out " + Str(Data) + " " + AwusbIO1.OutPort(Data)
End Sub

Private Sub In_Click()
    Message.Text = "In "
    ret = AwusbIO1.InPort()
    If (ret < 0) Then
        Message.Text = Message.Text + AwusbIO1.ErrorMessage(ret)
    Else
        Data = ret
        DataText.Text = Str(Data)
    End If
End Sub

Private Sub Pulse_Click()
    Dim StringData As String
    Message.Text = "Making Pulse "
    StringData = "ff ff 00 00"
    Message.Text = "Making Pulse " + AwusbIO1.OutPortBlock(StringData, 100)
End Sub

