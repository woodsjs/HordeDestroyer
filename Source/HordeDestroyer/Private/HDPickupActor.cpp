// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"
#include "HDPowerupActor.h"

// Sets default values
AHDPickupActor::AHDPickupActor()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(75.0f);
	RootComponent = SphereComp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AHDPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
	Respawn();
}

void AHDPickupActor::Respawn()
{
	if (PowerupClass == nullptr)
	{
		UE_LOG(LogTemp, Log, TEXT("Powerup Class is %s. Please update your blueprint"), *GetName());
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	PowerupInstance = GetWorld()->SpawnActor<AHDPowerupActor>(PowerupClass, GetTransform(), SpawnParams);
}

void AHDPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (PowerupInstance)
	{
		PowerupInstance->ActivatePowerup();
		PowerupInstance = nullptr;

		GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &AHDPickupActor::Respawn, CoolDownDuration);
	}
}

