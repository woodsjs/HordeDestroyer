// Fill out your copyright notice in the Description page of Project Settings.


#include "HDProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"

#include "DrawDebugHelpers.h"

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

//void AHDProjectileGrenade::OnDetonate(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
void AHDProjectileGrenade::OnDetonate()
{
	TArray<AActor*> ignoreList;
	bool bDidDamage = UGameplayStatics::ApplyRadialDamage(GetWorld(), 50.0f, this->GetActorLocation(), 20.0f, DamageType, ignoreList);
	UE_LOG(LogTemp, Log, TEXT("did damage %d"), bDidDamage);

	if (DetonateEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DetonateEffect, this->GetActorLocation(), this->GetActorRotation());
	}

	MakeNoise(1.0f, GetInstigator());
	Destroy();
	//}
}

