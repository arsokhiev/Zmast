// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Player/ZMASTBaseCharacter.h"
#include "Components/ZMASTWeaponComponent.h"
#include "Components/TimelineComponent.h"
#include "InputActionValue.h"
#include "ZMASTPlayerCharacter.generated.h"

class UZMASTMovementComponent;
class UCameraComponent;
class UZMASTSpringArmComponent;
class USphereComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ZMAST_API AZMASTPlayerCharacter : public AZMASTBaseCharacter
{
	GENERATED_BODY()

public:
	AZMASTPlayerCharacter(const FObjectInitializer& ObjInit);

	friend UZMASTWeaponComponent;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UZMASTMovementComponent* MovementComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UZMASTSpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USphereComponent* CameraCollisionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ClimbAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* RunAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MouseWheelAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* EquipWeaponAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AimAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UCurveFloat* CurveArmLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UCurveFloat* CurveAimShort;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UCurveFloat* CurveAimLong;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UCurveFloat* CurveStartShootFOV;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UCurveFloat* CurveCompleteShootFOV;
	
	virtual void BeginPlay() override;

public:
    UFUNCTION(BlueprintPure)
	UZMASTMovementComponent* GetZMASTMovementComponent() const { return MovementComponent; }

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UZMASTSpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }

	virtual bool IsRunning() const override;
	bool WantsToClimb() const;
	void WantsToClimb(bool Value);
	bool IsAiming() const;
	
	void StartShootFOVChange();
	void CancelShootFOVChange();
	void CompleteShootFOVChange();

private:
	FOnStartMoving OnStartMoving;
	
	FOnTimelineFloat ArmLengthTimelineProgress;
	FOnTimelineFloat AimTimelineProgress;
	FOnTimelineFloat ShootFOVTimelineProgress;
	FOnTimelineFloat CompleteShootFOVTimelineProgress;
	
	FTimeline ArmLengthTimeline;
	FTimeline AimTimeline;
	FTimeline ShootFOVTimeline;
	FTimeline CompleteShootFOVTimeline;
	
	bool bWantsToRun = false;
	bool bWantsToClimb = false;
	
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Climb(const FInputActionValue& Value);
	void Run(const FInputActionValue& Value);
	void ChangeSpringArmTargetLength(const FInputActionValue& Value);
	void ChangeWeaponState(const FInputActionValue& Value);

	void StartFire(const FInputActionValue& Value);
	void StopFire(const FInputActionValue& Value);
	
	void EnableAim(const FInputActionValue& Value);
	void DisableAim(const FInputActionValue& Value);

	UFUNCTION()
	void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									   const FHitResult& SweepResult);

	UFUNCTION()
	void OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void CheckCameraOverlap();
};
