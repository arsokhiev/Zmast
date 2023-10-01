// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Player/ZMASTBaseCharacter.h"
#include "ZMASTPlayerCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class ZMAST_API AZMASTPlayerCharacter : public AZMASTBaseCharacter
{
	GENERATED_BODY()

public:
	AZMASTPlayerCharacter();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
	void MoveForward(float Scale);
	void MoveRight(float Scale);
	
};
