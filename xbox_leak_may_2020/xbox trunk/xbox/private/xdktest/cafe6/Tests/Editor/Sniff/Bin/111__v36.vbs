'**********************************************************************************
' SAMPLE.VBS This file is a collection of sample editor macros. 
'**********************************************************************************
'MACROS:TestActive

Function Description(ByVal sMacroName)
  If (sMacroName = "TestActive") Then
    Description = "Calls a function to see if the IDE is active (a dummy)"
  Elseif (sMacroName = "") Then
    Desctiption = "CAFE VBS Sniff - Checks to make sure that VBS is enabled"
  Else
		Description = "Description unavailable."
	End If
End Function

Sub TestActive()
  ActiveDocument.Selection.Copy
End Sub

