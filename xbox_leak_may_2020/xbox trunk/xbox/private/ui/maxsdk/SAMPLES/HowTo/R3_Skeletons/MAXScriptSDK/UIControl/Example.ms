utility SUITest "SUITest"
(
	SimpleUI_Control SUIc "tester" width:100 height:100 align:#center
	label posout "[ , ]"
	
	on SUIc clicked pos do
	(
		posout.text = pos as string
		SUIc.inout = not SUIc.inout
	)	
)