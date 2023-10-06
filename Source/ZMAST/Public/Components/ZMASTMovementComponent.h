// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZMASTMovementComponent.generated.h"

UCLASS()
class ZMAST_API UZMASTMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.5", ClampMax = "10.0"))
	float RunModifier = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.4", ClampMax = "1"))
	float WalkWithGunModifier = 0.66f;

	virtual float GetMaxSpeed() const override;
};
