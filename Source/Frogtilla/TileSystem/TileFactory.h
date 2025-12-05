// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"

struct FTileSpawnParams;
class ATile;
/**
 * 
 */


class FROGTILLA_API TileFactory final
{
	using FCreateTileFunc = TFunction<ATile*()>;
public:
	static void RegisterTileType(const FString& TileName, FCreateTileFunc CreateTileCallback);
	static void UnregisterTileType(const FString& TileName);
	static ATile* CreateTileInstance(const FString& TileName, const FTileSpawnParams& SpawnParams);

private:
	TileFactory();
	~TileFactory();
	static TMap<FString, FCreateTileFunc>& GetRegistry();
};