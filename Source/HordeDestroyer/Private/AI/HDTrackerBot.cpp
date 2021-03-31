// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/HDTrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
// to use this we have to add NavigationSystem to HordeDestroyer.build.cs
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"

// Debug
#include "DrawDebugHelpers.h"

// our HealthComp
#include "Components/HDHealthComponent.h"

static int32 DebugTrackerBotDrawing = 0;
FAutoConsoleVariableRef CVARDebugTrackerBotDrawing(
	TEXT("HD.DebugTrackerBot"),
	DebugTrackerBotDrawing,
	TEXT("Draw Debug Lines for TrackerBots"),
	ECVF_Cheat);

// Sets default values
AHDTrackerBot::AHDTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	RootComponent = MeshComp;

	HealthComp = CreateDefaultSubobject<UHDHealthComponent>(TEXT("HealthComp"));
	HealthComp->SetIsReplicated(true);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
}

// Called when the game starts or when spawned
void AHDTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AHDTrackerBot::OnTakeDamage);

	NextPathPoint = GetNextPathPoint();
	
}

FVector AHDTrackerBot::GetNextPathPoint()
{
	// hack to get player location
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (PlayerPawn)
	{
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

		if (NavPath->PathPoints.Num() > 1)
		{
			// get next point in path. First point is current location
			return NavPath->PathPoints[1];
		}
	}

	return GetActorLocation();
}

// Called every frame
void AHDTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

	if ( DistanceToTarget <= RequiredDistanceToTarget )
	{
		NextPathPoint = GetNextPathPoint();

		if ( DebugTrackerBotDrawing > 0 )
		{
			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached", NULL, FColor::Yellow,0.0f,false, 1.0f);
		}

	}
	else
	{
		// keep moving towards next target
		FVector ForceDirection = NextPathPoint - GetActorLocation();
		ForceDirection.Normalize();

		ForceDirection *= MovementForce;

		MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

		//NextPathPoint = GetNextPathPoint();

		if ( DebugTrackerBotDrawing > 0 )
		{
			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.0f);
		}
	}

	if (DebugTrackerBotDrawing > 0)
	{
		DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 16, FColor::Yellow, false, 0.0f);
	}
}

void AHDTrackerBot::OnTakeDamage(UHDHealthComponent* MyHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
	class AController* InstigatedBy, AActor* DamageCauser)
{

	if (DebugTrackerBotDrawing > 0)
	{
		UE_LOG(LogTemp, Log, TEXT("Took damage: %f name: %s"), Health, *GetName());
	}

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f && !bExploded)
	{
		//Die!

		bExploded = true;

		// @TODO: Pulse on hit
		UE_LOG(LogTemp, Log, TEXT("Died"), Health);

		//SetLifeSpan(10);

	}
}
