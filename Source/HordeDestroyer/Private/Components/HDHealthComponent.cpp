// Fill out your copyright notice in the Description page of Project Settings.
/*

	In your header file
		// add health component
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UHDHealthComponent* MyHealthComp;

		UFUNCTION()
		void OnHealthChanged(UHDHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

		// pawn died previously
		UPROPERTY(BlueprintReadOnly, Category = "Player")
		bool bDied;

	In your constructor
		MyHealthComp = CreateDefaultSubobject<UHDHealthComponent>(TEXT("MyHealthComp"));

	In begin play
		MyHealthComp->OnHealthChanged.AddDynamic(this, &<<Name of class>>::OnHealthChanged);

	Add a method
		void AHDCharacter::OnHealthChanged(UHDHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType,
		class AController* InstigatedBy, AActor* DamageCauser)
		{
			if (Health <= 0.0f && !bDied)
			{
				//Die!

				bDied = true;

				//stop movement
				GetMovementComponent()->StopMovementImmediately();

				// don't allow collisions
				GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

				// we shouldn't possess this anymore
				DetachFromControllerPendingDestroy();

				SetLifeSpan(10);

			}
		}
*/

#include "Components/HDHealthComponent.h"
#include "Net/UnrealNetwork.h"
#include "HDGameMode.h"

// Sets default values for this component's properties
UHDHealthComponent::UHDHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	DefaultHealth = 100.0f;

	bIsDead = false;
	//SetIsReplicated(true);

	// ...
	TeamNum = 255;
}


// Called when the game starts
void UHDHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();
		if (MyOwner)
		{
			// Set up a subscription and handler for OnTakeAnyDamage, so we can respond to damage events
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHDHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health = DefaultHealth;
}

// stole this code
void UHDHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f || bIsDead)
	{
		return;
	}

	// Don't allow friendly fire
	if ( DamageCauser != DamagedActor && IsFriendly(DamagedActor, DamageCauser))
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	//UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	bIsDead = GetHealth() <= 0.0f;

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{
		OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
	}

	if (bIsDead)
	{
		AHDGameMode* GM = Cast<AHDGameMode>(GetWorld()->GetAuthGameMode());
		if (GM)
		{
			GM->OnActorKilled.Broadcast(GetOwner(), DamageCauser, InstigatedBy);
		}
	}

}

void UHDHealthComponent::Heal(float HealAmount)
{
	if (HealAmount <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);
	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s (+%s)"), *FString::SanitizeFloat(Health), *FString::SanitizeFloat(HealAmount));

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);

}

float UHDHealthComponent::GetHealth() const
{
	return Health;
}

bool UHDHealthComponent::IsFriendly(AActor* ActorA, AActor* ActorB)
{
	if(ActorA == nullptr || ActorB == nullptr)
	{
		return true;
	}

	UHDHealthComponent* HealthComponentA = Cast<UHDHealthComponent>(ActorA->GetComponentByClass(UHDHealthComponent::StaticClass()));
	UHDHealthComponent* HealthComponentB = Cast<UHDHealthComponent>(ActorB->GetComponentByClass(UHDHealthComponent::StaticClass()));

	if (HealthComponentA == nullptr || HealthComponentB == nullptr)
	{
		return true;
	}

	return HealthComponentA->TeamNum == HealthComponentB->TeamNum;
}

void UHDHealthComponent::onRep_Health(float OldHealth)
{
	float Damage = Health - OldHealth;
	OnHealthChanged.Broadcast(this, Health, Damage, nullptr, nullptr, nullptr);
}

void UHDHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHDHealthComponent, Health);
}
