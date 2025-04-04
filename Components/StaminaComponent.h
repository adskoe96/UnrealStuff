#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStaminaDepletedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStaminaChangedSignature, float, NewStamina);
UCLASS(ClassGroup=(StaminaSystem), meta=(BlueprintSpawnableComponent), Blueprintable)
class CPM_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintAssignable)
	FOnStaminaDepletedSignature OnStaminaDepleted;

	UPROPERTY(BlueprintAssignable)
	FOnStaminaChangedSignature OnStaminaChanged;

	UPROPERTY(ReplicatedUsing = OnRep_Stamina, VisibleAnywhere, Category = "CPM|GAME|CHARACTER|STAMINA")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "CPM|GAME|CHARACTER|STAMINA")
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "CPM|GAME|CHARACTER|STAMINA")
	float StaminaRegenRate = 20.0f;

	UPROPERTY(EditAnywhere, Category = "CPM|GAME|CHARACTER|STAMINA")
	float RegenDelayAfterConsumption = 2.0f;

	UPROPERTY(EditAnywhere, Category = "CPM|GAME|CHARACTER|STAMINA")
	float RegenDelayAfterDepletion = 5.0f;

	UPROPERTY(Replicated)
	bool bIsRegenerating;

	FTimerHandle StaminaRegenTimer;
	FTimerHandle StaminaRegenDelayTimer;
	FTimerHandle StaminaConsumptionTimer;

	UFUNCTION()
	void OnRep_Stamina();

public:
	UFUNCTION(BlueprintCallable, Category = "CPM|GAME|CHARACTER|STAMINA")
	void TryConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "CPM|GAME|CHARACTER|STAMINA")
	void StartConsumingStamina(float ConsumptionRate);

	UFUNCTION(BlueprintCallable, Category = "CPM|GAME|CHARACTER|STAMINA")
	void StopConsumingStamina();

	UFUNCTION(BlueprintPure, Category = "CPM|GAME|CHARACTER|STAMINA")
	float GetCurrentStamina() const { return Stamina; }

private:
	float CurrentConsumptionRate;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_ConsumeStamina(float Amount);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StartConsumingStamina(float ConsumptionRate);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_StopConsumingStamina();

	void StartRegeneration();
	void RegenerateStamina();
	void ConsumeStaminaOverTime();
};
