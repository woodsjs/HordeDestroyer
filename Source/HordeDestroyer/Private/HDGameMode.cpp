// Fill out your copyright notice in the Description page of Project Settings.


#include "HDGameMode.h"
#include "Components/HDHealthComponent.h"

AHDGameMode::AHDGameMode()
{
	TimeBetweenWaves = 2.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}


void AHDGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}

void AHDGameMode::StartWave()
{
	WaveCount++;

	NrOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AHDGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AHDGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);

	//PrepareForNextWave();
}

void AHDGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AHDGameMode::StartWave, TimeBetweenWaves , false, 0.0f);
}

void AHDGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AHDGameMode::CheckWaveState()
{

	// we don't want to start the next wave if there is an active wave!
	// or if we are already preparing for a new wave!
	// This handles the state where there are no bots on the field to be picked up by the
	// pawn iterator, but our timer is active and there are bots to spawn
	bool bIsPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NrOfBotsToSpawn > 0 || bIsPreparingForWave)
	{
		return;
	}

	bool bIsAnyBotAlive = false;

	// test each pawn. If it's a bot, it has health, then we don't want to start
	// a next wave.
	// keeps a list of pawns in teh level
	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();

		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UHDHealthComponent* HealthComponent = Cast<UHDHealthComponent>(TestPawn->GetComponentByClass(UHDHealthComponent::StaticClass()));
		if (HealthComponent && HealthComponent->GetHealth() > 0.0f)
		{
			bIsAnyBotAlive = true;
			break;
		}
	}

	if (!bIsAnyBotAlive)
	{
		PrepareForNextWave();
	}
}

void AHDGameMode::SpawnBotTimerElapsed()
{
	SpawnNewBot();

	NrOfBotsToSpawn--;

	if (NrOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

