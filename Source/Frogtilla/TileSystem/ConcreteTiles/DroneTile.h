// Copyright 2025 NFR, All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Frogtilla/TileSystem/Tile.h"
#include "DroneTile.generated.h"

/**
 * 
 */
UCLASS()
class FROGTILLA_API ADroneTile : public ATile
{
	GENERATED_BODY()
	
public:
	virtual FTileEffect GetTileEffectOnOverlap() const override;
	
	
};
