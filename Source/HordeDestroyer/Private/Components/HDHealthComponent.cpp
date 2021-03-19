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

// Sets default values for this component's properties
UHDHealthComponent::UHDHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
	DefaultHealth = 100;
}


// Called when the game starts
void UHDHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AActor* MyOwner = GetOwner();
	if (MyOwner)
	{
		// Set up a subscription and handler for OnTakeAnyDamage, so we can respond to damage events
		MyOwner->OnTakeAnyDamage.AddDynamic(this, &UHDHealthComponent::HandleTakeAnyDamage);
	}

	Health = DefaultHealth;
}

//void UHDHealthComponent::HandleTakeAnyDamage(AActor OnTakeAnyDamage, AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
void UHDHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	UE_LOG(LogTemp, Log, TEXT("Health Changed: %s"), *FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}


