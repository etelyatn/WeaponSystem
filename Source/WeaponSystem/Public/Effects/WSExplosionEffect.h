// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "WSTypes.h"
#include "GameFramework/Actor.h"
#include "WSExplosionEffect.generated.h"

class UPointLightComponent;
class USoundCue;

/**
 * Spawnable effect for explosion - NOT replicated to clients
 * Each explosion type should be defined as separate blueprint
 */
UCLASS(Abstract, Blueprintable)
class WEAPONSYSTEM_API AWSExplosionEffect : public AActor
{
	GENERATED_BODY()
	
public:	

	/** explosion FX */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	UParticleSystem* ExplosionFX;

	/** how long keep explosion light on? */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	float ExplosionLightFadeOut;

	/** explosion sound */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	USoundCue* ExplosionSound;
	
	/** explosion decals */
	UPROPERTY(EditDefaultsOnly, Category=Effect)
	struct FDecalData Decal;

	/** surface data for spawning */
	UPROPERTY(BlueprintReadOnly, Category=Surface)
	FHitResult SurfaceHit;

	AWSExplosionEffect();

	/** update fading light */
	virtual void Tick(float DeltaSeconds) override;

	/** Returns ExplosionLight subobject **/
	FORCEINLINE UPointLightComponent* GetExplosionLight() const { return ExplosionLight; }

protected:
	
	/** spawn explosion */
	virtual void BeginPlay() override;

private:

	/** explosion light */
	UPROPERTY(VisibleDefaultsOnly, Category=Effect)
	UPointLightComponent* ExplosionLight;

	/** Point light component name */
	FName ExplosionLightComponentName;
};
