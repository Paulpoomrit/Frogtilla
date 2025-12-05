// Copyright 2025 NFR, All rights reserved.


#include "RockTile.h"

FTileEffect ARockTile::GetTileEffectOnOverlap() const
{
	return FTileEffect{25, true, 0, 0, false,  true};
}
