// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZMASTPlayerController.generated.h"

UCLASS()
class ZMAST_API AZMASTPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AZMASTPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	
};
