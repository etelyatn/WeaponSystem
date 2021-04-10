// 2021 github.com/EugeneTel/WeaponSystem

#include "Components/WSCharacterWeaponComponent.h"
#include "GameFramework/Character.h"

APawn* UWSCharacterWeaponComponent::GetPawn()
{
	return Cast<APawn>(GetOwner());
}

USkeletalMeshComponent* UWSCharacterWeaponComponent::GetPawnMesh()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		return Character->GetMesh();
	}

	return nullptr;
}

APlayerController* UWSCharacterWeaponComponent::GetPlayerController()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character)
	{
		return Cast<APlayerController>(Character->GetController());
	}

	return nullptr;
}
