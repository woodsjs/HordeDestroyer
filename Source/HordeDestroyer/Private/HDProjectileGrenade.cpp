// Fill out your copyright notice in the Description page of Project Settings.


#include "HDProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHDProjectileGrenade::AHDProjectileGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = MeshComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	SetReplicates(true);
}

// We need to tick for a set time before we detonate after launching. 
// We're launced after initialization
void AHDProjectileGrenade::Launched()
{
	//GetWorldTimerManager().ClearTimer(TimerHandle_DetonateProjectile);
	GetWorldTimerManager().SetTimer(TimerHandle_DetonateProjectile, this, &AHDProjectileGrenade::OnDetonate, DetonateTime);
}

// Called every frame
void AHDProjectileGrenade::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AHDProjectileGrenade::OnDetonate()
{
	UE_LOG(LogTemp, Log, TEXT("OnDetonate fired. bExploded is %d"), bExploded);

	bExploded = true;

	//TArray<AActor*> ignoreList;
	//bool bDidDamage = UGameplayStatics::ApplyRadialDamage(GetWorld(), 50.0f, this->GetActorLocation(), 20.0f, DamageType, ignoreList);

	TArray<AActor*> IgnoreActor;
	float BaseDamage = 50.0f;
	float MinAppliedDamage = 0.0f;
	FVector Position = this->GetActorLocation();
	float BaseForceRadius = 0.0f;
	float DamageFalloff = 0.5f;
	TSubclassOf<UDamageType> AppliedDamageType = DamageType;

	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), BaseDamage, MinAppliedDamage,
		Position, 0.0f, BaseForceRadius, DamageFalloff, AppliedDamageType, IgnoreActor);

	//Destroy();
}

void AHDProjectileGrenade::OnRep_bExploded()
{
	UE_LOG(LogTemp, Log, TEXT("We should explode here"));
	PlayExplosionEffects();

	Destroy();

}

void AHDProjectileGrenade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	UE_LOG(LogTemp, Log, TEXT("Got rep lifetime props"));

	DOREPLIFETIME(AHDProjectileGrenade, bExploded);
}

void AHDProjectileGrenade::PlayExplosionEffects()
{
	UE_LOG(LogTemp, Log, TEXT("Play Explosion Effects"));


	if (DetonateEffect)
	{
		UE_LOG(LogTemp, Log, TEXT("We should have detonate effect here"));

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DetonateEffect, this->GetActorLocation(), this->GetActorRotation());
	}

	MakeNoise(1.0f, GetInstigator());

}
