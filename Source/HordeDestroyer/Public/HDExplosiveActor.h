// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDExplosiveActor.generated.h"

class UStaticMeshComponent;
class UMaterial;
class UParticleSystem;
class UHDHealthComponent;
class USphereComponent;
class UPrimitiveComponent;
class UDamageType;

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

	// add health component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHDHealthComponent* MyHealthComp;

	UFUNCTION()
	void OnHealthChanged(UHDHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_bExploded();

	// pawn died previously
	UPROPERTY(ReplicatedUsing= OnRep_bExploded, BlueprintReadOnly, Category = "Explosive")
	bool bExploded;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// our force
	UPROPERTY(EditDefaultsOnly, Category = "Explosive")
	float BaseForceStrength;

	//UPROPERTY(EditDefaultsOnly, Category = "Explosive")
	//float BaseForceRadius;

	// our damage
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Explosive")
	USphereComponent* DamageSphere;

	TArray<UPrimitiveComponent*> OverlappedActors;
	TArray<FOverlapResult> OverlappingActors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive")
	TSubclassOf<UDamageType> AppliedDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive")
	float BaseDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive")
	float MinAppliedDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explosive")
	float DamageFalloff;

	void PlayExplosionEffects();

	void ApplyExplosionDamage();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
