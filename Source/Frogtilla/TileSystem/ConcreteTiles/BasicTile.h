// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frogtilla/TileSystem/Tile.h"
#include "BasicTile.generated.h"

/**
 * 
 */
UCLASS()
class FROGTILLA_API ABasicTile : public ATile
{
	GENERATED_BODY()
	ABasicTile();
public:
	virtual FTileEffect GetTileEffectOnOverlap() const override;
	
};
