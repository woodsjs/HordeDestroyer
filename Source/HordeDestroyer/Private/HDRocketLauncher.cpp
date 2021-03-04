// Fill out your copyright notice in the Description page of Project Settings.


#include "HDRocketLauncher.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
//#include "GameFramework/ProjectileMovementComponent.h"
//#include "Components/SphereComponent.h"

// Sets default values
AHDRocketLauncher::AHDRocketLauncher()
{
}

//void AHDRocketLauncher::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
//{
//
//}

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

		// ***********************
	// try and fire a projectile
		//if (ProjectileClass)
		//{
		//	// Grabs location from the mesh that must have a socket called "Muzzle" in his skeleton
		//	FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		//	// Use controller rotation which is our view direction in first person
		//	FRotator MuzzleRotation = GetControlRotation();

		//	//Set Spawn Collision Handling Override
		//	FActorSpawnParameters ActorSpawnParams;
		//	ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		//	ActorSpawnParams.Instigator = this;

		//	// spawn the projectile at the muzzle
		//	GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);
		//}
		// **************************

		if (MuzzleEffect)
		{
			UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
		}

		if (TracerEffect)
		{
			FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
			UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);

			if (TracerComp)
			{
				// Set the vector in the emitter, that has the name stored in TracerTargetName, to the end point value
				TracerComp->SetVectorParameter(TracerTargetName, TracerEndPoint);
			}
		}
	}
}
