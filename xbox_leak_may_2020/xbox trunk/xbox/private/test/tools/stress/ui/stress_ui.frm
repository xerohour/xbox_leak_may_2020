VERSION 5.00
Begin VB.Form Form1 
   Caption         =   "Form1"
   ClientHeight    =   3270
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   2640
   LinkTopic       =   "Form1"
   ScaleHeight     =   3270
   ScaleWidth      =   2640
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Start Stress"
      Height          =   735
      Left            =   360
      TabIndex        =   3
      Top             =   2160
      Width           =   1935
   End
   Begin VB.ComboBox Combo3 
      Height          =   315
      ItemData        =   "stress_ui.frx":0000
      Left            =   360
      List            =   "stress_ui.frx":0010
      TabIndex        =   2
      Text            =   "Com Port"
      Top             =   960
      Width           =   1935
   End
   Begin VB.ComboBox Combo2 
      Height          =   315
      ItemData        =   "stress_ui.frx":002C
      Left            =   360
      List            =   "stress_ui.frx":0036
      TabIndex        =   1
      Text            =   "Video Card"
      Top             =   1560
      Width           =   1935
   End
   Begin VB.ComboBox Combo1 
      Height          =   315
      ItemData        =   "stress_ui.frx":004B
      Left            =   360
      List            =   "stress_ui.frx":0055
      TabIndex        =   0
      Text            =   "Recovery Disc Used"
      Top             =   360
      Width           =   1935
   End
End
Attribute VB_Name = "Form1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Private Sub Combo2_Change()

End Sub

Private Sub Combo3_Change()

End Sub
