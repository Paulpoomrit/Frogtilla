// Copyright 2025 NFR, All rights reserved.


#include "Tile.h"

#include "Components/BoxComponent.h"
#include "Frogtilla/Boat/BoatPawn.h"

namespace TileConst
{
	constexpr float TileExtent = 4096.0f;
}

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->TileCollisionBound = CreateDefaultSubobject<UBoxComponent>(TEXT("TileCollisionBound"));
	this->TileCollisionBound->SetBoxExtent(FVector(TileConst::TileExtent, TileConst::TileExtent, TileConst::TileExtent));
	this->TileCollisionBound->SetLineThickness(16);
	// Uncommented to visualize the tile boundary: this->TileCollisionBound->SetHiddenInGame(false);

	this->TileCollisionBound->SetGenerateOverlapEvents(true);
	this->TileCollisionBound->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	this->TileCollisionBound->SetCollisionResponseToAllChannels(ECR_Overlap);
	this->TileCollisionBound->OnComponentBeginOverlap.AddDynamic(this, &ATile::BeginOverlap);
	this->TileCollisionBound->OnComponentEndOverlap.AddDynamic(this, &ATile::EndOverlap);
	this->RootComponent = this->TileCollisionBound;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATile::SetTileCollisionBound(const FVector& TileCollisionBound)
{
	this->TileCollisionBound->SetBoxExtent(TileCollisionBound);
	this->RootComponent = this->TileCollisionBound;
}

void ATile::InitTile(const FTileSpawnParams& SpawnParams)
{
	this->SetTileOrigin(SpawnParams.TileOrigin);
	this->SetTileSize(SpawnParams.TileSize);
	this->SetTileColour(SpawnParams.TileColour);

	this->TileCollisionBound->SetBoxExtent(FVector(SpawnParams.TileSize));
}

void ATile::ApplyEffectTick()
{
	if(this->BoatPawn)
	{
		this->BoatPawn->ApplyTileEffect(GetTileEffectOnOverlap());
	}
}

void ATile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(ABoatPawn* Boat = Cast<ABoatPawn>(OtherActor))
	{
		if (Boat == this->BoatPawn) return;
		this->BoatPawn = Boat;
		HandleEventOnOverlap(Cast<ABoatPawn>(OtherActor));
	}
}

void ATile::EndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	ABoatPawn* Boat = Cast<ABoatPawn>(OtherActor);
	if(Boat && Boat == this->BoatPawn)
	{
		StopApplyingTileEffect();
	}
}

void ATile::StopApplyingTileEffect_Implementation()
{
	GetWorld()->GetTimerManager().ClearTimer(EffectTimerHandle);
	this->BoatPawn = nullptr;
} 

void ATile::StartApplyingTileEffect_Implementation(ABoatPawn* OverlappedBoat)
{
	if (!OverlappedBoat) return;
	this->BoatPawn = OverlappedBoat;
	const FTileEffect CurrentTileEffect = GetTileEffectOnOverlap();

	if(CurrentTileEffect.ApplyDamageOnce)
	{
		this->BoatPawn->ApplyTileEffect(CurrentTileEffect);
	} else if (CurrentTileEffect.DamageInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			EffectTimerHandle,
			this,
			&ATile::ApplyEffectTick,
			CurrentTileEffect.DamageInterval,
			true
			);
	}
}

void ATile::HandleEventOnOverlap_Implementation(ABoatPawn* OverlappedBoat)
{
	if (GetTileEffectOnOverlap().ShouldDeferApplyingDamage) return;
	StartApplyingTileEffect(OverlappedBoat);
}

