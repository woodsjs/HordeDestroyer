// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPickupActor.h"
#include "Components/SphereComponent.h"
#include "Components/DecalComponent.h"

// Sets default values
AHDPickupActor::AHDPickupActor()
{
	SphereCmp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereCmp->SetSphereRadius(75.0f);
	RootComponent = SphereCmp;

	DecalComp = CreateDefaultSubobject<UDecalComponent>(TEXT("DecalComp"));
	DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f));
	DecalComp->DecalSize = FVector(64.0f, 75.0f, 75.0f);
	DecalComp->SetupAttachment(RootComponent);


}

// Called when the game starts or when spawned
void AHDPickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AHDPickupActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	// TODO: Grant a powerup to player if available
}

