'------------------------------------------------------------------------------
' Common Library for VBScripts
' emmang@microsoft.com
'
' Exposes:
'
'   Function Init()        - initializes global variables
'   ProcessSubdirs(str)    - recursively processes a specified directory
'   ProcessFiles(str)      - processes the files in a specified directory
'
' Scripts that invoke ProcessSubdirs/ProcessFiles expose
' the following function[s]:
'
'   ProcessSingleFile(str) - process a single file

On Error Resume Next
Const ABORTFILE = "C:\abort.tmp"
Dim gobjFileSystem, gobjShell, gstrXDKInstall, gstrMsdevInstall

Function Init()
	On Error Resume Next
	'
	' create global tool objects
	'
	Set gobjFileSystem = CreateObject("Scripting.FileSystemObject")
    gobjFileSystem.DeleteFile(ABORTFILE)
	Set gobjShell = CreateObject("WScript.Shell")
	'
	' get location of Xbox SDK and VC++ IDE
	'
	gstrXDKInstall = gobjShell.RegRead("HKLM\Software\Microsoft\XboxSDK\InstallPath")
	gstrMsdevInstall = gobjShell.RegRead("HKLM\Software\Microsoft\Shared Tools\msdev.exe\Path")
	WScript.Echo "Xbox SDK Location: " & gstrXDKInstall & vbCrLf & _
            "Msdev Location: " & gstrMsdevInstall & vbCrLf & _
            "Click [OK] to continue"

    If ((gstrXDKInstall <> "") And (gstrMsdevInstall <> "")) Then
        Init = True
    Else
        WScript.Echo "Cannot find XDK and/or MsDev, aborting..."
        Init = False
    End If
End Function


Sub ProcessSubdirs(strFolderSpec)  
	' On Error Resume Next
	Dim objRoot, objFolder, objSubFolders
	Set objRoot = gobjFileSystem.GetFolder(strFolderSpec)
	Set objSubFolders = objRoot.SubFolders
    ProcessFiles(strFolderSpec)
	For each objFolder in objSubFolders
		ProcessFiles(objFolder.Path)
		ProcessSubdirs(objFolder.Path)
	Next  
End Sub


Sub ProcessFiles(strFolderSpec)
	' On Error Resume Next
	Dim objFolder, objFile, objFileCollection, objDict

	Set objFolder = gobjFileSystem.GetFolder(strFolderSpec)
	Set objFileCollection = objFolder.Files

    Set objDict = CreateObject("Scripting.Dictionary")

	For Each objFile In objFileCollection
        If gobjFileSystem.FileExists(ABORTFILE) Then
            Exit Sub
        End If

        If Not objDict.Exists(lcase(objFile.Path)) Then
		    ProcessSingleFile(objFile)
            objDict.Add lcase(objFile.Path), 1
        End If
	Next  
End Sub

