// 2021 github.com/EugeneTel/WeaponSystem


#include "Effects/WSImpactEffect.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "Sound/SoundCue.h"

AWSImpactEffect::AWSImpactEffect()
{
	SetAutoDestroyWhenFinished(true);
}

void AWSImpactEffect::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UPhysicalMaterial* HitPhysMat = SurfaceHit.PhysMaterial.Get();
	const EPhysicalSurface HitSurfaceType = UPhysicalMaterial::DetermineSurfaceType(HitPhysMat);

	// show particles
	UParticleSystem* ImpactFX = GetImpactFX(HitSurfaceType);
	if (ImpactFX)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, ImpactFX, GetActorLocation(), GetActorRotation());
	}

	// play sound
	USoundCue* ImpactSound = GetImpactSound(HitSurfaceType);
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}

	if (DefaultDecal.DecalMaterial)
	{
		FRotator RandomDecalRotation = SurfaceHit.ImpactNormal.Rotation();
		RandomDecalRotation.Roll = FMath::FRandRange(-180.0f, 180.0f);

		UGameplayStatics::SpawnDecalAttached(DefaultDecal.DecalMaterial, FVector(1.0f, DefaultDecal.DecalSize, DefaultDecal.DecalSize),
			SurfaceHit.Component.Get(), SurfaceHit.BoneName,
			SurfaceHit.ImpactPoint, RandomDecalRotation, EAttachLocation::KeepWorldPosition,
			DefaultDecal.LifeSpan);
	}
}

UParticleSystem* AWSImpactEffect::GetImpactFX(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	UParticleSystem* ImpactFX = nullptr;

	switch (SurfaceType)
	{
		case WS_SURFACE_Concrete:	ImpactFX = ConcreteFX; break;
		case WS_SURFACE_Dirt:		ImpactFX = DirtFX; break;
		case WS_SURFACE_Water:		ImpactFX = WaterFX; break;
		case WS_SURFACE_Metal:		ImpactFX = MetalFX; break;
		case WS_SURFACE_Wood:		ImpactFX = WoodFX; break;
		case WS_SURFACE_Grass:		ImpactFX = GrassFX; break;
		case WS_SURFACE_Glass:		ImpactFX = GlassFX; break;
		case WS_SURFACE_Flesh:		ImpactFX = FleshFX; break;
		default:					ImpactFX = DefaultFX; break;
	}

	return ImpactFX;
}

USoundCue* AWSImpactEffect::GetImpactSound(TEnumAsByte<EPhysicalSurface> SurfaceType) const
{
	USoundCue* ImpactSound = nullptr;

	switch (SurfaceType)
	{
		case WS_SURFACE_Concrete:	ImpactSound = ConcreteSound; break;
		case WS_SURFACE_Dirt:		ImpactSound = DirtSound; break;
		case WS_SURFACE_Water:		ImpactSound = WaterSound; break;
		case WS_SURFACE_Metal:		ImpactSound = MetalSound; break;
		case WS_SURFACE_Wood:		ImpactSound = WoodSound; break;
		case WS_SURFACE_Grass:		ImpactSound = GrassSound; break;
		case WS_SURFACE_Glass:		ImpactSound = GlassSound; break;
		case WS_SURFACE_Flesh:		ImpactSound = FleshSound; break;
		default:					ImpactSound = DefaultSound; break;
	}

	return ImpactSound;
}

