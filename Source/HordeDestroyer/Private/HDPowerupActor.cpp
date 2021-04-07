// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPowerupActor.h"

// Sets default values
AHDPowerupActor::AHDPowerupActor()
{


	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0.0f;
}

// Called when the game starts or when spawned
void AHDPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDPowerupActor::onTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TotalNumberOfTicks <= TicksProcessed)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void AHDPowerupActor::ActivatePowerup()
{
	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AHDPowerupActor::onTickPowerup, PowerupInterval, true, 0.0f);
	}
	else {
		onTickPowerup();
	}
}

