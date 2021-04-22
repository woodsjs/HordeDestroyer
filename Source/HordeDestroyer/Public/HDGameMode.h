// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HDGameMode.generated.h"

enum class EWaveState : uint8;

// Who was killed, who was the killer. This is going to be used to set a score
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnActorKilled, AActor*, VictimActor, AActor*, KillerActor, AController*, KillerController);

/**
 * 
 */
UCLASS()
class HORDEDESTROYER_API AHDGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FTimerHandle TimerHandle_BotSpawner;
	FTimerHandle TimerHandle_NextWaveStart;

	int32 NrOfBotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category="GameMode")
	float TimeBetweenWaves;

protected:

	virtual void StartPlay() override;

	void SetWaveState(EWaveState NewWaveState);

	void StartWave();

	void SpawnBotTimerElapsed();

	// hook for BP to spawn single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void EndWave();

	// Set timer for next bot wave
	void PrepareForNextWave();

	void CheckWaveState();

	void CheckAnyPlayerAlive();

	void GameOver();

public:
	AHDGameMode();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(BlueprintAssignable, Category="GameMode")
	FOnActorKilled OnActorKilled;
};
