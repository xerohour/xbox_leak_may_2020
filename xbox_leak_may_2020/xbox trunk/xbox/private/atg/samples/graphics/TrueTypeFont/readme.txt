//-----------------------------------------------------------------------------
// Name: TrueTypeFont Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The TrueTypeFont sample shows how to use a Windows TrueType font and bitmap
   fonts created by the MakeFont.exe tool to draw text on the Xbox.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   To compile the Bitmap files using the makefont.exe tool, run
   
   makefont -d media\fonts arial16Normal.rsp arial16normal.bmf
   makefont -s ITALICS -d media\fonts arial24Italic.rsp arial24Italic.bmf
   makefont -d media\fonts arial16kanji.rsp arial16kanji.bmf
   makefont -d media\fonts arial16katakana.rsp arial16katakana.bmf
   makefont -d media\fonts arial16hiragana.rsp arial16hiragana.bmf
   
   We load five fonts in the sample.  The first is the default font supplied in
   the XDK libraries.  The second font is the arialuni.ttf.  Loading it as a
   TrueType font allows you to set the font parameters like height, style, and
   anti-alias on the fly.  This is very slow but it can be done.  The third font
   is a bitmap font created by the MakeFont tool from a TrueType font.  It shows
   how you can create the font with just a file name.  The fourth font was also
   created using the MakeFont tool and it is loaded by passing in the memory
   image of the file instead of the file name.  This allows apps to control the
   allocation of the memory for the raw font data.  It is important that the 
   memory for the font NOT be deleted until you no longer need it to draw text!
   The fifth font shows how to display non-western characters from a non-western
   bitmap font.  

   It is very important to ALWAYS include the lower case 'g' with any font created
   using the makefont tool or the font will not load on the Xbox.  This character
   is U+0067.

