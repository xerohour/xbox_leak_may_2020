/***********************************************************************
* Microsoft Vulcan
*
* Microsoft Confidential.  Copyright 1994-2000 Microsoft Corporation.
*
* Component:
*
* File: vulcanbmp.h
*
* File Comments:
*
*
***********************************************************************/

#ifndef __CBmp__
#define __CBmp__

class VULCANDLL CBmp
{
public:
	    CBmp();
        CBmp(size_t, bool = false);
	    ~CBmp();

   // Allocate() initializes the bitmap with the given size.  Bit indices range
   // from 0 to this limit minus one.  If the bitmap can't be initialized,
   // this method throws an exception.	The initial value of each bit is specified
   // by the second parameter.

	    void Allocate(size_t, bool = false);

   // Attach() binds this CBmp to a previously allocated block of memory.

	    void Attach(void *, size_t);

   // CbFromCbit() returns the number of bytes needed to an attached bitmap of a
   // given size.

   static   size_t VULCANCALL CbFromCbit(size_t);

   // CbitSet() returns the number of bits set in the bitmap.

	    size_t CbitSet() const;

   // UNDONE: Comment FAttached()

	    bool FAttached() const { return(m_fAttached); };

   // FTest() returns true if the specified bit is set and false otherwise.
   // It is invalid to call this methods with a bit index beyond the limit
   // given to FInit().

	    bool FTest(size_t) const;

   // FTestAndReset() returns true if the specified bit is set and false
   // otherwise.  The bit is then unconditionally reset.  It is invalid to call
   // this method with a bit index beyond the limit given to FInit().

	    bool FTestAndReset(size_t);

   // FTestAndSet() returns true if the specified bit is set and false
   // otherwise.  The bit is then unconditionally set.	It is invalid to call
   // this method with a bit index beyond the limit given to FInit().

	    bool FTestAndSet(size_t);

   // IbitNextSet() returns the bit number of the first bit set following
   // the specified bit.  If there are no more bits set, the method returns
   // ibitNil.

	    size_t IbitNextSet(size_t) const;

   // IbitNextSet() returns the bit number of the first bit set following
   // the specified bit up to the specified upper bound.  If there are no
   // more bits set, the method returns ibitNil.

	    size_t IbitNextSet(size_t, size_t) const;

   // IbitNextSet() returns the bit number of the first bit set in both
   // bitmaps following the specified bit.  If there are no more bits set,
   // the method returns ibitNil.  It is invalid to call this function
   // unless both bitmaps are initialized to the same size.

   static   size_t VULCANCALL IbitNextSet(size_t, const CBmp&, const CBmp&);

   // IbitPrevSet() returns the bit number of the first bit set prior to
   // the specified bit.  If there are no more bits set, the method returns
   // ibitNil.

	    size_t IbitPrevSet(size_t) const;

   // ReInitialize() the entire bitmap.  After this method is done, the bitmap
   // is reinitialized to its initial state, as in after a call to Allocate().

	    void ReInitialize();

   // Release() releases the memory allocated in Allocate() for the bitmap.  If the
   // bitmap was attached via Attach(), the connection is released.

	    void Release();

   // Reset() clears the specified bit in the bitmap.  It is invalid to
   // call this method with a bit index beyond the limit given to FInit().

	    void Reset(size_t);

   // Set() sets the specified bit in the bitmap.  It is invalid to
   // call this method with a bit index beyond the limit given to FInit().

	    void Set(size_t);


  // Used for dataflow
            size_t CntBits()  {return m_cbit;}
            void   SetAll(bool);
            void   And(CBmp*);
            void   And(CBmp*, CBmp*);
            void   Or(CBmp*);
            void   Or(CBmp*, CBmp*);
            void   Xor(CBmp*);
            void   Xor(CBmp*, CBmp*);
            void   Minus(CBmp*);
            void   Minus(CBmp*, CBmp*);
            void   Copy(CBmp*);
            bool   Equal(CBmp*);
            void   PrintBmp();

private:
	    size_t m_cbit;
	    size_t m_cdw;
	    DWORD  m_dwInit;
	    bool m_fAttached;
	    DWORD* m_rgdw;

   static   size_t IdwFromIbit(size_t ibit) { return(ibit >> 5); };
   static   DWORD DwMaskFromIbit(size_t ibit) { return(1UL << (ibit & 0x1f)); };
};


const size_t ibitNil = (size_t) -1;

#endif
