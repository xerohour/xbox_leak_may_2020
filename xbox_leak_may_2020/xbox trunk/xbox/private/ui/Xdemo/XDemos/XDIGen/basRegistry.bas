Attribute VB_Name = "basRegistry"
Option Explicit
Option Compare Text

Type SECURITY_ATTRIBUTES
    nLength              As Long
    lpSecurityDescriptor As Long
    bInheritHandle       As Boolean
End Type

Declare Function RegCloseKey Lib "advapi32" (ByVal hKey As Long) As Long
Declare Function RegQueryValueEx Lib "advapi32" Alias "RegQueryValueExA" _
        (ByVal hKey As Long, ByVal lpszValueName As String, ByVal dwReserved As Long, _
        lpdwType As Long, lpbData As Any, cbData As Long) As Long
Declare Function RegSetValueEx Lib "advapi32" Alias "RegSetValueExA" _
        (ByVal hKey As Long, ByVal lpszValueName As String, ByVal dwReserved As Long, _
        ByVal fdwType As Long, lpbData As Any, ByVal cbData As Long) As Long
Declare Function RegOpenKeyEx Lib "advapi32.dll" Alias "RegOpenKeyExA" _
        (ByVal hKey As Long, ByVal lpSubKey As String, ByVal ulOptions As Long, ByVal samDesired As Long, _
        phkResult As Long) As Long
Declare Function RegCreateKeyEx Lib "advapi32.dll" Alias "RegCreateKeyExA" _
        (ByVal hKey As Long, ByVal lpSubKey As String, ByVal Reserved As Long, ByVal lpClass As String, _
        ByVal dwOptions As Long, ByVal samDesired As Long, lpSecurityAttributes As SECURITY_ATTRIBUTES, _
        phkResult As Long, lpdwDisposition As Long) As Long

'
'Public Constants
'
Public Const strREGBASE$ = "Software\Microsoft\Microsoft Games\XDIGen\1.0"

Public Const HKEY_CURRENT_USER = &H80000001
Public Const HKEY_LOCAL_MACHINE = &H80000002
Public Const ERROR_SUCCESS = 0&
Public Const ERROR_MORE_DATA = 234
Public Const REG_SZ = 1&
Public Const KEY_QUERY_VALUE = &H1
Public Const KEY_SET_VALUE = &H2
Public Const KEY_CREATE_SUB_KEY = &H4
Public Const REG_OPTION_NON_VOLATILE = 0

'
'Module Constants
'
Const lMAX_SUBKEYLEN& = 255

Sub vbReadRegString(ByVal strKey As String, strValue As String)
    Dim hRC As Long
    Dim strValBuffer As String
    Dim hkParent As Long
    Dim lStrLen As Long
        
    On Error GoTo RRSErr
    
    hkParent = 0
    hRC = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strREGBASE, 0, KEY_QUERY_VALUE, hkParent)
                         
    If ERROR_SUCCESS = hRC Then
        hRC = RegQueryValueEx(hkParent, strKey, 0, REG_SZ, "", lStrLen)
        If (ERROR_MORE_DATA = hRC) Then
            strValue = Space(lStrLen)
            hRC = RegQueryValueEx(hkParent, strKey, 0, REG_SZ, ByVal strValue, lStrLen)
        End If
        
        If hRC = ERROR_SUCCESS Then
            strValue = Left$(strValue, lStrLen - 1)
        End If
    End If

RRSErr:
    If hkParent <> 0 Then
        RegCloseKey (hkParent)
    End If

    Exit Sub
End Sub

Sub vbWriteRegString(ByVal strKey As String, ByVal strValue As String)
    Dim hRC As Long
    Dim strValBuffer As String
    Dim hkParent As Long
    Dim dwDisp As Long
    Dim lStrLen As Long
    Dim lAccess As Long
    Dim tSec As SECURITY_ATTRIBUTES
        
    On Error GoTo WRSErr
    
    hkParent = 0
    lAccess = KEY_SET_VALUE Or KEY_CREATE_SUB_KEY
    hRC = RegCreateKeyEx(HKEY_LOCAL_MACHINE, strREGBASE, 0, "", 0, lAccess, tSec, hkParent, dwDisp)
                         
    If (ERROR_SUCCESS = hRC) Then
        lStrLen = Len(strValue) + 1
        hRC = RegSetValueEx(hkParent, strKey, 0, REG_SZ, ByVal strValue, lStrLen)
    End If

WRSErr:
    If (hkParent) Then
        RegCloseKey (hkParent)
    End If

    Exit Sub
End Sub

Sub vbRegCloseKey(ByVal hkRegKey As Long)
    Dim hRC As Integer

    hRC = RegCloseKey(hkRegKey)
End Sub
