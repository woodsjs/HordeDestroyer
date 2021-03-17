// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDExplosiveActor.generated.h"

class UStaticMeshComponent;
class UMaterial;
class UParticleSystem;
class UHDHealthComponent;

UCLASS()
class HORDEDESTROYER_API AHDExplosiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHDExplosiveActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
	UMaterial* ExplodedMaterial;

	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
	UParticleSystem* ExplosionEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ActorHealth")
	UHDHealthComponent* ActorHealth;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
