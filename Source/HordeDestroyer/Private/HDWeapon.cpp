// Fill out your copyright notice in the Description page of Project Settings.


#include "HDWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Chaos/ChaosEngineInterface.h"
#include "HordeDestroyer/HordeDestroyer.h"
#include "Net/UnrealNetwork.h"

// test for recoil
//#include "Curves/CurveFloat.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing (
	TEXT("HD.DebugWeapons"), 
	DebugWeaponDrawing, 
	TEXT("Draw Debug Lines for Weapons"), 
	ECVF_Cheat);

// Sets default values
AHDWeapon::AHDWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleFlashSocket";
	// In the tracer emitter, we have a vector param called this
	// We change the vector later down the line
	TracerTargetName = "BeamEnd";

	BaseDamage = 20.0f;

	RateOfFire = 600;

	SetReplicates(true);

	// reduce latency. 33FPS vs 66FPS
	NetUpdateFrequency = 66.0f;
	MinNetUpdateFrequency = 33.0f;
}

void AHDWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60/RateOfFire;
}


void AHDWeapon::Fire()
{
	// if this is not the server
	// call the server RPC
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}

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
		QueryParams.bReturnPhysicalMaterial = true;

		// Particle "Target" Parameter value. 
		// If we hit nothing, we just go to the trace end
		// otherwise we go to the Hit.ImpactPoint
		FVector TracerEndPoint = TraceEnd;

		// use the default unless we override on a hit result
		// this avoids caching the surfce emitter
		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FHitResult Hit;
		if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
		{
			// Blocking hit, so process damage
			AActor* HitActor = Hit.GetActor();


			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;
			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);

			TracerEndPoint = Hit.ImpactPoint;

		}


		if (DebugWeaponDrawing > 0)
		{
			DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::White, false, 1.0f, 0, 1.0f);
		}

		PlayFireEffects(TracerEndPoint);

		// Replicate endpoint only, clients can derive the start point on their own
		// server only
		if (GetLocalRole() == ROLE_Authority)
		{
			HitScanTrace.TraceTo = TracerEndPoint;
			HitScanTrace.SurfaceType = SurfaceType;

		}

		LastFiredTime = GetWorld()->TimeSeconds;

		// we need to allow our designer to choose the curve!
		// Do we need this in tick?
		float HorizontalRecoil = 0.1f;

		if (HorizontalRecoilCurve)
		{
			// need to figure out how to get the time herew
			HorizontalRecoil = HorizontalRecoilCurve->GetFloatValue(0.0f);
		}
		APawn* OwnerPawn = Cast<APawn>(MyOwner);
		OwnerPawn->AddControllerPitchInput(-0.5f);
		OwnerPawn->AddControllerYawInput(HorizontalRecoilCurve->GetFloatValue(0.0f));
		
	}
}

void AHDWeapon::StartFire()
{
	// Set our delay to either 0 to fire NOW
	// or to the difference between when we last fired plus our delay, minus what time it is right now
	float FirstDelay = FMath::Max( LastFiredTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

	GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AHDWeapon::Fire, TimeBetweenShots, true, FirstDelay);
}

void AHDWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}

void AHDWeapon::PlayFireEffects(FVector TracerEndPoint)
{

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

	APawn* MyOwner = Cast<APawn>(GetOwner());
	if (MyOwner)
	{
		APlayerController* PC = Cast<APlayerController>(MyOwner->GetController());

		if (PC)
		{
			PC->ClientStartCameraShake(FireCameraShake );
		}
	}



}

void AHDWeapon::ServerFire_Implementation()
{
	// this is only called on the server
	Fire();
}

// anticheat
bool AHDWeapon::ServerFire_Validate()
{
	return true;
}

void AHDWeapon::OnRep_HitScanTrace()
{
	// Play effects
	PlayFireEffects(HitScanTrace.TraceTo);

	PlayImpactEffects(HitScanTrace.SurfaceType, HitScanTrace.TraceTo);
}


void AHDWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// since we're already playing the effects on the owning client,
	// we don't need to replicate this back to the same owning client
	// so COND_SkipOwner...
	DOREPLIFETIME_CONDITION(AHDWeapon, HitScanTrace, COND_SkipOwner);
}

void AHDWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		break;
	}


	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
}

