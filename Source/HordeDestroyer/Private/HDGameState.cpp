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

/*Changing a property that is replicated via OnRep Notify, only triggers that notify
  on the clients. So we need to handle this on the server too! 
  
  This method manually triggers OnRep_WaveState for the server only*/
void AHDGameState::SetWaveState(EWaveState NewWaveState)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		EWaveState OldState = WaveState;
		WaveState = NewWaveState;


		OnRep_WaveState(OldState);
	}
}
