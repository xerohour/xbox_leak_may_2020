// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFileObject_LoadTypeInfo.cpp
// Contents:  
// Revisions: 28-Jan-2002: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ TYPEDEFS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct {
    char szExt[MAX_PATH];
    char szType[MAX_PATH];
    HIMAGELIST himl;
    int iIcon;
} sTypeInfo;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ STATIC GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

sTypeInfo g_tiFolder = {NULL};
sTypeInfo g_tiFile   = {NULL};

static sTypeInfo *g_rgiconexts = NULL;
static int g_cLoadedExts    = 0;
static int g_cMaxLoadedExts = 0;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CFileObject::LoadTypeInfo()
{
    SHFILEINFO sfi;
    if (!m_fTypeInfoLoaded)
    {
        // Have we already loaded the icon information for this type of file?
        if (m_fIsDir)
        {
            // Loaded dir icon?
            if (g_tiFolder.himl == NULL)
            {
                g_tiFolder.himl = (HIMAGELIST)SHGetFileInfo(m_szPath, 0, &sfi,
                                                         sizeof(sfi),
                                                         SHGFI_SYSICONINDEX |
                                                         SHGFI_SMALLICON |
                                                         SHGFI_TYPENAME);
                g_tiFolder.iIcon = sfi.iIcon;
                strcpy(g_tiFolder.szType, sfi.szTypeName);
            }
            m_himl = g_tiFolder.himl;
            m_iIcon = g_tiFolder.iIcon;
            strcpy(m_szType, g_tiFolder.szType);
        }
        else
        {
            // Search by extension
            char *pszExt = strrchr(m_szPath, '.');
            if (pszExt == NULL)
            {
                // Force default extension; we could load the file's actual
                // icon, but it's not worth the perf hit
                if (g_tiFile.himl == NULL)
                {
                    g_tiFile.himl = (HIMAGELIST)SHGetFileInfo(m_szPath, 0, &sfi,
                                                        sizeof(sfi),
                                                        SHGFI_SYSICONINDEX |
                                                        SHGFI_SMALLICON |
                                                        SHGFI_TYPENAME);
                    g_tiFile.iIcon = sfi.iIcon;
                    strcpy(g_tiFile.szType, sfi.szTypeName);
                }
                m_himl = g_tiFile.himl;
                m_iIcon = g_tiFile.iIcon;
                strcpy(m_szType, g_tiFile.szType);
            }
            else
            {
                // Has extension already been loaded?
                // undone-perf: for now, array is fine, although eventually may
                // want to move to hash.
                bool fLoad = true;
                for (int i = 0; i < g_cLoadedExts; i++)
                {
                    if (!_stricmp(g_rgiconexts[i].szExt, pszExt))
                    {
                        // Found it
                        m_himl = g_rgiconexts[i].himl;
                        m_iIcon = g_rgiconexts[i].iIcon;
                        strcpy(m_szType, g_rgiconexts[i].szType);
                        fLoad = false;
                        break;
                    }
                }

                if (fLoad)
                {
                    m_himl = (HIMAGELIST)SHGetFileInfo(m_szPath, 0, &sfi, sizeof(sfi),
                                  SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_TYPENAME);
                    m_iIcon = sfi.iIcon;
                    strcpy(m_szType, sfi.szTypeName);

                    // Add to list
                    if (g_rgiconexts == NULL ||
                        g_cLoadedExts >= g_cMaxLoadedExts)
                    {
                        g_cMaxLoadedExts += 20;
                        g_rgiconexts = (sTypeInfo*)realloc(g_rgiconexts,
                                       g_cMaxLoadedExts * sizeof sTypeInfo);
                    }
                    g_rgiconexts[g_cLoadedExts].iIcon = m_iIcon;
                    g_rgiconexts[g_cLoadedExts].himl = m_himl;
                    strcpy(g_rgiconexts[g_cLoadedExts].szExt, pszExt);
                    strcpy(g_rgiconexts[g_cLoadedExts].szType, sfi.szTypeName);
                    g_cLoadedExts++;
                }
            }
        }
        m_fTypeInfoLoaded = true;
    }
}

