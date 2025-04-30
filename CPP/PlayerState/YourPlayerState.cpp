#include "AYourPlayerState.h"

AYourPlayerState::AYourPlayerState()
{
}

AYourPlayerController* AYourPlayerState::GetPlayerController() const
{
	return Cast<AYourPlayerController>(GetOwner());
}
