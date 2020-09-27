Setting up the environment

	Required things
	-	Full Xbox Source Tree
	-	Xbox Source Tree Update v1.2
	
	Extract the Full Xbox Source Tree in a folder (this document will assume C:\XBOX\).
	Extract the CPXXUPD.ZIP zip file into "C:\XBOX\CPXXUPD" folder.
	
Preparing your RAZZLE environment

	Open a DOS Prompt and go into "C:\XBOX\CPXXUPD"
	Execute "CPXXUPD.CMD C:\XBOX".
	Go into "C:\XBOX\PRIVATE\DEVELOPR\TEMPLATE\"
	Execute "INITRAZZLE.CMD C: NTROOT \XBOX".
	Go into "C:\XBOX\PRIVATE\DEVELOPR\<your username>\"
	Execute "DORAZZLE.CMD C: NTROOT \XBOX SETFRE".

Commands to remember

	CPXR.CMD - Rebuilds.
	CPXB.CMD - Builds.
	SETFRE.CMD - Switches building mode to Release
	SETCHK.CMD - Switches building mode to Debug
	CPXON - Builds with all the COMPLEX hacks, improvments (default).
	CPXOFF - Builds as-is, no hacks.
	DEVKITON - Builds for devkit, not for public (default).
	DEVKITOFF - Builds for public, not for devkit.
	FOCUSON - Builds for focus video tuner.
	FOCUSOFF - Builds for 871 viden tuner (default).
	SHOWOPT.CMD - Shows your the options you've got setup for building.

