// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WSWeapon.generated.h"

class USoundCue;
class UWSWeaponComponent;

/**
* Weapon Ammo Types
*/ 
UENUM(BlueprintType, Category="WeaponSystem|Weapon")
enum class EAmmoType: uint8
{
	EAT_Bullet UMETA(DisplayName = "Bullet"),
    EAT_Rocket UMETA(DisplayName = "Rocket"),
    EAT_Laser UMETA(DisplayName = "Laser"),
    EAT_Other UMETA(DisplayName = "Other"),
};

/**
 *	Weapon State
 */
UENUM(BlueprintType, Category="WeaponSystem|Weapon")
enum class EWeaponState: uint8
{
	EWS_Idle UMETA(DisplayName = "Idle"),
	EWS_Firing UMETA(DisplayName = "Firing"),
	EWS_Reloading UMETA(DisplayName = "Reloading"),
	EWS_Equipping UMETA(DisplayName = "Equipping"),
};

/**
 * Weapon data
 */
USTRUCT(BlueprintType, Category="WeaponSystem|Weapon")
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

    /** infinite ammo for reloads */
    UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteClip;

	/** name of bone/socket for muzzle in weapon mesh */
	UPROPERTY(EditDefaultsOnly)
	FName MuzzleAttachPoint;

	/** ammo type for the weapon */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	EAmmoType AmmoType;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 InitialClips;

	/** time between two consecutive shots */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float TimeBetweenShots;

	/** failsafe reload duration if weapon doesn't have any animation for it */
	UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	float NoAnimReloadDuration;

	// /** animation played on pawn */
	// UPROPERTY(EditDefaultsOnly, Category=WeaponStat)
	// UAnimMontage* Animation;

	/** defaults */
	FWeaponData():
		bInfiniteAmmo(false),
		bInfiniteClip(false),
		MuzzleAttachPoint(FName("Muzzle")),
		AmmoType(EAmmoType::EAT_Bullet),
		MaxAmmo(100),
		AmmoPerClip(20),
		InitialClips(4),
		TimeBetweenShots(0.2f),
		NoAnimReloadDuration(1.0f)
		// Animation(nullptr)
	{
	}
};

/**
 * Base weapon class
 */
UCLASS(Abstract, Blueprintable)
class WEAPONSYSTEM_API AWSWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// ctor
	AWSWeapon();

	// AActor interface
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void Destroyed() override;

	/** get pawn weapon component */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Weapon")
	UWSWeaponComponent* GetWeaponComponent() const;

	/** get weapon mesh */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Weapon")
	USkeletalMeshComponent* GetWeaponMesh() const;

	/** detaches weapon mesh from owner */
	virtual void DetachMesh();

	/** attaches weapon mesh to owner */
	virtual void AttachMesh(USceneComponent* Parent, FName SocketName);

	/** set the weapon's owning component and pawn */
	void SetOwningComponent(UWSWeaponComponent* NewComponent);

//----------------------------------------------------------------------------------------------------------------------
// State
//----------------------------------------------------------------------------------------------------------------------
	
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	EWeaponState GetCurrentState();

//----------------------------------------------------------------------------------------------------------------------
// Input
//----------------------------------------------------------------------------------------------------------------------

	/** [local + server] start weapon fire */
	virtual void StartFire();

	/** [local + server] stop weapon fire */
	virtual void StopFire();

	/** [all] start weapon reload */
	virtual void StartReload(bool bFromReplication = false);

	/** [local + server] interrupt weapon reload */
	virtual void StopReload();

	/** [server] performs actual reload */
	virtual void ReloadWeapon();

	/** trigger reload from server */
	UFUNCTION(reliable, client)
    void ClientStartReload();

//----------------------------------------------------------------------------------------------------------------------
// Ammo
//----------------------------------------------------------------------------------------------------------------------
	
	/** add ammo */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	void GiveAmmo(int AddAmount);

	/** consume a bullet */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	void UseAmmo();

	/** query ammo type */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	virtual EAmmoType GetAmmoType();
	
	/** get current ammo amount (total) */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	int32 GetCurrentAmmo() const;

	/** get current ammo amount (clip) */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	int32 GetCurrentAmmoInClip() const;

	/** get clip size */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	int32 GetAmmoPerClip() const;

	/** get max ammo amount */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	int32 GetMaxAmmo() const;

	/** check if weapon has infinite ammo (include owner's cheats) */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	bool HasInfiniteAmmo() const;

	/** check if weapon has infinite clip (include owner's cheats) */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem|Ammo")
	bool HasInfiniteClip() const;

	/** check if weapon can be reloaded */
	bool CanReload() const;

protected:

//----------------------------------------------------------------------------------------------------------------------
// Configs
//----------------------------------------------------------------------------------------------------------------------
	/** weapon mesh  */
	UPROPERTY(VisibleDefaultsOnly, Category="WeaponSystem|Config")
	USkeletalMeshComponent* Mesh;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem")
	FWeaponData WeaponConfig;

	/** Whether to allow automatic weapons to catch up with shorter refire cycles */
	UPROPERTY(Config)
	bool bAllowAutomaticWeaponCatchup = true;

	/** The weapon component attached to a Pawn */
	UPROPERTY(BlueprintReadWrite, Transient, ReplicatedUsing=OnRep_WeaponComponent, Category="WeaponSystem|Weapon")
	UWSWeaponComponent* WeaponComponent;

//----------------------------------------------------------------------------------------------------------------------
// State
//----------------------------------------------------------------------------------------------------------------------
	/** current weapon state */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category="WeaponSystem|Weapon")
	EWeaponState CurrentState;
	
	/** current total ammo */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated, Category="WeaponSystem|Weapon")
	int32 CurrentAmmo;

	/** current ammo - inside clip */
	UPROPERTY(BlueprintReadWrite, Transient, Replicated, Category="WeaponSystem|Weapon")
	int32 CurrentAmmoInClip;

	/** is fire animation playing? */
	bool bPlayingFireAnim;

	/** is weapon currently equipped? */
	bool bIsEquipped;

	/** is weapon fire active? */
	bool bWantsToFire;

	/** is reload animation playing? */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Reload)
	bool bPendingReload;

	/** is equip animation playing? */
	bool bPendingEquip;

	/** weapon is refiring */
	bool bRefiring;

	/** burst counter, used for replicating fire events to remote clients */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_BurstCounter)
	int32 BurstCounter;

	/** time of last successful weapon fire */
	float LastFireTime;
	
	/** last time when this weapon was switched to */
	float EquipStartedTime;

	/** how much time weapon needs to be equipped */
	float EquipDuration;
	
	/** Adjustment to handle frame rate affecting actual timer interval. */
	UPROPERTY(Transient)
	float TimerIntervalAdjustment;
	
	/** Handle for efficient management of OnEquipFinished timer */
	FTimerHandle TimerHandle_OnEquipFinished;

	/** Handle for efficient management of StopReload timer */
	FTimerHandle TimerHandle_StopReload;

	/** Handle for efficient management of ReloadWeapon timer */
	FTimerHandle TimerHandle_ReloadWeapon;

	/** Handle for efficient management of HandleFiring timer */
	FTimerHandle TimerHandle_HandleFiring;

//----------------------------------------------------------------------------------------------------------------------
// Sound
//----------------------------------------------------------------------------------------------------------------------

	/** is fire sound looped? */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	uint32 bLoopedFireSound : 1;
	
	/** single fire sound (bLoopedFireSound not set) */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* FireSound;

	/** looped fire sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* FireLoopSound;

	/** finished burst sound (bLoopedFireSound set) */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* FireFinishSound;

	/** out of ammo sound */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* OutOfAmmoSound;

	/** reload sound */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* ReloadSound;

	/** equip sound */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Sound")
	USoundCue* EquipSound;

	/** firing audio (bLoopedFireSound set) */
	UPROPERTY(Transient)
	UAudioComponent* FireAC;

//----------------------------------------------------------------------------------------------------------------------
// Animation
//----------------------------------------------------------------------------------------------------------------------
	/** is fire animation looped? */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	uint32 bLoopedFireAnim : 1;

	/** weapon reload animations */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	UAnimMontage* WeaponReloadAnim;
	
	/** weapon fire animations */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	UAnimMontage* WeaponFireAnim;
	
	/** pawn reload animations */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	UAnimMontage* PawnReloadAnim;

	/** pawn equip animations */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	UAnimMontage* PawnEquipAnim;

	/** pawn fire animations */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Animation")
	UAnimMontage* PawnFireAnim;



//----------------------------------------------------------------------------------------------------------------------
// VFX
//----------------------------------------------------------------------------------------------------------------------

protected:

	/** is muzzle FX looped? */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|VFX")
	bool bLoopedMuzzleFX;

	/** FX for muzzle flash */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|VFX")
	UParticleSystem* MuzzleFX;

	/** spawned component for muzzle FX */
	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------
	/** camera shake on firing */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Effects")
	TSubclassOf<UMatineeCameraShake> FireCameraShake;

	/** force feedback effect to play when the weapon is fired */
	UPROPERTY(EditDefaultsOnly, Category="WeaponSystem|Effects")
	UForceFeedbackEffect *FireForceFeedback;

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
public:

	/** weapon is being equipped by owner pawn */
	virtual void OnEquip(const AWSWeapon* LastWeapon);

	/** weapon is now equipped by owner pawn */
	virtual void OnEquipFinished();

	/** weapon is holstered by owner pawn */
	virtual void OnUnEquip();

	/** [server] weapon was added to pawn's inventory */
	virtual void OnEnterInventory(UWSWeaponComponent* InWeaponComponent);

	/** [server] weapon was removed from pawn's inventory */
	virtual void OnLeaveInventory();

	/** check if it's currently equipped */
	bool IsEquipped() const;

	/** check if mesh is already attached */
	bool IsAttachedToPawn() const;

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage
//----------------------------------------------------------------------------------------------------------------------

	/** [local] weapon specific fire implementation */
	virtual void FireWeapon() PURE_VIRTUAL(AWSWeapon::FireWeapon,);
	
	/** check if weapon can fire */
	bool CanFire() const;

	/** [server] fire & update ammo */
	UFUNCTION(reliable, server, WithValidation)
    void ServerHandleFiring();

	/** [local + server] handle weapon refire, compensating for slack time if the timer can't sample fast enough */
	void HandleReFiring();

	/** [local + server] handle weapon fire */
	void HandleFiring();

	/** [local + server] firing started */
	virtual void OnBurstStarted();

	/** [local + server] firing finished */
	virtual void OnBurstFinished();
		
	/** update weapon state */
	void SetWeaponState(EWeaponState NewState);
	
	/** determine current weapon state */
	void DetermineWeaponState();


//----------------------------------------------------------------------------------------------------------------------
// Effects
//----------------------------------------------------------------------------------------------------------------------

	/** the cosmetic fx for firing */
	virtual void SimulateWeaponFire();

	/** stop cosmetic fx (e.g. for a looping shot). */
	virtual void StopSimulatingWeaponFire();

	/** play weapon sounds */
	virtual UAudioComponent* PlayWeaponSound(USoundCue* Sound);

	/** play weapon animations */
	void PlayWeaponAnimation(UAnimationAsset* AnimationToPlay, const bool bIsLoopedAnim = false);

//----------------------------------------------------------------------------------------------------------------------
// Input - server side
//----------------------------------------------------------------------------------------------------------------------
	UFUNCTION(reliable, server, WithValidation)
	void ServerStartFire();

	UFUNCTION(reliable, server, WithValidation)
    void ServerStopFire();

	UFUNCTION(reliable, server, WithValidation)
    void ServerStartReload();

	UFUNCTION(reliable, server, WithValidation)
    void ServerStopReload();
	

//----------------------------------------------------------------------------------------------------------------------
// Replication
//----------------------------------------------------------------------------------------------------------------------
	UFUNCTION()
	void OnRep_WeaponComponent();

	UFUNCTION()
    void OnRep_BurstCounter();

	UFUNCTION()
    void OnRep_Reload();

//----------------------------------------------------------------------------------------------------------------------
// Helpers
//----------------------------------------------------------------------------------------------------------------------

	/** Get the aim of the weapon, allowing for adjustments to be made by the weapon */
	virtual FVector GetAdjustedAim() const;

	/** get the originating location for damage */
	virtual FVector GetDamageStartLocation(const FVector& AimDir) const;

	/** get the muzzle location of the weapon */
	FVector GetMuzzleLocation() const;

	/** get direction of weapon's muzzle */
	FVector GetMuzzleDirection() const;

	/** find hit */
	FHitResult WeaponTrace(const FVector& TraceFrom, const FVector& TraceTo) const;
};
