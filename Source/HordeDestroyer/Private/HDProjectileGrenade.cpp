// Fill out your copyright notice in the Description page of Project Settings.


#include "HDProjectileGrenade.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHDProjectileGrenade::AHDProjectileGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

		// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->SetCollisionProfileName("Projectile");
	//CollisionComp->OnComponentHit.AddDynamic(this, &AHDProjectileGrenade::OnDetonate);	// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;

	//SetReplicates(true);
	//SetReplicateMovement(true);
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
	// Only add impulse and destroy projectile if we hit a physics
	// TODO: Need to change this, we don't care if it's hitting something
	// but if something is in the radius, use UGameplayStatics::ApplyPointDamage
	//OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

	if (DetonateEffect)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DetonateEffect, this->GetActorLocation(), this->GetActorRotation());
	}

	//if (GetLocalRole() == ROLE_Authority)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("something something projectile"));

	MakeNoise(1.0f, GetInstigator());
	Destroy();
	//}
}

