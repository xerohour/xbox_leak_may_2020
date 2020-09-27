Attribute VB_Name = "SimulatorCode"
'*****************************************************************************
'
'Copyright (C) 2000-2001 Microsoft Corporation
'
'Module Name:
'
'    SimulatorCode
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

Public Type PacketHeader
   command      As Byte
   subCommand   As Byte
   param        As Byte ' the endpoint
   dataSize     As Integer
End Type

Public Type PacketData
    header      As PacketHeader
    data        As Variant
End Type

' Duke enumeration state machine variables
Public DukeState        ' The current state during the duke enumeration
Public DukeStateData    ' internal data used by the different states
Public INState          ' We saw an IN, deal with it once we get a NAK
Public data01           ' so we can alternate between data0 and data1
Public xidPacket        ' this is the input report


'
' PrintPacketHeader
'
' Returns a pretty printed version of only the network packet
' header
'
Public Function PrintPacketHeader(Packet As PacketHeader)
    Dim text As String: text = ""
    Dim newline: newline = Chr(13) & Chr(10)
    
    'Verbose mode
    'text = text & "Command    : " & packet.command & newline
    'text = text & "Sub Command: " & packet.subCommand & newline
    'text = text & "Param      : " & packet.param & newline
    'text = text & "Data Size  : " & packet.dataSize & newline
    
    'Non verbose mode
    text = text & Packet.command & ", "
    text = text & Packet.subCommand & ", "
    text = text & Packet.param & ", "
    text = text & Packet.dataSize & newline
    
    PrintPacketHeader = text
End Function


'
' PrintPacketData
'
' Returns a pretty printed version of the network packet header
' and its data.
'
Public Function PrintPacketData(Packet As PacketData)
    Dim text As String: text = ""
    Dim newline: newline = Chr(13) & Chr(10)
    Dim i
    
    text = text & PrintPacketHeader(Packet.header) & "   "
    For i = 0 To Packet.header.dataSize - 1
        text = text & Hex2(Packet.data(i)) & " "
    Next
    
    PrintPacketData = text & newline & newline
End Function


'
' HandlePacket
'
' This function manages the Duke state machine and responds to the commands
'
Public Function HandlePacket(Packet As PacketData)
    USBTestApp.responseText.text = USBTestApp.responseText.text & PrintPacketData(Packet)
    
    ' DATA0
    If Packet.header.command = 1 And Packet.header.subCommand = 0 And Packet.data(0) = &HC3 Then
        If CompareArray(Packet.data, MakeArray("\xC3\x80\x06\x00\x01\x00\x00\xff\x00")) = True Then
            DukeState = "Get Device Descriptor"
        ElseIf Packet.data(1) = &H0 And Packet.data(2) = &H5 And Packet.data(4) = &H0 And Packet.data(5) = &H0 And Packet.data(6) = &H0 And Packet.data(7) = &H0 And Packet.data(8) = &H0 Then
            DukeState = "Set Address"
            DukeStateData = Packet.data(3)
        ElseIf CompareArray(Packet.data, MakeArray("\xC3\x80\x06\x00\x02\x00\x00\x50\x00")) = True Then
            DukeState = "Get Configuration Descriptor"
            DukeStateData = 0
        ElseIf CompareArray(Packet.data, MakeArray("\xC3\x00\x09\x01\x00\x00\x00\x00\x00")) = True Then
            DukeState = "Set Configuration"
        ElseIf CompareArray(Packet.data, MakeArray("\xC3\xC1\x06\x00\x41\x00\x00\x08\x00")) = True Then
            DukeState = "Get XID Descriptor"
        End If
        
    ' IN
    ElseIf Packet.header.command = 1 And Packet.header.subCommand = 0 And Packet.data(0) = &H96 Then
        ' If we get an IN, we dont know if we already handled it until we see the next ACK/NAK in
        ' the next packet.
        INState = 1
        
    ' NAK
    ElseIf Packet.header.command = 2 And Packet.header.subCommand = 1 And Packet.data(0) = &H5A Then
        If INState = 1 Then
            ' we saw a IN, and it is one we havent responded to yet
            If DukeState = "Get Device Descriptor" Then
                SendData 1, 0, MakeArray("\xD2\x12\x01\x10\x01\x00\x00\x00\x08")
                DukeState = ""
            ElseIf DukeState = "Set Address" Then
                SendData 1, 0, MakeArray("\xD2")
                SendData 5, 2, MakeArray("\x" & Hex2(DukeStateData))
                DukeState = ""
            ElseIf DukeState = "Get Configuration Descriptor" Then
                If DukeStateData = 0 Then
                    SendData 1, 0, MakeArray("\xD2\x09\x02\x29\x00\x01\x01\x04\x08")
                ElseIf DukeStateData = 1 Then
                    SendData 1, 0, MakeArray("\xC3\x32\x09\x04\x00\x00\x02\x03\x00")
                ElseIf DukeStateData = 2 Then
                    SendData 1, 0, MakeArray("\xD2\x00\x05\x09\x21\x10\x01\x00\x01")
                ElseIf DukeStateData = 3 Then
                    SendData 1, 0, MakeArray("\xC3\x22\xC7\x00\x07\x05\x81\x03\x08")
                ElseIf DukeStateData = 4 Then
                    SendData 1, 0, MakeArray("\xD2\x00\x01\x07\x05\x02\x03\x08\x00")
                ElseIf DukeStateData = 5 Then
                    SendData 1, 0, MakeArray("\xC3\x01")
                    DukeState = ""
                End If
                DukeStateData = DukeStateData + 1
            ElseIf DukeState = "Set Configuration" Then
                SendData 1, 0, MakeArray("\xD2")
                DukeState = ""
            ElseIf DukeState = "Get XID Descriptor" Then
                SendData 1, 0, MakeArray("\xD2\x08\x41\x00\x01\x01\x01\x1A\x04")
                DukeState = ""
                
            Else ' just a normal IN - requesting the Input Report
                SendData 1, 0, MakeArray(GetData0or1() & xidPacket)
            End If
            
        End If
        INState = 0
        
    ' ACK
    ElseIf Packet.header.command = 2 And Packet.header.subCommand = 1 And Packet.data(0) = &H4B Then
        INState = 0
    End If
    
    HandlePacket = "Ok"
End Function


'
' SetupEndpoint
'
' This sends a Endpoint Setup packet to the simulator
'
Public Sub SetupEndpoint(endpoint As Integer, fifoSize As Byte, endpointType As Byte, autoRepeat As Byte)
    Dim endpointSetup(7) As Byte
    endpointSetup(0) = 5
    endpointSetup(1) = 1
    endpointSetup(2) = endpoint
    endpointSetup(3) = 1
    endpointSetup(4) = 0
    endpointSetup(5) = fifoSize
    endpointSetup(6) = endpointType
    endpointSetup(7) = autoRepeat
    
    USBTestApp.winsockTCP.SendData endpointSetup
End Sub


'
' SendData
'
' This sends an entire command and data packet
'
Public Sub SendData(command As Byte, subCommand As Byte, data)
    Dim dataSize: dataSize = UBound(data) + 1
    ReDim Packet(dataSize + 5 - 1) As Byte
    Dim i
    Packet(0) = command
    Packet(1) = subCommand
    Packet(2) = 0
    Packet(3) = dataSize Mod 256
    Packet(4) = dataSize / 256
    
    For i = 0 To dataSize - 1
        Packet(i + 5) = data(i)
    Next
    
    USBTestApp.winsockTCP.SendData Packet
End Sub


'
' GetData0or1
'
' Handles the alternating DATA0/DATA1
'
Public Function GetData0or1()
    If data01 = "\xD2" Then
        data01 = "\xC3"
    Else
        data01 = "\xD2"
    End If

    GetData0or1 = data01
End Function
