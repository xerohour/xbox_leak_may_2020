/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ani.h

Abstract:

    Startup animation control methods

--*/

#pragma once

// Starts the animation which will run on a background thread.  This API
// returns immediately.
//
void AniStartAnimation(BOOLEAN fShort);

// Shut down the animation.  This will abort any pending fades if the time
// passed to SetTerminationTime was incorrect.  This API may block briefly
// as the background thread is terminated.
//
void AniTerminateAnimation();

// Blocks until the animation has completed (until the animation is ready
// to display the Microsoft logo).
//
void AniBlockOnAnimation();

// Sets the pointer to compressed Microsoft logo.
//
VOID AniSetLogo(PVOID pv, ULONG Size);

