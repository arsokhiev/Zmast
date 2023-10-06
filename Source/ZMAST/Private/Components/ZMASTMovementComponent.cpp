// ZMAST, All Rights Reserved


#include "Components/ZMASTMovementComponent.h"
#include "Player/ZMASTBaseCharacter.h"
#include "Components/ZMASTWeaponComponent.h"
#include "ZMAST/ZMASTUtils.h"

float UZMASTMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	AZMASTBaseCharacter *BaseCharacter = Cast<AZMASTBaseCharacter>(GetPawnOwner());

	const auto WeaponComponent = ZMASTUtils::GetZMASTPlayerComponent<UZMASTWeaponComponent>(BaseCharacter);
	
	if (BaseCharacter
		&& (WeaponComponent && WeaponComponent->IsArmed())
		|| (WeaponComponent && WeaponComponent->IsAnimInProgress())) return MaxSpeed * WalkWithGunModifier;
	
	return BaseCharacter && BaseCharacter->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}