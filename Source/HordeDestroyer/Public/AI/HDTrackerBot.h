// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "HDTrackerBot.generated.h"

class UHDHealthComponent;
class UMaterialInstanceDynamic;
class UParticleSystem;

UCLASS()
class HORDEDESTROYER_API AHDTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHDTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UStaticMeshComponent* MeshComp;

	FVector GetNextPathPoint();

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float RequiredDistanceToTarget;

	// Take damage
	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UHDHealthComponent* HealthComp;

	UFUNCTION()
	void OnTakeDamage(UHDHealthComponent* MyHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	// pawn died previously
	UPROPERTY(BlueprintReadOnly, Category = "Trackerbot")
	bool bExploded;

	// Dynamic material - pulsing on damage taken
	UMaterialInstanceDynamic* MatInst;

	// Explosion effect
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	UParticleSystem* ExplosionEffect;

	// Self Destruct mechanism
	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionRadius;
	
	UPROPERTY(EditDefaultsOnly, Category = "TrackerBot")
	float ExplosionDamage;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
