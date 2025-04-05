#include "StaminaComponent.h"
#include "Net/UnrealNetwork.h"

UStaminaComponent::UStaminaComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();
	Stamina = MaxStamina;
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UStaminaComponent, Stamina);
	DOREPLIFETIME(UStaminaComponent, bIsRegenerating);
}


void UStaminaComponent::OnRep_Stamina()
{
	OnStaminaChanged.Broadcast(Stamina);
}

void UStaminaComponent::TryConsumeStamina(float Amount)
{
	if(GetOwnerRole() < ROLE_Authority)
	{
		Server_ConsumeStamina(Amount);
	}
	else 
	{
		Server_ConsumeStamina_Implementation(Amount);
	}
}

void UStaminaComponent::StartConsumingStamina(float ConsumptionRate)
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_StartConsumingStamina(ConsumptionRate);
	}
	else
	{
		Server_StartConsumingStamina_Implementation(ConsumptionRate);
	}
}

void UStaminaComponent::StopConsumingStamina()
{
	if (GetOwnerRole() < ROLE_Authority)
	{
		Server_StopConsumingStamina();
	}
	else
	{
		Server_StopConsumingStamina_Implementation();
	}
}

void UStaminaComponent::Server_StartConsumingStamina_Implementation(float ConsumptionRate)
{
	CurrentConsumptionRate = ConsumptionRate;
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
	GetWorld()->GetTimerManager().ClearTimer(StaminaRegenDelayTimer);
	bIsRegenerating = false;
	GetWorld()->GetTimerManager().SetTimer(
		StaminaConsumptionTimer,
		this,
		&UStaminaComponent::ConsumeStaminaOverTime,
		0.1f,
		true
	);
}

bool UStaminaComponent::Server_StartConsumingStamina_Validate(float ConsumptionRate)
{
	return ConsumptionRate > 0;
}

void UStaminaComponent::Server_StopConsumingStamina_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(StaminaConsumptionTimer);
	float Delay = (Stamina > 0) ? RegenDelayAfterConsumption : RegenDelayAfterDepletion;
	GetWorld()->GetTimerManager().SetTimer(
		StaminaRegenDelayTimer,
		this,
		&UStaminaComponent::StartRegeneration,
		Delay,
		false
	);
}

bool UStaminaComponent::Server_StopConsumingStamina_Validate()
{
	return true;
}

void UStaminaComponent::StartRegeneration()
{
	if(!bIsRegenerating)
	{
		bIsRegenerating = true;
		GetWorld()->GetTimerManager().SetTimer(
			StaminaRegenTimer,
			this,
			&UStaminaComponent::RegenerateStamina,
			1.0f,
			true
		);
	}
}

void UStaminaComponent::RegenerateStamina()
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate, 0.0f, MaxStamina);
	OnStaminaChanged.Broadcast(Stamina);

	if (Stamina >= MaxStamina)
	{
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
		bIsRegenerating = false;
	}
}

void UStaminaComponent::ConsumeStaminaOverTime()
{
	if (Stamina > 0)
	{
		float DeltaTime = 0.1f;
		float Consumption = CurrentConsumptionRate * DeltaTime;
		Stamina = FMath::Clamp(Stamina - Consumption, 0.0f, MaxStamina);
		OnStaminaChanged.Broadcast(Stamina);

		if (Stamina <= 0)
		{
			OnStaminaDepleted.Broadcast();
		}
	}
}

void UStaminaComponent::Server_ConsumeStamina_Implementation(float Amount)
{
	if(Stamina >= Amount)
	{
		Stamina = FMath::Clamp(Stamina - Amount, 0.0f, MaxStamina);
        
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenTimer);
		GetWorld()->GetTimerManager().ClearTimer(StaminaRegenDelayTimer);
		bIsRegenerating = false;
        
		if(Stamina <= 0)
		{
			GetWorld()->GetTimerManager().SetTimer(
				StaminaRegenDelayTimer,
				this,
				&UStaminaComponent::StartRegeneration,
				5.0f,
				false
			);
		}

		OnStaminaChanged.Broadcast(Stamina);
	}
}

bool UStaminaComponent::Server_ConsumeStamina_Validate(float Amount)
{
	return Amount > 0 && Amount <= MaxStamina;
}
