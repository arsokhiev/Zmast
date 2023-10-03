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
	void SetClampedViewPitch();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "-45", ClampMax = "-25"))
	float ViewPitchMin_ = -35;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pitch", meta = (ClampMin = "-20", ClampMax = "-10"))
	float ViewPitchMax_ = -13;

};
