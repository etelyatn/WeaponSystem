// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WSTypes.h"
#include "WSImpactEffect.generated.h"

class USoundCue;

/**
 * Spawnable effect for weapon hit impact - NOT replicated to clients
 * Each impact type should be defined as separate blueprint
 */
UCLASS(Abstract, Blueprintable)
class WEAPONSYSTEM_API AWSImpactEffect : public AActor
{
	GENERATED_BODY()

public:
	
	/** default impact FX used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TObjectPtr<UParticleSystem> DefaultFX;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> ConcreteFX;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> DirtFX;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> WaterFX;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> MetalFX;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> WoodFX;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> GlassFX;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> GrassFX;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category=Visual)
	TObjectPtr<UParticleSystem> FleshFX;

	/** default impact sound used when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	TObjectPtr<USoundCue> DefaultSound;

	/** impact FX on concrete */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> ConcreteSound;

	/** impact FX on dirt */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> DirtSound;

	/** impact FX on water */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> WaterSound;

	/** impact FX on metal */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> MetalSound;

	/** impact FX on wood */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> WoodSound;

	/** impact FX on glass */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> GlassSound;

	/** impact FX on grass */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> GrassSound;

	/** impact FX on flesh */
	UPROPERTY(EditDefaultsOnly, Category=Sound)
	TObjectPtr<USoundCue> FleshSound;

	/** default decal when material specific override doesn't exist */
	UPROPERTY(EditDefaultsOnly, Category=Defaults)
	struct FDecalData DefaultDecal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category=Surface)
	FHitResult SurfaceHit;

	AWSImpactEffect();

	/** spawn effect */
	virtual void PostInitializeComponents() override;

protected:

	/** get FX for material type */
	UParticleSystem* GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

	/** get sound for material type */
	USoundCue* GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const;

};
