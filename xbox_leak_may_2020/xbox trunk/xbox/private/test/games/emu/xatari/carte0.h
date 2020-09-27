#ifndef CARTRIDGEE0_HXX
#define CARTRIDGEE0_HXX

class CartridgeF8;

#include "bspf.h"
#include "Cart.h"

/**
  This is the cartridge class for Parker Brothers' 8K games.  In 
  this bankswitching scheme the 2600's 4K cartridge address space 
  is broken into four 1K segments.  The desired 1K slice of the
  ROM is selected by accessing 1FE0 to 1FE7 for the first 1K.
  1FE8 to 1FEF selects the slice for the second 1K, and 1FF0 to 
  1FF8 selects the slice for the third 1K.   The last 1K segment 
  always points to the last 1K of the ROM image.
  
  @author  Bradford W. Mott
  @version $Id: CartE0.hxx,v 1.2 1998/07/15 20:51:01 bwmott Exp $
*/
class CartridgeE0 : public Cartridge
{
  public:
    /**
      Create a new cartridge using the specified image

      @param image Pointer to the ROM image
    */
    CartridgeE0(const uInt8* image);
 
    /**
      Destructor
    */
    virtual ~CartridgeE0();

  public:
    /**
      Get a null terminated string which is the device's name (i.e. "M6532")

      @return The name of the device
    */
    virtual const char* name() const;

    /**
      Reset device to its power-on state
    */
    virtual void reset();

    /**
      Install cartridge in the specified system.  Invoked by the system
      when the cartridge is attached to it.

      @param system The system the device should install itself in
    */
    virtual void install(System& system);

  public:
    /**
      Get the byte at the specified address.

      @return The byte at the specified address
    */
    virtual uInt8 peek(uInt16 address);

    /**
      Change the byte at the specified address to the given value

      @param address The address where the value should be stored
      @param value The value to be stored at the address
    */
    virtual void poke(uInt16 address, uInt8 value);

  private:
    /**
      Install the specified slice for segment zero

      @param slice The slice to map into the segment
    */
    void segmentZero(uInt16 slice);

    /**
      Install the specified slice for segment one

      @param slice The slice to map into the segment
    */
    void segmentOne(uInt16 slice);

    /**
      Install the specified slice for segment two

      @param slice The slice to map into the segment
    */
    void segmentTwo(uInt16 slice);

  private:
    // Indicates the slice mapped into each of the four segments
    uInt16 myCurrentSlice[4];

    // The 8K ROM image of the cartridge
    uInt8 myImage[8192];
};
#endif

