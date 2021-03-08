// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HDProjectileGrenade.generated.h"

//class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystem;
class UStaticMeshComponent;
class UDamageType;

UCLASS()
class HORDEDESTROYER_API AHDProjectileGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHDProjectileGrenade();


	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void Launched();

protected:

	UFUNCTION()
	void OnDetonate();

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// we only want this if we explode on collision, or make sticky grenades
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	//	USphereComponent* CollisionComp;

	// The particle affect for explosion
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* DetonateEffect;

	//UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	FTimerHandle TimerHandle_DetonateProjectile;

	// used to set the length of time before the projectile detonates
	UPROPERTY(EditInstanceOnly, category = "Projectile")
	float DetonateTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
