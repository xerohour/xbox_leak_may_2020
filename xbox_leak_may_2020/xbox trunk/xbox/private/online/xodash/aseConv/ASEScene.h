#include "std.h"
#include "PrimitiveScene.h"
#include <list>

using namespace std;

#define NODE_NAME_LEN 64
class File;
class CSVFile;

  #pragma pack(1)

  struct ButtonInfo

    {

        long    m_nButtonSize;       // size of the particular ButtonInfo instance 
        char    m_cTextObjectID2;
        char    m_TG_ShapeID[NODE_NAME_LEN];   // ID of the base shape
        char    m_cTextObjectID;    // ID (index) of the text object defined below
        long    m_buttonEnumID;
        long    m_orientation;
        unsigned char       m_cStandardMatCount;   // TG_Shape info for setting materials
        unsigned char       m_cEgglowMatCount;
        float   m_fXLoc;
        float   m_fYLoc;
        char**  m_cStandardMaterials;
        char**  m_cEgglowMaterials;

    };
 

    struct TextInfo

    { 

        long  m_nTextInfoSize;  // the size of the particular TextInfo instance
        char    m_TG_ShapeID[NODE_NAME_LEN];       // ID of the base shape
        char    m_cVerticalAlignment;   // alignment
        char    m_cHorizontalAlignment; // alignment
        char    m_bSingleLine;          // wether this text wraps across multiple lines
        long    m_nFontType;            // body or header (NATALY, should this be a string?)  
        float   m_fHeight;              // height of the field
        float   m_fWidth;               // width of the field
        char    m_bIsHelpText;          // is this a help text
        float   m_fScaleX;              // text scale
        float   m_fScaleY;              // text scale
        float   m_fScrollRate;          
        float   m_fScrollDelay;
        float   m_fTimeToScroll;  
        bool    m_bLocalize;
        TCHAR*   m_pTextID;             // maybe an integer????  

    };


    struct SpinnerInfo
    {
        char    m_TG_ShapeID[NODE_NAME_LEN]; // ID of the base shape
        ButtonInfo* m_pButton;
    };


    struct ListBoxInfo
    {
        char    m_TG_ShapeID[NODE_NAME_LEN]; // ID of the base shape
        std::list< ButtonInfo* >  m_buttonList;
    };

    #pragma pack()

    struct StringInfo // read from csv
    {
       char     m_szFileName[64];                 // friendly name, first column of csv
       char     m_TG_ShapeID[NODE_NAME_LEN];     // shape this belongs to
       char     m_TextID[64];
       bool     m_bLocalize;
       float    m_fXScale;
       float    m_fYScale; 
       float    m_fScrollRate;
       float    m_fScrollDelay;
       char     m_cHorizAlignment;
       char     m_cVertAlignment;
       bool     m_bMultiLine;
       char     m_Font;
       bool     m_bHelp;


    };


class ASEScene : public CPrimitiveScene
{
 
    public:

    ASEScene();

    virtual ~ASEScene();
    
    // HELPER FUNCTIONS
    HRESULT CreateIntelligentObjects( TG_Shape* pShap, const char* pNameInCSV );
    HRESULT CreateButtonObject( TG_Shape* pShape, const char* pNameInCSV );
    HRESULT CreateTextObject( TG_Shape* pShape, const char* pNameInCSV );
    HRESULT CreateSpinner( TG_Shape* pShape, const char* pNameInCSV );
    HRESULT CreateListBox( TG_Shape* pShape, const char* pNameInCSV );



    void WriteButtons( File* file );
    void WriteTexts( File* file );
    void WriteSpinners( File* file );
    void WriteListBoxes( File* file );

    HRESULT SaveBinary( const char* strFilename  );


   	virtual void Load( const char* pBuffer );

    void    LoadStrings( CSVFile* pFile, const char* aseName, const char** pSceneName, long sceneCount   );

    void    FindTG_ShapeWildCards( std::list< TG_Shape* >&,TG_Shape* pShape,  const char* pWildCard );


    void    SortButtons(const char* pIDName );
    void    ClearIntelligentObjects();




    private:

        char    m_ButtonCount;
        char    m_TextCount;
        char    m_ListBoxCount;
        char    m_SpinnerCount;

        char    m_FirstHorizontalButton;
        char    m_FirstVerticalButton;

        long    m_ButtonOffset;     // offset to the first one in the file
        long    m_TextOffset;       // offset to the first one in the file
        long    m_ListBoxOffset;    // offset to the first one in the file
        long    m_SpinnerOffset;    // offset to the first one in the file

        char    m_SceneName[256];   // name of the ase file
        char**  m_SceneStringNames; // name in the csv file identifying strings
        long    m_SceneStringNameCount; // number of the above

        std::list< ButtonInfo*>  m_ButtonList;
        std::list< TextInfo* >    m_TextList;
        std::list< SpinnerInfo* > m_SpinnerList;
        std::list< ListBoxInfo* > m_ListBoxList;
        std::list< StringInfo* >  m_StringList;

};