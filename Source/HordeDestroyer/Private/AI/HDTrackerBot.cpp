// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/HDTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
// to use this we have to add NavigationSystem to HordeDestroyer.build.cs
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"

// Sets default values
AHDTrackerBot::AHDTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);

	RootComponent = MeshComp;
}

// Called when the game starts or when spawned
void AHDTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AHDTrackerBot::GetNextPathPoint()
{
	// hack to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		// get next point in path. First point is current location
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

// Called every frame
void AHDTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


