// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Delegates/DelegateCombinations.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class ABoatPawn;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMultiDispatcher);

class UBoxComponent;

USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains info about how a tile should effect the boat pawn"))
struct FTileEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount= 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ApplyDamageOnce = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageInterval = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SteeringChaosFactor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EndGameOnOverlap = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldDeferApplyingDamage = false;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains spawn params for a tile"))
struct FTileSpawnParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector TileOrigin = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TileSize = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor TileColour = FColor::Transparent;
};

UCLASS(Abstract)
class FROGTILLA_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	FVector GetTileOrigin() const
	{
		return TileOrigin;
	}
	void SetTileOrigin(const FVector& TileOrigin)
	{
		this->TileOrigin = TileOrigin;
	}
	float GetTileSize() const
	{
		return TileSize;
	}
	void SetTileSize(const float TileSize)
	{
		this->TileSize = TileSize;
	}
	FColor GetTileColour() const
	{
		return TileColour;
	}
	void SetTileColour(const FColor& TileColour)
	{
		this->TileColour = TileColour;
	}
	UBoxComponent* GetTileCollisionBound() const
	{
		return TileCollisionBound;
	}
	UFUNCTION(BlueprintCallable)
	void SetTileCollisionBound(const FVector& TileCollisionBound);
private:
	float TileSize;
	FColor TileColour;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Data", meta = (AllowPrivateAccess = "true"))
	FVector TileOrigin;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Data", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TileCollisionBound;

public:
	virtual FTileEffect GetTileEffectOnOverlap() const PURE_VIRTUAL(GetTileEffectOnOverlap, return FTileEffect(););
	FMultiDispatcher EventOnBoatBeginOverlap;

	// Init w SpawParams -> intended to be used after the tile is created by the factory
	virtual void InitTile(const FTileSpawnParams& SpawnParams);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Effects", meta = (AllowPrivateAccess = "true"))
	FTimerHandle EffectTimerHandle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tile Effects", meta = (AllowPrivateAccess = "true"))
	ABoatPawn* BoatPawn = nullptr;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile Effects")
	void HandleEventOnOverlap(ABoatPawn* OverlappedBoat);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile Effects")
	void StartApplyingTileEffect(ABoatPawn* OverlappedBoat);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Tile Effects")
	void StopApplyingTileEffect();
	
	UFUNCTION(BlueprintCallable, Category = "Tile Effects")
	void ApplyEffectTick();

	UFUNCTION(BlueprintCallable, Category = "Tile Effects")
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable, Category = "Tile Effects")
	void EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};


