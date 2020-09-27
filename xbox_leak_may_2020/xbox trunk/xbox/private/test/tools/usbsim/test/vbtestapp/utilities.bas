Attribute VB_Name = "Utilities"
'*****************************************************************************
'
'Copyright (C) 2000-2001 Microsoft Corporation
'
'Module Name:
'
'    Utilities
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
' DebugPrint
'
' Writes a string to the output window
'
Public Sub DebugPrint(msg)
    Dim newline: newline = Chr(13) & Chr(10)
    USBTestApp.responseText.text = USBTestApp.responseText.text & msg & newline
End Sub


'
' MakeArray
'
' Takes a string consisting of C style byte definitions (in hex)
' and returns a Byte Array consisting of the data
'
' Example: theArray = MakeArray("\x00\x55\xAA\xFF")
'
Public Function MakeArray(data As String)
    Dim length: length = Len(data) / 4
    ReDim newarray(length - 1)
    Dim i: i = 0
    
    While i < length
        newarray(i) = CInt("&H" & Mid(data, 3, 2))
        data = Mid(data, 5)
        i = i + 1
    Wend
    
    MakeArray = newarray
End Function


'
' CompareArray
'
' Compares two Byte Arrays
'
Public Function CompareArray(array1, array2) As Boolean
    Dim size1: size1 = UBound(array1)
    Dim size2: size2 = UBound(array2)
    CompareArray = False
    
    If size1 <> size2 Then Exit Function
    
    While size1 > 0
        If array1(size1) <> array2(size1) Then Exit Function
        size1 = size1 - 1
    Wend
    
    CompareArray = True
End Function


'
' Hex2
'
' returns a 2 byte string representing the hex value of
' the number passed in
'
' Example: Hex2(12) = "0C"
' Example: Hex2(75) = "4B"
'
Public Function Hex2(hexVal) As String
    If hexVal < 16 Then
        Hex2 = "0" & Hex(hexVal)
    Else: Hex2 = Hex(hexVal)
    End If
End Function
