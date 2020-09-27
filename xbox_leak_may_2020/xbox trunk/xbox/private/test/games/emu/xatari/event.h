#ifndef EVENT_HXX
#define EVENT_HXX

class Event;

#include "bspf.h"

/**
  @author  Bradford W. Mott
  @version $Id: Event.hxx,v 1.2 1998/07/15 20:51:11 bwmott Exp $
*/
class Event
{
  public:
    /**
      Enumeration of console and controller event types
    */
    enum Type
    {
      ConsoleOn, ConsoleOff, ConsoleColor, ConsoleBlackWhite,
      ConsoleLeftDifficultyA, ConsoleLeftDifficultyB,
      ConsoleRightDifficultyA, ConsoleRightDifficultyB,
      ConsoleSelect, ConsoleReset,

      JoystickZeroUp, JoystickZeroDown, JoystickZeroLeft,
      JoystickZeroRight, JoystickZeroFire,
      JoystickOneUp, JoystickOneDown, JoystickOneLeft,
      JoystickOneRight, JoystickOneFire,

      BoosterGripZeroTrigger, BoosterGripZeroBooster,
      BoosterGripOneTrigger, BoosterGripOneBooster,

      PaddleZeroResistance, PaddleZeroFire,
      PaddleOneResistance, PaddleOneFire,
      PaddleTwoResistance, PaddleTwoFire,
      PaddleThreeResistance, PaddleThreeFire,

      KeyboardZero1, KeyboardZero2, KeyboardZero3,
      KeyboardZero4, KeyboardZero5, KeyboardZero6,
      KeyboardZero7, KeyboardZero8, KeyboardZero9,
      KeyboardZeroStar, KeyboardZero0, KeyboardZeroPound,

      KeyboardOne1, KeyboardOne2, KeyboardOne3,
      KeyboardOne4, KeyboardOne5, KeyboardOne6,
      KeyboardOne7, KeyboardOne8, KeyboardOne9,
      KeyboardOneStar, KeyboardOne0, KeyboardOnePound,

      DrivingZeroClockwise, DrivingZeroCounterClockwise, DrivingZeroFire,
      DrivingOneClockwise, DrivingOneCounterClockwise, DrivingOneFire,

      LastType
    };

  public:
    /**
      Create a new event object
    */
    Event();
 
    /**
      Destructor
    */
    virtual ~Event();

  public:
    /**
      Get the value associated with the event of the specified type
    */
    virtual Int32 get(Type type) const;

    /**
      Set the value associated with the event of the specified type
    */
    virtual void set(Type type, Int32 value);

  protected:
    // Number of event types there are
    const Int32 myNumberOfTypes;

    // Array of values associated with each event type
    Int32 myValues[LastType];
};
#endif

