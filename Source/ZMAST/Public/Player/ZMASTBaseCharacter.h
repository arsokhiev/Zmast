// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ZMASTBaseCharacter.generated.h"

class UZMASTWeaponComponent;
class UZMASTHealthComponent;

UCLASS()
class ZMAST_API AZMASTBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AZMASTBaseCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UZMASTHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UZMASTWeaponComponent* WeaponComponent;
	
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
