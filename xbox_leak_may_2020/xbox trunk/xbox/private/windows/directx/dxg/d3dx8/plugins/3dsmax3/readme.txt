A 3DSMaxR3 -> X-file exporter with support for Physique modifiers.

Installation
------------

Copy XSkinExp.dle into <3DSMAXR3 path>\plugins

Usage
-----

1) Setup your scene.
2) Choose export (or export selected) from the File menu.
3) Select a filename through the standard dialog that should pop up.
4) Select an X-file data format to use (text, binary or binary+compressed).
5) Hit Go!

Notes
-----

1) The exporter currently only exports base objects (i.e. modifier stacks are ignored).  So any modifiers that you apply must be collapsed before export.  You can save your data to a MAX file before collapsing if you want to get your modifier stacks back.  The exception is the physique modifiers - see (2).

2) If you use physique, before export make sure that no modifiers exist below either one in the stack.  Any modifiers above or below physique are just ignored.

3) Currently only the diffuse texture is exported (absolute file path), and at most one set of texture coordinates (applied with a UVW modifier for example, and then collapsed before export).

4) Material properties exported are:
	- diffuse color
	- transparency
	- power (shininess strengh in 3DS... correct?)
	- specular color
	- emissive color
	- texture file name (diffuse texture absolute file path).

5) Materials may be per-mesh or per-face (multi-material).  

6) Faces are always triangles.

7) The export process guarantees that all frames (bones/nodes, whatever you want to call them) in the scene hierarchy get unique string names.  If a node has a name in 3DS, the exported name will use that, and append numbers if necessary to guarantee uniqueness.  3DS node names also have to be "censored" to support the character restrictions for strings in the x-file format.

8) See the Notes on New Templates to learn about extensions added to the X-File format...

9) The definitions of any new templates are always listed at the beginning of exported x-files.

Notes on New Templates
----------------------
The c header file appended to the bottom of this readme may be used to help read the custom templates added to the x-file format.

For now, these templates are:

1) VertexDuplicationIndices

        template VertexDuplicationIndices
        {
            <B8D65549-D7C9-4995-89CF-53A9A8B031E3>
            DWORD nIndices;
            DWORD nOriginalVertices;
            array DWORD indices[nIndices];
        }

This template is instantiated on a per-mesh basis, holding information about which vertices in the mesh are duplicates of each other.  Duplicates result when a vertex sits on a smoothing group or material boundary.  The purpose of this template is to allow the loader to determine which vertices exhibiting different peripheral parameters are actually the same vertex in the model.  Certain applications (mesh simplification for example) can make use of this information.

Fields: 

DWORD nIndices --  the number of indices in the array indices[nIndices].  This will be equal to the number of vertices in the current mesh.

DWORD nOriginalVertices -- the number of vertices in the mesh before any duplication occured.

array DWORD indices[nIndices] -- indices[n] holds the vertex index that vertex[n] in the vertex array for the mesh would have had if no duplication had occured.  So any indices in this array that are the same indicate duplicated vertices.

2) XSkinMeshHeader

        template XSkinMeshHeader
        {
            <3CF169CE-FF7C-44ab-93C0-F78F62D172E2>
            WORD nMaxSkinWeightsPerVertex;
            WORD nMaxSkinWeightsPerFace;
            WORD nBones; 
        }

This template is instantiated on a per-mesh basis only in meshes that contain exported skinning information.  The purpose of this template is to provide information about the nature of the skinning information that was exported.

Fields:
WORD nMaxSkinWeightsPerVertex -- the maximum number of transfoms that affect a vertex in the mesh.

WORD nMaxSkinWeightsPerFace -- the maximum number of unique transforms that affect the three vertices of any face.

WORD nBones -- the number of bones that affect vertices in this mesh

3) SkinWeights

        template SkinWeights
        { 
            <6F0D123B-BAD2-4167-A0D0-80224F25FABB> 
            STRING transformNodeName;
            DWORD nWeights; 
            array DWORD vertexIndices[nWeights]; 
            array float weights[nWeights]; 
            Matrix4x4 matrixOffset; 
        } 

This template is instantiated on a per-mesh basis.  Within a mesh, a sequence of n instances of this template will show up, where n is the number of bones (X file frames) which influence the vertices in the mesh.  Each instance of the template basically defines the influence of a particular bone on the mesh.  There is a list of vertex indices, and a corresponding list of weights.

Fields:
STRING transformNodeName -- the name of the bone whose influence we are defining.

DWORD nWeights -- the number of vertices affected by this bone

array DWORD vertexIndices[nWeights] -- the vertices influenced by this bone

array float weights[nWeights] -- the weights for each of the vertices influenced by this bone

Matrix4x4 matrixOffset -- this matrix transforms the mesh vertices to the space of the bone. This when cancatenated to the bone's transform will give the world space coordinates of the mesh as affected by the bone.

4) Addition to AnimationKey

	template AnimationKey
	{
	    <10DD46A8-775B-11cf-8F52-0040333594A3>
	    DWORD keyType;
	    DWORD nKeys;
	    array keys TimedFloatKeys[nKeys];
	}

This is an existing template in .X files. This has been extended to support a new key type of 4 which is a matrix key (in addition to position, rotation and scale). For example:

AnimationKey 
{
    4;
    2;
    0;16;   0.999841,-0.017505, 0.003500,0.000000,
            0.017664, 0.998492,-0.051979,0.000000,
           -0.002585, 0.052033, 0.998642,0.000000,
	   28.181364,-0.033845, 0.001760,1.000000;;,
    160;16; 0.999854,-0.016764, 0.003258,0.000000,
	    0.016922, 0.998239,-0.056864,0.000000,
	   -0.002299, 0.056911, 0.998377,0.000000,
	   28.181427,-0.033822, 0.001927,1.000000;;,
}
This was added to export animations from Biped plug-in. Biped does not give access to position / rotation / scale components of the animation. These matrices need to be decomposed by the application to get interpolable components.

Useful Header
-------------
The rest of this readme contains a useful header file, used in the export process, and useful for your loader, if you use the X File loading API in the DirectX SDK.

//-----------------------------------------------------------------------------
// File: XSkinExpTemplates.h
//
// Desc: Custom templates used for skin export format.
//
// Copyright (C) 1998-1999 Microsoft Corporation. All Rights Reserved.
//-----------------------------------------------------------------------------
#ifndef __XSKINEXPTEMPLATES_H__
#define __XSKINEXPTEMPLATES_H__

// {3CF169CE-FF7C-44ab-93C0-F78F62D172E2}
DEFINE_GUID(DXFILEOBJ_XSkinMeshHeader,
0x3cf169ce, 0xff7c, 0x44ab, 0x93, 0xc0, 0xf7, 0x8f, 0x62, 0xd1, 0x72, 0xe2);

// {B8D65549-D7C9-4995-89CF-53A9A8B031E3}
DEFINE_GUID(DXFILEOBJ_VertexDuplicationIndices, 
0xb8d65549, 0xd7c9, 0x4995, 0x89, 0xcf, 0x53, 0xa9, 0xa8, 0xb0, 0x31, 0xe3);

// {6F0D123B-BAD2-4167-A0D0-80224F25FABB}
DEFINE_GUID(DXFILEOBJ_SkinWeights, 
0x6f0d123b, 0xbad2, 0x4167, 0xa0, 0xd0, 0x80, 0x22, 0x4f, 0x25, 0xfa, 0xbb);

#define XSKINEXP_TEMPLATES \
        "xof 0303txt 0032\
        template XSkinMeshHeader \
        { \
            <3CF169CE-FF7C-44ab-93C0-F78F62D172E2> \
            WORD nMaxSkinWeightsPerVertex; \
            WORD nMaxSkinWeightsPerFace; \
        } \
        template VertexDuplicationIndices \
        { \
            <B8D65549-D7C9-4995-89CF-53A9A8B031E3> \
            DWORD nIndices; \
            DWORD nOriginalVertices; \
            array DWORD indices[nIndices]; \
        } \
        template SkinWeights \
        { \
            <6F0D123B-BAD2-4167-A0D0-80224F25FABB> \
            STRING transformNodeName;\
            DWORD nWeights; \
            array DWORD vertexIndices[nWeights]; \
            array float weights[nWeights]; \
            Matrix4x4 matrixOffset; \
        } \
#endif //__XSKINEXPTEMPLATES_H__