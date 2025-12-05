// Copyright 2025 NFR, All rights reserved.


#include "DroneTile.h"

FTileEffect ADroneTile::GetTileEffectOnOverlap() const
{
	return FTileEffect{5, false, 1, 1, false};
}
