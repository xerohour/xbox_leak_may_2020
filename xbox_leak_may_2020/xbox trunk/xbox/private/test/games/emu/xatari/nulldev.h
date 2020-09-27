#ifndef NULLDEVICE_HXX
#define NULLDEVICE_HXX

class System;

#include "bspf.h"
#include "Device.h"

/**
  Class that represents a "null" device.  The basic idea is that a
  null device is installed in a 6502 based system anywhere there are
  holes in the address space (i.e. no real device attached). 
 
  @author  Bradford W. Mott
  @version $Id: NullDev.hxx,v 1.2 1998/07/15 20:51:43 bwmott Exp $
*/
class NullDevice : public Device
{
  public:
    /**
      Create a new null device
    */
    NullDevice();

    /**
      Destructor
    */
    virtual ~NullDevice();

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
      Install device in the specified system.  Invoked by the system
      when the device is attached to it.

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
};
#endif
 
