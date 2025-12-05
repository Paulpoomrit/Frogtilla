// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TileBoard.generated.h"


class ATile;

USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains spawn params for a tileboard"))
struct FTileBoardSpawnParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int NumRowsAndColumns = 20;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CellWidth = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CellHeight = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector StartingPos = FVector::ZeroVector;
};

USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains info ab concrete types of tile"))
struct FConcreteTileInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<ATile> TileClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnChance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ShouldSpawn;
};

UCLASS()
class FROGTILLA_API ATileBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitTileBoard(const FTileBoardSpawnParams& SpawnParams,
						const TArray<FConcreteTileInfo>& ConcreteTileInfos);
	
	ATile* GetBoatSpawnSpot();
	TArray<ATile*> GetBoatSpawnArea();

	UFUNCTION(BlueprintCallable)
	static ATileBoard* GetTileBoard(const FTileBoardSpawnParams& SpawnParams, const TArray<FConcreteTileInfo>& ConcreteTileInfos);

	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Tileboard", meta = (AllowPrivateAccess = "true"))
	TArray<ATile*> TileBoard;

	int NumRowsAndColumns;
	int CellWidth;
	int CellHeight;
	FVector StartingPos;

};
