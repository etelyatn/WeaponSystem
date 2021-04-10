// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "WSWeaponComponent.generated.h"

class AWSPlayerController;
class AWSWeapon;

UCLASS(meta=(BlueprintSpawnableComponent))
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
// Weapon usage
//----------------------------------------------------------------------------------------------------------------------
	/** [local] starts weapon fire */
	virtual void StartWeaponFire();

	/** [local] stops weapon fire */
	virtual void StopWeaponFire();

	/** check if pawn can fire weapon */
	virtual bool CanFire() { return true; }

	/** check if pawn can reload weapon */
	virtual bool CanReload() { return true; }
	
	/** get targeting state */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem|Weapon")
    bool IsTargeting() const;
	
	/** get targeting state */
	UFUNCTION(BlueprintCallable, Category = "WeaponSystem|Weapon")
    bool IsFiring() const;


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
	virtual USkeletalMeshComponent* GetPawnMesh();

	/** get current player controller */
	virtual APlayerController* GetPlayerController();

	/** checks is weapon owner locally controlled */
	virtual bool IsLocallyControlled();

	/** checks is camera shake enabled (for pawn, game mode etc.) */
	virtual bool IsCameraShakeEnabled() { return true; };

	/** checks is vibration enabled (for pawn, game mode etc.) */
	virtual bool IsVibrationEnabled() { return true; };

	/** play camera shake */
	virtual void PlayCameraShake(TSubclassOf<UMatineeCameraShake> Shake, float Scale = 1.f);

	/** play client force feedback if allowed */
	virtual void PlayForceFeedback(class UForceFeedbackEffect* ForceFeedbackEffect, FForceFeedbackParameters Params = FForceFeedbackParameters());

	
protected:

	/** currently equipped weapon */
	UPROPERTY(BlueprintReadWrite, Category="WeaponSystem|Inventory")
	AWSWeapon* CurrentWeapon;

	/** default inventory list */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="WeaponSystem|Inventory")
	TArray<TSubclassOf<AWSWeapon>> DefaultInventoryClasses;

	/** weapons in inventory */
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Transient, /*Replicated,*/ Category="WeaponSystem|Inventory")
	TArray<AWSWeapon*> Inventory;

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
	UPROPERTY(Transient/*, Replicated*/)
	bool bIsTargeting;
};
