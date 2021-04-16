// Fill out your copyright notice in the Description page of Project Settings.


#include "HDCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "HDWeapon.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "HordeDestroyer/HordeDestroyer.h"
#include "Components/HDHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AHDCharacter::AHDCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	// since this is attached to a pawn, use the pawn's rotation to control the camera
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanJump = true;

	GetCapsuleComponent()->SetCollisionResponseToChannel(COLLISION_WEAPON, ECR_Ignore);

	MyHealthComp = CreateDefaultSubobject<UHDHealthComponent>(TEXT("MyHealthComp"));
	MyHealthComp->SetIsReplicated(true);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20.0f;

	WeaponAttachSocketName = "hand_r_weapon_socket";
}

// Called when the game starts or when spawned
void AHDCharacter::BeginPlay()
{
	Super::BeginPlay();

	DefaultFOV = CameraComp->FieldOfView;

	MyHealthComp->OnHealthChanged.AddDynamic(this, &AHDCharacter::OnHealthChanged);

	// spawn weapons from the server side
	// this way everyone sees em
	if (GetLocalRole() == ROLE_Authority)
	{
		// Spawn Default Weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		CurrentWeapon = GetWorld()->SpawnActor<AHDWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon)
		{
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}


}

void AHDCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AHDCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AHDCharacter::BeginCrouch()
{
	Crouch();
}

void AHDCharacter::EndCrouch()
{
	UnCrouch();
}

void AHDCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void AHDCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void AHDCharacter::Fire()
{
	//if (CurrentWeapon)
	//{
	//	CurrentWeapon->Fire();
	//}
}

void AHDCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}

void AHDCharacter::StopFire()
{
	if ( CurrentWeapon )
	{
		CurrentWeapon->StopFire();
	}
}

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

void AHDCharacter::onRep_IsInvisible(bool bOldVisiblility)
{

	// get skeletel mesh
	this->GetMesh()->SetOnlyOwnerSee(!bOldVisiblility);

}

void AHDCharacter::SetInvisibility(bool bNewInvisible)
{
	bInvisible = bNewInvisible;


	//if (GetLocalRole() == ROLE_Authority)
	//{
	//	UE_LOG(LogTemp, Log, TEXT("Why aint you doin"));
	//	USkeletalMesh* SkeleMesh = GetMesh()->SkeletalMesh;

	//	Cast<UPrimitiveComponent>(SkeleMesh)->SetOnlyOwnerSee(bNewInvisible);
	//	//UE_LOG(LogTemp, Log, TEXT("You are %d"), Cast<UPrimitiveComponent>(SkeleMesh)->bOnlyOwnerSee);
	//}
}

void AHDCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHDCharacter, CurrentWeapon);
	DOREPLIFETIME(AHDCharacter, bDied);
	DOREPLIFETIME(AHDCharacter, bInvisible);
}

// Called every frame
void AHDCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// we put this in tick so we could interpolate this!
	// zoom our FOV so we have a look down sight
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;
	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);

	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void AHDCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AHDCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHDCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("LookUp", this, &AHDCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &AHDCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &AHDCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &AHDCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AHDCharacter::Jump);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AHDCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AHDCharacter::EndZoom);

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHDCharacter::Fire);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHDCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AHDCharacter::StopFire);


}

// let's not start line tracing from our eye location, we want to do this from teh camera location 
// because it's acting as our eyes in this setup
FVector AHDCharacter::GetPawnViewLocation() const
{
	if (CameraComp)
	{
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

