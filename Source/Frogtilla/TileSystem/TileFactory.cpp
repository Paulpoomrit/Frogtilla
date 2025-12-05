// Copyright 2025 NFR, All rights reserved.


#include "TileFactory.h"
#include "Tile.h"


TMap<FString, TileFactory::FCreateTileFunc>& TileFactory::GetRegistry()
{
	static TMap<FString, FCreateTileFunc> Registry;
	return Registry;
}

TileFactory::TileFactory()
{
}

TileFactory::~TileFactory()
{
}

void TileFactory::RegisterTileType(const FString& TileName, FCreateTileFunc CreateTileCallback)
{
	GetRegistry().Add(TileName, MoveTemp(CreateTileCallback));
}

void TileFactory::UnregisterTileType(const FString& TileName)
{
	GetRegistry().Remove(TileName);
}

ATile* TileFactory::CreateTileInstance(const FString& TileName, const FTileSpawnParams& SpawnParams)
{
	auto& Registry = GetRegistry();
	if (const FCreateTileFunc* FuncPtr = Registry.Find(TileName))
	{
		ATile* Tile = (*FuncPtr)();
		if (!Tile)
		{
			UE_LOG(LogTemp, Warning, TEXT("TileFactory: constructor for %s returned null"), *TileName);
		}
		Tile->InitTile(SpawnParams);
		return Tile;
	}
	UE_LOG(LogTemp, Warning, TEXT("TileFactory: no constructor registered for %s"), *TileName);
	return nullptr;
}


