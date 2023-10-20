// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ZMASTMovementComponent.generated.h"

class UAnimComposite;

UCLASS()
class ZMAST_API UZMASTMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "1.5", ClampMax = "10.0"))
	float RunModifier = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement", meta = (ClampMin = "0.4", ClampMax = "1"))
	float WalkWithGunModifier = 0.66f;

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;

	void TryClimbing();
	void CancelClimbing();

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character movement: Climbing")
	int CollisionCapsuleRadius = 50;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character movement: Climbing")
	int CollisionCapsuleHalfHeight = 72;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="1.0", ClampMax="75.0"))
	float MinHorizontalDegreesToStartClimbing = 25;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="0.0", ClampMax="80.0"))
	float ClimbingCollisionShrinkAmount = 30;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="10.0", ClampMax="500.0"))
	float MaxClimbingSpeed = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="10.0", ClampMax="2000.0"))
	float MaxClimbingAcceleration = 380.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="0.0", ClampMax="3000.0"))
	float BrakingDecelerationClimbing = 550.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="1.0", ClampMax="12.0"))
	int ClimbingRotationSpeed = 6;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="0.0", ClampMax="60.0"))
	float ClimbingSnapSpeed = 4.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="0.0", ClampMax="80.0"))
	float DistanceFromSurface = 45.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing", meta=(ClampMin="1.0", ClampMax="500.0"))
	float FloorCheckDistance = 100.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing")
	UAnimMontage* LedgeClimbMontage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Character Movement: Climbing")
	UAnimMontage* StandMontage;

	UPROPERTY()
	UAnimInstance* AnimInstance;
	
	TArray<FHitResult> CurrentWallHits;

	FCollisionQueryParams ClimbQueryParams;

private:
	bool bShouldBeStraightUp = false;
	void ChangeShouldBeStraightUp(USkeletalMeshComponent* MeshComp);
	
	void InitAnimation();
	
	bool EyeHeightTrace(const float TraceDistance) const;
	bool IsFacingSurface(const float Steepness) const;
	bool CanStartClimbing();
	bool ClimbDownToFloor() const;
	bool CheckFloor(FHitResult& FloorHit) const;
	bool TryClimbUpLedge() const;
	void SetRotationToStand(USkeletalMeshComponent* MeshComp) const;

	bool HasReachedEdge() const;
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	bool CanMoveToLedgeClimbLocation() const;
	
	void SweepAndStoreWallHits();

	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

	// Climb Physics
	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;
	
	void PhysClimbing(float deltaTime, int32 Iterations);
	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float deltaTime);
	bool ShouldStopClimbing() const;
	void StopClimbing(float deltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float deltaTime);
	void SnapToClimbingSurface(float deltaTime) const;
	
	FQuat GetClimbingRotation(float deltaTime) const;
};
