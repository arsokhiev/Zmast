// ZMAST, All Rights Reserved


#include "Components/ZMASTMovementComponent.h"
#include "Player/ZMASTBaseCharacter.h"

float UZMASTMovementComponent::GetMaxSpeed() const
{
	const float MaxSpeed = Super::GetMaxSpeed();
	AZMASTBaseCharacter *BaseCharacter = Cast<AZMASTBaseCharacter>(GetPawnOwner());
	
	return BaseCharacter && BaseCharacter->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}