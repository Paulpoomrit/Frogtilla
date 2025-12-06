# Frogtilla
> The main repository is hosted on Diversion and is being imported here for convenience.

**Game Title**: Frogtilla

**Team Name**: NFR (Norman Frogging Rockwell)

**Game Platform**: PC, Mac

**Genre**: Strategy, resource-management

**Game Engine**: Unreal Engine

**Game-Summary**:  HERE WE GO AGAIN. Evil frogs bully baby frogs; good frogs bring help via the Frogtilla.

**Similar Pieces/ Inspiration**:

- Dredge + Death Stranding

**Itch.io page**: [https://theapricotcocktail.itch.io/frogtilla](https://theapricotcocktail.itch.io/frogtilla)

---

## Gallery

https://github.com/user-attachments/assets/eee95837-7100-4b03-8c1a-78decb48486d

<img width="256" height="144" alt="Screenshot 2025-10-24 215209" src="https://github.com/user-attachments/assets/bfabb45c-39b5-44a3-95ef-5464ce14c007" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215529" src="https://github.com/user-attachments/assets/14b76270-b1ae-431e-94a5-7e0cd6af74bd" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215456" src="https://github.com/user-attachments/assets/07dd0f84-e17d-447d-bb3c-50c87a982f19" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215357" src="https://github.com/user-attachments/assets/7db89b02-3b77-40be-b6b0-17d8f1e23985" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215333" src="https://github.com/user-attachments/assets/ca95651c-6dd7-48be-88ca-f8dd675a44f0" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215239" src="https://github.com/user-attachments/assets/35d9757a-5e6b-4420-8019-45995b0abb6e" />
<img width="256" height="144" alt="Screenshot 2025-10-24 215710" src="https://github.com/user-attachments/assets/84d5fa4b-732a-49cb-8cf6-e1ad7579c248" />
<img width="256" height="144" alt="Screenshot 2025-10-24 220029" src="https://github.com/user-attachments/assets/5b38bdb7-519a-45aa-8a56-da52f6320126" />
<img width="256" height="144" alt="Screenshot 2025-10-24 220007" src="https://github.com/user-attachments/assets/6449b575-e1f7-4fd3-ae68-116c6da555aa" />

---

## Project Architecture

<img width="1360" height="1490" alt="image" src="https://github.com/user-attachments/assets/a7bd541f-e05b-4b8b-9428-9e0c62d049e8" />

---

## Implementation Details

(This is not entirely comprehensive, but should give enough understand of all basic components of the game).

### BP_LevelManager

The main logic component for the game loop, which would 

1. Initialize the Tileboard object
2. Initialize HUD
3. Initialize the player’s boat according to the ConfigInfo
4. Render all graphics components to the screen.

---

### `TileEffect` Struct

includes the information on how a particular tile should affect (logically) the state of the boat.

At this state, TileEffect includes the following:

- `DamageAmount` (float)
- `ApplyDamagaeOnce` = true (bool)
- `DamageInterval` [in sec] (float) → apply damage every … if `ApplyDamagaeOnce` is false.
- `SteeringChaosFactor` [0, inf) (int)
- `EndGameOnOverlap` = false (bool)

---

### `Tile`

is an abstract base class for all the tiles in the game.

It has the following attributes:

- `TileOrigin` starting top left (FVector)
- `TileSize` (width/height) (Float)
- `TileColour` (FColor)
- `TileCollisionBounds`

It has the following public interface (a pure virtual function):

- `GetTileEffectOnOverlap` (BoatState Struct).
- `EventOnBoatEnterdTile` (event dispatcher)
    - is being called by the boat every time the boat enters that particular tile.
    - intended to be used/overriden by the BP concrete tile classes to update its graphics.
- `EventOnDamageApplied` (event dispatcher)
    - is being called every time the boat is being damaged (once OR at an interval based on `DamageInterval`).

---

### `Tileboard` object and On `InitializeTileboard()`

- `Tileboard` represents the 2d board (with x and y coordinates) of tiles.
    - Has the following attributes:
        - TArray of tiles: TileBoardArray
        - A struct TileboardShapeInfo
    - Has the following methods:
        - InitializeTileboard
        - GetTileAt(row, column)
        - GetTileBoardArray
- The container we used is `TArray` of tiles.
- When initializing with `InitializeTileboard()` , we don’t want the tileboard object to know ahead of time what specific kinds of tile we will be spawning; instead, we’ll let the user type those types in at runtime (so we could pass in the bp types as well) → Use Factory Pattern
    
    ---
    

### `TileFactory` Class

First, we’ll have to make sure that all concrete tile class has a static (callback) function called `create` that would create a `new`  concrete tile and returns a Tile* (there’s probably a better way to do this, so that the program enforces that this function exists for all tiles, but I'm tired now lol).

- Attributes:
    - std::map of string and callback function (Tile ID and its `create` callback function).
- Methods
    - RegisterTileType(string type, CrateTileCallback)
    - UnregisterTileType(string type)
    - CreateTileInstance(string type, other info)
        - Every time the tile is being created, the function looks up the map, grabs the appropriate `create`, and returns a pointer to the concrete tile.
    
    ---
    

Now, back to `InitializeTileboard()` 

- Takes in the following params:
    - a struct of `TileboardShapeInfo`:
        - IndividualTileLength
        - TotalTileboardLength
        - StartingPOS
    - An array of pairs `<Fstring TileType, CallbackFunction Create>`
- Does the following:
    1. Loop over the array and register all of the tile types and their correspond callback functions.
    2. Loop over the number tiles to spawn and:
        1. Get a random tile type
        2. Spawn the tile
        3. Add to the TArray of tiles
    3. emit OnSpawnSuccess dispatcher.

---

### Boat-Tile Interaction

Originally, the idea was very simple: on event overlap, the boat would grab the TileEffect from the tile and then apply it to itself. Done.

Now the problem is sometimes, the tile effect should not be applied on overlap, but after some time (that should be determined by the tile). Also, sometimes the effect is applied not once but on an interval (e.g., the drone tile is applied damage once every 2 sec).

The original solution implies that there is a function call

`ApplyTileEffect` , which would take in the following TileEffect struct:

```cpp
USTRUCT(BlueprintType, meta = (ShortToolTip = "Contains info about how a tile should effect the boat pawn"))
struct FTileEffect
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageAmount= 25;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool ApplyDamageOnce = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageInterval = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SteeringChaosFactor = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool EndGameOnOverlap = false;	
};
```

and then manage all its internal properties within this function:

```cpp
void ABoatPawn::ApplyTileEffect_Implementation(const FTileEffect& TileEffect)
{
	if (TileEffect.EndGameOnOverlap)
	{
		EventOnEndLevel.Broadcast();
	}
	
	// Apply steering chaos factor
	const int BoatAngularDampingStep = 3; // step==3 gives us [0,18] damping from x in range [0,6]
	const int NewBoatDamp = (BoatConfig.SteeringPrecision - TileEffect.SteeringChaosFactor) * BoatAngularDampingStep;
	BoatMeshComponent->SetAngularDamping(FMath::Max(NewBoatDamp, 0));

	// Apply Damage
	this->Health = FMath::Max(this->Health-TileEffect.DamageAmount, 0);
	if (this->Health <= 0)
	{
		EventOnEndLevel.Broadcast();
	}
}
```

Now, the only modification we need here is that instead of calling `ApplyTileEffect` on the boat side, we construct an intermediate timer class which would:

- contain a reference to the boat itself
- has two public interfaces:
    - apply damage once
    - apply damage on an interval

Then we pass this timer to the tile class, then let it call one of these two functions whenever it deems necessary.

Note that the tile can affect the boat only if it is within the bounds:

- To add this restriction, we simply add an array of references to the timers inside the boat itself, construct and add a new timer when it overlaps with a tile, and remove it when the end overlaps with that particular tile.

---

### Boat Configuration

`Food`

- The amount of extra food to carry with the vessel.
- More food → More bonus → Increase Weight and Reduce Precision

`Fuel`

- The amount of extra fuel to carry with the vessel.
- More fuel → More time to reach the destination without running out.

`SteeringPrecision`

- The amount of control we have over the boat
- Longer grace period to rebalance and prevent sinking.
- Recommended Food to Precision ratio → 1:2

---

### `WBP_ConfigScreen`

- Pops up every time at the beginning of the game loop.
- Let the player adjust (buy) the following `BoatConfig`
    - Food
    - Fuel
    - SteeringCPrecision
- Public Interface:
    - Static constructor called `DoConfigScreen`
        - → intended to be called by the `BP_GamelManager` and would
        - → Takes in
            - the previous config from the previous game loop (if any)
        - → return the struct `ConfigInfo`

---

### `WBP_ConfigBar`

is a general widget blueprint representing each of the three `BoatConfig` 

- It can be thought of as a general array with n elements of `WBP_ConfigElement` (an individual watermelon, fuel, or steering precision) with an additional + and - button that would dynamically add to or remove from the element within an array.
- Upon construction, the user must specify the concrete WBP_ConfigElements alongside the n and the specific type.
- Has one public interface call `GetStat` That would return the number of elements in the array.

---
