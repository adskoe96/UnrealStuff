#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AYourPlayerState.generated.h"

UCLASS()
class YourGameAPI AYourPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	AYourPlayerState();

    // Used for Unreal Engine 4.27.2 (because this version has not "GetPlayerController" method in PlayerState class)
    // Your player controller class
	UFUNCTION(BlueprintPure, Category = "GAME|PLAYERSTATE|UTILS")
	AYourPlayerController* GetPlayerController() const;
};
