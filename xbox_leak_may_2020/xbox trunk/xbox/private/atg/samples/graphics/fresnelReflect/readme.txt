//-----------------------------------------------------------------------------
// Name: Fresnel Reflectivity Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample demonstrates Fresnel Reflectivity.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   This sample shows how to make glass or other shiny things look much more realistic 
by adding a Fresnel Reflection to the Sphere Map.  Most Reflective objects do not 
reflect perfectly over their entire surfaces.  In general, the more direct the view
angle to surface normal, the less reflective the substance is and the more oblique 
the view angle, the more reflective it is.  The fresnel3 shader has been setup to 
demonstrate this.  Also included are two other shaders which show Fresnel 
transparency using the same technique.  They are viewed by changing the 
object to the untextured teapot.  You can see how the frensel3 shader gives an 
almost frosted glass look while the frensel2 shader gives a holographic look to 
object.  The fresnel1 shader is a "pure" Fresnel shader without the biases applied 
to the fresnel3 shader.
   Also included is an example of using the fresnel-type shader for reflectivity on
a car.  Note how the same shader can be used for the windows and the main body by
just changing the constants passed to it.
   Also included is an example of using the fresnel-type shader for transparency.
Changing the object to the planet Earth, we use the fresnel transparency shader to
simulate the thickening of the atmosphere at the planet's limb.  In order to keep
the sample focused on the fresnel shader, the earth is drawn as three separate globes.
The first is the color map, the second is the cloud map and the third is the fresnel
transparency.
