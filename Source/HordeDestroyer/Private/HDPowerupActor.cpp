// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPowerupActor.h"

// Sets default values
AHDPowerupActor::AHDPowerupActor()
{


	PowerupCooldown = 0.0f;
	MaxTicks = 0.0f;
}

// Called when the game starts or when spawned
void AHDPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDPowerupActor::onTickPowerup()
{
	TicksProcessed++;
	UE_LOG(LogTemp, Log, TEXT("TicksProcessed: %i - MaxTicks: %i"), TicksProcessed, MaxTicks);

	if (MaxTicks < TicksProcessed)
	{
		UE_LOG(LogTemp, Log, TEXT("We expired"));
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}

	OnPowerupTicked();

}

void AHDPowerupActor::ActivatePowerup()
{
	OnActivated();

	if (PowerupCooldown > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AHDPowerupActor::onTickPowerup, PowerupCooldown, true, 0.0f);
	}
	else {
		onTickPowerup();
	}
}

