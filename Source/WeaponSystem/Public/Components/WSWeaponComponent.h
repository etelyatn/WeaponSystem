// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WSWeaponComponent.generated.h"

class AWSPlayerController;
class AWSWeapon;

/** On weapon updates ammo */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponSystemUpdateAmmo, APawn*, Pawn, int32, CurrentAmmoInClip, int32, CurrentAmmo);
/** On Weapon start reload */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSystemStartReload, APawn*, Pawn, float, ReloadingTime);
/** On Equip new weapon */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnWeaponSystemEquipWeapon, APawn*, Pawn, AWSWeapon*, Weapon, float, EquipDuration);
/** On Un Equip weapon */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponSystemUnEquipWeapon, APawn*, Pawn, AWSWeapon*, Weapon);

/**
 * The weapon component must be attached to the actor where the weapon is to be used.
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class WEAPONSYSTEM_API UWSWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWSWeaponComponent();

	// AActor interface
	virtual void BeginPlay() override;

	/** get currently equipped weapon */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem|Weapon")
	AWSWeapon* GetWeapon() const;

	/** attaches weapon mesh to pawn's mesh */
	void AttachWeaponToPawn(AWSWeapon* InWeapon);

	/** detaches weapon mesh from pawn */
	void DetachWeaponFromPawn();

	/** get infinite ammo cheat */
	bool HasInfiniteAmmo() const;

	/** get infinite clip cheat */
	bool HasInfiniteClip() const;
	
	/** set infinite ammo cheat */
	void SetInfiniteAmmo(bool bEnable);

	/** set infinite clip cheat */
	void SetInfiniteClip(bool bEnable);

//----------------------------------------------------------------------------------------------------------------------
// Reading data
//----------------------------------------------------------------------------------------------------------------------

	/** notification when a pawn Equips a weapon. */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponSystemEquipWeapon NotifyEquipWeapon;
	
	/** notification when a pawn UnEquips a weapon. */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponSystemUnEquipWeapon NotifyUnEquipWeapon;
	
	/** notification when a weapon updates ammo. */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponSystemUpdateAmmo NotifyUpdateAmmo;
	
	/** notification when a weapon starts reloading. */
	UPROPERTY(BlueprintAssignable)
	FOnWeaponSystemStartReload NotifyStartReload;

//----------------------------------------------------------------------------------------------------------------------
// State
//----------------------------------------------------------------------------------------------------------------------

	/** get targeting state */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
    bool IsTargeting() const;

	/** set targeting state */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem")
	void SetIsTargeting(const bool NewState);
	
	/** is currently firing */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
    bool IsFiring() const;

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------

	/**
	* [server] add weapon to inventory
	*
	* @param Weapon	Weapon to add.
	*/
	void AddWeapon(class AWSWeapon* Weapon);
	
	/**
	* [server] Spawn and add weapon to inventory
	*
	* @param WeaponClass	Class of weapon to find.
	*/
	AWSWeapon* AddWeapon(TSubclassOf<AWSWeapon> WeaponClass);

	/**
	* [server] remove weapon from inventory
	*
	* @param Weapon	Weapon to remove.
	*/
	void RemoveWeapon(class AWSWeapon* Weapon);

	/**
	* Find in inventory
	*
	* @param WeaponClass	Class of weapon to find.
	*/
	class AWSWeapon* FindWeapon(TSubclassOf<class AWSWeapon> WeaponClass);

	/**
	* [server + local] equips weapon from inventory
	*
	* @param Weapon	Weapon to equip
	*/
	void EquipWeapon(class AWSWeapon* Weapon);

	/** equip next weapon from inventory */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	void NextWeapon();

	/** equip previous weapon from inventory */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	void PrevWeapon();

	/** get total number of inventory items */
	int32 GetInventoryCount() const;

	/**
	* get weapon from inventory at index. Index validity is not checked.
	*
	* @param Index Inventory index
	*/
	class AWSWeapon* GetInventoryWeapon(int32 Index) const;

//----------------------------------------------------------------------------------------------------------------------
// Weapon usage
//----------------------------------------------------------------------------------------------------------------------
	/** [local] starts weapon fire */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	virtual void StartWeaponFire();

	/** [local] stops weapon fire */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	virtual void StopWeaponFire();

	/** reload current weapon */
	UFUNCTION(BlueprintCallable, Category="WeaponSystem")
	virtual void Reload();

	/** check if pawn can fire weapon */
	virtual bool CanFire() { return true; }

	/** check if pawn can reload weapon */
	virtual bool CanReload() { return true; }

//----------------------------------------------------------------------------------------------------------------------
// Pawn
//----------------------------------------------------------------------------------------------------------------------

	/** play pawn weapon animations. returns duration */
	virtual float PlayPawnAnimation(UAnimMontage* AnimMontage, float InPlayRate = 1.f);

	/** stop pawn playing weapon animations. returns duration */
	virtual void StopPawnAnimation(UAnimMontage* AnimMontage, float InPlayRate = 1.f);

	/** get owning pawn */
	virtual APawn* GetPawn();

	/** get owning pawn mesh component */
	virtual UMeshComponent* GetPawnMesh();

	/** get current player controller */
	virtual APlayerController* GetPlayerController();

	/** checks is weapon owner locally controlled */
	virtual bool IsLocallyControlled();

	/** checks is camera shake enabled (for pawn, game mode etc.) */
	virtual bool IsCameraShakeEnabled() { return true; };

	/** checks is vibration enabled (for pawn, game mode etc.) */
	virtual bool IsVibrationEnabled() { return true; };

	/** play camera shake */
	virtual void PlayCameraShake(TSubclassOf<UCameraShakeBase> Shake, float Scale = 1.f);

	/** play client force feedback if allowed */
	virtual void PlayForceFeedback(class UForceFeedbackEffect* ForceFeedbackEffect, FForceFeedbackParameters Params = FForceFeedbackParameters());

	
protected:

	/** currently equipped weapon */
	UPROPERTY(BlueprintReadWrite, Transient, ReplicatedUsing=OnRep_CurrentWeapon, Category="WeaponSystem|Inventory")
	TObjectPtr<AWSWeapon> CurrentWeapon;

	/** default inventory list */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="WeaponSystem|Inventory")
	TArray<TSubclassOf<AWSWeapon>> DefaultInventoryClasses;

	/** weapons in inventory */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Transient, Replicated, Category="WeaponSystem|Inventory")
	TArray<TObjectPtr<AWSWeapon>> Inventory;

	/** socket or bone name for attaching weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "WeaponSystem|Config")
	FName WeaponAttachPoint;

	/** infinite ammo cheat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSystem|Config")
	bool bInfiniteAmmo;

	/** infinite clip cheat */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="WeaponSystem|Config")
	bool bInfiniteClip;

	/** current firing state */
	bool bWantsToFire;

	/** current targeting state */
	UPROPERTY(Transient, Replicated)
	bool bIsTargeting;

//----------------------------------------------------------------------------------------------------------------------
// Inventory
//----------------------------------------------------------------------------------------------------------------------
	
	/** updates current weapon */
	void SetCurrentWeapon(class AWSWeapon* NewWeapon, class AWSWeapon* LastWeapon = nullptr);

	/** current weapon rep handler */
	UFUNCTION()
    void OnRep_CurrentWeapon(class AWSWeapon* LastWeapon);

	/** [server] spawns default inventory */
	void SpawnDefaultInventory();

	/** [server] remove all weapons from inventory and destroy them */
	void DestroyInventory();

	/** equip weapon */
	UFUNCTION(reliable, server, WithValidation)
    void ServerEquipWeapon(class AWSWeapon* NewWeapon);
};
