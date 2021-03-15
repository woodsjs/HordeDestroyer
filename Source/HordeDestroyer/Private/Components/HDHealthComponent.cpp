// Fill out your copyright notice in the Description page of Project Settings.


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
}


