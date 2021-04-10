// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "WSWeaponComponent.h"
#include "WSCharacterWeaponComponent.generated.h"

/**
 * Weapon component for Character
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class WEAPONSYSTEM_API UWSCharacterWeaponComponent : public UWSWeaponComponent
{
	GENERATED_BODY()

public:

	virtual APawn* GetPawn() override;

	virtual USkeletalMeshComponent* GetPawnMesh() override;

	virtual APlayerController* GetPlayerController() override;
	
};
