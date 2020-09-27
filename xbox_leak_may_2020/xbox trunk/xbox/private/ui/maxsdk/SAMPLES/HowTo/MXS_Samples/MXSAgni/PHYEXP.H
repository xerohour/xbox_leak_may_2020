/******************************************************************************
 *<
	FILE: PhyExp.h
				  
	DESCRIPTION:  Export Interface Functionality for Physique

	CREATED BY: John Chadwick with a lot of help from Jurie Horneman

	HISTORY: created July 10, 1997, modified for Version 2.0 March 5, 1998

 *>     Copyright (c) Unreal Pictures, Inc. 1997, 1998 All Rights Reserved.
 *******************************************************************************/

#ifndef PHYEXP_H
#define PHYEXP_H

#ifdef BLD_PHYSIQUE
#define PHYExport __declspec( dllexport )
#else
#define PHYExport __declspec( dllimport )
#endif

#include "max.h"
#include "modstack.h"

// this is the interface ID for a Physique Modifier	Interface
#define I_PHYINTERFACE	0x00100100 


// version control 
#define CURRENT_VERSION	1


// these are the Class ID defines for Physique Modifier: 
// Physique Class_ID = ClassID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B);
#define PHYSIQUE_CLASS_ID_A 0x00100
#define PHYSIQUE_CLASS_ID_B	0x00000


#define RIGID_TYPE		1
#define	DEFORMABLE_TYPE	2
#define	BLENDED_TYPE	4		

#define RIGID_NON_BLENDED_TYPE		(RIGID_TYPE)
#define	DEFORMABLE_NON_BLENDED_TYPE (DEFORMABLE_TYPE)
#define RIGID_BLENDED_TYPE			(RIGID_TYPE | BLENDED_TYPE)
#define DEFORMABLE_BLENDED_TYPE		(DEFORMABLE_TYPE | BLENDED_TYPE)


// Using the Physique Export Interface:
// 
//  1.	Find the Physique Modifier you which to export rigid vertex assignments from.
//		(there is a comment at the bottom of this file giving an example of this)
//
//	2.	Given Physique Modifier Modifier *phyMod get the Physique Export Interface:
//		IPhysiqueExport *phyExport = (IPhysqiueExport *)phyMod->GetInterface(I_PHYINTERFACE);		
//
//  3.	For a given Object's INode get this ModContext Interface from the Physique Export Interface:
//		IPhyContextExport *contextExport = (IPhyContextExport *)phyExport->GetContextInterface(INode* nodePtr);
//
//  4.  For each vertex of the Object get the Vertex Interface from the ModContext Export Interface:
//		IPhyVertexExport *vtxExport = (IPhyVertexExport *)contextExport->GetVertexInterface(int i);	
//		NOTE: only Rigid Vertices are currently supported: (see ConvertToRigid(TRUE) to make all vertices rigid)
//		IPhyRigidVertex *rigidExport = (IPhyRigidVertex *)contextExport->GetVertexInterface(int i);
//
//  5.	Get the INode for each Rigid Vertex Interface:
//		INode *nodePtr = rigidExport->GetNode();
//
//  6.	Get the Point3 Offset Vector in INode coordinates for each Rigid Vertex Interface:
//		Point3 offsetVector = rigidExport->GetOffsetVector();





// IPhyVertexExport: this is the base class for Vertex Export Interface
//					 NOTE: currently only RIGID_TYPE vertices are supported (IPhyRigidVertex)
//					 When a vertex is not assigned Rigid in Physique, the VertexInterface will be NULL
//					 Unless you call IPhyContextExport->ConvertToRigid(TRUE) (see IPhyContextExport below)
//					 With ConvertToRigid(TRUE) you will always get a IPhyRigidVertex 
//					 from IPhyContextExport->GetVertexInterface(i) 

class IPhyVertexExport
{
	public:

		PHYExport virtual ~IPhyVertexExport() {}

		// NOTE: currently only type RIGID_TYPE is supported
		PHYExport virtual int GetVertexType() = 0;
};



class IPhyRigidVertex : public IPhyVertexExport
{
	public:

		PHYExport virtual ~IPhyRigidVertex() {}

		// GetNode() will return the INode pointer of the link of the given VertexInterface
		PHYExport virtual INode *GetNode() = 0;

		// GetOffsetVector() will return the coordinates of the vertex 
		// in the local coordinates of associated INode pointer from GetNode
		// this is NOT THE SAME as the .vph file coordinates. (It is simpler)
		// the world coordinates of the vertex have been transformed by the Inverse	of the INode pointer.
		PHYExport virtual Point3 GetOffsetVector() = 0;
};

class IPhyBlendedRigidVertex : public IPhyVertexExport
{
	public:

		PHYExport virtual ~IPhyBlendedRigidVertex() {}

		// GetNumberNodes() returns the number of nodes assigned to the given VertexInterface
		PHYExport virtual int GetNumberNodes() = 0;

		// GetNode(i) will return the i'th INode pointer of the link of the given VertexInterface
		PHYExport virtual INode *GetNode(int i) = 0;

		// GetOffsetVector(i) will return the coordinates of the vertex 
		// in the local coordinates of associated i'th INode pointer from GetNode(i)
		// this is NOT THE SAME as the .vph file coordinates. (It is simpler)
		// the world coordinates of the vertex have been transformed by the Inverse	of the INode pointer.
		PHYExport virtual Point3 GetOffsetVector(int i) = 0;

		// GetWeight(i) will return the weight of the vertex associated with the i'th Node
		// pointer from GetNode(i)
		PHYExport virtual float GetWeight(int i) = 0;
};





// IPhyContextExport: This class can be returned by :
// passing the INode pointer of an Object with the given Physique Modifier's IPhysiqueExport::GetContextInterface(node)
// If this Node does contain the Physique Modifier associated with this interface an interface to this object's
// interface is returned (several object's may share the same physique modifier), else returns NULL.

class IPhyContextExport
{
	public:

		PHYExport virtual ~IPhyContextExport() {}
		
		// this returns the number of vertices for the given modContext's Object
		PHYExport virtual int GetNumberVertices() = 0;

		// If ConvertToRigid is set to TRUE, all GetVertexInterface calls will be IPhyRigidVertex OR IPhyBlendedRigidVertex.
		// Vertices which are not Rigid in Physique will be converted to Rigid for the VertexInterface.
		// When ConvertToRigid is set to FALSE, GetVertexInterface for non-Rigid vertices currently returns NULL.
		// By default, if you do not call ConvertToRigid, it is set to FALSE.
		PHYExport virtual void ConvertToRigid(BOOL flag = TRUE) = 0;

		// If AllowBlending is set to FALSE then GetVertexInterface will return a non-blended subclass.
		// Currently the only valid VertexInterface subclasses are IPhyRigidVertex, and IPhyBlendedRigidVertex.
		// When AllowBlending is FALSE, either IPhyRigidVertex or NULL will be returned.
		// If ConvertToRigid is TRUE and AllowBlending is FALSE, all vertices will return
		// IPhyRigidVertex. By default AllowBlending is set to TRUE.
		PHYExport virtual void AllowBlending(BOOL flag = TRUE) = 0;

		// GetVertexInterface return's a VertexInterface (IPhyVertexExport *) for the i'th vertex.
		// If ConvertToRigid has been set to TRUE, then all VertexInterfaces will return IPhyRigidVertex OR IPhyBlendedRigidVertex.
		// When ConvertToRigid has been set to FALSE, non-Rigid vertices will return NULL (CURRENTLY).
		PHYExport virtual IPhyVertexExport *GetVertexInterface(int i) = 0;

		// You must call ReleaseVertexInterface to delete the VertexInterface when finished with it.
		PHYExport virtual void ReleaseVertexInterface(IPhyVertexExport *vertexExport) = 0;
};


// IPhysiqueExport: This class can be returned by calling the method GetInterface() for a Physique Modifier
// given Modifier *mod points to a Physique Modifier, then:
// IPhysiqueExport *phyInterface = (IPhysiqueExport *)( mod->GetInterface(I_PHYINTERFACE) );
// will return the interface for this Physique Modifier, else returns NULL.

class IPhysiqueExport
{
	public:
		
		PHYExport virtual ~IPhysiqueExport() {}

		//GetContextInterface will return a pointer to IPhyContextExport Interface for a given INode.
		// Ff the given INode does not contain the Physique Modifier of this IPhysique Export then NULL is returned.
		PHYExport virtual IPhyContextExport *GetContextInterface(INode* nodePtr) = 0;

		// You must call ReleaseContextInterface to delete the COntextInterface when finished with it.
		PHYExport virtual void ReleaseContextInterface(IPhyContextExport *contextExport) = 0;
};

// example code to find if a given node contains a Physique Modifier
// DerivedObjectPtr requires you include "modstack.h" from the MAX SDK

/*Modifier* FindPhysiqueModifier (INode* nodePtr)
{
	// Get object from node. Abort if no object.
	Object* ObjectPtr = nodePtr->GetObjectRef();
	

	if (!ObjectPtr) return NULL;

	// Is derived object ?
	if (ObjectPtr->SuperClassID() == GEN_DERIVOB_CLASS_ID)
	{
		// Yes -> Cast.
		IDerivedObject* DerivedObjectPtr = static_cast<IDerivedObject*>(ObjectPtr);

		// Iterate over all entries of the modifier stack.
		int ModStackIndex = 0;
		while (ModStackIndex < DerivedObjectPtr->NumModifiers())
		{
			// Get current modifier.
			Modifier* ModifierPtr = DerivedObjectPtr->GetModifier(ModStackIndex);

			// Is this Physique ?
			if (ModifierPtr->ClassID() == Class_ID(PHYSIQUE_CLASS_ID_A, PHYSIQUE_CLASS_ID_B))
			{
				// Yes -> Exit.
				return ModifierPtr;
			}

			// Next modifier stack entry.
			ModStackIndex++;
		}
	}

	// Not found.
	return NULL;
}*/




#endif


