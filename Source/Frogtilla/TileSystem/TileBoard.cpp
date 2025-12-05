// Copyright 2025 NFR, All rights reserved.


#include "TileBoard.h"

#include "ConcreteTiles/BasicTile.h"
#include "Tile.h"
#include "TileFactory.h"
#include <__random/random_device.h>
#include <random>

namespace TileBoardConst
{
	constexpr int SpawnAreaHorizontalRange = 3;
	constexpr int SpawnAreaVerticalRange = 6;
}

// Sets default values
ATileBoard::ATileBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATileBoard::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATileBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATileBoard::InitTileBoard(const FTileBoardSpawnParams& SpawnParams,
							   const TArray<FConcreteTileInfo>& ConcreteTileInfos)
{
	this->NumRowsAndColumns = SpawnParams.NumRowsAndColumns;
	this->CellWidth = SpawnParams.CellWidth;
	this->CellHeight = SpawnParams.CellHeight;
	this->StartingPos = SpawnParams.StartingPos;
	std::vector<float> TileSpawnChances;

	for (const FConcreteTileInfo& ConcreteTileInfo : ConcreteTileInfos)
	{
		const FString& TileName = ConcreteTileInfo.Name;
		UClass* TileClass = ConcreteTileInfo.TileClass;
		TileFactory::RegisterTileType(TileName,  [TileClass, this]() -> ATile*
		{
			ATile* NewTile = NewObject<ATile>(GetTransientPackage(), TileClass);
			return NewTile;
		});
		TileSpawnChances.push_back(ConcreteTileInfo.SpawnChance);
	}

	// Define tile spawn probability dist
	std::mt19937 gen(std::random_device{}());
	std::discrete_distribution<std::size_t> Distribution{TileSpawnChances.begin(), TileSpawnChances.end()};
	
	// Initialize board
	for (int Row = 0; Row < this->NumRowsAndColumns; Row++)
	{
		for (int Col = 0; Col < this->NumRowsAndColumns; Col++)
		{
			const int Index = Row * this->NumRowsAndColumns + Col;
			const FVector SpawnCoord = FVector(
				this->StartingPos.X + this->CellWidth * Col,  
				this->StartingPos.Y + this->CellHeight * Row, 
				this->StartingPos.Z);

			const FTileSpawnParams NewTileSpawnParams = FTileSpawnParams(SpawnCoord, this->CellWidth);

			const FString TileTypeName = ConcreteTileInfos[Distribution(gen)].Name;
			ATile* NewTile = TileFactory::CreateTileInstance(TileTypeName, NewTileSpawnParams);
			NewTile->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
			NewTile->SetActorLocation(SpawnCoord);
			this->TileBoard.Push(NewTile);
		}
	}

	// Define spawn area -> Replace with simple tile
	TArray<ATile*> BoatSpawnArea = GetBoatSpawnArea();
	for (ATile* Tile : BoatSpawnArea)
	{
		if (!Tile) continue;

		FVector TileLocation = Tile->GetTileOrigin();
		Tile->Destroy();
		
		ABasicTile* SimpleTile = NewObject<ABasicTile>(GetTransientPackage(), ABasicTile::StaticClass());
		SimpleTile->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		SimpleTile->SetTileOrigin(TileLocation);
		
		const int Index = this->TileBoard.Find(Tile);
		if (Index != INDEX_NONE)
		{
			this->TileBoard[Index] = SimpleTile;
		}
	}
}

ATile* ATileBoard::GetBoatSpawnSpot()
{
	const int LastRow = this->NumRowsAndColumns - 1;
	const int MiddleCol = this->NumRowsAndColumns/2;

	const int Index = LastRow * this->NumRowsAndColumns + MiddleCol;
	return this->TileBoard[Index] ? this->TileBoard[Index] : nullptr;	
}

TArray<ATile*> ATileBoard::GetBoatSpawnArea()
{
	TArray<ATile*> SpawnArea;

	const int MiddleCol = this->NumRowsAndColumns/2;
	
	// Define horizontal range
	const int MinCol = FMath::Max(0, MiddleCol - TileBoardConst::SpawnAreaHorizontalRange);
	const int MaxCol = FMath::Min(this->NumRowsAndColumns - 1, MiddleCol + TileBoardConst::SpawnAreaHorizontalRange);

	// Define vertical range
	const int MinRow = FMath::Max(0, this->NumRowsAndColumns - TileBoardConst::SpawnAreaVerticalRange);
	const int MaxRow = this->NumRowsAndColumns - 1;

	for (int Row = MinRow; Row <= MaxRow; ++Row)
	{
		for (int Col = MinCol; Col <= MaxCol; ++Col)
		{
			const int Index = Row * this->NumRowsAndColumns + Col;
			if (this->TileBoard.IsValidIndex(Index))
			{
				SpawnArea.Add(this->TileBoard[Index]);
			}
		}
	}

	return SpawnArea;
}


ATileBoard* ATileBoard::GetTileBoard(const FTileBoardSpawnParams& SpawnParams,
                                     const TArray<FConcreteTileInfo>& ConcreteTileInfos)
{
	ATileBoard* TileBoard = NewObject<ATileBoard>();
	TileBoard->InitTileBoard(SpawnParams, ConcreteTileInfos);
	return TileBoard;
}


