// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class AHDPowerupActor;

UCLASS()
class HORDEDESTROYER_API AHDPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHDPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	TSubclassOf<AHDPowerupActor> PowerupClass;

	UPROPERTY(EditDefaultsOnly, Category = "PickupActor")
	float CoolDownDuration;

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void Respawn();

	AHDPowerupActor* PowerupInstance;

public:	
	// overlap override
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
