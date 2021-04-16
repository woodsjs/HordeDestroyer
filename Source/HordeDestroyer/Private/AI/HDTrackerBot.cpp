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
#include "Components/SphereComponent.h"
#include "HDCharacter.h"

// our sound
#include "Sound/SoundCue.h"

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

	TargetOverlapComp = CreateDefaultSubobject<USphereComponent>(TEXT("TargetOverlapComp"));
	TargetOverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TargetOverlapComp->SetSphereRadius(200);
	TargetOverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	// for our multiplier on the trackerbot
	TargetOverlapComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);

	// for our pawn
	TargetOverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	TargetOverlapComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;
	ExplosionDamage = 40;
	ExplosionRadius = 200;
	SelfDamageInterval = 0.25f;
	MultiBotDamageMultiplier = 2;
}

// Called when the game starts or when spawned
void AHDTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	HealthComp->OnHealthChanged.AddDynamic(this, &AHDTrackerBot::OnTakeDamage);
	//OnActorBeginOverlap.AddDynamic(this);

	if (GetLocalRole() == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();
	}

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

	if (GetLocalRole() == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();

			if (DebugTrackerBotDrawing > 0)
			{
				DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached", NULL, FColor::Yellow, 0.0f, false, 1.0f);
			}

		}
		else
		{
			// keep moving towards next target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			if (DebugTrackerBotDrawing > 0)
			{
				DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Red, false, 0.0f);
			}
		}

		if (DebugTrackerBotDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, 20.0f, 16, FColor::Yellow, false, 0.0f);
		}
	}



	// attenuate the sound of teh audio as the ball rolls
	//const float CurrentVelocity = GetVelocity().Size();
	// This is map range clamped from blueprint
	//float ClampedVelocity = FMath::GetMappedRangeValueClamped(FVector2D(10, 1000), FVector2D(0.1, 2), CurrentVelocity);
	//get audio comp
	// set volume multiplier, target audio comp, new multiplier the output of map range clamped
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
		SelfDestruct();
	}
}

void AHDTrackerBot::SelfDestruct()
{

	if (bExploded)
	{
		return;
	}

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, this->GetActorLocation());

	// we don't need to attach, since we're exploding anyway
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	if (GetLocalRole() == ROLE_Authority)
	{
		TArray<AActor*> IgnoreActor;
		IgnoreActor.Add(this);

		// when we overlap other tracker bots, we want to increase the damage power of the bot 
		// we do this with a multiplier, and the num of overlapped tracker bots
		// Not sure why, but there are times when we have less than 0 overlapped bots
		ExplosionDamage = (OverlappedTrackerBots <= 0) ? ExplosionDamage : ExplosionDamage * (MultiBotDamageMultiplier * OverlappedTrackerBots);

		if (DebugTrackerBotDrawing > 0)
		{
			DrawDebugString(GetWorld(), NextPathPoint, FString::SanitizeFloat( ExplosionDamage), this, FColor::Red, 2.0f, false, 2.0f);
			UE_LOG(LogTemp, Log, TEXT("Blewd up %f"), ExplosionDamage);
		}

		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoreActor, this, GetInstigatorController(), true);

		if (DebugTrackerBotDrawing > 0)
		{
			DrawDebugSphere(GetWorld(), NextPathPoint, ExplosionDamage, 16, FColor::Red, false, 2.0f);
		}

		SetLifeSpan(0.1);
		//Destroy();
	}

}

void AHDTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

void AHDTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!bStartedSelfDestruction && !bExploded)
	{

		AHDCharacter* PlayerPawn = Cast<AHDCharacter>(OtherActor);

		// overlapped a player
		if (PlayerPawn)
		{
			bStartedSelfDestruction = true;

			if (GetLocalRole() == ROLE_Authority)
			{
				// odd enough, we'll inflict damage on ourselves until we explode
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &AHDTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}

		AHDTrackerBot* TrackerActor = Cast<AHDTrackerBot>(OtherActor);
		if (TrackerActor)
		{
			// set count
			OverlappedTrackerBots += 1;

			if (MatInst == nullptr)
			{
				MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
			}

			if (MatInst)
			{
				MatInst->SetScalarParameterValue("PowerLevelAlpha", OverlappedTrackerBots);
			}
		}

	}
}

// we want to decrement our trackerbot count when they leave
void AHDTrackerBot::NotifyActorEndOverlap(AActor* OtherActor)
{
	// TODO: Add call to super here

	AHDTrackerBot* TrackerActor = Cast<AHDTrackerBot>(OtherActor);
	if (TrackerActor)
	{
		// set count
		OverlappedTrackerBots -= 1;


		if (MatInst == nullptr)
		{
			MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
		}

		if (MatInst)
		{
			MatInst->SetScalarParameterValue("PowerLevelAlpha", OverlappedTrackerBots);
		}
	}
}
