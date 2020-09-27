// wrl2xm.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef enum _D3DPRIMITIVETYPE {
    D3DPT_POINTLIST             = 1,
    D3DPT_LINELIST              = 2,
    D3DPT_LINESTRIP             = 3,
    D3DPT_TRIANGLELIST          = 4,
    D3DPT_TRIANGLESTRIP         = 5,
    D3DPT_TRIANGLEFAN           = 6,

    D3DPT_FORCE_DWORD           = 0x7fffffff
} D3DPRIMITIVETYPE;


#define D3DFVF_XYZ              0x002
#define D3DFVF_NORMAL           0x010
#define D3DFVF_DIFFUSE          0x040
#define D3DFVF_TEX1             0x100


typedef DWORD D3DCOLOR;

#define D3DCOLOR_ARGB(a,r,g,b) \
    ((D3DCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)

#define D3DCOLOR_COLORVALUE(r,g,b,a) \
    D3DCOLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))


enum FIELDTYPE
{
	MFColor,
	MFFloat,
	MFInt32,
	MFNode,
	MFRotation,
	MFString,
	MFTime,
	MFVec2f,
	MFVec3f,
	SFBool,
	SFColor,
	SFFloat,
	SFImage,
	SFInt32,
	SFNode,
	SFRotation,
	SFString,
	SFTime,
	SFVec2f,
	SFVec3f
};

struct FieldDesc
{
	char* m_szFieldID;
	FIELDTYPE m_type;
};

#define FIELDDESC(foo, type, name, def) { #name, type },
#define START_FIELDS(name) FieldDesc name##Fields [] = {
#define END_FIELDS() };

struct NodeDesc
{
	char* m_szNodeTypeID;
	int m_nFieldCount;
	FieldDesc* m_rgfields;
};

#define NODEDESC(name) { #name, sizeof (name##Fields) / sizeof (FieldDesc), name##Fields },


void LegalizeFileName(char* szDest, const char* szSrc)
{
	for (;;)
	{
		if (*szSrc == '/' || *szSrc == '>' || *szSrc == '<' || *szSrc == '|' || *szSrc == '+')
			*szDest = '_';
		else
			*szDest = *szSrc;

		if (*szSrc == 0)
			return;

		szDest += 1;
		szSrc += 1;
	}
}

class CVrmlNode;

struct CNodeValue
{
};

struct CNodeValueArray : public CNodeValue
{
	CNodeValueArray()
	{
		m_nAlloc = 0;
		m_nCount = 0;
		m_values = NULL;
	}

	~CNodeValueArray()
	{
	}

	void Add(CNodeValue* pNewValue)
	{
		if (m_nCount + 1 > m_nAlloc)
		{
			m_nAlloc += 1024;
			CNodeValue** values = new CNodeValue* [m_nAlloc];
			CopyMemory(values, m_values, sizeof (CNodeValue*) * m_nCount);
			delete [] m_values;
			m_values = values;
		}

		m_values[m_nCount] = pNewValue;
		m_nCount += 1;
	}

	int m_nAlloc;
	int m_nCount;
	CNodeValue** m_values;
};

struct VSFBool : public CNodeValue
{
	bool m_value;
};

struct VSFColor : public CNodeValue
{
	float m_red, m_green, m_blue;
};

struct VSFFloat : public CNodeValue
{
	float m_value;
};

struct VSFImage : public CNodeValue
{
};

struct VSFInt32 : public CNodeValue
{
	int m_value;
};

struct VSFNode : public CNodeValue
{
	VSFNode()
	{
		m_bUSE = false;
		m_value = NULL;
	}

	bool m_bUSE;
	CVrmlNode* m_value;
};

struct VSFRotation : public CNodeValue
{
	float m_x, m_y, m_z, m_angle;
};

struct VSFString : public CNodeValue
{
	VSFString()
	{
		m_value = NULL;
	}

	~VSFString()
	{
		delete m_value;
	}

	char* m_value;
};

struct VSFTime : public CNodeValue
{
	float m_value;
};

struct VSFVec2f : public CNodeValue
{
	float m_value [2];
};

struct VSFVec3f : public CNodeValue
{
	float m_value [3];
};






class CVrmlNode
{
public:
	CVrmlNode(NodeDesc* pNodeDesc, const char* szNameID);
	~CVrmlNode();

	NodeDesc* m_pNodeDesc;
	char* m_szNameID;
	CNodeValue** m_rgpNodeValues;

	int FindField(const char* szFieldID);
	CNodeValue* GetValue(const char* szFieldID);

	inline CNodeValueArray* GetArray(const char* szFieldID) { return (CNodeValueArray*)GetValue(szFieldID); }
	inline VSFBool* GetSFBool(const char* szFieldID) { return (VSFBool*)GetValue(szFieldID); }
	inline VSFColor* GetSFColor(const char* szFieldID) { return (VSFColor*)GetValue(szFieldID); }
	inline VSFFloat* GetSFFloat(const char* szFieldID) { return (VSFFloat*)GetValue(szFieldID); }
	inline VSFImage* GetSFImage(const char* szFieldID) { return (VSFImage*)GetValue(szFieldID); }
	inline VSFInt32* GetSFInt32(const char* szFieldID) { return (VSFInt32*)GetValue(szFieldID); }
	inline VSFNode* GetSFNode(const char* szFieldID) { return (VSFNode*)GetValue(szFieldID); }
	inline VSFRotation* GetSFRotation(const char* szFieldID) { return (VSFRotation*)GetValue(szFieldID); }
	inline VSFString* GetSFString(const char* szFieldID) { return (VSFString*)GetValue(szFieldID); }
	inline VSFTime* GetSFTime(const char* szFieldID) { return (VSFTime*)GetValue(szFieldID); }
	inline VSFVec2f* GetSFVec2f(const char* szFieldID) { return (VSFVec2f*)GetValue(szFieldID); }
	inline VSFVec3f* GetSFVec3f(const char* szFieldID) { return (VSFVec3f*)GetValue(szFieldID); }
};

CVrmlNode::CVrmlNode(NodeDesc* pNodeDesc, const char* szNameID)
{
	if (szNameID == NULL || szNameID[0] == '\0')
	{
		m_szNameID = NULL;
	}
	else
	{
		m_szNameID = new char [strlen(szNameID) + 1];
		strcpy(m_szNameID, szNameID);
	}

	m_pNodeDesc = pNodeDesc;
	m_rgpNodeValues = new CNodeValue* [m_pNodeDesc->m_nFieldCount];
	for (int i = 0; i < m_pNodeDesc->m_nFieldCount; i += 1)
		m_rgpNodeValues[i] = NULL;
}

CVrmlNode::~CVrmlNode()
{
	for (int i = 0; i < m_pNodeDesc->m_nFieldCount; i += 1)
		delete m_rgpNodeValues[i];
	delete [] m_rgpNodeValues;
	delete [] m_szNameID;
}

int CVrmlNode::FindField(const char* szFieldID)
{
	for (int i = 0; i < m_pNodeDesc->m_nFieldCount; i += 1)
	{
		if (strcmp(m_pNodeDesc->m_rgfields[i].m_szFieldID, szFieldID) == 0)
			return i;
	}

	return -1;
}


CNodeValue* CVrmlNode::GetValue(const char* szFieldID)
{
	int nValue = FindField(szFieldID);
	if (nValue == -1)
		return NULL;

	return m_rgpNodeValues[nValue];
}


CNodeValueArray rootNodes;
CNodeValueArray defNodes;



START_FIELDS(Anchor)
	FIELDDESC(eventIn, MFNode, addChildren, NULL)
	FIELDDESC(eventIn, MFNode, removeChildren, NULL)
	FIELDDESC(exposedField, MFNode, children, "[]")
	FIELDDESC(exposedField, SFString, description, "")
	FIELDDESC(exposedField, MFString, parameter, "[]")
	FIELDDESC(exposedField, MFString, url, "[]")
	FIELDDESC(field, SFVec3f, bboxCenter, "0 0 0 # (-inf,inf)")
	FIELDDESC(field, SFVec3f, bboxSize, "-1 -1 -1 # (0,inf) or -1,-1,-1")
END_FIELDS()

START_FIELDS(Appearance)
	FIELDDESC(exposedField, SFNode, material, "NULL")
	FIELDDESC(exposedField, SFNode, texture, "NULL")
	FIELDDESC(exposedField, SFNode, textureTransform, "NULL")
END_FIELDS()

START_FIELDS(AudioClip)
	FIELDDESC(exposedField,   SFString, description,      "")
	FIELDDESC(exposedField,   SFBool,   loop,             "FALSE")
	FIELDDESC(exposedField,   SFFloat,  pitch,            "1.0        # (0,inf)")
	FIELDDESC(exposedField,   SFTime,   startTime,        "0          # (-,inf)")
	FIELDDESC(exposedField,   SFTime,   stopTime,         "0          # (-,inf)")
	FIELDDESC(exposedField,   MFString, url,              "[]")
	FIELDDESC(eventOut,       SFTime,   duration_changed, NULL)
	FIELDDESC(eventOut,       SFBool,   isActive,         NULL)
END_FIELDS()

/*
START_FIELDS(Background)
END_FIELDS()

START_FIELDS(Billboard)
END_FIELDS()

START_FIELDS(Box)
END_FIELDS()

START_FIELDS(Collision)
END_FIELDS()
*/

START_FIELDS(Color)
	FIELDDESC(exposedField, MFColor, color,  "[]         # [0,1]")
END_FIELDS()

/*
START_FIELDS(ColorInterpolator)
END_FIELDS()

START_FIELDS(Cone)
END_FIELDS()
*/

START_FIELDS(Coordinate)
	FIELDDESC(exposedField, MFVec3f, point,  "[]      # (-,inf)")
END_FIELDS()

/*
START_FIELDS(CoordinateInterpolator)
END_FIELDS()

START_FIELDS(Cylinder)
END_FIELDS()

START_FIELDS(CylinderSensor)
END_FIELDS()

START_FIELDS(DirectionalLight)
END_FIELDS()

START_FIELDS(ElevationGrid)
END_FIELDS()

START_FIELDS(Extrusion)
END_FIELDS()

START_FIELDS(Fog)
END_FIELDS()

START_FIELDS(FontStyle)
END_FIELDS()
*/

START_FIELDS(Group)
	FIELDDESC(eventIn,      MFNode,  addChildren,   NULL)
	FIELDDESC(eventIn,      MFNode,  removeChildren,NULL)
	FIELDDESC(exposedField, MFNode,  children,      "[]")
	FIELDDESC(field,        SFVec3f, bboxCenter,    "0 0 0     # (-,inf)")
	FIELDDESC(field,        SFVec3f, bboxSize,      "-1 -1 -1  # (0,) or -1,-1,-1")
END_FIELDS()

START_FIELDS(ImageTexture)
	FIELDDESC(exposedField, MFString, url,     "[]")
	FIELDDESC(field,        SFBool,   repeatS, "TRUE")
	FIELDDESC(field,        SFBool,   repeatT, "TRUE")
END_FIELDS()

START_FIELDS(IndexedFaceSet)
	FIELDDESC(eventIn,       MFInt32, set_colorIndex,    NULL)
	FIELDDESC(eventIn,       MFInt32, set_coordIndex,    NULL)
	FIELDDESC(eventIn,       MFInt32, set_normalIndex,   NULL)
	FIELDDESC(eventIn,       MFInt32, set_texCoordIndex, NULL)
	FIELDDESC(exposedField,  SFNode,  color,             "NULL")
	FIELDDESC(exposedField,  SFNode,  coord,             "NULL")
	FIELDDESC(exposedField,  SFNode,  normal,            "NULL")
	FIELDDESC(exposedField,  SFNode,  texCoord,          "NULL")
	FIELDDESC(field,         SFBool,  ccw,               "TRUE")
	FIELDDESC(field,         MFInt32, colorIndex,        "[]        # [-1,inf)")
	FIELDDESC(field,         SFBool,  colorPerVertex,    "TRUE")
	FIELDDESC(field,         SFBool,  convex,            "TRUE")
	FIELDDESC(field,         MFInt32, coordIndex,        "[]        # [-1,inf)")
	FIELDDESC(field,         SFFloat, creaseAngle,       "0         # [0,inf)")
	FIELDDESC(field,         MFInt32, normalIndex,       "[]        # [-1,inf)")
	FIELDDESC(field,         SFBool,  normalPerVertex,   "TRUE")
	FIELDDESC(field,         SFBool,  solid,             "TRUE")
	FIELDDESC(field,         MFInt32, texCoordIndex,     "[]        # [-1,inf)")
END_FIELDS()

START_FIELDS(IndexedLineSet)
	FIELDDESC(eventIn,       MFInt32, set_colorIndex,    NULL)
	FIELDDESC(eventIn,       MFInt32, set_coordIndex,    NULL)
	FIELDDESC(exposedField,  SFNode,  color,             "NULL")
	FIELDDESC(exposedField,  SFNode,  coord,             "NULL")
	FIELDDESC(field,         MFInt32, colorIndex,        "[]     # [-1,inf)")
	FIELDDESC(field,         SFBool,  colorPerVertex,    "TRUE")
	FIELDDESC(field,         MFInt32, coordIndex,        "[]     # [-1,inf)")
END_FIELDS()

/*
START_FIELDS(Inline)
END_FIELDS()

START_FIELDS(LOD)
END_FIELDS()
*/

START_FIELDS(Material)
	FIELDDESC(exposedField, SFFloat, ambientIntensity,  "0.2         # [0,1]")
	FIELDDESC(exposedField, SFColor, diffuseColor,      "0.8 0.8 0.8 # [0,1]")
	FIELDDESC(exposedField, SFColor, emissiveColor,     "0 0 0       # [0,1]")
	FIELDDESC(exposedField, SFFloat, shininess,         "0.2         # [0,1]")
	FIELDDESC(exposedField, SFColor, specularColor,     "0 0 0       # [0,1]")
	FIELDDESC(exposedField, SFFloat, transparency,      "0           # [0,1]")
END_FIELDS()

START_FIELDS(MaxMaterial) // extension
	FIELDDESC(exposedField, SFString, name,             "")
END_FIELDS()

/*
START_FIELDS(MovieTexture)
END_FIELDS()

START_FIELDS(NavigationInfo)
END_FIELDS()
*/

START_FIELDS(Normal)
	FIELDDESC(exposedField, MFVec3f, vector,  "[]   # (-,inf)")
END_FIELDS()

/*
START_FIELDS(NormalInterpolator)
END_FIELDS()
*/

START_FIELDS(OrientationInterpolator)
	FIELDDESC(eventIn,      SFFloat,    set_fraction,  NULL)
	FIELDDESC(exposedField, MFFloat,    key,           "[]    # (-inf,inf)")
	FIELDDESC(exposedField, MFRotation, keyValue,      "[]    # [-1,1],(-inf,inf)")
	FIELDDESC(eventOut,     SFRotation, value_changed, NULL)
END_FIELDS()

/*
START_FIELDS(PixelTexture)
END_FIELDS()

START_FIELDS(PlaneSensor)
END_FIELDS()

START_FIELDS(PointLight)
END_FIELDS()

START_FIELDS(PointSet)
END_FIELDS()
*/

START_FIELDS(PositionInterpolator)
	FIELDDESC(eventIn,      SFFloat, set_fraction,  NULL)
	FIELDDESC(exposedField, MFFloat, key,           "[]    # (-inf,inf)")
	FIELDDESC(exposedField, MFVec3f, keyValue,      "[]    # (-inf,inf)")
	FIELDDESC(eventOut,     SFVec3f, value_changed, NULL)
END_FIELDS()

/*
START_FIELDS(ProximitySensor)
END_FIELDS()

START_FIELDS(ScalarInterpolator)
END_FIELDS()

START_FIELDS(Script)
END_FIELDS()
*/

START_FIELDS(Shape)
	FIELDDESC(exposedField, SFNode, appearance, "NULL")
	FIELDDESC(exposedField, SFNode, geometry,   "NULL")
END_FIELDS()

START_FIELDS(Sound)
	FIELDDESC(exposedField, SFVec3f,  direction,     "0 0 1   # (-inf,inf)")
	FIELDDESC(exposedField, SFFloat,  intensity,     "1       # [0,1]")
	FIELDDESC(exposedField, SFVec3f,  location,      "0 0 0   # (-inf,inf)")
	FIELDDESC(exposedField, SFFloat,  maxBack,       "10      # [0,inf)")
	FIELDDESC(exposedField, SFFloat,  maxFront,      "10      # [0,inf)")
	FIELDDESC(exposedField, SFFloat,  minBack,       "1       # [0,inf)")
	FIELDDESC(exposedField, SFFloat,  minFront,      "1       # [0,inf)")
	FIELDDESC(exposedField, SFFloat,  priority,      "0       # [0,1]")
	FIELDDESC(exposedField, SFNode,   source,        "NULL")
	FIELDDESC(field,        SFBool,   spatialize,    "TRUE")
END_FIELDS()

START_FIELDS(Sphere)
	FIELDDESC(field, SFFloat, radius,  "1    # (0,inf)")
END_FIELDS()

/*
START_FIELDS(SphereSensor)
END_FIELDS()

START_FIELDS(SpotLight)
END_FIELDS()

START_FIELDS(Switch)
END_FIELDS()

START_FIELDS(Text)
END_FIELDS()
*/

START_FIELDS(TextureCoordinate)
	FIELDDESC(exposedField, MFVec2f, point,  "[]      # (-inf,inf)")
END_FIELDS()

START_FIELDS(TextureTransform)
	FIELDDESC(exposedField, SFVec2f, center,      "0 0     # (-inf,inf)")
	FIELDDESC(exposedField, SFFloat, rotation,    "0       # (-inf,inf)")
	FIELDDESC(exposedField, SFVec2f, scale,       "1 1     # (-inf,inf)")
	FIELDDESC(exposedField, SFVec2f, translation, "0 0     # (-inf,inf)")
END_FIELDS()

START_FIELDS(TimeSensor)
	FIELDDESC(exposedField, SFTime,   cycleInterval, "1       # (0,inf)")
	FIELDDESC(exposedField, SFBool,   enabled,       "TRUE")
	FIELDDESC(exposedField, SFBool,   loop,          "FALSE")
	FIELDDESC(exposedField, SFTime,   startTime,     "0       # (-inf,inf)")
	FIELDDESC(exposedField, SFTime,   stopTime,      "0       # (-inf,inf)")
	FIELDDESC(eventOut,     SFTime,   cycleTime,     NULL)
	FIELDDESC(eventOut,     SFFloat,  fraction_changed,      "# [0, 1]")
	FIELDDESC(eventOut,     SFBool,   isActive,       NULL)
	FIELDDESC(eventOut,     SFTime,   time,           NULL)
END_FIELDS()

/*
START_FIELDS(TouchSensor)
END_FIELDS()
*/

START_FIELDS(Transform)
	FIELDDESC(eventIn,      MFNode,      addChildren,      NULL)
	FIELDDESC(eventIn,      MFNode,      removeChildren,   NULL)
	FIELDDESC(exposedField, SFVec3f,     center,           "0 0 0    # (-inf,inf)")
	FIELDDESC(exposedField, MFNode,      children,         "[]")
	FIELDDESC(exposedField, SFRotation,  rotation,         "0 0 1 0  # [-1,1],(-inf,inf)")
	FIELDDESC(exposedField, SFVec3f,     scale,            "1 1 1    # (0,inf)")
	FIELDDESC(exposedField, SFRotation,  scaleOrientation, "0 0 1 0  # [-1,1],(-inf,inf)")
	FIELDDESC(exposedField, SFVec3f,     translation,      "0 0 0    # (-inf,inf)")
	FIELDDESC(field,        SFVec3f,     bboxCenter,       "0 0 0    # (-inf,inf)")
	FIELDDESC(field,        SFVec3f,     bboxSize,         "-1 -1 -1 # (0,inf) or -1,-1,-1")
END_FIELDS()

START_FIELDS(Viewpoint)
	FIELDDESC(eventIn,      SFBool,     set_bind,       NULL)
	FIELDDESC(exposedField, SFFloat,    fieldOfView,    "0.785398  # (0,inf)")
	FIELDDESC(exposedField, SFBool,     jump,           "TRUE")
	FIELDDESC(exposedField, SFRotation, orientation,    "0 0 1 0   # [-1,1],(-inf,inf)")
	FIELDDESC(exposedField, SFVec3f,    position,       "0 0 10    # (-inf,inf)")
	FIELDDESC(field,        SFString,   description,    "")
	FIELDDESC(eventOut,     SFTime,     bindTime,       NULL)
	FIELDDESC(eventOut,     SFBool,     isBound,        NULL)
END_FIELDS()

/*
START_FIELDS(VisibilitySensor)
END_FIELDS()

START_FIELDS(WorldInfo)
END_FIELDS()
*/

NodeDesc rgnodes [] =
{
	NODEDESC(Anchor)
	NODEDESC(Appearance)
	NODEDESC(AudioClip)
/*
	NODEDESC(Background)
	NODEDESC(Billboard)
	NODEDESC(Box)
	NODEDESC(Collision)
*/
	NODEDESC(Color)
/*
	NODEDESC(ColorInterpolator)
	NODEDESC(Cone)
*/
	NODEDESC(Coordinate)
/*
	NODEDESC(CoordinateInterpolator)
	NODEDESC(Cylinder)
	NODEDESC(CylinderSensor)
	NODEDESC(DirectionalLight)
	NODEDESC(ElevationGrid)
	NODEDESC(Extrusion)
	NODEDESC(Fog)
	NODEDESC(FontStyle)
*/
	NODEDESC(Group)
	NODEDESC(ImageTexture)
	NODEDESC(IndexedFaceSet)
	NODEDESC(IndexedLineSet)
/*
	NODEDESC(Inline)
	NODEDESC(LOD)
*/
	NODEDESC(Material)
	NODEDESC(MaxMaterial) // extension
/*
	NODEDESC(MovieTexture)
	NODEDESC(NavigationInfo)
*/
	NODEDESC(Normal)
/*
	NODEDESC(NormalInterpolator)
*/
	NODEDESC(OrientationInterpolator)
/*
	NODEDESC(PixelTexture)
	NODEDESC(PlaneSensor)
	NODEDESC(PointLight)
	NODEDESC(PointSet)
*/
	NODEDESC(PositionInterpolator)
/*
	NODEDESC(ProximitySensor)
	NODEDESC(ScalarInterpolator)
	NODEDESC(Script)
*/
	NODEDESC(Shape)
/*
	NODEDESC(Sound)
*/
	NODEDESC(Sphere)
/*
	NODEDESC(SphereSensor)
	NODEDESC(SpotLight)
	NODEDESC(Switch)
	NODEDESC(Text)
*/
	NODEDESC(TextureCoordinate)
	NODEDESC(TextureTransform)
	NODEDESC(TimeSensor)
/*
	NODEDESC(TouchSensor)
*/
	NODEDESC(Transform)
	NODEDESC(Viewpoint)
/*
	NODEDESC(VisibilitySensor)
	NODEDESC(WorldInfo)
*/
};


FILE* pFile;
int nLine;
char szLine [256];
const char* pchLine;
char szToken [256];

bool FetchLine()
{
	if (fgets(szLine, sizeof (szLine), pFile) == NULL)
		return false;
/*
	char* pchEnd = strchr(szLine, '#');
	if (pchEnd != NULL)
		*pchEnd = 0;
*/
	pchLine = szLine;
	nLine += 1;

//	fprintf(stderr, "%d: %s", nLine, szLine);

	return true;
}

inline bool IsWhite(char ch)
{
	return (ch == ' ' || ch == '\t' || ch == ',' || ch == '\r' || ch == '\n');
}

const char* SkipWhite(const char* pch)
{
	while (IsWhite(*pch))
		pch += 1;

	return pch;
}

char PeekChar()
{
	pchLine = SkipWhite(pchLine);

	while (*pchLine == '\0' || *pchLine == '#')
	{
		if (!FetchLine())
			return 0;

		pchLine = SkipWhite(szLine);
	}

	return *pchLine;
}

inline bool IsTerminal(char ch)
{
	return /*ch == '.' || */ch == '{' || ch == '}' || ch == '[' || ch == ']';
}

bool Token()
{
	switch (PeekChar())
	{
	case '\0':
		return false;

//	case '.':
	case '{':
	case '}':
	case '[':
	case ']':
		szToken[0] = *pchLine++;
		szToken[1] = '\0';
		return true;

	case '"':
		{
			pchLine += 1;
			char* pchToken = szToken;
			while (*pchLine != '"')
			{
				if (*pchLine == '\0' && !FetchLine())
					break;

				if (*pchLine == '\\')
					pchLine += 1;
				*pchToken++ = *pchLine++;
			}
			*pchToken = '\0';
			pchLine += 1;
		}
		return true;
	}

	char* pchToken = szToken;
	while (!IsWhite(*pchLine) && !IsTerminal(*pchLine))
		*pchToken++ = *pchLine++;
	*pchToken = '\0';

	return true;
}

CNodeValue* ParseNode();

CNodeValue* ParseMF(CNodeValue*(*ParseSF)())
{
	CNodeValueArray* pValueArray = new CNodeValueArray;

	if (PeekChar() == '[')
	{
		Token();

		while (PeekChar() != ']')
		{
			CNodeValue* pValue = ParseSF();
			pValueArray->Add(pValue);
		}

		Token();
	}
	else
	{
		CNodeValue* pValue = ParseSF();
		pValueArray->Add(pValue);
	}

	return pValueArray;
}

CNodeValue* ParseSFFloat()
{
	Token();

	VSFFloat* pValue = new VSFFloat;

	if (sscanf(szToken, "%f", &pValue->m_value) != 1)
	{
		fprintf(stderr, "%d: invalid SFFloat value '%s'\n", nLine, szToken);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseSFInt32()
{
	Token();

	VSFInt32* pValue = new VSFInt32;

	if (sscanf(szToken, "%d", &pValue->m_value) != 1)
	{
		fprintf(stderr, "%d: invalid SFInt32 value '%s'\n", nLine, szToken);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseSFBool()
{
	Token();

	VSFBool* pValue = new VSFBool;

	if (strcmp(szToken, "TRUE") == 0)
	{
		pValue->m_value = true;
	}
	else if (strcmp(szToken, "FALSE") == 0)
	{
		pValue->m_value = false;
	}
	else
	{
		fprintf(stderr, "%d: invalid SFBool value '%s'\n", nLine, szToken);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseSFVec3f()
{
	char szVec3f [256];

	Token();
	strcpy(szVec3f, szToken);
	strcat(szVec3f, " ");

	Token();
	strcat(szVec3f, szToken);
	strcat(szVec3f, " ");

	Token();
	strcat(szVec3f, szToken);

	VSFVec3f* pValue = new VSFVec3f;

	if (sscanf(szVec3f, "%f %f %f", &pValue->m_value[0], &pValue->m_value[1], &pValue->m_value[2]) != 3)
	{
		fprintf(stderr, "%d: invalid SFVec3f value '%s'\n", nLine, szVec3f);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseSFColor()
{
	char szColor [256];

	Token();
	strcpy(szColor, szToken);
	strcat(szColor, " ");

	Token();
	strcat(szColor, szToken);
	strcat(szColor, " ");

	Token();
	strcat(szColor, szToken);

	VSFColor* pValue = new VSFColor;

	if (sscanf(szColor, "%f %f %f", &pValue->m_red, &pValue->m_green, &pValue->m_blue) != 3)
	{
		fprintf(stderr, "%d: invalid SFColor value '%s'\n", nLine, szColor);
		exit(1);
	}

	return pValue;
}

CNodeValue*  ParseMFColor()
{
	return ParseMF(ParseSFColor);
}

CNodeValue*  ParseSFRotation()
{
	char szRotation [256];

	Token();
	strcpy(szRotation, szToken);
	strcat(szRotation, " ");

	Token();
	strcat(szRotation, szToken);
	strcat(szRotation, " ");

	Token();
	strcat(szRotation, szToken);
	strcat(szRotation, " ");

	Token();
	strcat(szRotation, szToken);

	VSFRotation* pValue = new VSFRotation;

	if (sscanf(szRotation, "%f %f %f %f", &pValue->m_x, &pValue->m_y, &pValue->m_z, &pValue->m_angle) != 4)
	{
		fprintf(stderr, "%d: invalid SFRotation value '%s'\n", nLine, szRotation);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseMFRotation()
{
	return ParseMF(ParseSFRotation);
}

CNodeValue* ParseSFString()
{
	Token();

	VSFString* pValue = new VSFString;
	pValue->m_value = new char [strlen(szToken) + 1];
	strcpy(pValue->m_value, szToken);

	return pValue;
}

CNodeValue* ParseMFString()
{
	return ParseMF(ParseSFString);
}

CNodeValue* ParseSFTime()
{
	Token();

	VSFTime* pValue = new VSFTime;

	if (sscanf(szToken, "%f", &pValue->m_value) != 1)
	{
		fprintf(stderr, "%d: invalid SFTime value '%s'\n", nLine, szToken);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseMFTime()
{
	return ParseMF(ParseSFTime);
}

CNodeValue* ParseSFVec2f()
{
	char szVec2f [256];

	Token();
	strcpy(szVec2f, szToken);
	strcat(szVec2f, " ");

	Token();
	strcat(szVec2f, szToken);

	VSFVec2f* pValue = new VSFVec2f;

	if (sscanf(szVec2f, "%f %f", &pValue->m_value[0], &pValue->m_value[1]) != 2)
	{
		fprintf(stderr, "%d: invalid SFVec2f value '%s'\n", nLine, szVec2f);
		exit(1);
	}

	return pValue;
}

CNodeValue* ParseMFVec2f()
{
	return ParseMF(ParseSFVec2f);
}

CNodeValue* ParseSFImage()
{
	// TODO
	return NULL;
}



CNodeValue* ParseMFNode()
{
	return ParseMF(ParseNode);
}

CNodeValue* ParseMFVec3f()
{
	return ParseMF(ParseSFVec3f);
}

CNodeValue* ParseMFFloat()
{
	return ParseMF(ParseSFFloat);
}

CNodeValue* ParseMFInt32()
{
	return ParseMF(ParseSFInt32);
}



void ParseField(CVrmlNode* pNode, const char* szFieldID)
{
	NodeDesc* pNodeDesc = pNode->m_pNodeDesc;

	for (int i = 0; strcmp(szFieldID, pNodeDesc->m_rgfields[i].m_szFieldID) != 0; i += 1)
	{
		if (i >= pNodeDesc->m_nFieldCount - 1)
		{
			fprintf(stderr, "(%d): Unknown field ID: %s\n", nLine, szFieldID);
			exit(1);
		}
	}

	CNodeValue* pValue = NULL;

	switch (pNodeDesc->m_rgfields[i].m_type)
	{
	case MFColor:
		pValue = ParseMFColor();
		break;

	case MFFloat:
		pValue = ParseMFFloat();
		break;

	case MFInt32:
		pValue = ParseMFInt32();
		break;

	case MFNode:
		pValue = ParseMFNode();
		break;

	case MFRotation:
		pValue = ParseMFRotation();
		break;

	case MFString:
		pValue = ParseMFString();
		break;

	case MFTime:
		pValue = ParseMFTime();
		break;

	case MFVec2f:
		pValue = ParseMFVec2f();
		break;

	case MFVec3f:
		pValue = ParseMFVec3f();
		break;

	case SFBool:
		pValue = ParseSFBool();
		break;

	case SFColor:
		pValue = ParseSFColor();
		break;

	case SFFloat:
		pValue = ParseSFFloat();
		break;

	case SFImage:
		pValue = ParseSFImage();
		break;

	case SFInt32:
		pValue = ParseSFInt32();
		break;

	case SFNode:
		pValue = ParseNode();
		break;

	case SFRotation:
		pValue = ParseSFRotation();
		break;

	case SFString:
		pValue = ParseSFString();
		break;

	case SFTime:
		pValue = ParseSFTime();
		break;

	case SFVec2f:
		pValue = ParseSFVec2f();
		break;

	case SFVec3f:
		pValue = ParseSFVec3f();
		break;
	}

	pNode->m_rgpNodeValues[i] = pValue;
}

void ParseRoute()
{
	Token(); // nodeNameID.eventOutID

	Token(); // TO
	if (strcmp(szToken, "TO") != 0)
	{
		fprintf(stderr, "%d: expected 'TO'\n", nLine);
		exit(1);
	}

	Token(); // nodeNameID.eventInID
}

CNodeValue* ParseNode()
{
	char szNodeNameID [64];
	szNodeNameID[0] = 0;

	if (!Token())
		return NULL;

	if (strcmp(szToken, "ROUTE") == 0)
	{
		ParseRoute();
		return NULL;
	}

	if (strcmp(szToken, "USE") == 0)
	{
		Token();

		for (int i = defNodes.m_nCount - 1; i >= 0; i -= 1)
		{
			CVrmlNode* pVrmlNode = ((VSFNode*)defNodes.m_values[i])->m_value;

			if (strcmp(pVrmlNode->m_szNameID, szToken) == 0)
			{
				VSFNode* pSFNode = new VSFNode;
				pSFNode->m_value = pVrmlNode;
				pSFNode->m_bUSE = true;
				return pSFNode;
			}
		}

		fprintf(stderr, "%d: undefined USE node '%s'\n", nLine, szToken);
		return NULL;
	}

	if (strcmp(szToken, "DEF") == 0)
	{
		Token();
		strcpy(szNodeNameID, szToken);
		Token();
	}

	char szNodeTypeID [64];
	strcpy(szNodeTypeID, szToken);

	for (int i = 0; strcmp(szNodeTypeID, rgnodes[i].m_szNodeTypeID) != 0; i += 1)
	{
		if (i == sizeof (rgnodes) / sizeof (rgnodes[0]) - 1)
		{
			fprintf(stderr, "%d: unknown node type '%s'\n", nLine, szNodeTypeID);
			exit(1);
		}
	}

	CVrmlNode* pNode = new CVrmlNode(&rgnodes[i], szNodeNameID);

	if (PeekChar() == '{')
	{
		// Node Body
		Token();

		while (PeekChar() != '}')
		{
			Token();

			if (strcmp(szToken, "ROUTE") == 0)
			{
				ParseRoute();
			}
			else
			{
				char szFieldID [64];
				strcpy(szFieldID, szToken);

				ParseField(pNode, szFieldID);
			}
		}

		Token();
	}

	VSFNode* pValue = new VSFNode;
	pValue->m_value = pNode;

	if (pNode->m_szNameID != NULL)
		defNodes.Add(pValue);

	return pValue;
}

void ProcessFile(const char* szFileName)
{
	fprintf(stderr, "Converting %s...\n", szFileName);

	pFile = fopen(szFileName, "r");
	if (pFile == NULL)
	{
		fprintf(stderr, "Cannot open %s\n", szFileName);
		return;
	}

	pchLine = szLine;
	while (PeekChar() != 0)
	{
		CNodeValue* pValue = ParseNode();
		if (pValue != NULL)
			rootNodes.Add(pValue);
	}

	fclose(pFile);
}


////////////////////////////////////////////////////////////////////////////

int nTab = 0;

void Tabs()
{
	for (int i = 0; i < nTab; i += 1)
		printf("    ");
}

void DumpNode(VSFNode* pNode);

void DumpValue(FIELDTYPE type, CNodeValue* pValue)
{
	FIELDTYPE arrayType;

	switch (type)
	{
	case SFBool:
		{
			VSFBool* pNode = (VSFBool*)pValue;
			printf("%s\n", pNode->m_value ? "TRUE" : "FALSE");
		}
		break;

	case SFTime:
	case SFFloat:
		{
			VSFFloat* pNode = (VSFFloat*)pValue;
			printf("%f\n", pNode->m_value);
		}
		break;

	case SFInt32:
		{
			VSFInt32* pNode = (VSFInt32*)pValue;
			printf("%d\n", pNode->m_value);
		}
		break;

	case SFVec2f:
		{
			VSFVec2f* pNode = (VSFVec2f*)pValue;
			printf("%f %f\n", pNode->m_value[0], pNode->m_value[1]);
		}
		break;

	case SFColor:
	case SFVec3f:
		{
			VSFVec3f* pNode = (VSFVec3f*)pValue;
			printf("%f %f %f\n", pNode->m_value[0], pNode->m_value[1], pNode->m_value[2]);
		}
		break;

	case SFRotation:
		{
			VSFRotation* pNode = (VSFRotation*)pValue;
			printf("%f %f %f %f\n", pNode->m_x, pNode->m_y, pNode->m_z, pNode->m_angle);
		}
		break;

	case SFString:
		{
			VSFString* pNode = (VSFString*)pValue;
			printf("\"%s\"\n", pNode->m_value); // REVIEW: Need to escape quotes and back-slashes
		}
		break;

	case SFImage:
		break;

	case MFColor:
		arrayType = SFColor;
		goto LArray;

	case MFFloat:
		arrayType = SFFloat;
		goto LArray;

	case MFInt32:
		arrayType = SFInt32;
		goto LArray;

	case MFRotation:
		arrayType = SFRotation;
		goto LArray;

	case MFString:
		arrayType = SFString;
		goto LArray;

	case MFTime:
		arrayType = SFTime;
		goto LArray;

	case MFVec2f:
		arrayType = SFVec2f;
		goto LArray;

	case MFVec3f:
		arrayType = SFVec3f;
		goto LArray;

LArray:
		{
			CNodeValueArray* pValueArray = (CNodeValueArray*)pValue;

			if (pValueArray->m_nCount == 0)
			{
				printf("[]\n");
			}
			else if (pValueArray->m_nCount == 1)
			{
				// No brackets around single items!
				DumpValue(arrayType, pValueArray->m_values[0]);
			}
			else
			{
				printf("\n");
				Tabs();
				printf("[\n");
				nTab += 1;

				for (int i = 0; i < pValueArray->m_nCount; i += 1)
				{
					Tabs();
					printf("/* %d */ ", i);
					DumpValue(arrayType, pValueArray->m_values[i]);
	//				if (i < pValueArray->m_nCount - 1)
	//					printf(",\n");
				}

				nTab -= 1;
				Tabs();
				printf("]\n");
			}
		}
		break;

	case SFNode:
		DumpNode((VSFNode*)pValue);
		break;

	case MFNode:
		{
			printf("\n");
			Tabs();
			printf("[\n");
			nTab += 1;

			CNodeValueArray* pValueArray = (CNodeValueArray*)pValue;
			for (int i = 0; i < pValueArray->m_nCount; i += 1)
			{
				Tabs();
				DumpNode((VSFNode*)pValueArray->m_values[i]);
			}

			nTab -= 1;
			Tabs();
			printf("]\n");
		}
		break;

	default:
		printf("\n");
		break;
	}
}

void DumpNode(VSFNode* pSFNode)
{
	CVrmlNode* pVrmlNode = pSFNode->m_value;

	if (pSFNode->m_bUSE)
	{
		printf("USE %s\n", pVrmlNode->m_szNameID);
		return;
	}

	if (pVrmlNode->m_szNameID != NULL)
		printf("DEF %s ", pVrmlNode->m_szNameID);

	if (pVrmlNode->m_pNodeDesc->m_rgfields == IndexedFaceSetFields)
	{
		if (pVrmlNode->m_szNameID != NULL)
		{
			char szBuf [MAX_PATH];
			LegalizeFileName(szBuf, pVrmlNode->m_szNameID);
			printf("Mesh { url \"%s.xm\" }\n", szBuf);
			return;
		}
	}

	printf("%s\n", pVrmlNode->m_pNodeDesc->m_szNodeTypeID);

	Tabs();
	printf("{\n");

	nTab += 1;

	for (int i = 0; i < pVrmlNode->m_pNodeDesc->m_nFieldCount; i += 1)
	{
		if (pVrmlNode->m_rgpNodeValues[i] != NULL)
		{
			Tabs();
			printf("%s ", pVrmlNode->m_pNodeDesc->m_rgfields[i].m_szFieldID);
			DumpValue(pVrmlNode->m_pNodeDesc->m_rgfields[i].m_type, pVrmlNode->m_rgpNodeValues[i]);
		}
	}

	nTab -= 1;

	Tabs();
	printf("}\n");
}

void DumpNodes()
{
	for (int i = 0; i < rootNodes.m_nCount; i += 1)
		DumpNode((VSFNode*)rootNodes.m_values[i]);
}


////////////////////////////////////////////////////////////////////////////

void ConvertMesh(CVrmlNode* pNode)
{
	VSFNode* coord = pNode->GetSFNode("coord");
	CNodeValueArray* coordIndex = pNode->GetArray("coordIndex");
	VSFNode* normal = pNode->GetSFNode("normal");
	CNodeValueArray* normalIndex = pNode->GetArray("normalIndex");
	VSFNode* color = pNode->GetSFNode("color");
	CNodeValueArray* colorIndex = pNode->GetArray("colorIndex");
	VSFNode* texCoord = pNode->GetSFNode("texCoord");
	CNodeValueArray* texCoordIndex = pNode->GetArray("texCoordIndex");

	int nVertexStride = 0;
	DWORD fvf = 0;
	if (coord != NULL && coordIndex != NULL)
	{
		fvf |= D3DFVF_XYZ;
		nVertexStride += sizeof (float) * 3;
	}

	if (normal != NULL && normalIndex != NULL)
	{
		fvf |= D3DFVF_NORMAL;
		nVertexStride += sizeof (float) * 3;
	}

	if (color != NULL && colorIndex != NULL)
	{
		fvf |= D3DFVF_DIFFUSE;
		nVertexStride += sizeof (DWORD);
	}

	if (texCoord != NULL && texCoordIndex != NULL)
	{
		fvf |= D3DFVF_TEX1;
		nVertexStride += sizeof (float) * 2;
	}

	if (fvf == 0)
	{
		fprintf(stderr, "WARNING: skipping empty mesh '%s'\n", pNode->m_szNameID);
		return;
	}

	CNodeValueArray* coordPoint = coord->m_value->GetArray("point");
	if (coordPoint == NULL)
	{
		fprintf(stderr, "WARNING: skipping mesh with no Coordinate '%s'\n", pNode->m_szNameID);
		return;
	}

	CNodeValueArray* normalVector = NULL;
	if (normal != NULL)
		normalVector = normal->m_value->GetArray("vector");

	CNodeValueArray* colorColor = NULL;
	if (color != NULL)
		colorColor = color->m_value->GetArray("color");

	CNodeValueArray* texCoordPoint = NULL;
	if (texCoord != NULL)
		texCoordPoint = texCoord->m_value->GetArray("point");


	int nVertexCount = coordPoint->m_nCount;

	if (nVertexCount > 65535)
	{
		fprintf(stderr, "ERRR: mesh has too many vertices '%s'\n", pNode->m_szNameID);
		return;
	}

	int nTriangles = (coordIndex->m_nCount + 1) / 4;

	// Build an interleaved vertex buffer
	BYTE* pVertexBuffer = new BYTE [nVertexStride * (nTriangles * 3)];
	int nDestVertex = 0;
	for (int i = 0; i < coordIndex->m_nCount; i += 1)
	{
		int nVertexIndex = ((VSFInt32*)coordIndex->m_values[i])->m_value;
		if (nVertexIndex == -1)
			continue;

		BYTE* pbVert = pVertexBuffer + nVertexStride * nDestVertex;
		nDestVertex += 1;

		VSFVec3f* pVec3 = (VSFVec3f*)coordPoint->m_values[nVertexIndex];
		CopyMemory(pbVert, pVec3->m_value, sizeof (float) * 3);
		pbVert += sizeof (float) * 3;

		if (normalVector != NULL)
		{
			int nNormalIndex = ((VSFInt32*)normalIndex->m_values[i])->m_value;
			CopyMemory(pbVert, (VSFVec3f*)normalVector->m_values[nNormalIndex], sizeof (float) * 3);
			pbVert += sizeof (float) * 3;
		}

		if (colorColor != NULL)
		{
			int nColorIndex = ((VSFInt32*)colorIndex->m_values[i])->m_value;
			VSFColor* pColor = (VSFColor*)colorColor->m_values[nColorIndex];
			*(D3DCOLOR*)pbVert = D3DCOLOR_COLORVALUE(pColor->m_red, pColor->m_green, pColor->m_blue, 1.0f);
			pbVert += sizeof (DWORD);
		}

		if (texCoordPoint != NULL)
		{
			int nTexCoordIndex = ((VSFInt32*)texCoordIndex->m_values[i])->m_value;
			VSFVec2f* pVec2f = (VSFVec2f*)texCoordPoint->m_values[nTexCoordIndex];
			float vec [2];
			vec[0] = pVec2f->m_value[0];
			vec[1] = 1.0f - pVec2f->m_value[1];
			CopyMemory(pbVert, vec, sizeof (float) * 2);
			pbVert += sizeof (float) * 2;
		}
	}

	nVertexCount = nDestVertex;

	bool ccw = pNode->GetSFBool("ccw")->m_value;

	WORD* pIndexBuffer = new WORD [nTriangles * 3];
	for (i = 0; i < nTriangles; i += 1)
	{
		if (ccw)
		{
			pIndexBuffer[i * 3 + 0] = (WORD)i * 3 + 2;
			pIndexBuffer[i * 3 + 1] = (WORD)i * 3 + 1;
			pIndexBuffer[i * 3 + 2] = (WORD)i * 3 + 0;
		}
		else
		{
			pIndexBuffer[i * 3 + 0] = (WORD)i * 3 + 0;
			pIndexBuffer[i * 3 + 1] = (WORD)i * 3 + 1;
			pIndexBuffer[i * 3 + 2] = (WORD)i * 3 + 2;
		}
	}

	// TODO: Optimize buffer by removing duplicate vertices and combining indices...

/*
	// Build an interleaved vertex buffer
	BYTE* pVertexBuffer = new BYTE [nVertexStride * nVertexCount];
	for (int i = 0; i < coordIndex->m_nCount; i += 1)
	{
		int nVertexIndex = ((VSFInt32*)coordIndex->m_values[i])->m_value;
		if (nVertexIndex == -1)
			continue;

		BYTE* pbVert = pVertexBuffer + nVertexStride * nVertexIndex;

		VSFVec3f* pVec3 = (VSFVec3f*)coordPoint->m_values[nVertexIndex];
		CopyMemory(pbVert, pVec3->m_value, sizeof (float) * 3);
		pbVert += sizeof (float) * 3;

		if (normalVector != NULL)
		{
			int nNormalIndex = ((VSFInt32*)normalIndex->m_values[i])->m_value;
			CopyMemory(pbVert, (VSFVec3f*)normalVector->m_values[nNormalIndex], sizeof (float) * 3);
			pbVert += sizeof (float) * 3;
		}

		if (colorColor != NULL)
		{
			int nColorIndex = ((VSFInt32*)colorIndex->m_values[i])->m_value;
			VSFColor* pColor = (VSFColor*)colorColor->m_values[nColorIndex];
			*(D3DCOLOR*)pbVert = D3DCOLOR_COLORVALUE(pColor->m_red, pColor->m_green, pColor->m_blue, 1.0f);
			pbVert += sizeof (DWORD);
		}

		if (texCoordPoint != NULL)
		{
			int nTexCoordIndex = ((VSFInt32*)texCoordIndex->m_values[i])->m_value;
			VSFVec2f* pVec2f = (VSFVec2f*)texCoordPoint->m_values[nTexCoordIndex];
			float vec [2];
			vec[0] = pVec2f->m_value[0];
			vec[1] = 1.0f - pVec2f->m_value[1];
			CopyMemory(pbVert, vec, sizeof (float) * 2);
			pbVert += sizeof (float) * 2;
		}
	}

	int nTriangles = (coordIndex->m_nCount + 1) / 4;
	WORD* pIndexBuffer = new WORD [nTriangles * 3];
	for (i = 0; i < nTriangles; i += 1)
	{
		int n1 = ((VSFInt32*)coordIndex->m_values[i * 4 + 0])->m_value;
		int n2 = ((VSFInt32*)coordIndex->m_values[i * 4 + 1])->m_value;
		int n3 = ((VSFInt32*)coordIndex->m_values[i * 4 + 2])->m_value;
		int n4 = (i * 4 + 3 < coordIndex->m_nCount) ? ((VSFInt32*)coordIndex->m_values[i * 4 + 3])->m_value : -1;

		if (n1 == -1 || n2 == -1 || n3 == -1 || n4 != -1)
		{
			fprintf(stderr, "ERROR: mesh is not triangulated '%s'\n", pNode->m_szNameID);
			delete [] pIndexBuffer;
			delete [] pVertexBuffer;
			return;
		}

		if ((unsigned)n1 > 65535 || (unsigned)n2 > 65535 || (unsigned)n3 > 65535)
		{
			fprintf(stderr, "ERROR: mesh has out of range index '%s'\n", pNode->m_szNameID);
			delete [] pIndexBuffer;
			delete [] pVertexBuffer;
			return;
		}

		pIndexBuffer[i * 3 + 0] = (WORD)n1;
		pIndexBuffer[i * 3 + 1] = (WORD)n2;
		pIndexBuffer[i * 3 + 2] = (WORD)n3;
	}
*/

	D3DPRIMITIVETYPE m_primitiveType = D3DPT_TRIANGLELIST;
	int m_nFaceCount = nTriangles;
	DWORD m_fvf = fvf;
	int m_nVertexStride = nVertexStride;
	int m_nVertexCount = nVertexCount;
	int m_nIndexCount = nTriangles * 3;
	BYTE* m_pVertexBuffer = pVertexBuffer;
	WORD* m_pIndexBuffer = pIndexBuffer;

	// BLOCK: Write the file...
	{
		char szFileName [MAX_PATH];
		LegalizeFileName(szFileName, pNode->m_szNameID);
		strcat(szFileName, ".xm");

		HANDLE hFile = CreateFile(szFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			fprintf(stderr, "ERROR: cannot create file '%s'\n", szFileName);
//			exit(1);
		}
		else
		{
			DWORD dwValue, dwWritten;

			dwValue = (DWORD)m_primitiveType;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			dwValue = (DWORD)m_nFaceCount;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			dwValue = (DWORD)m_fvf;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			dwValue = (DWORD)m_nVertexStride;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			dwValue = (DWORD)m_nVertexCount;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			dwValue = (DWORD)m_nIndexCount;
			WriteFile(hFile, &dwValue, 4, &dwWritten, NULL);

			WriteFile(hFile, m_pVertexBuffer, m_nVertexCount * m_nVertexStride, &dwWritten, NULL);

			WriteFile(hFile, m_pIndexBuffer, m_nIndexCount * sizeof (WORD), &dwWritten, NULL);

			CloseHandle(hFile);
		}
	}

	delete [] pIndexBuffer;
	delete [] pVertexBuffer;
}

void SaveMeshes()
{
	// Looking at defNodes since we need named nodes to get file names...
	for (int i = 0; i < defNodes.m_nCount; i += 1)
	{
		CVrmlNode* pNode = ((VSFNode*)defNodes.m_values[i])->m_value;

		if (pNode->m_pNodeDesc->m_rgfields == IndexedFaceSetFields)
		{
			fprintf(stderr, "saving mesh %s\n", pNode->m_szNameID);
			ConvertMesh(pNode);
		}
	}
}

////////////////////////////////////////////////////////////////////////////

bool bDumpNodes = false;

float force_floating_point;

int __cdecl main(int argc, char* argv[])
{
	force_floating_point = sinf(1.0f);

	bool bNeedHelp = true;

	for (int i = 1; i < argc; i += 1)
	{
		const char* sz = argv[i];

		if (sz[0] == '-' || sz[0] == '/')
		{
			sz += 1;
			while (*sz != '\0')
			{
				switch (*sz++)
				{
				case '?':
					goto LNeedHelp;

				case 'd':
				case 'D':
					bDumpNodes = true;
					break;
				}
			}
		}
		else
		{
			bNeedHelp = false;
			ProcessFile(sz);
		}
	}

	if (bNeedHelp)
	{
LNeedHelp:
		fprintf(stderr, "This program converts a VRML97 file to XDash style 3D mesh files.\n");
		fprintf(stderr, "Usage:\n\twrl2xm <file.wml>\n");
		exit(1);
	}

	if (bDumpNodes)
		DumpNodes();

	SaveMeshes();

	return 0;
}
