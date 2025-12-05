// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frogtilla/TileSystem/Tile.h"
#include "RockTile.generated.h"

/**
 * 
 */
UCLASS()
class FROGTILLA_API ARockTile : public ATile
{
	GENERATED_BODY()
public:
	virtual FTileEffect GetTileEffectOnOverlap() const override;
	
};
