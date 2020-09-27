// -*- Mode:C++ -*-
//
//      Copyright (c) 1997 Microsoft Corporation.  All rights reserved.
//
//    This work is a trade secret of Microsoft Corporation.
//    Unauthorized disclosure is unlawful.
//
// $Header: /ast/clients/cmdcl/project.h 13    7/12/98 5:03p Rfc $
// $NoKeywords: $
//
//
// File:    clients/cmdcl/project.h
// Creator: Roger Crew

// Exported Function List: 

#ifndef INC_PROJECT_H
#define INC_PROJECT_H

///////////////////////////////////////////////////////////////////////////////
// Class structure for projects
//
//   hit ^X^E at end of this line (cxx-replace-class-hierarchy)

class AstProject;

////END////


#pragma warning(disable:4512 4511 4100)
#include "ast.h"
#include "c1cmdln.h"

class AstProject : public WithOwnUnalignedArena
{
  public:
    class ObjFile;
    struct ObjFileIterator;

    class LinkEntry;
    struct LinkEntryIterator;

    enum LinkKind {
        lkUnknown,              // LIB/LINK we have no idea what it's doing
        lkLink,                 // LINK (build an .exe or .dll)
        lkLib,                  // LIB  (build a .lib)
        lkDef,                  // LIB /DEF (build .lib + .exp)
        lkExtract,              // LIB /EXTRACT (extract .obj)
        lkError                 // illegal value
    };

    class ObjAuxFile;           // pdb/pch reference

    typedef ObjFile *(*ObjFileBuilder)(
        Allocator *resultAllocator, int key, const C1Cmdline &fdata
      );

    typedef LinkEntry *(*LinkBuilder)(
        Allocator *resultAllocator, LinkKind kind, 
        const char *binname, const char *wd, const char **args
      );

  private:
    static char *ildirectory;   // where to create new IL files

    char     *   name;          // print name
    ObjFile **   objfiles;      // C1/C1xx/C2 entries
    int          objcount;
    LinkEntry ** linkentries;   // LINK/LIB entries
    int          linkcount;

  internal:
    Allocator *GetStringAllocator() { return pAllocator; }

  public:
    AstProject(char *name):
        name(name),
        objfiles(NULL), 
        objcount(0),
        linkentries(NULL),
        linkcount(0)
      { }
    ~AstProject();

    // Two ways to create a project.  In both cases ObjFiles are
    // allocated in resultAllocator and fileBuilder (NULL => DefaultBuilder)
    // is used to create them.
    // 
    // (1) Create a project from a file
    static AstProject *NewFileProject(
        char *filename,
        ArenaAllocator *resultAllocator,
        ObjFileBuilder fileBuilder = NULL,
        LinkBuilder linkBuilder    = NULL,
        char *defaultName          = NULL,
        char *ilDirectory          = NULL,
        bool fNoRelocation         = FALSE
      );

    // (2) Create a project from an invocation of the CL driver on argc/argv
    static AstProject *NewCLProject(
        int argc, 
        char **argv,
        ArenaAllocator *resultAllocator,
        ObjFileBuilder fileBuilder = NULL,
        LinkBuilder linkBuilder    = NULL,
        char *defaultName          = NULL,
        char *ilDirectory          = NULL
      );

    static char * GetIlDirectory()           { return ildirectory; }
    static char * InitIlDirectory(char *ild = NULL, bool fMustWork = TRUE);

    char *        GetPrintName()             { return name ? name : "<<unnamed>>"; }
    void          SetPrintName(char *rename) { name = rename; }
    ObjFile **    GetFileArray()             { return objfiles; }
    int           GetFileCount()             { return objcount; }
    ObjFile *     NthFile(int n) const 
      { return (0 <= n && n < objcount) ? objfiles[n] : NULL; }

    // Write out project to OUTS,
    // Exclude -E files unless fKeepCPPs is true
    // Exclude ilname==NULL files if fStripUnbuilt is true
    // Reset all .fGenerated flags
    void          Write(OutputStream *outs, 
                        bool fKeepCPPs = FALSE, 
                        bool fStripUnbuilt = FALSE);

    enum  AuxKind { akPDB = 0, akPCH, akERROR, akCOUNT=akERROR };
    void          RelocateAuxFiles(AuxKind, Allocator *);
    static bool   fTrustPCHdates;

    void          RestoreEmbeddedSources();

    friend class  PFileReader;
    friend class  ProjectBuilder;
    friend class  ObjFile;
    friend struct ObjFileIterator;
    friend struct LinkEntryIterator;
};


class AstProject::ObjFile : public C1Cmdline
{
    friend class PFileReader;
    friend class PCapture;
    friend class ProjectBuilder;
    friend class AstProject;
    friend class AstProject::ObjAuxFile;

    AstProject *parent;

    int   key;                  // 
    FrontEndType language;      // C, C++, or what?

    char  *ilname;              // name generated for IL (NULL => must run C1)
    time_t ildate;              // creation time for ilname+"DB"
    IlPath ilPath;
    bool   fGenerated;          // true iff the IL files were generated during this
                                // session and the corresponding IL name has not
                                // been written out to a new log file.

    bool    fc2Dll;             // are c2 args in DLL format?
    char  **c2args;             // argument vector for c2 (NULL => c2 never run)
    char   *pdbname;            // name of corresponding .pdb file or NULL
    char   *pchname;            // name of corresponding .pch file or NULL
    time_t       pchdate;
    unsigned long pchsig;

    char **esource;             // embedded source

    // how might building IL depend on a .pch/.pdb file
    typedef AstProject::ObjAuxFile ObjAuxFile;
  public:
    typedef AstProject::AuxKind AuxKind;
    enum AuxRefKind {
        arkNone,                // no dependency
        arkCreator,             // building IL creates it (-Yc -YX -Zi)
        arkUser,                // it must exist before IL can be built (-Yu)
    };
  internal:
    void SetAuxName(AuxKind ak, char *newname) {
        switch(ak) { 
          default:    AST_NOT_REACHED;   break;
          case akPCH: pchname = newname; break;
          case akPDB: pdbname = newname; break;
        }
    }

  private:
    struct AuxRef {
        AuxRefKind kind;
        ObjAuxFile *file;
        AuxRef(): kind(arkNone), file(NULL) { }
    };
    AuxRef aux[akCOUNT];

    Module *module;

    // set .ilname if args[ilindex] is for real
    void CheckExistingIL(time_t ildate);
    FrontEndType ExecLanguage(char *path);

  public:
    int          GetKey() const              { return key; }
    char *       GetSrcName() const          { return srcname; }
    char *       GetObjName() const          { return objname; }
    char *       GetWorkingDirectory() const { return cwd; }
    char *       GetILName() const           { return ilname; }
    IlPath *     GetILPath()                 { return &ilPath; }
    FrontEndType GetLanguage() const         { return language; }
    char **      GetC1Args() const          { return args; }
    char **      GetC2Args() const          { return c2args; }

    void         SetC1Args(char **newargs) { args   = newargs; }
    void         SetC2Args(char **newargs) { c2args = newargs; }

    bool         ReplaceC1Arg(char *arg, int prefix, char *newarg);
    void         AppendC1Arg(char *newarg);

    // if necessary, generate ilname and run C1
    bool         EnsureIlPath(bool fRequired = TRUE, bool fForceRebuild = FALSE);       
    bool         FIlExists()         { return ilname != NULL; }
    bool         FModuleExists()     { return module != NULL; }
    bool         FSrcExists();

    // Run preprocessor on this file write output to outfile.
    bool         RunCPP(char *outfile);
    static bool  fDebugC1;      // use astc1d instead of astc1?

    ObjFile(int key, const C1Cmdline &fdata):
        C1Cmdline(fdata),
        parent(NULL), key(key), language(ExecLanguage(fdata.args[0])), 
        ilname(NULL), fGenerated(FALSE), ildate(0),
        module(NULL),
        pchname(NULL), pdbname(NULL),
        pchdate(0), pchsig(0),
        esource(NULL),
        c2args(NULL)
      { }
    ~ObjFile();

    static ObjFile *DefaultBuilder(
        Allocator *resultAllocator,
        // args to ObjFile constructor
        int key, 
        const C1Cmdline &fdata)
      { 
          return new(resultAllocator) ObjFile(key, fdata);
      }

    void PutName(OutputStream *o)
      { o->Printf("%s(%d)", objname, key); }

    Module *        GetModule(bool fRequired = TRUE);
    void            KillModule(bool fIlToo = FALSE);
    void            KillIlFiles();
    SymbolTable *   GetGlobalSymbolTable() { return GetModule()->GetGlobalSymbolTable(); }
    AstBlock *      GetInitBlock()         { return GetModule()->GetInitBlock(); }
    SymbolManager * GetSymbolManager()     { return GetModule()->GetSymbolManager(); }
    TypeManager *   GetTypeManager()       { return GetModule()->GetTypeManager(); }
    AstManager *    GetAstManager()        { return GetModule()->GetAstManager(); }

    void Write(OutputStream *outs);
  internal:
    void SetParent(AstProject *p) { parent = p; }
};

struct AstProject::ObjFileIterator
{
    typedef AstProject::ObjFile ObjFile;
    ObjFile **after;
    ObjFile **current;
  public:
    ObjFileIterator(AstProject *project): 
        current(project->objfiles - 1),
        after(project->objfiles + project->objcount)
      { }
    bool      FNext()    { return (++current < after); }
    ObjFile * NextFile() { return (++current < after) ? *current : NULL; }
    ObjFile * GetFile() const
      { AstAssert (current < after); return *current; }

    // included for hack purposes, ignore it for now...
    ObjFile * PrevFile() { return current[-1]; }
};

class AstProject::ObjAuxFile
{
  public:
    typedef AstProject::ObjFile::AuxKind Kind;

    Kind        kind;              // .pch? .pdb? or what?
    bool        fBuilt;            // IL has been built for some creator
    ObjFile    *firstCreator;      // the first creator (always non-NULL)
    ObjAuxFile *prev;              // previous instance of this file
                                   // (i.e., if file was rewritten during build)

    char *GetWorkingDirectory()   
      { return firstCreator->GetWorkingDirectory(); }
    char *GetName() 
      { 
          switch (kind) { 
            default: 
              AST_NOT_REACHED; 
              return NULL;      // shut up compiler
            case AstProject::akPCH: 
              return firstCreator->pchname;
            case AstProject::akPDB: 
              return firstCreator->pdbname;
          }
      }

    ObjAuxFile(ObjFile *creator, Kind kind, bool fBuilt, ObjAuxFile *prev):
        firstCreator(creator),
        kind(kind),
        prev(prev),
        fBuilt(fBuilt)
      { }

    // If there exists a creator for which ilname is set
    // (and it passes CheckExistingIL()) .firstCreator is the first such;
    // otherwise, .firstCreator is the first creator in the file.

    // It is possible for a .pch to be referenced only via -Yu flags,
    // in which case, we do not bother to create an ObjAuxFile entry
    // since there will be nothing we can do if the .pch does not exist
    // (or is bogus).

    // General:
    //   If IL has already been built for some creator,
    //     fBuilt will be set TRUE and we assume that
    //     the auxiliary file is genuine
    //   When building IL for a creator and fBuilt is FALSE,
    //     we first check if the auxiliary file exists and,
    //     if so, blow it away (since nobody is depending
    //     on it and it might be bogus),
    //   When building IL for a user and fBuilt is FALSE
    //     we first invoke the IL build for .firstCreator
  internal:
    void RemoveIfBogus(char *name);
    void SetParent(AstProject *p) { } // keep V5 compiler happy
};


class AstProject::LinkEntry
{
    friend class PFileReader;
    friend class PCapture;
    friend class ProjectBuilder;
    friend class AstProject;

    typedef AstProject::ObjFile  ObjFile;
    typedef AstProject::LinkKind LinkKind;
  public:

    LinkKind      GetKind() const             { return kind; }
    const char *  GetWorkingDirectory() const { return wd; }
    const char ** GetArgs() const             { return args; }
    const char *  GetBinName() const          { return bin; }

    // list of .obj files this depends on
    size_t        GetFileCount() const        { return C1Cmdline::CountArgs(firstFile); }
    const char ** GetFilenameVector() const   { return firstFile; }
    ObjFile    ** GetObjFileVector() const    { return objIndex; }

    static LinkEntry *DefaultBuilder(
        Allocator *resultAllocator, 
        // args to LinkEntry constructor
        LinkKind kind, 
        const char *binname, 
        const char *wd, 
        const char **args)
      { 
          return new(resultAllocator) LinkEntry(kind, binname, wd, args);
      }

    LinkEntry(LinkKind kind, const char *binName, const char *wd, const char **args);

    ~LinkEntry() { }

    void Write(OutputStream *outs);

  private:
    LinkKind kind;
    const char *  wd;
    const char *  bin;          // what this entry builds
    const char ** args;         // all arguments (arg0, then switches, then files)

    const char ** firstFile;    // first filename argument
    AstProject *  parent;
    int           objsBefore;   // number of c1 entries preceding this one
    ObjFile **    objIndex;     // for each file, corresponding ObjFile or NULL

  internal:
    void SetParent(AstProject *p) { parent = p; }
  public:
    static const char *LinkDescription(LinkKind);
};

struct AstProject::LinkEntryIterator
{
    typedef AstProject::LinkEntry LinkEntry;
    LinkEntry **after;
    LinkEntry **current;
  public:
    LinkEntryIterator(AstProject *project): 
        current(project->linkentries - 1),
        after(project->linkentries + project->linkcount)
      { }
    bool        FNext()     { return (++current < after); }
    LinkEntry * NextEntry() { return (++current < after) ? *current : NULL; }
    LinkEntry * GetEntry() const
      { AstAssert (current < after); return *current; }
};

#endif // INC_PROJECT_H
