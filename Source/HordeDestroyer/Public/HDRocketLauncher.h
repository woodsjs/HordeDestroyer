// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HDWeapon.h"
#include "HDRocketLauncher.generated.h"

class AHDProjectileGrenade;

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

		/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
		TSubclassOf<AHDProjectileGrenade> ProjectileClass;

protected:

	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	virtual void Fire() override;

};
