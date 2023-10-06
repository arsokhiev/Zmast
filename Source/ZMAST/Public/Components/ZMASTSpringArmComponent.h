// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "ZMASTSpringArmComponent.generated.h"

UCLASS()
class ZMAST_API UZMASTSpringArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:
	void SetClampedViewPitch() const;
	void SetFullViewPitch() const;

	float GetClampedViewPitchMin() const { return ClampedViewPitchMin; }
	float GetClampedViewPitchMax() const { return ClampedViewPitchMax; }
	float GetAimViewPitchMin() const { return AimViewPitchMin; }
	float GetAimViewPitchMax() const { return AimViewPitchMax; }

	UFUNCTION(BlueprintCallable, Category = "SpringArm")
	void SetTargetArmLength(float InTargetArmLength)
	{
		TargetArmLength = InTargetArmLength;
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "-45", ClampMax = "-25"))
	float ClampedViewPitchMin = -35;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "-20", ClampMax = "-10"))
	float ClampedViewPitchMax = -13;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "-55", ClampMax = "-35"))
	float AimViewPitchMin = -45;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "10", ClampMax = "35"))
	float AimViewPitchMax = 20;
};