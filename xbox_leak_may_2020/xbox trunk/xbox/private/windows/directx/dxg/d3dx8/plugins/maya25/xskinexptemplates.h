//-----------------------------------------------------------------------------
// File: XSkinExpTemplates.h
//
// Desc: Custom templates used for skin export format.
//
// Copyright (C) 1998-2000 Microsoft Corporation. All Rights Reserved.
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

// {A3EB5D44-FC22-429d-9AFB-3221CB9719A6}
DEFINE_GUID(DXFILEOBJ_Patch, 
0xa3eb5d44, 0xfc22, 0x429d, 0x9a, 0xfb, 0x32, 0x21, 0xcb, 0x97, 0x19, 0xa6);

// {D02C95CC-EDBA-4305-9B5D-1820D7704BBF}
DEFINE_GUID(DXFILEOBJ_PatchMesh, 
0xd02c95cc, 0xedba, 0x4305, 0x9b, 0x5d, 0x18, 0x20, 0xd7, 0x70, 0x4b, 0xbf);

#define XSKINEXP_TEMPLATES \
        "xof 0303txt 0032\
        template XSkinMeshHeader \
        { \
            <3CF169CE-FF7C-44ab-93C0-F78F62D172E2> \
            WORD nMaxSkinWeightsPerVertex; \
            WORD nMaxSkinWeightsPerFace; \
            WORD nBones; \
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
        template Patch \
        { \
            <A3EB5D44-FC22-429D-9AFB-3221CB9719A6> \
            DWORD nControlIndices; \
            array DWORD controlIndices[nControlIndices]; \
        } \
        template PatchMesh \
        { \
            <D02C95CC-EDBA-4305-9B5D-1820D7704BBF> \
            DWORD nVertices; \
            array Vector vertices[nVertices]; \
            DWORD nPatches; \
            array Patch patches[nPatches]; \
        } "


#endif //__XSKINEXPTEMPLATES_H__

