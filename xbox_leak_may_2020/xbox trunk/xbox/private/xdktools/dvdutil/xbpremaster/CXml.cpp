// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CXml.cpp
// Contents:  Simple XML generation class.
// Revisions: 4-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ EXTERNAL VARIABLE DECLARATIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// g_fOutOfMemory   -- Set if the application runs out of allocatable memory.  Caught by caller.
extern bool g_fOutOfMemory;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXmlNode::CXmlNode
// Purpose:   Constructor for a node in an xml tree.
// Arguments: szName        -- Name of the node
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXmlNode::CXmlNode(char *szName)
{
    strcpy(m_szName, szName);
    m_rgpxnChildren = NULL;
    m_cpxnAlloc = 0;
    m_cChildren = 0;
    m_cAttrs = 0;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXmlNode::~CXmlNode
// Purpose:   Destructor for CXmlNode.  Frees it's allocated children (recursively).
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXmlNode::~CXmlNode()
{
    // Free up all of our child nodes
    for (int i = 0; i < m_cChildren; i++)
        delete m_rgpxnChildren[i];
    free(m_rgpxnChildren);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXmlNode::AddElement
// Purpose:   Creates a new node with the specified name and adds it to our list of child nodes
// Arguments: szName            -- name of the newly created node
// Return:    A pointer to the newly created node.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CXmlNode *CXmlNode::AddElement(char *szName)
{
    /* Make sure we have room for another node */
    if(m_cChildren == m_cpxnAlloc) {
        CXmlNode **rgpxnOld = m_rgpxnChildren;
        m_rgpxnChildren = (CXmlNode **)malloc((m_cpxnAlloc += 100) *
            sizeof(CXmlNode *));
        if(rgpxnOld) {
            memcpy(m_rgpxnChildren, rgpxnOld, m_cChildren * sizeof(CXmlNode *));
            free(rgpxnOld);
        }
    }

    // Create the new node and add it to our list of children
    CXmlNode *pxmlNew = new CXmlNode(szName);
    if (pxmlNew == NULL)
    {
        // We're out of memory.  Mark that that is what happened, and throw an exception (which
        // will be caught by the caller)
        g_fOutOfMemory = true;
        throw;
    }

    m_rgpxnChildren[m_cChildren++] = pxmlNew;

    // Return a pointer to the newly created node
    return pxmlNew;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXmlNode::AddAttr
// Purpose:   Adds a new attribute to this node
// Arguments: szName            -- Name of the attribute to add
//            szValue           -- Value of the newly added attribute
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CXmlNode::AddAttr(char *szName, char *szValue)
{
    // This very simple XML tree object only supports a fixed number of attributes per node.  This
    // is to avoid the hassle of generating and freeing up linked lists.  The same is done for
    // child nodes above.  While a linked list would be minimal effort to add, it's not worth the
    // potential for added bugs when we know we'll never need it.
    assert (m_cAttrs < MAX_XML_ATTRIBUTES);

    // Track the attribute's name and value.
    strcpy(m_rgszAttrName[m_cAttrs], szName);
    strcpy(m_rgszAttrValue[m_cAttrs], szValue);
    m_cAttrs++;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CXmlNode::WriteToFile
// Purpose:   Recursively dump this XmlNode's structure to the specified file.
// Arguments: pdlg              -- Dialog that will handle error I/O
//            poutfile          -- File to dump to.
//            nDepth            -- Depth of the tree; used for indenting (ease of reading)
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
HRESULT CXmlNode::WriteToFile(CDlg *pdlg, CFile *poutfile, int nDepth)
{
    char sz[1024], szIndent[100], sz2[1024];
    HRESULT hr;

    // Create the indentation string.  Included just for ease of reading the generated XML file.
    memset(szIndent, ' ', nDepth * 4);
    szIndent[nDepth * 4] = '\0';

    // Output this node's tag.  Special case the schema.
    if (nDepth == 0)
        sprintf(sz, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n%s<%s", szIndent, m_szName);
    else
        sprintf(sz, "%s<%s", szIndent, m_szName);

    // Output this node's attributes.
    for (int i = 0; i < m_cAttrs; i++)
    {
        sprintf(sz2, " %s=\"%s\"", m_rgszAttrName[i], m_rgszAttrValue[i]);
        strcat(sz, sz2);
    }

    // Close off this node's attributes
    if (m_cChildren == 0)
        strcat(sz, "/");
    strcat(sz, ">\r\n");

    // Dump this node's contents to the file, and then each of it's children
    hr = poutfile->Write((BYTE*)sz, strlen(sz));
    if (hr != S_OK)
    {
        poutfile->InformError(pdlg->GetHwnd(), hr, poutfile->GetFilename());
        return hr;
    }

    // Iterate over each child
    for (i = 0; i < m_cChildren; i++)
    {
        hr = m_rgpxnChildren[i]->WriteToFile(pdlg, poutfile, nDepth + 1);
        if (hr != S_OK)
        {
            // The child's function already called InformError, so just exit
            return hr;
        }
    }

    // If there were any children, then we need to close off the tag.
    if (m_cChildren)
    {
        // Dump the closing tag (ie "</REGION>")
        sprintf(sz, "%s</%s>\r\n", szIndent, m_szName);
        poutfile->Write((BYTE*)sz, strlen(sz));
        if (hr != S_OK)
        {
            poutfile->InformError(pdlg->GetHwnd(), hr, poutfile->GetFilename());
            return hr;
        }
    }
    return S_OK;
}
    
