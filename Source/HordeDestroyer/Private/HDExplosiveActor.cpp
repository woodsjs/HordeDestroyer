// Fill out your copyright notice in the Description page of Project Settings.


#include "HDExplosiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HDHealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Components/SphereComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/EngineTypes.h"
#include "../HordeDestroyer.h"

static int32 DebugExplosiveDrawing = 0;
FAutoConsoleVariableRef CVARDebugExplosiveDrawing(
	TEXT("HD.DebugExplosives"),
	DebugExplosiveDrawing,
	TEXT("Draw Debug Info for Explosives"),
	ECVF_Cheat);

// Sets default values
AHDExplosiveActor::AHDExplosiveActor()
{
	// lol looking at URadialForceComponent, I just recreated part of what it does.  

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = BaseMesh;

	// Use a sphere as a simple collision representation
	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphereComp"));
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->SetGenerateOverlapEvents(true);

	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	// Anything that is seen, should be blown away
	FCollisionResponseContainer OverlapChannels = FCollisionResponseContainer(ECollisionResponse::ECR_Ignore);
	OverlapChannels.SetResponse(ECC_Destructible, ECR_Overlap);
	OverlapChannels.SetResponse(ECC_Pawn, ECR_Overlap);
	OverlapChannels.SetResponse(ECC_PhysicsBody, ECR_Overlap);
	OverlapChannels.SetResponse(ECC_Vehicle, ECR_Overlap);

	DamageSphere->SetCollisionResponseToChannels(OverlapChannels);

	//DamageSphere->SetSphereRadius(BaseForceRadius);
	DamageSphere->SetupAttachment(RootComponent);

	MyHealthComp = CreateDefaultSubobject<UHDHealthComponent>(TEXT("MyHealthComp"));

}

// Called when the game starts or when spawned
void AHDExplosiveActor::BeginPlay()
{
	Super::BeginPlay();

	MyHealthComp->OnHealthChanged.AddDynamic(this, &AHDExplosiveActor::OnHealthChanged);

	FVector Position = GetActorLocation();
	FQuat Rotation;
	BaseMesh->GetRelativeRotationFromWorld(Rotation);

	// Get all components that overlap with our sphere. We'll use the Visibility collision channel
	// Anything that can be seen needs to be blown away
	DamageSphere->ComponentOverlapMulti(OverlappingActors, GetWorld(), Position, Rotation, 
		ECollisionChannel::ECC_Visibility);

	if (DebugExplosiveDrawing > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Radius %f"), DamageSphere->GetUnscaledSphereRadius());
	}

}

// might want another mesh as well to set when we explode
void AHDExplosiveActor::OnHealthChanged(UHDHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f && !bExploded)
	{
		//Die!
		bExploded = true;

		// play particle effect
		if (ExplosionEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, this->GetActorLocation(), this->GetActorRotation());
		}

		// set exploded material
		if (ExplodedMaterial)
		{
			BaseMesh->SetMaterial(0, ExplodedMaterial);
		}

		// Look at each component and apply a radial impulse, originating from our actor's location
		// We can do different if we'd like
		// Also set the force and strength to be params
		FVector Position = GetActorLocation();

		for (FOverlapResult OverlappedActor : OverlappingActors)
		{
			float BaseForceRadius = this->DamageSphere->GetUnscaledSphereRadius();

			if (DebugExplosiveDrawing > 0)
			{
				UE_LOG(LogTemp, Log, TEXT("We have an overlap %s"), *OverlappedActor.GetActor()->GetName());
			}

			// Simulate Physics needs to be on for this next bit to work
			// so let's do that
			OverlappedActor.GetComponent()->AddRadialImpulse(Position, BaseForceRadius, BaseForceStrength, ERadialImpulseFalloff::RIF_Linear, true);

			TArray<AActor*> IgnoreActor;
			UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, MinAppliedDamage,
				Position, 0.0f, BaseForceRadius, DamageFalloff, AppliedDamageType, IgnoreActor);
		}
	}
}

// Called every frame
void AHDExplosiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

