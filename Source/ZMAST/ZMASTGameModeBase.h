// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ZMASTGameModeBase.generated.h"

UCLASS()
class ZMAST_API AZMASTGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AZMASTGameModeBase();

	virtual void StartPlay() override;
};
