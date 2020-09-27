#include "xarena.h"
#include "coll.h"

//-----------------------------------------------------------------------------
// Name: CollCheck
// Desc: Basic collision detection
//       For the walls, we just check absolute positions based on defined
//       size of the floor.
//       For tanks, we do a radius check. We could miss a few collisions
//         with this method due to the speed of some of the weapons, but
//         thats ok for our simpleton purposes.
//       For obstacle objects, we do a check based on the type of the object.
//-----------------------------------------------------------------------------
CXObject *CollCheck(D3DXVECTOR3 *pos, float radius)
{
	CXObject *collobj = NULL;
	float rad2, orad2, d2;
	D3DXVECTOR3 *opos, v;
	DWORD i;

	// check against walls
	if(pos->x < (-FLOORX/2.0f+radius))
	{
		pos->x = -FLOORX/2.0f+radius;
		collobj = COLLOBJ_ARENA;
	}
	else if(pos->x > (FLOORX/2.0f-radius))
	{
		pos->x = FLOORX/2.0f-radius;
		collobj = COLLOBJ_ARENA;
	}

	if(pos->z < (-FLOORZ/2.0f+radius))
	{
		pos->z = -FLOORZ/2.0f+radius;
		collobj = COLLOBJ_ARENA;
	}
	else if(pos->z > (FLOORZ/2.0f-radius))
	{
		pos->z = FLOORZ/2.0f-radius;
		collobj = COLLOBJ_ARENA;
	}

	// check against other tanks
	for(i=0; i<g_dwNumTanks; i++)
	{
		opos = &g_pTank[i]->m_pObj->m_vPosition;

		// if this is us, just bail
		if(pos==opos)
			continue;

		// not us, get radius of object
		orad2 = g_pTank[i]->m_pObj->GetRadius2();
		rad2 = radius*radius;							// get square of our radius

		d2 = (pos->x-opos->x)*(pos->x-opos->x) + 
				(pos->y-opos->y)*(pos->y-opos->y) + 
				(pos->z-opos->z)*(pos->z-opos->z);

		if(d2<(rad2+orad2))
		{
			collobj = g_pTank[i]->m_pObj;

			v = *pos - *opos;							// get position delta
			D3DXVec3Normalize(&v, &v);					// normalize it
			*pos = *opos + (float)sqrt(orad2+rad2)*v;	// separate us by combined radius
		}
	}

	return collobj;
}
