// Copyright 2025 NFR, All rights reserved.


#include "StormTile.h"

FTileEffect AStormTile::GetTileEffectOnOverlap() const
{
	return FTileEffect{0, true, 1, 10, false};
}
