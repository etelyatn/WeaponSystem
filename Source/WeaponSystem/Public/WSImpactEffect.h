// 2021 github.com/EugeneTel/WeaponSystem

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WSImpactEffect.generated.h"

UCLASS()
class WEAPONSYSTEM_API AWSImpactEffect : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWSImpactEffect();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
