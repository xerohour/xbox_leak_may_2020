
#include <math.h>
#include "VoxRound.h"

#include "xvocver.h"

long VoxPosRound(float x)
{
  return (long)(floor(x+0.5F));
}
