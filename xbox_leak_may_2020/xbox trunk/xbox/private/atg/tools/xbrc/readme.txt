XBox Resource Compiler
Version: 0.2
Date: 2001.11.9
Author: Jed Lengyel

This is the 2nd pass prototype of xbrc, built on top of a snapshot of
the bundler for .xpr file creation.  The final format for the resource
source files (see xdx.txt) and destination files (see xbr.txt) is still
being worked out. The xbrc implementation needs a lot of cleanup.

BUILD NOTES

Run msxml3.exe before compiling to install msxml3.dll. The xbrc tool
uses msxml3.dll for XML parsing.


MOTIVATION

Currently on the Xbox, there are several DirectX file formats to
describe the state needed to draw an object. 

X files (.x) provide a source format for geometry and animation data,
but in a custom pre-XML style of describing the data schema.

The MakeXBG converter takes .x files as input to create .xbg files.
The .xbg files retain frame transformation structure and are quick to
load, but do not have animation or skinning data, or general vertex
shaders or vertex buffers.

The 'bundler' takes a custom input format (.rdf) and produces textures
and vertex buffers in a format (.xpr) that is quick to load on the
Xbox.

Effect files (.fx) describe vertex shaders, pixel shaders, render
state, and multiple passes, but do not include geometry data, and are
not really supported on the Xbox.


BENEFITS

Putting all of these separate file formats into a common schema has
the following benefits:

* Source Format for Content

The art should be part of the source tree, just like the code, for
better history tracking, change merging, etc.

* Fast Prototyping and Preview

Currently, each new game effort has to start by creating a scene
description language and then writing a previewer.  This takes a lot
of effort.  By giving developers an example language and an starting
viewer (with links across the wire to the dev machine) will give both
game developers and middleware developers a good place to start.

Artists must optimize the art with direct visual feedback on the
Xbox. Other optimization paths (like viewing on a PC monitor) have
problems with color matching and performance estimation.

Imagine doing gameplay first, with rough models, instead of six months
into the project!

* Compilation and Optimization

Compilation should be good (when implemented) at catching things that
people are not so good at.  By having all the state needed for a
"graphics subroutine call" in the same description file, the compiler
should be able to catch things like the following:

   * Mismatched vertex buffer formatting and vertex shader
     declarations.
   
   * Unused inputs to or outputs from the shaders that need not be
     calculated.  (Vertex buffer data, vertex registers, pixel
     registers, render state settings, etc., that do not effect the output.)
   
   * Outputs missing from the vertex shader needed by pixel shader.

   * Mismatch between texture type and texture modes in pixel shader.
   
Having all the render and texture state in a single description will
allow the minimization of state changes across the scene graph.  The
compilation phase can target the most efficient rendering scheme on
the Xbox (compiled PushBuffers, for example.)

Compression and level-of-detail filtering is currently done only for
images.  Meshes and animations need to be optimized, filtered, and
compressed, too.

Conversion of geometry to and from images is useful for things like height
fields, displacement maps, and normal maps.

Trying to do this with .x files is possible, but not as clean as using
XML, since the low-level geometry descriptions that have become
standard are pretty far from the data types actually used in DirectX
on the XBox.

* Debugging

The low-level Xbox graphics types (vertex buffers, index buffers,
textures, vertex shaders, vertex shader constants, pixel shaders, and
render states) can be converted back to the standard input format for
display and modification.

* XML Tools

The XML tools that are being developed in the .Net framework will be
able to parse, transform, and put scene objects in and out of
databases.
