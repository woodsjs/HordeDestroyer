// Fill out your copyright notice in the Description page of Project Settings.


#include "HDRocketLauncher.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "HDProjectileGrenade.h"

// Sets default values
AHDRocketLauncher::AHDRocketLauncher()
{
}

void AHDRocketLauncher::fire()
{
	// Trace the world from pawn eyes to crosshair location
	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		FVector EyeLocation;
		FRotator EyeRotation;

		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector ShotDirection = EyeRotation.Vector();

		// We use 10k as the multiplier because that's kind of standard. 
		// This allows the trace to go "far enough" to hit something.
		FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(MyOwner);
		QueryParams.AddIgnoredActor(this);
		// gives exact collision of where we hit
		// if we want to set up nice effects, we need complex (like head vs foot shot)
		QueryParams.bTraceComplex = true;

		// Particle "Target" Parameter value. 
		// If we hit nothing, we just go to the trace end
		// otherwise we go to the Hit.ImpactPoint
		FVector TracerEndPoint = TraceEnd;

	// try and fire a projectile
		if (ProjectileClass)
		{

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
			ActorSpawnParams.Instigator = Cast<APawn>(MyOwner);

			// spawn the projectile at the muzzle
			AHDProjectileGrenade* grenade = GetWorld()->SpawnActor<AHDProjectileGrenade>(ProjectileClass, EyeLocation, EyeRotation, ActorSpawnParams);
			grenade->Launched();
		}

		PlayFireEffects(TracerEndPoint);
	}
}
