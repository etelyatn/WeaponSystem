// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "WSWeapon.h"
#include "WSWeapon_Projectile.generated.h"

class AWSProjectile;

USTRUCT(BlueprintType)
struct FProjectileWeaponData
{
	GENERATED_USTRUCT_BODY()

    /** projectile class */
    UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<AWSProjectile> ProjectileClass;

	/** life time */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	float ProjectileLife;

	/** damage at impact point */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	int32 ExplosionDamage;

	/** radius of damage */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float ExplosionRadius;

	/** type of damage */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	TSubclassOf<UDamageType> DamageType;

	/** defaults */
	FProjectileWeaponData():
		ProjectileLife(10.0f),
		ExplosionDamage(100.0f),
		ExplosionRadius(300.0f)
	{
	}
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class WEAPONSYSTEM_API AWSWeapon_Projectile : public AWSWeapon
{
	GENERATED_BODY()

public:

	// ctor
	AWSWeapon_Projectile();
	
	/** apply config on projectile */
	void ApplyWeaponConfig(FProjectileWeaponData& Data);

protected:

	/** weapon config */
	UPROPERTY(EditDefaultsOnly, Category=WeaponSystem)
	FProjectileWeaponData ProjectileConfig;

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage
//----------------------------------------------------------------------------------------------------------------------

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() override;

	/** spawn projectile on server */
	UFUNCTION(reliable, server, WithValidation)
    void ServerFireProjectile(FVector Origin, FVector_NetQuantizeNormal ShootDir);
	
};
