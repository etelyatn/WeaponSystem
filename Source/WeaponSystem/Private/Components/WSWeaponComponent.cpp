// 2021 github.com/EugeneTel/WeaponSystem

#include "Components/WSWeaponComponent.h"
#include "WSWeapon.h"
#include "Net/UnrealNetwork.h"

UWSWeaponComponent::UWSWeaponComponent()
{

	// defaults
	bIsTargeting = false;
	bWantsToFire = false;
	SetIsReplicatedByDefault(true);
}

// Called when the game starts
void UWSWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		// Needs to happen after character is added to repgraph
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &UWSWeaponComponent::SpawnDefaultInventory);
	}
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

bool UWSWeaponComponent::IsTargeting() const
{
	return bIsTargeting;
}

void UWSWeaponComponent::SetIsTargeting(const bool NewState)
{
	bIsTargeting = NewState;
}

void UWSWeaponComponent::AddWeapon(AWSWeapon* Weapon)
{
	if (Weapon && GetPawn()->GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnEnterInventory(this);
		Inventory.AddUnique(Weapon);
	}
}

void UWSWeaponComponent::RemoveWeapon(AWSWeapon* Weapon)
{
	if (Weapon && GetPawn()->GetLocalRole() == ROLE_Authority)
	{
		Weapon->OnLeaveInventory();
		Inventory.RemoveSingle(Weapon);
	}
}

AWSWeapon* UWSWeaponComponent::FindWeapon(TSubclassOf<AWSWeapon> WeaponClass)
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] && Inventory[i]->IsA(WeaponClass))
		{
			return Inventory[i];
		}
	}

	return nullptr;
}

void UWSWeaponComponent::EquipWeapon(AWSWeapon* Weapon)
{
	if (Weapon)
	{
		if (GetPawn()->GetLocalRole() == ROLE_Authority)
		{
			SetCurrentWeapon(Weapon, CurrentWeapon);
		}
		else
		{
			ServerEquipWeapon(Weapon);
		}
	}
}

void UWSWeaponComponent::NextWeapon()
{
	if (Inventory.Num() >= 2 && (CurrentWeapon == nullptr || CurrentWeapon->GetCurrentState() != EWeaponState::EWS_Equipping))
	{
		const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
		AWSWeapon* NextWeapon = Inventory[(CurrentWeaponIdx + 1) % Inventory.Num()];
		EquipWeapon(NextWeapon);
	}
}

void UWSWeaponComponent::PrevWeapon()
{
	if (Inventory.Num() >= 2 && (CurrentWeapon == nullptr || CurrentWeapon->GetCurrentState() != EWeaponState::EWS_Equipping))
	{
		const int32 CurrentWeaponIdx = Inventory.IndexOfByKey(CurrentWeapon);
		AWSWeapon* PrevWeapon = Inventory[(CurrentWeaponIdx - 1 + Inventory.Num()) % Inventory.Num()];
		EquipWeapon(PrevWeapon);
	}
}

int32 UWSWeaponComponent::GetInventoryCount() const
{
	return Inventory.Num();
}

AWSWeapon* UWSWeaponComponent::GetInventoryWeapon(int32 Index) const
{
	return Inventory[Index];
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

void UWSWeaponComponent::Reload()
{
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
	return (GetPlayerController() && GetPlayerController()->IsLocalController());
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

void UWSWeaponComponent::SetCurrentWeapon(AWSWeapon* NewWeapon, AWSWeapon* LastWeapon)
{
	AWSWeapon* LocalLastWeapon = nullptr;

	if (LastWeapon != nullptr)
	{
		LocalLastWeapon = LastWeapon;
	}
	else if (NewWeapon != CurrentWeapon)
	{
		LocalLastWeapon = CurrentWeapon;
	}

	// unequip previous
	if (LocalLastWeapon)
	{
		LocalLastWeapon->OnUnEquip();
	}

	CurrentWeapon = NewWeapon;

	// equip new one
	if (NewWeapon)
	{
		// Make sure weapon's MyPawn is pointing back to us. During replication, we can't guarantee APawn::CurrentWeapon will rep after AWeapon::MyPawn!
		NewWeapon->SetOwningComponent(this);

		NewWeapon->OnEquip(LastWeapon);
	}
}

void UWSWeaponComponent::OnRep_CurrentWeapon(AWSWeapon* LastWeapon)
{
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void UWSWeaponComponent::SpawnDefaultInventory()
{
	if (GetPawn()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	const int32 NumWeaponClasses = DefaultInventoryClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (DefaultInventoryClasses[i])
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			AWSWeapon* NewWeapon = GetWorld()->SpawnActor<AWSWeapon>(DefaultInventoryClasses[i], SpawnInfo);
			AddWeapon(NewWeapon);
		}
	}

	// equip first weapon in inventory
	if (Inventory.Num() > 0)
	{
		EquipWeapon(Inventory[0]);
	}
}

void UWSWeaponComponent::DestroyInventory()
{
	if (GetPawn()->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// remove all weapons from inventory and destroy them
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		AWSWeapon* Weapon = Inventory[i];
		if (Weapon)
		{
			RemoveWeapon(Weapon);
			Weapon->Destroy();
		}
	}
}

void UWSWeaponComponent::ServerEquipWeapon_Implementation(AWSWeapon* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool UWSWeaponComponent::ServerEquipWeapon_Validate(AWSWeapon* NewWeapon)
{
	return true;
}

void UWSWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// only to local owner: weapon change requests are locally instigated, other clients don't need it
	DOREPLIFETIME_CONDITION(UWSWeaponComponent, Inventory, COND_OwnerOnly);

	// everyone except local owner: flag change is locally instigated
	DOREPLIFETIME_CONDITION(UWSWeaponComponent, bIsTargeting, COND_SkipOwner);

	// everyone
	DOREPLIFETIME(UWSWeaponComponent, CurrentWeapon);
}

