/**********************************************************************
 *<
 *> Copyright (c), All Rights Reserved.
 **********************************************************************/

#define STRICT
#include <strbasic.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <math.h>
#include "gfxlib.h"
#include "proflib.h"
#include "mesh.h"

// 3DS DOS did this backwards!
#define ABLINE (1<<2)
#define BCLINE (1<<1)
#define CALINE 1

#define VSEL_A 0x8000		/* Group selectors (same as faces) */
#define VSEL_B 0x4000
#define VSEL_C 0x2000


static int Progress(int percent,void *data)
	{
	if (data) {
		MeshOpProgress *p = (MeshOpProgress*)data;
		return p->Progress(percent);
	} else {
		return 1;
		}
	}
typedef struct {
 Point3	pt,Norm;
} SavePt;

/*void FillInUniform(int infaces,int c,int vertices,Mesh *mesh,ParticleSys *parts,SavePt *s,Point3 *lastv,Face *lastf,Matlist m)
{ int i,*lst;
  float *arealst,TotalArea,rx,ry;
  Point3 Norm;

  arealst=new float[infaces];
  lst=new int[infaces];
  assert(arealst && lst);
  TotalArea=0.0f;
  for (i=0;i<infaces;i++)
    TotalArea+=(arealst[i]=GetFaceArea(&mesh->faces[i].v[0],mesh->verts));
  SpreadOutParts(arealst,TotalArea,infaces-1,infaces,c,lst);
  Point3 V0,V1,V2,V01,V02,lastpt,lV0,lV01,lV02;
  for (i=0;i<infaces;i++)
  { V01=(V1=mesh->verts[mesh->faces[i].v[1]])-(V0=mesh->verts[mesh->faces[i].v[0]]);
    V02=(V2=mesh->verts[mesh->faces[i].v[2]])-V0;
	lV01=(lastv[lastf[i].v[1]])-(lV0=lastv[lastf[i].v[0]]);
    lV02=(lastv[lastf[i].v[2]])-lV0;
    Norm=GetNormalizedNormal(&mesh->faces[i].v[0],mesh->verts);
    for (int j=0;j<lst[i];j++)
    { parts->vels[vertices]=Norm;
      rx=RND01();ry=RND01();
      if (rx+ry>1) {rx=1-rx;ry=1-ry;}
	  parts->points[vertices]=V0+V01*rx+V02*ry;
	  if (s[vertices].M>0.0f)
	  { lastpt=lV0+lV01*rx+lV02*ry;
	    s[vertices].Ve=parts->points[vertices]*(m.newtm)-lastpt*(m.oldtm);
	  }
	  if (doTVs)
	  { s[vertices].pmtl=mesh->faces[i].getMatID();
	    Point3 tv0=mesh->tVerts[mesh->tvFace[i].t[0]];
		Point3 tv1=mesh->tVerts[mesh->tvFace[i].t[1]];
		Point3 tv2=mesh->tVerts[mesh->tvFace[i].t[2]];
	    s[vertices].tv=Point3((1-ry)*(rx*tv1.x+tv0.x-rx*tv0.x)+ry*tv2.x,(1-ry)*(rx*tv1.y+tv0.y-rx*tv0.y)+ry*tv2.y,0.0f);
	  }
	  vertices++;
	}
  }
  delete[] arealst;
  delete[] lst;
}

float Checkfordup(DWORD *curface,int P0,int P1,float *arealst,fEdge *edgelst,int *edges,int face,Point3 *vlst,Face *f)
{ int i,found;
  float area;
  Point3 norm;

  found=i=0;area=0.0f;
  while ((!found)&&(i<*edges))
   { found=((edgelst[i].vto==curface[P0])&&(edgelst[i].vfrom==curface[P1]))||
            ((edgelst[i].vfrom==curface[P0])&&(edgelst[i].vto==curface[P1]));
     if (!found) i++;
   }
   if (found)
   { norm=GetNormalizedNormal(curface,vlst);
     edgelst[i].normal=AveNormal(norm,edgelst[i].normal);
     if (f[face].getMatID()!=f[edgelst[i].face].getMatID())
     { if (GetFaceArea(curface,vlst)>GetFaceArea(&f[edgelst[i].face].v[0],vlst))
         edgelst[i].face=face;
     }
   }
   else
   {area=(arealst[i]=Length(vlst[curface[P0]]-vlst[curface[P1]]));
    edgelst[i].vfrom=curface[P0];edgelst[i].vto=curface[P1];
    edgelst[i].normal=GetNormalizedNormal(curface,vlst);
    edgelst[i].face=face;
    (*edges)++;
   }
 return(area);
}

float GetEdgeArea(Mesh *mesh,float *arealst,fEdge *edgelst,int *edges,int face)
{ float area;
  DWORD *curface=&(mesh->faces[face]).v[0],flags=mesh->faces[face].flags;
  area=0.0f;
  if ((flags & EDGE_A)>0)
    area+=Checkfordup(curface,0,1,arealst,edgelst,edges,face,mesh->verts,mesh->faces);
  if ((flags & EDGE_B)>0)
    area+=Checkfordup(curface,1,2,arealst,edgelst,edges,face,mesh->verts,mesh->faces);
  if ((flags & EDGE_C)>0)
    area+=Checkfordup(curface,2,0,arealst,edgelst,edges,face,mesh->verts,mesh->faces);
  return(area);
}

void FillByEdges(int infaces,int c,int vertices,Mesh *mesh,ParticleSys *parts,SavePt *s,Point3 *lastv,Face *lastf,Matlist m)
{ int i,edges,numedges=3*infaces,*lst;
  float *arealst,TotalArea,r;
  fEdge *edgelst;
  Point3 lastpt;

  edges=0;
  assert((arealst=new float[numedges])!=NULL);
  assert((edgelst=new fEdge[numedges])!=NULL);
  lst=new int[numedges];assert(lst);
  TotalArea=0.0f;
  for (i=0;i<infaces;i++)
    TotalArea+=GetEdgeArea(mesh,arealst,edgelst,&edges,i);
  SpreadOutParts(arealst,TotalArea,edges-1,edges,c,lst);
  for (i=0;i<edges;i++)
  { Point3 pt=mesh->verts[edgelst[i].vfrom],pdist=mesh->verts[edgelst[i].vto]-pt;
    Point3 lpt=lastv[edgelst[i].vfrom],lpdist=lastv[edgelst[i].vto]-lpt;
	for (int j=0;j<lst[i];j++)
	{ r=RND01();
      parts->points[vertices]=pt+pdist*r;
	  if (s[vertices].M>0.0f)
	  { lastpt=lpt+lpdist*r;
	    s[vertices].Ve=parts->points[vertices]*(m.newtm)-lastpt*(m.oldtm);
	  }
      parts->vels[vertices]=edgelst[i].normal;
	if (doTVs) 
	{ DWORD *vlst;vlst=&mesh->faces[edgelst[i].face].v[0];
	  int st0=(vlst[0]==edgelst[i].vfrom?0:(vlst[1]==edgelst[i].vfrom?1:2));
	  int st1=(vlst[0]==edgelst[i].vto?0:(vlst[1]==edgelst[i].vto?1:2));
	  Point3 tv0=mesh->tVerts[mesh->tvFace[edgelst[i].face].t[st0]];
	  Point3 tv1=mesh->tVerts[mesh->tvFace[edgelst[i].face].t[st1]];
	  s[vertices].tv=Point3(tv0.x+(tv1.x-tv0.x)*r,tv0.y+(tv1.y-tv0.y)*r,0.0f);
	  s[vertices].pmtl=mesh->faces[edgelst[i].face].getMatID();
	}
      vertices++;
    }
  }
  delete[] arealst;
  delete[] edgelst;
}

BOOL VertexinWhichFace(int v,Face *flst,int infaces,int *aface)
{ BOOL found;

  found=0;
  while ((*aface<infaces)&&(!found))
  { found=amatch(flst[*aface].v,v);
    if (!found) (*aface)++;
  }
  return(found);
}

void FillInFaces(int faces,int vertices,Mesh *mesh,int c,ParticleSys *parts,SavePt *s,Point3 *lastv,Face *lastf,Matlist m)
{ int i,j,*lst,localf;
  Point3 lastpt;
  lst=new int[faces];
  SelectEmitterPer(faces,c,lst);

  for (i=0;i<faces;i++)
  if (lst[i]>0)
  { parts->points[vertices]=(mesh->verts[mesh->faces[i].v[0]]+mesh->verts[mesh->faces[i].v[1]]+mesh->verts[mesh->faces[i].v[2]])/3.0f;
    parts->vels[vertices]=GetNormalizedNormal(&mesh->faces[i].v[0],mesh->verts);
	if (s[vertices].M>0.0f)
	{ lastpt=(lastv[lastf[i].v[0]]+lastv[lastf[i].v[1]]+lastv[lastf[i].v[2]])/3.0f;
	  s[vertices].Ve=parts->points[vertices]*(m.newtm)-lastpt*(m.oldtm);
	}
    if (doTVs)
	{ s[vertices].tv=(mesh->tVerts[mesh->tvFace[i].t[0]]+mesh->tVerts[mesh->tvFace[i].t[1]]+mesh->tVerts[mesh->tvFace[i].t[2]])/3.0f;
	  s[vertices].pmtl=mesh->faces[i].getMatID();
	}
	localf=vertices+1;
	for (j=1;j<lst[i];j++)
	{ parts->points[localf]=parts->points[vertices];
	  parts->vels[localf]=parts->vels[vertices];
	  s[localf].Ve=s[vertices].Ve;
	  if (doTVs) 
	  { s[localf].tv=s[vertices].tv;
	    s[localf].pmtl=s[vertices].pmtl;
      }
	  localf++;
	}
	vertices=localf;
  }
  delete[] lst;
}	*/

void FillInVertex(int inverts,int infaces,Mesh *mesh,SavePt *center)
{ int i,face,ncounter,firstface;
  Point3 newNorm,zero=Point3(0.0f,0.0f,0.0f);

  for (i=0;i<inverts;i++)
  { center->pt[i]=mesh->verts[i];
    face=firstface=ncounter=0;
	newNorm=zero;
    while (VertexinWhichFace(i,mesh->faces,infaces,&face))
	{ newNorm+=GetNormalizedNormal(&mesh->faces[face].v[0],mesh->verts);
	  if (ncounter==0) firstface=face;
	  ncounter++;face++;
	}
	center->norm[i]=Normalize(ncounter>0?newNorm/(float)ncounter:Point3(RND11(),RND11(),RND11()));
  }
}
int DoScatter(Mesh &mesh, Mesh &mesh1, Mesh &mesh2,int op, MeshOpProgress *prog,
		Matrix3 *tm1,Matrix3 *tm2,int whichInv,	int weld)
{  	double cnt;
	center.x = center.y = center.z = 0.0;
	int nfaces, res;
	BitArray weldSet;

	if (prog) prog->Init(100);

	mesh.setNumVerts(bool_vcount);
	mesh.setNumFaces(bool_fcount);
	weldSet.SetSize(bool_vcount);
	nfaces = 0;
	mesh.numFaces = nfaces;

	return 1;
}
