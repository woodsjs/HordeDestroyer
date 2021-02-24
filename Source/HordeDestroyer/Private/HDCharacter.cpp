// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCharacter.h"

// Sets default values
AHDCharacter::AHDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHDCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AHDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

