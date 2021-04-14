// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogWeaponSystem, Log, All);

// define weapon default collisions if not set
#ifndef COLLISION_WEAPON
	#define COLLISION_WEAPON	ECC_GameTraceChannel1
#endif
#ifndef COLLISION_PROJECTILE
	#define COLLISION_PROJECTILE	ECC_GameTraceChannel2
#endif

class FWeaponSystemModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
