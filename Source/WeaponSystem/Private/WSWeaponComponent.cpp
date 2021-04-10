// 2021 github.com/EugeneTel/WeaponSystem

#include "WSWeaponComponent.h"
#include "WSWeapon.h"

UWSWeaponComponent::UWSWeaponComponent()
{

}

// Called when the game starts
void UWSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

}

AWSWeapon* UWSWeaponComponent::GetWeapon() const
{
	return CurrentWeapon;
}

void UWSWeaponComponent::AttachWeaponToPawn(AWSWeapon* InWeapon)
{
	if (GetPawnMesh() && InWeapon && !WeaponAttachPoint.IsNone())
	{
		// Remove and hide current mesh
		DetachWeaponFromPawn();

		InWeapon->AttachMesh(GetPawnMesh(), WeaponAttachPoint);
	}
}

void UWSWeaponComponent::DetachWeaponFromPawn()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->DetachMesh();
	}
}

bool UWSWeaponComponent::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

bool UWSWeaponComponent::HasInfiniteClip() const
{
	return bInfiniteClip;
}

void UWSWeaponComponent::SetInfiniteAmmo(bool bEnable)
{
	bInfiniteAmmo = bEnable;
}

void UWSWeaponComponent::SetInfiniteClip(bool bEnable)
{
	bInfiniteClip = bEnable;
}

void UWSWeaponComponent::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}

void UWSWeaponComponent::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}

bool UWSWeaponComponent::IsTargeting() const
{
	return bIsTargeting;
}

bool UWSWeaponComponent::IsFiring() const
{
	return bWantsToFire;
}

float UWSWeaponComponent::PlayPawnAnimation(UAnimMontage* AnimMontage, float InPlayRate)
{
	float Duration = 0.0f;
	if (GetPawnMesh() && AnimMontage)
	{
		USkeletalMeshComponent* UseMesh = GetPawnMesh();
		if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
		{
			Duration = UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
		}
	}

	return Duration;
}

void UWSWeaponComponent::StopPawnAnimation(UAnimMontage* AnimMontage, float InPlayRate)
{
	if (GetPawnMesh() && AnimMontage)
	{
		USkeletalMeshComponent* UseMesh = GetPawnMesh();
		if (AnimMontage && UseMesh && UseMesh->AnimScriptInstance)
		{
			UseMesh->AnimScriptInstance->Montage_Play(AnimMontage, InPlayRate);
		}
	}
}

APawn* UWSWeaponComponent::GetPawn()
{
	return nullptr;
}

USkeletalMeshComponent* UWSWeaponComponent::GetPawnMesh()
{
	return nullptr;
}

APlayerController* UWSWeaponComponent::GetPlayerController()
{
	return nullptr;
}

bool UWSWeaponComponent::IsLocallyControlled()
{
	return GetPawn() ? GetPawn()->IsLocallyControlled() : true;
}

void UWSWeaponComponent::PlayCameraShake(TSubclassOf<UMatineeCameraShake> Shake, float Scale)
{
	APlayerController* PC = GetPlayerController();

	if (Shake && PC && IsCameraShakeEnabled() )
	{
		PC->ClientStartCameraShake(Shake, Scale);
	}
}

void UWSWeaponComponent::PlayForceFeedback(UForceFeedbackEffect* ForceFeedbackEffect, FForceFeedbackParameters Params)
{
	APlayerController* PC = GetPlayerController();

	if (ForceFeedbackEffect && PC && IsVibrationEnabled())
	{
		FForceFeedbackParameters FFParams;
		FFParams.Tag = "Weapon";
		PC->ClientPlayForceFeedback(ForceFeedbackEffect, FFParams);
	}
}

