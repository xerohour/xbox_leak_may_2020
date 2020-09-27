#ifndef __FMATERIALDATABASE_H
#define __FMATERIALDATABASE_H

#ifdef EXPORTER
#include <stdio.h>
#endif

struct FMaterial;
struct FTextureDatabase;

struct FMaterialDatabase
{
	FTextureDatabase  *m_textures;              // all textures in one place

	int                m_materials;             // number of materials
	FMaterial         *m_material;              // actual materials

	void               m_Relocate(void *base, void *baseGfx);

#ifdef EXPORTER
	bool               m_Create(void);
	void               m_WriteIndirectData(FILE *fp, FILE *fpGfx, FMaterialDatabase *header);
	FMaterialDatabase *m_WriteHeader(FILE *fp, FMaterialDatabase *header);
	void               m_CleanHeader(void);

	FMaterialDatabase();
	~FMaterialDatabase();
#endif
};

#endif
