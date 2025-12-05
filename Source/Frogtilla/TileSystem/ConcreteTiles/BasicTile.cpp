// Copyright 2025 NFR, All rights reserved.


#include "BasicTile.h"

ABasicTile::ABasicTile() 
{

}

FTileEffect ABasicTile::GetTileEffectOnOverlap() const
{
	return FTileEffect{0, true, 0, 0, false};
}
