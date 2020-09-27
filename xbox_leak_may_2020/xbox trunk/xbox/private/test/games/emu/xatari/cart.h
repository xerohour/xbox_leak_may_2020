#ifndef CARTRIDGE_HXX
#define CARTRIDGE_HXX

class Cartridge;
class Properties;
class System;

#include "bspf.h"
#include "Device.h"

/**
  A cartridge is a device which contains the machine code for a 
  game and handles any bankswitching performed by the cartridge.
 
  @author  Bradford W. Mott
  @version $Id: Cart.hxx,v 1.2 1998/07/15 20:24:05 bwmott Exp $
*/
class Cartridge : public Device
{
  public:
    /**
      Create a new cartridge object allocated on the heap.  The
      type of cartridge created depends on the properties object.

      @param image A pointer to the ROM image
      @param size The size of the ROM image 
      @param properties The properties associated with the game
      @return Pointer to the new cartridge object allocated on the heap
    */
    static Cartridge* create(const uInt8* image, uInt32 size, 
        const Properties& properties);

  public:
    /**
      Create a new cartridge
    */
    Cartridge();
 
    /**
      Destructor
    */
    virtual ~Cartridge();

  private:
    /**
      Try to auto-detect the bankswitching type of the cartridge

      @param image A pointer to the ROM image
      @param size The size of the ROM image 
      @return The "best guess" for the cartridge type
    */
    static string autodetectType(const uInt8* image, uInt32 size);

  private:
    // Copy constructor isn't supported by cartridges so make it private
    Cartridge(const Cartridge&);

    // Assignment operator isn't supported by cartridges so make it private
    Cartridge& operator = (const Cartridge&);
};
#endif

