// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HDHealthComponent.generated.h"

// Delegate for our health changes
DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UHDHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class HORDEDESTROYER_API UHDHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHDHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// let's replicate health around
	//UPROPERTY(Replicated, ReplicatedUsing=onRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	UPROPERTY(ReplicatedUsing=onRep_Health, BlueprintReadOnly, Category = "HealthComponent")
	float Health;

	UFUNCTION()
	void onRep_Health(float OldHealth);

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HealthComponent")
	float DefaultHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
		
public:

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);
};
