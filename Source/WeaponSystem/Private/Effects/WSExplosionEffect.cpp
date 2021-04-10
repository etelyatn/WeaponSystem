// 2021 github.com/EugeneTel/WeaponSystem


#include "Effects/WSExplosionEffect.h"

// Sets default values
AWSExplosionEffect::AWSExplosionEffect()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWSExplosionEffect::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWSExplosionEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

