// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Pawn.h"
#include "BoatPawn.generated.h"

class UBoxComponent;
class UCameraComponent;
class UCapsuleComponent;
class UInputAction;
class UInputMappingContext;
class USpringArmComponent;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoatDispatcher);

USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains boat configuration"))
struct FBoatConfigData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Food = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Fuel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SteeringPrecision = 1;
};

UENUM(BlueprintType)
enum class EBoatGameMode : uint8
{
	GameMode_ConfigScreen,
	GameMode_MainLevel
};

UCLASS()
class FROGTILLA_API ABoatPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABoatPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FBoatConfigData GetBoatConfig() const
	{
		return BoatConfig;
	}
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boat")
	void SetBoatConfig(const FBoatConfigData& BoatConfig);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boat")
	void ApplyTileEffect(const FTileEffect& TileEffect);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boat")
	void SetBoatGameMode(const EBoatGameMode NewGameMode);

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Dispatcher")
	FBoatDispatcher EventOnEndLevel;
private:
	// Game-loop-related properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay", meta = (AllowPrivateAccess = "true"))
	int Health = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay", meta = (AllowPrivateAccess = "true"))
	FBoatConfigData BoatConfig;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BoatMeshComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* CollisionMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* BalanceAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float Speed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float MaxSpeed = 2000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float TurnRate = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement", meta = (AllowPrivateAccess = "true"))
	float AvgTimeToGainBalance = 10.0f;
	
	void Move(const FInputActionValue& Value);
	void Turn(const FInputActionValue& Value);
	void TryGainingBalance();	
};
