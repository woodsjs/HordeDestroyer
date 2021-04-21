// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "HDGameState.generated.h"

UENUM(BlueprintType)
enum class EWaveState : uint8
{
	WaitingToStart,

	WaveInProgress,

	// no longer spawning new bots, waiting for player to finishing killing bots on screen
	WaitingToComplete,

	WaveComplete,

	GameOver,
};

/**
 * 
 */
UCLASS()
class HORDEDESTROYER_API AHDGameState : public AGameStateBase
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnRep_WaveState(EWaveState OldState);

	UFUNCTION(BlueprintImplementableEvent, Category="GameState")
	void WaveStateChanged(EWaveState NewState, EWaveState OldState);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_WaveState, Category="GameState")
	EWaveState WaveState;
	
};
