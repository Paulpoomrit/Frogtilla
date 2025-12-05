// Copyright 2025 NFR, All rights reserved.


#include "BoatPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Frogtilla/TileSystem/Tile.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

namespace BoatPawnConst
{
	constexpr int BoatAngularDampingStep = 3; // step==3 gives us [0,18] damping from x in range [0,6]
}

// Sets default values
ABoatPawn::ABoatPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create default components
	this->BoatMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoatMesh"));
	this->RootComponent = this->BoatMeshComponent;
	this->SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	this->SpringArmComponent->SetupAttachment(this->BoatMeshComponent);
	this->CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	this->CameraComponent->SetupAttachment(SpringArmComponent);
	this->CollisionMesh = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionMesh"));
	this->CollisionMesh->SetupAttachment(this->BoatMeshComponent);

	this->BoatConfig = FBoatConfigData(1,1,1);
}

// Called when the game starts or when spawned
void ABoatPawn::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);
	const APlayerController* PlayerController = Cast<APlayerController>(Controller);
	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem =	ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	Subsystem->AddMappingContext(DefaultMappingContext, 0);
}

// Called every frame
void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABoatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABoatPawn::Move);
	EnhancedInputComponent->BindAction(RotateAction, ETriggerEvent::Triggered, this, &ABoatPawn::Turn);
	EnhancedInputComponent->BindAction(BalanceAction, ETriggerEvent::Triggered, this, &ABoatPawn::TryGainingBalance);
}

void ABoatPawn::SetBoatGameMode_Implementation(const EBoatGameMode NewGameMode)
{
	switch (NewGameMode)
	{
	case EBoatGameMode::GameMode_ConfigScreen:
		this->BoatMeshComponent->SetSimulatePhysics(false);
		DisableInput(GetWorld()->GetFirstPlayerController());
		return;
	case EBoatGameMode::GameMode_MainLevel:
		this->BoatMeshComponent->SetSimulatePhysics(true);
		EnableInput(GetWorld()->GetFirstPlayerController());
		return;
	}
	std::abort();
}

void ABoatPawn::ApplyTileEffect_Implementation(const FTileEffect& TileEffect)
{
	if (TileEffect.EndGameOnOverlap)
	{
		EventOnEndLevel.Broadcast();
	}
	
	// Apply steering chaos factor
	const int NewBoatDamp = (BoatConfig.SteeringPrecision - TileEffect.SteeringChaosFactor) * BoatPawnConst::BoatAngularDampingStep;
	this->BoatMeshComponent->SetAngularDamping(FMath::Max(NewBoatDamp, 0));

	UE_LOG(LogTemp, Warning, TEXT("Applying SteeringChaosFactor=%d, resulting AngularDamping=%d"),
	   TileEffect.SteeringChaosFactor, NewBoatDamp);

	// Apply Damage	
	this->Health = FMath::Max(this->Health-TileEffect.DamageAmount, 0);
	UE_LOG(LogTemp, Warning, TEXT("Applying Damage=%.2f, CurrentHealth = %.2d"), 
	   TileEffect.DamageAmount, Health);

	if (this->Health <= 0)
	{
		EventOnEndLevel.Broadcast();
	}
}

void ABoatPawn::Move(const FInputActionValue& Value)
{
	const float InputValue = Value.Get<float>();
	const FVector Forward = this->BoatMeshComponent->GetForwardVector().GetSafeNormal2D();
	const float Mass = this->BoatMeshComponent->GetMass();
	const FVector Force = Forward * InputValue * this->Speed * Mass;
	this->BoatMeshComponent->AddForce(Force);

	const FVector Velocity = this->BoatMeshComponent->GetPhysicsLinearVelocity();
	const float SpeedSq = Velocity.SizeSquared();
	if (SpeedSq > MaxSpeed * MaxSpeed)
	{
		this->BoatMeshComponent->SetPhysicsLinearVelocity(Velocity.GetSafeNormal() * MaxSpeed);
	}
}

void ABoatPawn::Turn(const FInputActionValue& Value)
{
	const float InputValue = Value.Get<float>();
	FRotator DeltaRotation = FRotator::ZeroRotator;
	DeltaRotation.Yaw = InputValue * this->TurnRate * UGameplayStatics::GetWorldDeltaSeconds(this);
	AddActorWorldRotation(DeltaRotation);
}

void ABoatPawn::TryGainingBalance()
{
	const FRotator CurrentRotation = this->BoatMeshComponent->GetComponentRotation();
	const FRotator TargetRotation = FRotator(0.0f, CurrentRotation.Yaw, 0.0f);
	const FRotator InterpPos = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), AvgTimeToGainBalance);
	SetActorRotation(InterpPos);
}

void ABoatPawn::SetBoatConfig_Implementation(const FBoatConfigData& BoatConfig)
{
	this->BoatConfig = BoatConfig;
	this->BoatMeshComponent->SetAngularDamping(BoatConfig.SteeringPrecision * BoatPawnConst::BoatAngularDampingStep);
}


