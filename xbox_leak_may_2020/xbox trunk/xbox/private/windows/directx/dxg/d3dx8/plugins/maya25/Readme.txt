======================
 MAYA X-FILE EXPORTER
======================


Installation
------------

	1] Make sure that Maya is not running
	2] Copy "xExport.mll" to "<mayapathname>\bin\plug-ins"
	3] Copy "xfileTranslatorOpts.mel" to "<mayapathname>\scripts\others"
	4] Copy "bicubicBezierPatches.mel" to "<mayapathname>\scripts\others"
	5] Run Maya
	6] Select "Window->General Editors->Plug-in Manager" from the main menu bar
	7] Turn on the "auto load" checkbox next to "xExport.mll"
	8] Turn on the "loaded" checkbox next to "xExport.mll"


Usage
-----

	1] Select "File->Export All" from the main menu bar
	2] Select "XFile (*.*)" from the "File of type" drop-down menu
	3] Select a file name from the browse window
	4] Click the "Export" button
		-OR-
	   Click the "Options..." button for further options (recommended)

Notes
-----
	- If a material contains a texture, then the diffuse factor is used for the RGB components
		of the face color.
	- See the notes on New Templates to learn about extensions added to the X-File format.
	- TRS animation (as opposed to vertex animation) is exported.
	- Polygons are not triangulated.
	- Use the MEL command bicubicBezierPatches() to convert a NURBS surface into a
		bicubic-bezier-quad-patch-mesh.  
	- If a NURBS surface is not bicubic-bezier-quad-patch-mesh then it will be tesselated.
	- Usually setting the display quality to 1 works best.  If the operation fails, try again
		with the display quality set to 2, and then if it still doesn't work, try with
		the display quality set to 3.


Issues
------
	- Unique names for meshes and joints are not checked for.
	- Scenes created with older versions of Maya may not be exported correctly.
	- Since different viewers may interpolate between TRS keyframes differently, animation of 	
		an object is not guaranteed to look the same as in Maya except at the keyframes of 
		that object.  To overcome this, choose Animation Mode to be "regular" instead of
		keyframed.
	- Currently bezier-patch exporting is limited to at most 1 material per mesh.
	- Currently bezier-patch exporting does not include skinning information.