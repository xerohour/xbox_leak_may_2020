#include "std.h"


inline bool equal(D3DXVECTOR3* a, D3DXVECTOR3* b)
{
	return (a->x == b->x && a->y == b->y && a->z == b->z);
}


static void calcnormvec(D3DXVECTOR3* facenormals, int facenum, int* faces, int fn, float thresh_value, D3DXVECTOR3* vertnormal)
{
	D3DXVECTOR3* facenormal = &facenormals[facenum];
	int currentface, i;
	float skalarpr;

	*vertnormal = *facenormal;

	for (i = 0; i < fn; i++)
	{
		currentface = faces[i];
		if (facenum != currentface)
		{
			skalarpr = D3DXVec3Dot(&facenormals[currentface], facenormal);
			if (skalarpr > thresh_value)
				*vertnormal += facenormals[currentface];
		}
	}
}


inline void normalize(D3DXVECTOR3* normal)
{
	D3DXVec3Normalize(normal, normal);
}

int autosmooth(
	int* coordindex, int numvertinds,  // in: coordIndex, no. of coordindices
	D3DXVECTOR3* facenormals, int numfaces,  // in: face normals (calculated by buildFacenormals), no. of faces
	float creaseAngle,  // in: crease angle
	D3DXVECTOR3* normallist, int* normalindex // out: list of normals and normal index (already allocated)
	)
{
	int* vertinds = coordindex;
	int* normalinds = (int*)normalindex;

	int maxvert = 0, i = numvertinds, fn = 0, same_normal = 0, normal_index = 0;
	int* vi = vertinds;
	int *facespervertex, *numfacespervertex, *normalspervertex, *numnormalspervertex;
	int curindex, j, found;
	float thresh_value = cosf(creaseAngle);

	// find maximum vertexindex
	while (i--)
	{
		if (*vi > maxvert)
			maxvert = *vi;
		vi++;
	}

	// create list of faces containing the vertices
	maxvert += 1;
	facespervertex = new int [numfaces * maxvert];

	numfacespervertex = new int [maxvert];
	ZeroMemory(numfacespervertex, sizeof (int) * maxvert);

	for (i = numvertinds, vi = vertinds; i--; vi++)
	{
		if (*vi >= 0)
		{
			facespervertex[*vi * numfaces + numfacespervertex[*vi]] = fn;
			numfacespervertex[*vi]++;
		}
		else
		{
			fn++;
		}
	}

	// create list of normals and normalindeces
	normalspervertex = new int [numfaces * maxvert];

	numnormalspervertex = new int [maxvert];
	ZeroMemory(numnormalspervertex, sizeof (int) * maxvert);

	fn = 0;
	for (i = 0; i < numvertinds; i++)
	{
		curindex = vertinds[i];
		if (curindex >= 0)
		{
			calcnormvec(facenormals, fn, facespervertex + (curindex * numfaces), numfacespervertex[curindex], thresh_value, &normallist[normal_index]);
			normalize(&normallist[normal_index]);

			found = 0;
			for (j = 0; j < numnormalspervertex[curindex] && !found; j++)
			{
				same_normal = normalspervertex[curindex * numfaces + j];
				found = equal(&normallist[same_normal], &normallist[normal_index]);
			}

			if (found)
			{
				normalinds[i] = same_normal;
			}
			else if ((normal_index > 0) && equal(&normallist[normal_index], &normallist[normal_index - 1]))
			{
				normalinds[i] = normal_index - 1;
			}
			else
			{
				normalinds[i] = normal_index;
				normalspervertex[curindex * numfaces + numnormalspervertex[curindex]] = normal_index;
				numnormalspervertex[curindex]++;
				normal_index++;
			}
		}
		else
		{
			fn++;
			normalinds[i] = -1;
		}
	}

	delete [] facespervertex;
	delete [] numfacespervertex;
	delete [] normalspervertex;
	delete [] numnormalspervertex; 

	return normal_index;
}

#if 0
{
	int ncoordinds = 3 * nTriangles;
	int* normalindex = new int [ncoordinds];
	D3DXVECTOR3 normals = new D3DXVECTOR3 [ncoordinds];
	int numnormals = autosmooth(coordinds, ncoordinds, facenormals, numfaces, creaseAngle, normals, normalindex);

	if (numnormals != ncoordinds)
	{
		// We shrunk the normals array, so reallocate it here...
		D3DXVECTOR3 normallist = new D3DXVECTOR3 [numnormals];
		CopyMemory(normallist, normals, sizeof (D3DXVECTOR3) * numnormals);
		delete [] normals;
		normals = normallist;
	}

	// Now normal [] contains all of the normals and numnormals contains the count of them...
	// normalindex [] contains a normal index per vertex
}
#endif