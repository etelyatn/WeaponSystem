// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "WSTypes.generated.h"

#define WS_SURFACE_Default		SurfaceType_Default
#define WS_SURFACE_Concrete		SurfaceType1
#define WS_SURFACE_Dirt			SurfaceType2
#define WS_SURFACE_Water		SurfaceType3
#define WS_SURFACE_Metal		SurfaceType4
#define WS_SURFACE_Wood			SurfaceType5
#define WS_SURFACE_Grass		SurfaceType6
#define WS_SURFACE_Glass		SurfaceType7
#define WS_SURFACE_Flesh		SurfaceType8

/** keep in sync with WSImpactEffect */
UENUM(BlueprintType)
enum class EPhysMaterialType : uint8
{
	PMT_Unknown UMETA(DisplayName = "Bullet"),
	PMT_Concrete UMETA(DisplayName = "Concrete"),
	PMT_Dirt UMETA(DisplayName = "Dirt"),
	PMT_Water UMETA(DisplayName = "Water"),
	PMT_Metal UMETA(DisplayName = "Metal"),
	PMT_Wood UMETA(DisplayName = "Wood"),
	PMT_Grass UMETA(DisplayName = "Grass"),
	PMT_Glass UMETA(DisplayName = "Glass"),
	PMT_Flesh UMETA(DisplayName = "Flesh"),
};

USTRUCT()
struct FDecalData
{
	GENERATED_BODY()

	/** material */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	UMaterial* DecalMaterial;

	/** quad size (width & height) */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float DecalSize;

	/** lifespan */
	UPROPERTY(EditDefaultsOnly, Category=Decal)
	float LifeSpan;

	/** defaults */
	FDecalData()
		: DecalMaterial(nullptr)
		, DecalSize(256.f)
		, LifeSpan(10.f)
	{
	}
};

/** replicated information on a hit we've taken */
USTRUCT()
struct FTakeHitInfo
{
	GENERATED_BODY()

	/** The amount of damage actually applied */
	UPROPERTY()
	float ActualDamage;

	/** The damage type we were hit with. */
	UPROPERTY()
	UClass* DamageTypeClass;

	/** Who hit us */
	UPROPERTY()
	TWeakObjectPtr<AActor> PawnInstigator;

	/** Who actually caused the damage */
	UPROPERTY()
	TWeakObjectPtr<AActor> DamageCauser;

	/** Specifies which DamageEvent below describes the damage received. */
	UPROPERTY()
	int32 DamageEventClassID;

	/** Rather this was a kill */
	UPROPERTY()
	uint32 bKilled:1;

private:

	/** A rolling counter used to ensure the struct is dirty and will replicate. */
	UPROPERTY()
	uint8 EnsureReplicationByte;

	/** Describes general damage. */
	UPROPERTY()
	FDamageEvent GeneralDamageEvent;

	/** Describes point damage, if that is what was received. */
	UPROPERTY()
	FPointDamageEvent PointDamageEvent;

	/** Describes radial damage, if that is what was received. */
	UPROPERTY()
	FRadialDamageEvent RadialDamageEvent;

public:
	FTakeHitInfo();

	FDamageEvent& GetDamageEvent();
	void SetDamageEvent(const FDamageEvent& DamageEvent);
	void EnsureReplication();
};