// Fill out your copyright notice in the Description page of Project Settings.


#include "HDExplosiveActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/HDHealthComponent.h"

// Sets default values
AHDExplosiveActor::AHDExplosiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	ActorHealth = CreateDefaultSubobject<UHDHealthComponent>(TEXT("ActorHealth"));
}

// Called when the game starts or when spawned
void AHDExplosiveActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHDExplosiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

