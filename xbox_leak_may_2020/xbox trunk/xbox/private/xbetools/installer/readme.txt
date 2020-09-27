This tool is designed to help you create installable demos discs. To create an installable
demo CD, you need to place the files in this package as described below. Note: the CD-R or
DVD-R that you create must be in UDF format. The Xbox does not the support CDFS file system,
so CDFS CD's will not boot.

To create an install, place the default.xbe file in the root of a UDF cd image. Make sure
it is named "default.xbe". Place the media folder in the root directory as well.

Place all of your files and directories under a folder named "Files", that lives in the root
of the CD. Every file or folder in Files\<file> will be placed in on the Xbox hard drive at
E:\<file>. Consider the sample CD layout below:

CD IMAGE
========
default.xbe		(from this demo installer zip file)
\Media
	resource.xpr	(from this demo installer zip file)
	font.xpr	(from this demo installer zip file)

\Files			(everything below here is your files)
	\Samples
		\Explosion
			Explosion.xbe
			\Media
				resource.xpr
				font.xpr

Files added after installing on the Xbox hard drive
===================================================
XE:\
	\Samples
		\Explosion
			Explosion.xbe
			\Media
				resource.xpr
				font.xpr

You can run the demo installer multiple times and it will copy over existing files and
directories (allowing you to upgrade build over build).

Notes:
	- all files are copied without file attributes (that means no file will be
	  read-only, hidden, or system)

	- if an existing file is found at the install point, it is overwritten

	- if you don't have enough disk space for a clean install, you will be prompted
	  and warned but you can still install (thus allowing build-over-build upgrades)
