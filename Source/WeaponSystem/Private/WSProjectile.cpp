// 2021 github.com/EugeneTel/WeaponSystem


#include "WSProjectile.h"

// Sets default values
AWSProjectile::AWSProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AWSProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWSProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

