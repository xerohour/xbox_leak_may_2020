#include "stdafx.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Switches::Switches(const Event& event, const Properties& properties)
    : myEvent(event),
      mySwitches(0xFF)
{
  if(properties.get("Console.RightDifficulty") == "B")
  {
    mySwitches &= ~0x80;
  }
  else
  {
    mySwitches |= 0x80;
  }

  if(properties.get("Console.LeftDifficulty") == "B")
  {
    mySwitches &= ~0x40;
  }
  else
  {
    mySwitches |= 0x40;
  }

  if(properties.get("Console.TelevisionType") == "Color")
  {
    mySwitches |= 0x08;
  }
  else
  {
    mySwitches &= ~0x08;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
Switches::~Switches()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
uInt8 Switches::read()
{
  if(myEvent.get(Event::ConsoleColor) != 0)
  {
    mySwitches |= 0x08;
  }
  else if(myEvent.get(Event::ConsoleBlackWhite) != 0)
  {
    mySwitches &= ~0x08;
  }

  if(myEvent.get(Event::ConsoleRightDifficultyA) != 0)
  {
    mySwitches &= ~0x80;
  }
  else if(myEvent.get(Event::ConsoleRightDifficultyB) != 0) 
  {
    mySwitches |= 0x80;
  }

  if(myEvent.get(Event::ConsoleLeftDifficultyA) != 0)
  {
    mySwitches &= ~0x40;
  }
  else if(myEvent.get(Event::ConsoleLeftDifficultyB) != 0)
  {
    mySwitches |= 0x40;
  }

  if(myEvent.get(Event::ConsoleSelect) != 0)
  {
    mySwitches &= ~0x02;
  }
  else 
  {
    mySwitches |= 0x02;
  }

  if(myEvent.get(Event::ConsoleReset) != 0)
  {
    mySwitches &= ~0x01;
  }
  else 
  {
    mySwitches |= 0x01;
  }

  return mySwitches;
}

