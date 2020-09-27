// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CXmlNode.h
// Contents:  Simple XML generation class
// Revisions: 4-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// MAX_XML_ATTRIBUTES       -- Maximum number of attributes in an XML node.
#define MAX_XML_ATTRIBUTES 10


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Forward Class declarations
class CFile;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CXmlNode
// Purpose: A single node in an XML tree.  Can be root, middle, or leaf node.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CXmlNode
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    
    // CXmlNode     -- CXmlNode Constructor
    CXmlNode(char *szName);

    // ~CXmlNode    -- CXmlNode Destructor
    ~CXmlNode();


    // ++++ MISCELLANEOUS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // AddElement   -- Creates a new CXmlNode and adds it to our list of child ndoes
    CXmlNode *AddElement(char *szName);

    // AddAttr      -- Adds a new attribute to this node
    void AddAttr(char *szName, char *szValue);

    // WriteToFile  -- Dumps this node's XML tree to the specified file in human-readable format.
    HRESULT WriteToFile(CDlg *pdlg, CFile *poutfile, int nDepth);


private:

    // m_rgpxnChildren  -- The list this node's of child nodes.
    CXmlNode **m_rgpxnChildren;

    // m_cpxnAlloc      -- The maximum size of the m_rgpxnChildren array
    int m_cpxnAlloc;

    // m_cChildren      -- The number of child nodes.
    int m_cChildren;

    // m_rgszAttrName   -- The list of attribute names.
    char m_rgszAttrName[MAX_XML_ATTRIBUTES][100];

    // m_rgszAttrValue  -- The list of attribute values.
    char m_rgszAttrValue[MAX_XML_ATTRIBUTES][100];

    // m_cAttrs         -- The number of attributes this node has
    int m_cAttrs;

    // m_szName         -- This node's name
    char m_szName[100];
};
