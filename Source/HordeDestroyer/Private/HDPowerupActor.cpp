// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPowerupActor.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHDPowerupActor::AHDPowerupActor()
{


	PowerupCooldown = 0.0f;
	MaxTicks = 0.0f;
	bIsPowerupActive = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AHDPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDPowerupActor::onTickPowerup()
{
	TicksProcessed++;

	if (MaxTicks < TicksProcessed)
	{
		OnExpired();


		bIsPowerupActive = false;
		// this isn't called on the server
		// so have to manually call
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	OnPowerupTicked();

}

void AHDPowerupActor::OnRep_PowerupActive()
{
	OnPowerupActiveChanged(bIsPowerupActive);
}

/* This is only called on the server, per HDPickupActor */
void AHDPowerupActor::ActivatePowerup()
{
	OnActivated();

	bIsPowerupActive = true;
	// this isn't called on the server
	// so have to manually call
	OnRep_PowerupActive();

	if (PowerupCooldown > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AHDPowerupActor::onTickPowerup, PowerupCooldown, true, 0.0f);
	}
	else {
		onTickPowerup();
	}
}

void AHDPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHDPowerupActor, bIsPowerupActive);
}