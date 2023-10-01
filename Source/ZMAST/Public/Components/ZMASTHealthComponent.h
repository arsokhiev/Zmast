// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZMASTHealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZMAST_API UZMASTHealthComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	

public:	
	UZMASTHealthComponent();

protected:
	virtual void BeginPlay() override;
};
