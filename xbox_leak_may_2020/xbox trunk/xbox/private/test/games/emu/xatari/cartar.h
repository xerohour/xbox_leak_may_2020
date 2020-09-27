#ifndef CARTRIDGEAR_HXX
#define CARTRIDGEAR_HXX

class CartridgeAR;
class M6502High;

#include "bspf.h"
#include "Cart.h"

/**
  This is the cartridge class for Arcadia (aka Starpath) Supercharger 
  games.  Christopher Salomon provided most of the details and code 
  used for this class.

  The Supercharger has four 2K banks.  There are three banks of RAM 
  and one bank of ROM.  All 6K of the RAM can be read and written.

  @author  Bradford W. Mott
  @version $Id: CartAR.hxx,v 1.2 1998/07/15 20:29:01 bwmott Exp $
*/
class CartridgeAR : public Cartridge
{
  public:
    /**
      Create a new cartridge using the specified image and size

      @param image Pointer to the ROM image
      @param size The size of the ROM image
    */
    CartridgeAR(const uInt8* image, uInt32 size);

    /**
      Destructor
    */
    virtual ~CartridgeAR();

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
      Get the byte at the specified address

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
    // Handle a change to the bank configuration
    void bankConfiguration(uInt8 configuration);

    // Handle setting the current image from the load images
    void setupCurrentImage(uInt8 load);

    // Sets up a "dummy" bootstrap ROM in the ROM bank of the cartridge
    void setupROM();

  private:
    // Pointer to the 6502 processor in the system
    M6502High* my6502;

    // Indicates the offest within the image for the corresponding bank
    uInt32 myImageOffset[2];

    // Current 8k ROM Image of the cartridge
    uInt8 myImage[8192];

    // 256 byte header of the cartridge
    uInt8 myHeader[256];

    // All of the load images of the cartridge (needed for multiloads)
    uInt8* myLoadImages;

    // Indicates how many 8K loads images there are
    uInt8 myNumberOfLoadImages;

    // Indicates if the RAM is write enabled
    bool myWriteEnabled;

    // Indicates if the ROM's power is on or off
    bool myPower;

    // Indicates when the power was last turned on
    Int32 myPowerRomCycle;

    // Indicates the "value" address which was accessed
    uInt16 myLastAccess;

    // Indicates the number of distinct access when the "value" address was set
    uInt32 myNumberOfDistinctAccesses;

    // Indicates if a write is pending or not
    bool myWritePending;
};
#endif

