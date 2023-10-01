// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZMASTWeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZMAST_API UZMASTWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UZMASTWeaponComponent();

protected:
	virtual void BeginPlay() override;
	
};
