VERSION 4.00
Begin VB.Form Form1 
   Caption         =   "Debugger Automation Snap"
   ClientHeight    =   1344
   ClientLeft      =   5376
   ClientTop       =   1512
   ClientWidth     =   3492
   Height          =   1728
   Left            =   5328
   LinkTopic       =   "Form1"
   ScaleHeight     =   1344
   ScaleWidth      =   3492
   Top             =   1176
   Width           =   3588
   Begin VB.CommandButton StepThroughTest 
      Caption         =   "Step Through Test"
      Height          =   492
      Left            =   240
      TabIndex        =   1
      Top             =   720
      Width           =   3012
   End
   Begin VB.CommandButton RunTestAtFullSpeed 
      Caption         =   "Run Test At Full Speed"
      Height          =   492
      Left            =   240
      TabIndex        =   0
      Top             =   120
      Width           =   3012
   End
End
Attribute VB_Name = "Form1"
Attribute VB_Creatable = False
Attribute VB_Exposed = False
Const DBG_STOP As Integer = 0
Const DBG_BREAK As Integer = 1
Const DBG_RUN As Integer = 2

Dim flagStepThroughTest As Integer
Dim Application, Debugger As Object


Sub WaitUntilDebuggerState(state As Integer)
    While (Debugger.state <> state)
    Wend
End Sub


Sub StepComment(comment As String)
     
    OutputDebugString ("TEST: " + comment)
    AppActivate ("Debugger Automation Snap")
    
    If flagStepThroughTest Then
        
        ret = InputBox("", "Debugger Automation Snap", comment, 5000, 6000)
        
        If ret = "" Then
            End
        End If
        
    End If

End Sub


Sub OutputDebugString(str As String)
        Debug.Print (str)
End Sub


Private Sub TestDebugger()
    
    Set Application = CreateObject("msdevd.application")
    Set Debugger = Application.GetPackageExtension("Debugger")
    
	OutputDebugString("")
	OutputDebugString("*****************")
	OutputDebugString("* STARTING TEST *")
	OutputDebugString("*****************")

	'*** TEST ***
	' verify debugger is in stop mode with no project loaded.
	DebuggerStateIs(DBG_STOP)

	'*** TEST ***
	' start debugging with no project loaded.
	' TODO (michma): re-enable when orion: 8109 is fixed.
	'Debugger.StepInto
	'DebuggerStateIs(DBG_STOP)

	'*** TEST ***
	' set and remove a breakpoint with no project loaded.
	' TODO (michma): how do we verify this?
	Debugger.SetBreakpoint
	Debugger.ClearBreakpoint

	'*** TEST ***
	' evaluate constant expression with no project loaded.
	If(Debugger.Evaluate("1 + 1") <> "2") Then
		OutputDebugString("FAIL: evaluate constant expresion with no project loaded")
	End If

	'*** TEST ***
	' evaluate variable expression with no project loaded.
	' TODO(michma): re-enable when orion: 8056 is fixed.
	'If(Debugger.Evaluate("i") <> "some error message") Then
	'	OutputDebugString("FAIL: evaluate variable expresion with no project loaded")
	'End If

	'*** TEST ***
	' verify Debugger.Application ==  Application.
	If(Debugger.Application <> Application) Then
		OutputDebugString("* FAIL * Application")
	End If

	'*** TEST ***
	' verify Debugger.Parent == Application.
	If(Debugger.Parent <> Application) Then
		OutputDebugString("* FAIL * Parent")
	End If

	'*** TEST ***
	'TODO(michma): the proceeding jit/rpc tests may need to be changed depending
	'on the resolution to orion: 7910. new tests may also need to be added.

	'*** TEST ***
	' verify jit is off by default.
	If(Debugger.JustInTimeDebugging) Then
		OutputDebugString("* FAIL * get just-in-time debugging")
	End If

	'*** TEST ***
	' turn jit on.
	Debugger.JustInTimeDebugging = True
	If(Not Debugger.JustInTimeDebugging) Then
		OutputDebugString("* FAIL * turn on just-in-time debugging")
	End If

	'*** TEST ***
	' turn jit off.
	Debugger.JustInTimeDebugging = False
	If(Debugger.JustInTimeDebugging) Then
		OutputDebugString("* FAIL * turn off just-in-time debugging")
	End If

	'*** TEST ***
	' verify rpc is off by default.
	If(Debugger.RemoteProcedureCallDebugging) Then
		OutputDebugString("* FAIL * get remote-procedure-call debugging")
	End If

	'*** TEST ***
	' turn rpc on.
	Debugger.RemoteProcedureCallDebugging = True
	If(Not Debugger.RemoteProcedureCallDebugging) Then
		OutputDebugString("* FAIL * turn on remote-procedure-call debugging")
	End If

	'*** TEST ***
	' turn rpc off.
	Debugger.RemoteProcedureCallDebugging = False
	If(Debugger.RemoteProcedureCallDebugging) Then
		OutputDebugString("* FAIL * turn off remote-procedure-call debugging")
	End If

	' open project used to test automation
	Application.Documents.Open("autodbg.mak")
	Application.Build

	'*** TEST ***
	comment = "set a bp while not debugging, and go"
	StepComment(comment)
	Application.Documents.Open("autodbg.cpp")
	' TODO (michma): change this to set a bp on "WinMain".
	Application.ActiveDocument.Selection.LineDown 0, 9
	Debugger.SetBreakpoint
    Debugger.Go
	DebuggerIsAtSymbol("WinMain")

	' TODO (michma): clear a bp while not debugging

 	'*** TEST ***
    comment = "run to cursor"
	StepComment(comment)
    Application.ActiveDocument.Selection.LineDown 0, 2
    Debugger.RunToCursor
	DebuggerIsAtSymbol("AFTER_RUN_TO_CURSOR")

	'*** TEST ***
    comment = "set breakpoint"
	StepComment(comment)
    Application.ActiveDocument.Selection.LineDown 0, 2
    Debugger.SetBreakpoint

	'*** TEST ***
    comment = "go to breakpoint"
	StepComment(comment)
    Debugger.Go
	DebuggerIsAtSymbol("AFTER_GO_TO_BREAKPOINT")
	
	'*** TEST ***
    comment = "clear breakpoint"
	StepComment(comment)
    Debugger.ClearBreakpoint

	'*** TEST ***
	comment = "show next statement"
	StepComment(comment)
	Application.ActiveDocument.Selection.LineDown 0, 10
	Debugger.ShowNextStatement
	If(Application.ActiveDocument.Name <> "autodbg.cpp") Or _
	  (Application.ActiveDocument.Selection.CurrentLine <> 14) Then
		OutputDebugString("* FAIL * " + comment)
	End If	

	'*** TEST ***
    comment = "get radix"
	StepComment(comment)
	If(Debugger.DefaultRadix <> 10) Then
		OutputDebugString("* FAIL * " + comment)
	End If
	
	'*** TEST ***
	comment = "set radix to 16"
	StepComment(comment)
	Debugger.DefaultRadix = 16
	If(Debugger.DefaultRadix <> 16) Or _
	  (Debugger.Evaluate("i") <> "0x00000000") Then
		OutputDebugString("* FAIL * " + comment)
	End If

	'*** TEST ***
	comment = "set radix to 10"
	StepComment(comment)
	Debugger.DefaultRadix = 10
	If(Debugger.DefaultRadix <> 10) Or (Debugger.Evaluate("i") <> "0") Then
		OutputDebugString("* FAIL * " + comment)
	End If

	'*** TEST ***
	comment = "step over function"
	StepComment(comment)
    Debugger.StepOver
	DebuggerIsAtSymbol("AFTER_STEP_OVER")

	'*** TEST ***
    comment = "step into function"
	StepComment(comment)
    Debugger.StepInto
	DebuggerIsAtSymbol("AFTER_STEP_INTO")

	'*** TEST ***
    comment = "step out of function"
	StepComment(comment)
    Debugger.StepOut
	DebuggerIsAtSymbol("AFTER_STEP_OUT")

	'*** TEST ***
    comment = "restart debugger"
	StepComment(comment)
    Debugger.restart
	DebuggerIsAtSymbol("WinMain")

	'*** TEST ***
    'TODO(michma):re-enable this case once asynchronous option is available
    'comment = "break into the debugger"
	'StepComment(comment)
    'Debugger.Go
	'DebuggerStateIs(DBG_RUN)
    'Debugger.Break
	'DebuggerStateIs(DBG_BREAK)
        
	'*** TEST ***
    comment = "stop debugger"
	StepComment(comment)
    Debugger.Stop
	DebuggerStateIs(DBG_STOP)

	'*** TEST ***
    comment = "run app stand-alone"
	StepComment(comment)
    Debugger.RunStandalone

	Application.Documents.CloseAll

End Sub

Sub DebuggerIsAtSymbol(symbol)
    If (Debugger.Evaluate("EIP == " + symbol) <> "1") Then
        OutputDebugString ("* FAIL * " + comment)
    End If
End Sub

Sub DebuggerStateIs(state)
    If (Debugger.state <> state) Then
        OutputDebugString ("* FAIL * " + comment)
    End If
End Sub

Private Sub RunTestAtFullSpeed_Click()
    flagStepThroughTest = False
    TestDebugger
End Sub


Private Sub StepThroughTest_Click()
    flagStepThroughTest = True
    TestDebugger
End Sub


