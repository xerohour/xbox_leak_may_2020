#ifndef __CAR_WHEEL_NUMBERS
#define __CAR_WHEEL_NUMBERS

//      X
//      ^
//      |
//     0 1
// Y<--
//     2 3

// note that this is partially handy because it means
// that as we iterate through the wheels, we render the
// two instances of each wheel in order.
enum
{
	CAR_WHEEL_LF = 0,
	CAR_WHEEL_RF,
	CAR_WHEEL_LR,
	CAR_WHEEL_RR,

	CAR_WHEELS
};

#define CAR_WHEEL_MASK_RIGHT 1
#define CAR_WHEEL_MASK_REAR  2

#define CAR_WHEEL_FRONT(w) (!((w) & CAR_WHEEL_MASK_REAR))
#define CAR_WHEEL_BACK(w )   ((w) & CAR_WHEEL_MASK_REAR)
#define CAR_WHEEL_RIGHT(w)   ((w) & CAR_WHEEL_MASK_RIGHT)
#define CAR_WHEEL_LEFT(w)  (!((w) & CAR_WHEEL_MASK_RIGHT))

#endif
