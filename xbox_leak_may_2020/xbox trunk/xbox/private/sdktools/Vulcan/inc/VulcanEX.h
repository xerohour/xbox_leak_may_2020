/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: Vulcanex.h
*
* File Comments:
*
*
***********************************************************************/


#ifndef __VULCANAPI__
#error  This file should be included via vulcanapi.h
#endif

class VSrcInfo
{
public:
    // Returns the file name (exactly as it appears in the pdb information)
    VULCANDLL const char *FileName();

    // Returns the line number
    VULCANDLL int LineStart();
protected:
    VSrcInfo();
    ~VSrcInfo();
};

class VSrcIter
{
public:
    inline VSrcIter();
    inline VSrcIter( VSrcIter &v);

    inline VSrcInfo        *Curr() const;
    inline bool             Done() const;
    inline bool             IsEmpty() const;
    inline size_t           Length() const;
    VULCANDLL void          MoveToFirst();
    VULCANDLL void          MoveToLast();
    VULCANDLL void          Next();
    VULCANDLL void          Prev();

protected:
    VSrcInfo *m_pSrcCur;
    int m_cSrcAfter;
    int m_cSrcBefore;
};

class VSect : public CDListElem_VSect
{
public:
    // Get the component this section is in
    virtual VComp *ParentComp() = 0;

    // Get the first/last/next/prev section in the componennt
    virtual VSect* Next() = 0;
    virtual VSect* Prev() = 0;
    virtual VSect* First() = 0;
    virtual VSect* Last() = 0;

    // Insert a section before/after this section
    virtual void InsertPrev(VSect*) = 0;
    virtual void InsertNext(VSect*) = 0;

    // Remove this section from the component
    virtual void Remove() = 0;

    // Get the first code procedure in this section
    // Don't forget to use (Prev/Next)SectProc to iterate
    virtual VProc* FirstProc() = 0;
    virtual VProc* LastProc() = 0;

    // Get the first procedure in this section (code or data)
    // Don't forget to use (Prev/Next)AllSectProc to iterate
    virtual VProc* FirstAllProc() = 0;
    virtual VProc* LastAllProc() = 0;

    // Add the first/last proc in this section
    virtual void InsertFirstProc(VProc *) = 0;
    virtual void InsertLastProc(VProc *) = 0;

    // How many code procs in this section
    virtual size_t CountProcs() = 0;

    // How many procs (code or data) in this section
    virtual size_t CountAllProcs() = 0;

    // Get the name of this section
    virtual const char *Name() = 0;


    // Get the address of this section
    virtual ADDR Addr() = 0;

    // Get the file offset to this section.
    virtual size_t FileOffset() = 0;

    // Get the raw size (on file) of this section.
    virtual size_t RawSize() = 0;

    // Get the initial size of this section
    virtual size_t Size() = 0;

    // Get a pointer to the raw bytes of the section
    virtual const void *Raw(VComp *pComp) = 0;

    // Create a new, empty procedure in this section
    virtual VProc *NewProc( const char *Name) = 0;
    virtual VProc *NewProc() = 0;

    // Is this section Executable
    virtual bool IsExecutable() = 0;

    // Is this section Inserted
    virtual bool IsInserted() = 0;

    // Is this section Read-Only
    virtual bool IsReadOnly() = 0;

    // Is this section Read-Write
    virtual bool IsReadWrite() = 0;

    // Is this section Regenerated?
    virtual bool IsRegenerated() = 0;

    // Is this section Resource
    virtual bool IsResource() = 0;

    // Get the flags of this section (SectionFlags)
    virtual DWORD Flags() = 0;

    // Delete this section
    virtual void Destroy() = 0;

    // Get the emitted size of this section (only available after VComp::Write())
    virtual size_t SizeEmit() = 0;

    // Get the emitted address of this section (only available after VComp::Write())
    virtual ADDR AddrEmit() = 0;

    // Set Callback function for the linker
    typedef void (VULCANCALL *PFNLINKCALLBACK)(VSect *, void *);
    virtual void SetCallBackForLink(PFNLINKCALLBACK pfn) = 0;

    typedef bool (VULCANCALL *PFNAACALLBACK)(VSect *);
    virtual void SetCallBackForAssignAddr( PFNAACALLBACK pfn) = 0;
};


template <class T>
class VKIter : public CKDListIter<T>
{
public:
    VKIter( const CKDListIter<T> &t ) : CKDListIter<T>( t ) {}
    VKIter()                          : CKDListIter<T>()    {}

    void          AddNext(T t)      {CKDListIter<T>::AddNext(t);}
    void          AddLast(T t)      {CKDListIter<T>::AddLast(t);}
    void          AddPrev(T t)      {CKDListIter<T>::AddPrev(t);}
    void          AddFirst(T t)     {CKDListIter<T>::AddFirst(t);}
    T             Curr() const      {return CKDListIter<T>::Curr();}
    T             Delete()          {return CKDListIter<T>::Delete();}
    bool          Done() const      {return CKDListIter<T>::Done();}
    bool          IsEmpty() const   {return CKDListIter<T>::IsEmpty();}
    bool          IsFirst() const   {return CKDListIter<T>::IsFirst();}
    bool          IsLast() const    {return CKDListIter<T>::IsLast();}
    size_t        Length() const    {return CKDListIter<T>::Length();}
    void          MoveToFirst()     {CKDListIter<T>::MoveToFirst();}
    void          MoveToLast()      {CKDListIter<T>::MoveToLast();}
    void          Next()            {CKDListIter<T>::Next();}
    void          Prev()            {CKDListIter<T>::Prev();}
};
typedef VKIter<VReloc *> VRelocIter;

class VReloc : CKDListElem<VReloc *>
{
public:
    // A "Reloc" is used to describe a pointer from a data block to another part of the executable.
    // The relocs allow the data in the block to be updated to the right number during linking and loading of the component.

    // The thing pointed at is desribed by a block and an offset
    VULCANDLL VBlock  *RelocTarget();
    VULCANDLL size_t   OffsetTarget();
    VULCANDLL VInst   *InstTarget();

    // The location of the pointer in the data block is also desribed by an offset.
    VULCANDLL size_t   OffsetReloc();

    // The pointers can different types
    enum Type
    {
        Absolute = 0,       // Absolute pointer (these are 32 or 64 bits depending on archicture)
        BaseRelative32 = 1, // Relative to the base of the image
        BaseRelative24 = 2, // Relative to the base of the image (max 16MB!)
        SectRelative8 = 3,  // Relative to the beginning of a section (1 byte long)
        SectRelative32 = 4, // Relative to the beginning of a section (full size)
        PcRelative32 = 5,   // Relative to the byte after the pointer
        UnknownType = 64,
    };
    VULCANDLL Type RelocType();

    // Move the reloc's target block
    VULCANDLL void SetRelocTarget(VBlock *pBlk);

    // For creating your own Data blocks with relocs in them (then add them to the reloc iter for the block)
    static VULCANDLL VReloc * VULCANCALL Create(VBlock *reloctarget, size_t offsettarget, size_t offsetreloc, Type type );
    static VULCANDLL VReloc * VULCANCALL Create(VInst *pInstTarget, size_t offsetreloc, Type type);

    //Destroy the reloc when you are done with it
    VULCANDLL void Destroy();
protected:
    VReloc() {}
    ~VReloc() {}
};

class VOperand
{
public:
    inline VOperand();
    inline VOperand(VOperand &v);

    inline VOperand( ERegister reg );
    inline VOperand( VAddress *pAddr );
    inline VOperand( long immed );
    inline VOperand( VBlock *pBlk );
    inline VOperand( VBlock *pBlk, long offset );
    inline VOperand( VInst *pInst );
    inline VOperand( BYTE *pb );

    // Returns the type of the operand - then use the methods before to retrieve specifics
    inline EOpndTypes Type();

    // Type() == OpndReg or OpndRegPair
    inline ERegister Register();

    // Type() == OpndAddress - indirect references to memory
    // constant because you shouldn't modify it (you can copy it to change it)
    inline const VAddress *Address();

    // Type() == OpndImmediate, OpndAddress, or OpndGenAddressBlock (otherwise returns 0)
    inline long Immediate();

    // Type() == OpndBlock, OpndAddress, or OpndGenAddressBlock (otherwise returns NULL)
    inline VBlock * Block();

    // Type() == OpndGenAddressBlock and !Block()->IsDataBlock()
    inline VInst * Inst();

    // Type() == Anything - give me it as a void
    inline void * Anything();

    // Modifying operands.. they can modify the type of operand, and will fail if it does not make sense
    inline bool SetBlock( VBlock * );
    inline bool SetImmediate( long );
    inline bool SetInst( VInst * );


private:
    static VULCANDLL VBlock * VULCANCALL BlockGA(void *operand);
    static VULCANDLL long VULCANCALL ImmediateGA(void *operand);
    static VULCANDLL VInst * VULCANCALL InstGA(void *operand);
    static VULCANDLL void *VULCANCALL CreateGA(VBlock *, long);
    static VULCANDLL void *VULCANCALL CreateGA(VInst *);
protected:
    void       *m_operand;
    EOpndTypes  m_type;
};

class VAddress
{
public:
    // An address can represent from just [ESP] to DS::[ESP + block + 4 + 4 * ECX]

    // Constuct an address like [ESP + 4]
    static VULCANDLL VAddress *VULCANCALL Create(ERegister reg, long disp);

    // Constuct an address like [pblock]
    static VULCANDLL VAddress *VULCANCALL Create(VBlock *pBlk, long disp = 0);

    // Constuct an address like [pinst]
    static VULCANDLL VAddress *VULCANCALL Create(VInst *pInst);

    // Empty construction (build with set functions below)
    static VULCANDLL VAddress *VULCANCALL Create();

    // Copy from const - if you want to change one
    static VULCANDLL VAddress *VULCANCALL Create(const VAddress *);

    // Base register (in [ESP + 4] ESP is the base register)
    VULCANDLL bool HasBaseReg() const;
    VULCANDLL ERegister BaseReg() const;
    VULCANDLL void SetBaseReg( ERegister reg );
    VULCANDLL void ClearBaseReg();

    // Offset (in [ESP + 4], the offset is 4)
    VULCANDLL long HasOffset() const;
    VULCANDLL long Offset() const;
    VULCANDLL void SetOffset(long offset);
    VULCANDLL void ClearOffset() {SetOffset(0);}

    // Inst (really block + offset of inst)
    VULCANDLL long HasInst() const;
    VULCANDLL VInst *Inst() const;
    VULCANDLL void SetInst(VInst *pInst);
    VULCANDLL void ClearInst();

    // Index register (in [ESP + 2 * EBX], EBX is the index register)
    VULCANDLL bool HasIndexReg() const;
    VULCANDLL ERegister IndexReg() const;
    VULCANDLL void SetIndexReg( ERegister reg );
    VULCANDLL void ClearIndexReg();

    // Scale (in [ESP + 2 * EBX], the scale is 2 (valid values are 1, 2, 4, 8)
    VULCANDLL int  Scale() const;
    VULCANDLL void SetScale(int scale);
    VULCANDLL void ClearScale() {SetScale(1);}

    // Block (in [pblock + 4], pblock is the block)
    VULCANDLL bool HasBlock() const;
    VULCANDLL VBlock *Block() const;
    VULCANDLL void SetBlock(VBlock *);
    VULCANDLL void ClearBlock() {SetBlock(NULL);}

    // Segment Override Register
    VULCANDLL bool HasSegReg() const;
    VULCANDLL ERegister SegReg() const;
    VULCANDLL void SetSegReg(ERegister reg);
    VULCANDLL void ClearSegReg();

    //Destroy the address when you are done with it
    VULCANDLL void Destroy();
};


typedef WORD ORD16;

class VExport : public CDListElem_VExport
{
public:
    // Get the next/prev export
    inline VExport *Prev();
    inline VExport *Next();
    inline VExport *First();
    inline VExport *Last();

    // Get the next/prev export of this same block
    inline VExport *NextProc();
    inline VExport *PrevProc();
    inline VExport *FirstProc();
    inline VExport *LastProc();

    // Get the ordinal that this export corresponds to
    VULCANDLL virtual ORD16 Ordinal() const = 0;

    // Set the ordinal
    VULCANDLL virtual void SetOrdinal(ORD16) = 0;

    // Get the name of this export
    VULCANDLL virtual const char *Name() const = 0;

    // Get the block that is exported (this will be NULL for exports forwarded to another component)
    VULCANDLL virtual VBlock *Block() const = 0;

    // Redirect this export to a new block. (Block must already be something or it fails!)
    VULCANDLL virtual bool Redirect(VBlock *, VComp *) = 0;

    // OBSOLETE: Use VExport::Redirect instead.
    VULCANDLL virtual bool SetBlock(VBlock *) = 0;

    // Get export module name.
    VULCANDLL virtual const char *ModuleName() const = 0;

    // Set export module name.
    // Note: This method changes the module name for all exports that shares the same module name.
    VULCANDLL virtual void SetModuleName(const char *) = 0;

    // Set name of this export.
    VULCANDLL virtual void SetName(const char *) = 0;
};


class VImport : public CDListElem_VImport
{
public:
    // Get next/prev/first/last import
    inline VImport *Next();
    inline VImport *Prev();
    inline VImport *First();
    inline VImport *Last();

    // Get the ordinal that this import corresponds to
    virtual DWORD Ordinal() = 0;

    // Get the name of this import
    virtual const char *Name() = 0;

    // Get the block calls to this import will call to
    virtual VBlock *Block() = 0;

    // Redirect the import to a new import
    virtual void Redirect(VImport *, VComp * = NULL) = 0;

    // Redirect the import to a local procedure
    virtual void Redirect(VProc *, VComp *) = 0;

    // (OBSOLETE): Use VImport::Redirect instead
    virtual void SetBlock(VBlock *, VComp * = NULL) = 0;

    // Get the name of the dll that is imported
    virtual const char *DllName() = 0;

    // is this a delay-load import
    virtual bool IsDelayLoad() = 0;

    // Has this import been redirected to another import?
    virtual bool IsRedirected() = 0;

    // Has this import been redirected to a proc.
    virtual bool IsRedirectedToProc() = 0;

    // Returns the proc that the import redirects to.
    virtual VProc *RedirectProc() = 0;
};

class VUserData
{
public:
    // Create a user-data object
    static VULCANDLL VUserData * VULCANCALL Create();

    // Free the user data resource associated with this object
    virtual void Destroy() = 0;

    // Remove all the user data associated with this object
    virtual void RemoveAll() = 0;

    // Get the data associated with key
    virtual void *GetUserData(void *pvKey) = 0;

    // Get the data associated with key
    virtual void SetUserData(void *pvData, void *pvKey) = 0;
};

class VDiff
{
public:
   enum EMatchType {DEFAULT};

   // Create a Diff
   static VULCANDLL VDiff* VULCANCALL Create(VComp* pComp1,VComp* pComp2,EMatchType type = DEFAULT );

   // Matches pBlk from either component to corrosponding VBBlock* in the other component.  return NULL if no match.
   virtual VBlock *Match(VBlock* pBlk) = 0;

   // Free up memory when done with diff.
   virtual void Destroy() = 0;

protected:
   VDiff() {}
   ~VDiff() {}
};


class VSymIter
{
public:
   VULCANDLL virtual const char   *SymName() = 0;              /* OBSOLETE */
   VULCANDLL virtual const char   *Name() = 0;                 /* OBSOLETE */
   VULCANDLL virtual bool         IsPublic() = 0;

   VULCANDLL virtual void         First() = 0;
   VULCANDLL virtual void         Next() = 0;
   VULCANDLL virtual void         Prev() = 0;
   VULCANDLL virtual bool         Done() = 0;
   VULCANDLL virtual void         Destroy() = 0;

   VULCANDLL virtual ADDR         Addr() = 0;      // may return NULL
   VULCANDLL virtual VBlock      *Block(DWORD *pib = NULL) = 0;     // may return NULL
   VULCANDLL virtual size_t       SymName(char *, size_t cb) = 0;
};

struct VAddrRange
{
   ADDR  addrBegin;
   ADDR  addrEnd;
};

class VSymbol : public CDListElem_VSymbol
{
public:

   // Get the name of the type
   virtual size_t Type(char *szBuf, size_t cchBuf, char **ppchName = NULL) = 0;

   // Get the symbol which this type corresponds to
   virtual const char *Name() = 0;

   // The register that this param/local is stored in (if it is)
   virtual bool IsInReg() = 0;
   virtual ERegister Register()= 0;

   // The offset of this param/local on the stack (the return address is at offset 0)
   virtual size_t Offset() = 0;

   // The size of the param/local
   virtual size_t Size() = 0;

   // Is this symbol implicit (not specified by the user)
   virtual bool IsImplicit() = 0;

   // Is this a pointer (and if it is, PTarget is the sybol under it (which could be another pointer))
   virtual bool IsPointer() = 0;
   virtual VSymbol *PTarget() = 0;

   // Is this type a primitive type (or enum)
   virtual bool IsPrimitive() = 0;

   // Is this primitive type signed?
   virtual bool IsSigned() = 0;

   // Bitfield accessors
   virtual bool IsBitField() = 0;
   virtual size_t BitOffset() = 0;
   virtual size_t BitSize() = 0;

   // Modifier accessors
   virtual bool IsConst() = 0;
   virtual bool IsVolatile() = 0;
   virtual bool IsUnaligned() = 0;

   // Get a pointer to the first field (non-primitive types)
   virtual VSymbol *FirstField() = 0;

   // Get a pointer to the first base type (non-primitive types)
   virtual VSymbol *FirstBase() = 0;

   // Get the next thing... this works for params, locals, and fields
   virtual VSymbol *Next() = 0;
};


class VAnnotation
{
public:
   // The address of the annotation.

   virtual ADDR Addr() const = 0;

   // Retrieve a string by zero based index to buffer given size.
   // Returns string length including terminator or zero for failure

   virtual size_t String(size_t, char *, size_t) const = 0;

   // The length of the string including terminator identified by zero based index.

   virtual size_t StringLength(size_t) const = 0;

   // The number of strings in the annotation.

   virtual size_t Count() const = 0;
};


class VAnnotationIter
{
public:
   // You need to "delete pIter" once you are finished with a
   // VAnnotationIter.
   //
   virtual                   ~VAnnotationIter() = 0;

   //
   // Get the current VAnnotation.  Returns NULL if there aren't any more
   // values within the specified range.
   //
   virtual const VAnnotation *Curr() = 0;

   //
   // Are you finished?  Note that you can use the iterator over again by
   // rebinding to the same thing you're bound to, binding to something
   // completely different, etc.
   //
   virtual bool               Done() const = 0;

   //
   // Go to the next VAnnotation that matches any Limit() currently in
   // effect.  Return value is next VAnnotation, or NULL if there aren't
   // any more.
   //
   virtual const VAnnotation *Next() = 0;
};
