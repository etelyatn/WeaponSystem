// 2021 github.com/EugeneTel/WeaponSystem


#include "WSWeapon.h"

#include "WSWeaponComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
AWSWeapon::AWSWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AWSWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void AWSWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponConfig.InitialClips > 0)
	{
		CurrentAmmoInClip = WeaponConfig.AmmoPerClip;
		CurrentAmmo = WeaponConfig.AmmoPerClip * WeaponConfig.InitialClips;
	}

	DetachMesh();
}

void AWSWeapon::Destroyed()
{
	Super::Destroyed();

	StopSimulatingWeaponFire();
}

UWSWeaponComponent* AWSWeapon::GetWeaponComponent() const
{
	return WeaponComponent;
}

USkeletalMeshComponent* AWSWeapon::GetWeaponMesh() const
{
	return Mesh;
}

void AWSWeapon::DetachMesh()
{
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh->SetHiddenInGame(true);
}

void AWSWeapon::AttachMesh(USceneComponent* Parent, FName SocketName)
{
	Mesh->SetHiddenInGame(false);
	Mesh->AttachToComponent(Parent, FAttachmentTransformRules::KeepRelativeTransform, SocketName);
}

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------

void AWSWeapon::StartFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerStartFire();
	}

	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void AWSWeapon::StopFire()
{
	if ((GetLocalRole() < ROLE_Authority) && WeaponComponent && WeaponComponent->IsLocallyControlled())
	{
		ServerStopFire();
	}

	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

void AWSWeapon::StartReload(bool bFromReplication)
{
	if (!bFromReplication && GetLocalRole() < ROLE_Authority)
	{
		ServerStartReload();
	}

	if (bFromReplication || CanReload())
	{
		bPendingReload = true;
		DetermineWeaponState();

		float AnimDuration = WeaponComponent->PlayPawnAnimation(ReloadAnim);
		if (AnimDuration <= 0.0f)
		{
			AnimDuration = WeaponConfig.NoAnimReloadDuration;
		}

		GetWorldTimerManager().SetTimer(TimerHandle_StopReload, this, &AWSWeapon::StopReload, AnimDuration, false);
		if (GetLocalRole() == ROLE_Authority)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_ReloadWeapon, this, &AWSWeapon::ReloadWeapon, FMath::Max(0.1f, AnimDuration - 0.1f), false);
		}
		
		if (WeaponComponent && WeaponComponent->IsLocallyControlled())
		{
			PlayWeaponSound(ReloadSound);
		}
	}
}

void AWSWeapon::StopReload()
{
	if (CurrentState == EWeaponState::EWS_Reloading)
	{
		bPendingReload = false;
		DetermineWeaponState();
		WeaponComponent->StopPawnAnimation(ReloadAnim);
	}
}

void AWSWeapon::ReloadWeapon()
{
	int32 ClipDelta = FMath::Min(WeaponConfig.AmmoPerClip - CurrentAmmoInClip, CurrentAmmo - CurrentAmmoInClip);

	if (HasInfiniteClip())
	{
		ClipDelta = WeaponConfig.AmmoPerClip - CurrentAmmoInClip;
	}

	if (ClipDelta > 0)
	{
		CurrentAmmoInClip += ClipDelta;
	}

	if (HasInfiniteClip())
	{
		CurrentAmmo = FMath::Max(CurrentAmmoInClip, CurrentAmmo);
	}
}

void AWSWeapon::ClientStartReload_Implementation()
{
	StartReload();
}

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------

void AWSWeapon::GiveAmmo(int AddAmount)
{
	const int32 MissingAmmo = FMath::Max(0, WeaponConfig.MaxAmmo - CurrentAmmo);
	AddAmount = FMath::Min(AddAmount, MissingAmmo);
	CurrentAmmo += AddAmount;

	// TODO: Implement for AI 

	// start reload if clip was empty
	if (GetCurrentAmmoInClip() <= 0 &&
        CanReload() &&
        WeaponComponent &&
        WeaponComponent->GetWeapon() == this)
	{
		ClientStartReload();
	}
}

void AWSWeapon::UseAmmo()
{
	if (!HasInfiniteAmmo())
	{
		CurrentAmmoInClip--;
	}

	if (!HasInfiniteAmmo() && !HasInfiniteClip())
	{
		CurrentAmmo--;
	}

	// @TODO: AI actions
}

EAmmoType AWSWeapon::GetAmmoType()
{
	return WeaponConfig.AmmoType;
}

int32 AWSWeapon::GetCurrentAmmo() const
{
	return CurrentAmmo;
}

int32 AWSWeapon::GetCurrentAmmoInClip() const
{
	return CurrentAmmoInClip;
}

int32 AWSWeapon::GetAmmoPerClip() const
{
	return WeaponConfig.AmmoPerClip;
}

int32 AWSWeapon::GetMaxAmmo() const
{
	return WeaponConfig.MaxAmmo;
}

bool AWSWeapon::HasInfiniteAmmo() const
{
	return WeaponConfig.bInfiniteAmmo || (WeaponComponent && WeaponComponent->HasInfiniteAmmo());
}

bool AWSWeapon::HasInfiniteClip() const
{
	return WeaponConfig.bInfiniteClip || (WeaponComponent && WeaponComponent->HasInfiniteClip());
}

bool AWSWeapon::CanReload() const
{
	const bool bCanReload = (!WeaponComponent || WeaponComponent->CanReload());
	const bool bGotAmmo = ( CurrentAmmoInClip < WeaponConfig.AmmoPerClip) && (CurrentAmmo - CurrentAmmoInClip > 0 || HasInfiniteClip());
	const bool bStateOKToReload = ( ( CurrentState ==  EWeaponState::EWS_Idle ) || ( CurrentState == EWeaponState::EWS_Firing) );
	return ( ( bCanReload == true ) && ( bGotAmmo == true ) && ( bStateOKToReload == true) );
}

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------

void AWSWeapon::OnEquip(const AWSWeapon* LastWeapon)
{
	if (!WeaponComponent)
	{
		return;
	}

	// attach mesh to pawn
	WeaponComponent->AttachWeaponToPawn(this);
	
	bPendingEquip = true;
	DetermineWeaponState();

	// Only play animation if last weapon is valid
	if (LastWeapon)
	{
		float Duration = WeaponComponent->PlayPawnAnimation(EquipAnim);
		if (Duration <= 0.0f)
		{
			// failsafe
			Duration = 0.5f;
		}
		EquipStartedTime = GetWorld()->GetTimeSeconds();
		EquipDuration = Duration;

		GetWorldTimerManager().SetTimer(TimerHandle_OnEquipFinished, this, &AWSWeapon::OnEquipFinished, Duration, false);
	}
	else
	{
		OnEquipFinished();
	}

	if (WeaponComponent && WeaponComponent->IsLocallyControlled())
	{
		PlayWeaponSound(EquipSound);
	}
}

void AWSWeapon::OnEquipFinished()
{
	bIsEquipped = true;
	bPendingEquip = false;

	// Determine the state so that the can reload checks will work
	DetermineWeaponState(); 
	
	if (WeaponComponent)
	{
		WeaponComponent->AttachWeaponToPawn(this);
		
		// try to reload empty clip
		if (WeaponComponent->IsLocallyControlled() &&
            CurrentAmmoInClip <= 0 &&
            CanReload())
		{
			StartReload();
		}
	}
}

void AWSWeapon::OnUnEquip()
{
	DetachMesh();
	
	bIsEquipped = false;
	
	StopFire();

	if (bPendingReload)
	{
		WeaponComponent->StopPawnAnimation(ReloadAnim);
		bPendingReload = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_StopReload);
		GetWorldTimerManager().ClearTimer(TimerHandle_ReloadWeapon);
	}

	if (bPendingEquip)
	{
		WeaponComponent->StopPawnAnimation(EquipAnim);
		bPendingEquip = false;

		GetWorldTimerManager().ClearTimer(TimerHandle_OnEquipFinished);
	}

	DetermineWeaponState();
}

void AWSWeapon::OnEnterInventory(UWSWeaponComponent* InWeaponComponent)
{
	WeaponComponent = InWeaponComponent;
}

void AWSWeapon::OnLeaveInventory()
{
	if (IsAttachedToPawn())
	{
		OnUnEquip();
	}

	if (GetLocalRole() == ROLE_Authority)
	{
		WeaponComponent = nullptr;
	}
}

bool AWSWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool AWSWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

bool AWSWeapon::CanFire() const
{
	const bool bCanFire = WeaponComponent && WeaponComponent->CanFire();
	const bool bStateOKToFire = ( ( CurrentState ==  EWeaponState::EWS_Idle ) || ( CurrentState == EWeaponState::EWS_Firing) );	
	return (( bCanFire == true ) && ( bStateOKToFire == true ) && ( bPendingReload == false ));
}

void AWSWeapon::ServerHandleFiring_Implementation()
{
	const bool bShouldUpdateAmmo = (CurrentAmmoInClip > 0 && CanFire());

	HandleFiring();

	if (bShouldUpdateAmmo)
	{
		// update ammo
		UseAmmo();

		// update firing FX on remote clients
		BurstCounter++;
	}
}

bool AWSWeapon::ServerHandleFiring_Validate()
{
	return true;
}

void AWSWeapon::HandleReFiring()
{
	// Update TimerIntervalAdjustment
	UWorld* MyWorld = GetWorld();

	const float SlackTimeThisFrame = FMath::Max(0.0f, (MyWorld->TimeSeconds - LastFireTime) - WeaponConfig.TimeBetweenShots);

	if (bAllowAutomaticWeaponCatchup)
	{
		TimerIntervalAdjustment -= SlackTimeThisFrame;
	}

	HandleFiring();
}

void AWSWeapon::HandleFiring()
{
	if ((CurrentAmmoInClip > 0 || HasInfiniteClip() || HasInfiniteAmmo()) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (WeaponComponent && WeaponComponent->IsLocallyControlled())
		{
			FireWeapon();

			UseAmmo();
			
			// update firing FX on remote clients if function was called on server
			BurstCounter++;
		}
	}
	else if (CanReload())
	{
		StartReload();
	}
	else if (WeaponComponent && WeaponComponent->IsLocallyControlled())
	{
		if (GetCurrentAmmo() == 0 && !bRefiring)
		{
			PlayWeaponSound(OutOfAmmoSound);

			// @TODO: Notify out of ammo
		}
		
		// stop weapon fire FX, but stay in Firing state
		if (BurstCounter > 0)
		{
			OnBurstFinished();
		}
	}
	else
	{
		OnBurstFinished();
	}

	if (WeaponComponent && WeaponComponent->IsLocallyControlled())
	{
		// local client will notify server
		if (GetLocalRole() < ROLE_Authority)
		{
			ServerHandleFiring();
		}

		// reload after firing last round
		if (CurrentAmmoInClip <= 0 && CanReload())
		{
			StartReload();
		}

		// setup refire timer
		bRefiring = (CurrentState == EWeaponState::EWS_Firing && WeaponConfig.TimeBetweenShots > 0.0f);
		if (bRefiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWSWeapon::HandleReFiring, FMath::Max<float>(WeaponConfig.TimeBetweenShots + TimerIntervalAdjustment, SMALL_NUMBER), false);
			TimerIntervalAdjustment = 0.f;
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}

void AWSWeapon::OnBurstStarted()
{
	// start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && WeaponConfig.TimeBetweenShots > 0.0f &&
        LastFireTime + WeaponConfig.TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &AWSWeapon::HandleFiring, LastFireTime + WeaponConfig.TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void AWSWeapon::OnBurstFinished()
{
	// stop firing FX on remote clients
	BurstCounter = 0;

	// stop firing FX locally, unless it's a dedicated server
	//if (GetNetMode() != NM_DedicatedServer)
	//{
	StopSimulatingWeaponFire();
	//}
	
	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bRefiring = false;

	// reset firing interval adjustment
	TimerIntervalAdjustment = 0.0f;
}

void AWSWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentState;

	if (PrevState == EWeaponState::EWS_Firing && NewState != EWeaponState::EWS_Firing)
	{
		OnBurstFinished();
	}

	CurrentState = NewState;

	if (PrevState != EWeaponState::EWS_Firing && NewState == EWeaponState::EWS_Firing)
	{
		OnBurstStarted();
	}
}

void AWSWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::EWS_Idle;

	if (bIsEquipped)
	{
		if( bPendingReload  )
		{
			if( CanReload() == false )
			{
				NewState = CurrentState;
			}
			else
			{
				NewState = EWeaponState::EWS_Reloading;
			}
		}		
		else if ( (bPendingReload == false ) && ( bWantsToFire == true ) && ( CanFire() == true ))
		{
			NewState = EWeaponState::EWS_Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::EWS_Equipping;
	}

	SetWeaponState(NewState);
}


void AWSWeapon::SimulateWeaponFire()
{
	if (GetLocalRole() == ROLE_Authority && CurrentState != EWeaponState::EWS_Firing)
	{
		return;
	}

	if (MuzzleFX)
	{
		if (!bLoopedMuzzleFX || !MuzzlePSC)
		{
			MuzzlePSC = UGameplayStatics::SpawnEmitterAttached(MuzzleFX, GetWeaponMesh(), MuzzleAttachPoint);
			
		}
	}

	if (!bLoopedFireAnim || !bPlayingFireAnim)
	{
		WeaponComponent->PlayPawnAnimation(FireAnim);
		bPlayingFireAnim = true;
	}

	if (bLoopedFireSound)
	{
		if (FireAC)
		{
			FireAC = PlayWeaponSound(FireLoopSound);
		}
	}
	else
	{
		PlayWeaponSound(FireSound);
	}
	
	if (WeaponComponent && WeaponComponent->IsLocallyControlled())
	{
		if (FireCameraShake)
		{
			WeaponComponent->PlayCameraShake(FireCameraShake, 1);
		}
		if (FireForceFeedback)
		{
			WeaponComponent->PlayForceFeedback(FireForceFeedback);
		}
	}
}

void AWSWeapon::StopSimulatingWeaponFire()
{
	if (bLoopedMuzzleFX)
	{
		if(MuzzlePSC)
		{
			MuzzlePSC->DeactivateSystem();
			MuzzlePSC = nullptr;
		}
	}

	if (bLoopedFireAnim && bPlayingFireAnim && WeaponComponent)
	{
		WeaponComponent->StopPawnAnimation(FireAnim);
		bPlayingFireAnim = false;
	}

	if (FireAC)
	{
		FireAC->FadeOut(0.1f, 0.0f);
		FireAC = nullptr;

		PlayWeaponSound(FireFinishSound);
	}
}

UAudioComponent* AWSWeapon::PlayWeaponSound(USoundCue* Sound)
{
	UAudioComponent* AC = nullptr;
	if (Sound && WeaponComponent)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, Mesh);
	}

	return AC;
}

//----------------------------------------------------------------------------------------------------------------------
// Input - server side
//----------------------------------------------------------------------------------------------------------------------

void AWSWeapon::ServerStopFire_Implementation()
{
	StopFire();
}

bool AWSWeapon::ServerStopFire_Validate()
{
	return true;
}

void AWSWeapon::ServerStartFire_Implementation()
{
	StartFire();
}

bool AWSWeapon::ServerStartFire_Validate()
{
	return true;
}

void AWSWeapon::ServerStartReload_Implementation()
{
	StartReload();
}

bool AWSWeapon::ServerStartReload_Validate()
{
	return true;
}

bool AWSWeapon::ServerStopReload_Validate()
{
	return true;
}

void AWSWeapon::ServerStopReload_Implementation()
{
	StopReload();
}

//----------------------------------------------------------------------------------------------------------------------
// Replication
//----------------------------------------------------------------------------------------------------------------------

void AWSWeapon::OnRep_WeaponComponent()
{
	if (WeaponComponent)
	{
		OnEnterInventory(WeaponComponent);
	}
	else
	{
		OnLeaveInventory();
	}
}

void AWSWeapon::OnRep_BurstCounter()
{
	if (BurstCounter > 0)
	{
		SimulateWeaponFire();
	}
	else
	{
		StopSimulatingWeaponFire();
	}
}

void AWSWeapon::OnRep_Reload()
{
	if (bPendingReload)
	{
		StartReload(true);
	}
	else
	{
		StopReload();
	}
}

FVector AWSWeapon::GetAdjustedAim() const
{
	// @TODO
	return FVector::ZeroVector;
}

FVector AWSWeapon::GetDamageStartLocation(const FVector& AimDir) const
{
	return GetMuzzleLocation();
}

FVector AWSWeapon::GetMuzzleLocation() const
{
	return Mesh ? Mesh->GetSocketLocation(MuzzleAttachPoint) : FVector::ZeroVector;
}

FVector AWSWeapon::GetMuzzleDirection() const
{
	return Mesh ? Mesh->GetSocketRotation(MuzzleAttachPoint).Vector() : FVector::ZeroVector;
}

FHitResult AWSWeapon::WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, WeaponComponent->GetPawn());
	TraceParams.bReturnPhysicalMaterial = true;
	TraceParams.bDebugQuery = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceFrom, TraceTo, ECC_Visibility, TraceParams);

	return Hit;
}

