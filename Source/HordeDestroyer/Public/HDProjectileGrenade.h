// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDProjectileGrenade.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystem;

UCLASS()
class HORDEDESTROYER_API AHDProjectileGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHDProjectileGrenade();

	UFUNCTION()
	void OnDetonate();

	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void Launched();

protected:

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// The particle affect for explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* DetonateEffect;

	//UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UMeshComponent* MeshComp;

	void DetonateProjectile();

	FTimerHandle TimerHandle_DetonateProjectile;

	// used to set the length of time before the projectile detonates
	UPROPERTY(EditInstanceOnly, category = "Projectile")
	float DetonateTime = 1.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
