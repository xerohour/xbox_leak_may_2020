Attribute VB_Name = "basWinAPI"
Option Explicit

Public Type RECT
        Left As Long
        Top As Long
        Right As Long
        Bottom As Long
End Type

Declare Sub GetClientRect Lib "user32" (ByVal hWnd As Long, lpRect As RECT)

Public Sub GetClientArea(ByVal hWnd As Long, pRect As RECT)
  Exit Sub
End Sub


