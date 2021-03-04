// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDWeapon.h"
#include "HDRocketLauncher.generated.h"

class AHDProjectileGrenade;
class UMeshComponent;
//class UProjectileMovementComponent;
//class USphereComponent;

/**
 * 
 */
UCLASS()
class HORDEDESTROYER_API AHDRocketLauncher : public AHDWeapon
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AHDRocketLauncher();

	///** called when projectile hits something */
	//UFUNCTION()
	//	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	///** Returns CollisionComp subobject **/
	//USphereComponent* GetCollisionComp() const { return CollisionComp; }

	///** Returns ProjectileMovement subobject **/
	//UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

		/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AHDProjectileGrenade> ProjectileClass;

protected:

	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void fire() override;


	///** Sphere collision component */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	//	USphereComponent* CollisionComp;

	///** Projectile movement component */
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	//	UProjectileMovementComponent* ProjectileMovement;
};
