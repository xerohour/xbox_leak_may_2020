VERSION 5.00
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form USBTestApp 
   Caption         =   "USB Simulator Test App"
   ClientHeight    =   7485
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   5550
   Icon            =   "USBSimTestApp.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   ScaleHeight     =   7485
   ScaleWidth      =   5550
   StartUpPosition =   3  'Windows Default
   Begin MSWinsockLib.Winsock winsockTCP 
      Left            =   120
      Top             =   1080
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
   End
   Begin VB.TextBox Text1 
      Alignment       =   1  'Right Justify
      BackColor       =   &H80000004&
      Height          =   285
      Left            =   0
      TabIndex        =   6
      TabStop         =   0   'False
      Text            =   "© 2000-2001 Microsoft"
      Top             =   7200
      Width           =   5535
   End
   Begin VB.CommandButton BtnGetIP 
      Caption         =   "Get IP"
      Enabled         =   0   'False
      Height          =   375
      Left            =   3600
      TabIndex        =   1
      Top             =   240
      Width           =   1695
   End
   Begin MSWinsockLib.Winsock winsockUDP 
      Left            =   720
      Top             =   1080
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      Protocol        =   1
      RemoteHost      =   "0.0.0.0"
      RemotePort      =   200
   End
   Begin VB.TextBox responseText 
      Height          =   5535
      Left            =   0
      MultiLine       =   -1  'True
      ScrollBars      =   3  'Both
      TabIndex        =   8
      Top             =   1680
      Width           =   5535
   End
   Begin VB.CommandButton BtnDisconnect 
      Caption         =   "Disconnect"
      Enabled         =   0   'False
      Height          =   375
      Left            =   3600
      TabIndex        =   5
      Top             =   1080
      Width           =   1695
   End
   Begin VB.TextBox SimIP 
      Height          =   285
      Left            =   1800
      TabIndex        =   0
      Text            =   "157.56.10.94"
      Top             =   240
      Width           =   1695
   End
   Begin VB.CommandButton BtnConnect 
      Caption         =   "Connect"
      Height          =   375
      Left            =   1800
      TabIndex        =   4
      Top             =   1080
      Width           =   1695
   End
   Begin VB.TextBox SimPort 
      Height          =   285
      Left            =   1800
      TabIndex        =   3
      Text            =   "201"
      Top             =   600
      Width           =   1695
   End
   Begin VB.Label Label1 
      Caption         =   "TCP Port"
      Height          =   255
      Left            =   240
      TabIndex        =   2
      Top             =   600
      Width           =   1215
   End
   Begin VB.Label Label2 
      Caption         =   "IP Address"
      Height          =   255
      Left            =   240
      TabIndex        =   7
      Top             =   240
      Width           =   1335
   End
End
Attribute VB_Name = "USBTestApp"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
'*****************************************************************************
'
'Copyright (C) 2000-2001 Microsoft Corporation
'
'Module Name:
'
'    USBSimTestApp
'
'Abstract:
'
'
'
'Author:
'
'    Josh Poley (jpoley)
'
'Revision History:
'
'*****************************************************************************
Option Explicit


'
' BtnGetIP Click Event
'
' Send the IPQuery UDP broadcast
'
Private Sub BtnGetIP_Click()
    Dim ipquery(4) As Byte
    ipquery(0) = 3
    ipquery(1) = 0
    ipquery(2) = 0
    ipquery(3) = 0
    ipquery(4) = 0
    
    winsockUDP.RemotePort = 200
    winsockUDP.SendData ipquery
    ' According to a netmon trace the packet is being sent, but not to port 200
    ' thus the server never gets it.
End Sub


'
' UDP DataArrival Event
'
' If a simulator responds to the broadcast, we grab its IP address
'
Private Sub winsockUDP_DataArrival(ByVal bytesTotal As Long)
    Dim strData
    winsockUDP.GetData strData
    SimIP.text = winsockUDP.RemoteHostIP
End Sub


'
' BtnConnect Click Event
'
' Connect to the server specified by the edit boxes
'
Private Sub BtnConnect_Click()
    winsockTCP.RemoteHost = SimIP.text
    winsockTCP.RemotePort = SimPort.text
    winsockTCP.Connect
    BtnConnect.Enabled = False
    BtnDisconnect.Enabled = True
    
    ' initialize the duke enumeration state machine
    DukeState = ""
    DukeStateData = 0
    INState = 0
    data01 = ""
    xidPacket = "\x00\x14\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
End Sub


'
' BtnDisconnect Click Event
'
' Disconnecting from the server will "unplug" the simulated USB device
'
Private Sub BtnDisconnect_Click()
    winsockTCP.Close
    BtnConnect.Enabled = True
    BtnDisconnect.Enabled = False
End Sub


'
' Connect event
'
' This event fires when the TCP control connects to a remote server
'
' Once we are connected, the first thing we need to send the endpoint
' configuration commands to the simulator
'
Private Sub winsockTCP_Connect()
    SetupEndpoint 0, 1, 3, 0
    SetupEndpoint 1, 1, 2, 0
    SetupEndpoint 2, 1, 2, 0
    SetupEndpoint 3, 1, 2, 0
    SetupEndpoint 4, 1, 2, 0
End Sub


'
' TCP DataArrival Event
'
' The simulator is sending us USB data. Parse out the data into our packet
' structure and then handle the input.
'
Private Sub winsockTCP_DataArrival(ByVal bytesTotal As Long)
    Dim strData
    Dim Packet As PacketData
    Dim i As Integer: i = 0
    Dim err
    
    While i < bytesTotal
        winsockTCP.GetData strData, , 5
        Packet.header.command = strData(0)
        Packet.header.subCommand = strData(1)
        Packet.header.param = strData(2)
        Packet.header.dataSize = strData(3) + strData(4) * 256
        winsockTCP.GetData Packet.data, , Packet.header.dataSize
        
        i = i + 5 + Packet.header.dataSize
        err = HandlePacket(Packet)
    Wend
End Sub


