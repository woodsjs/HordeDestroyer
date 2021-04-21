// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameState.h"
#include "Net/UnrealNetwork.h"

void AHDGameState::OnRep_WaveState(EWaveState OldState)
{
	WaveStateChanged(WaveState, OldState);
}

void AHDGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHDGameState, WaveState);
}