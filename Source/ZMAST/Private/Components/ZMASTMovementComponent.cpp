// ZMAST, All Rights Reserved


#include "Components/ZMASTMovementComponent.h"
#include "Player/ZMASTBaseCharacter.h"
#include "Components/ZMASTWeaponComponent.h"
#include "Components/CapsuleComponent.h"
#include "ZMAST/Public/ZMASTUtils.h"
#include "ZMASTCoreTypes.h"
#include "ZMASTPlayerCharacter.h"
#include "Animations/AnimUtils.h"
#include "Animations/ZMASTClimbFinishedAnimNotify.h"
#include "Animations/ZMASTClimbStartedAnimNotify.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogZMASTMovementComponent, All, All)

bool UZMASTMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom &&
		CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

FVector UZMASTMovementComponent::GetClimbSurfaceNormal() const
{
	return CurrentClimbingNormal;
}

void UZMASTMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAnimation();

	AnimInstance = Cast<AZMASTPlayerCharacter>(GetCharacterOwner())->GetMesh()->GetAnimInstance();
	
	ClimbQueryParams.AddIgnoredActor(GetOwner());
}

void UZMASTMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	SweepAndStoreWallHits();
	
	//const auto Capsule = CharacterOwner->GetCapsuleComponent();
	//DrawDebugCapsule(GetWorld(), Capsule->GetComponentLocation(), Capsule->GetScaledCapsuleHalfHeight(), Capsule->GetScaledCapsuleRadius(), FQuat::Identity, FColor::Orange);

	if (bShouldBeStraightUp)
	{
		const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
		if (!PlayerCharacter || PlayerCharacter->GetMesh() != GetCharacterOwner()->GetMesh()) return;
	
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
	
		const auto InterpolatedRotation = UKismetMathLibrary::RInterpTo(UpdatedComponent->GetRelativeRotation(), StandRotation, DeltaTime, 10);
		UpdatedComponent->SetWorldRotation(InterpolatedRotation);
	}
}

float UZMASTMovementComponent::GetMaxSpeed() const
{
	if (IsClimbing()) return MaxClimbingSpeed;
	
	const float MaxSpeed = Super::GetMaxSpeed();
	AZMASTBaseCharacter *BaseCharacter = Cast<AZMASTBaseCharacter>(GetPawnOwner());

	const auto WeaponComponent = ZMASTUtils::GetZMASTPlayerComponent<UZMASTWeaponComponent>(BaseCharacter);
	
	if (BaseCharacter
		&& (WeaponComponent && WeaponComponent->IsArmed())
		|| (WeaponComponent && WeaponComponent->IsAnimInProgress())) return MaxSpeed * WalkWithGunModifier;
	
	return BaseCharacter && BaseCharacter->IsRunning() ? MaxSpeed * RunModifier : MaxSpeed;
}

float UZMASTMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

void UZMASTMovementComponent::TryClimbing()
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;
	
	if (CanStartClimbing())
	{
		PlayerCharacter->WantsToClimb(true);
	}
}

void UZMASTMovementComponent::CancelClimbing()
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;
	
	PlayerCharacter->WantsToClimb(false);
}

void UZMASTMovementComponent::InitAnimation()
{
	const auto ClimbStartedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTClimbStartedAnimNotify>(LedgeClimbMontage);
	const auto ClimbFinishedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTClimbFinishedAnimNotify>(LedgeClimbMontage);

	if (ClimbStartedAnimNotify && ClimbFinishedAnimNotify)
	{
		ClimbStartedAnimNotify->OnNotified.AddUObject(this, &UZMASTMovementComponent::ChangeShouldBeStraightUp);
		ClimbFinishedAnimNotify->OnNotified.AddUObject(this, &UZMASTMovementComponent::ChangeShouldBeStraightUp);
	}
}

void UZMASTMovementComponent::ChangeShouldBeStraightUp(USkeletalMeshComponent* MeshComp)
{
	bShouldBeStraightUp == false ? bShouldBeStraightUp = true : bShouldBeStraightUp = false;
}
bool UZMASTMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + 40/*ClimbingCollisionShrinkAmount*/ : BaseEyeHeight;
	
	const FVector Start = UpdatedComponent->GetComponentLocation() + UpdatedComponent->GetUpVector() * EyeHeightOffset;
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	DrawDebugLine(GetWorld(), Start, End, FColor::Orange, false);
	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UZMASTMovementComponent::IsFacingSurface(const float Steepness) const
{
	constexpr float BaseLength = 80;
	const float SteepnessMultiplier = 1 + (1 - Steepness) * 5;

	return EyeHeightTrace(BaseLength * SteepnessMultiplier);
}

bool UZMASTMovementComponent::CanStartClimbing()
{
	for (FHitResult& Hit : CurrentWallHits)
	{
		const FVector HorizontalNormal = Hit.Normal.GetSafeNormal2D();

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(Hit.Normal, HorizontalNormal);

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing
			&& !bIsCeiling && IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}
	return false;
}

void UZMASTMovementComponent::SweepAndStoreWallHits()
{
	const FCollisionShape CollisionShape = FCollisionShape::MakeCapsule(CollisionCapsuleRadius, CollisionCapsuleHalfHeight);
	
	const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20;

	// Avoid using the same Start/End location for a Sweep, as it doesn't trigger hits on Landscapes.
	const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
	const FVector End = Start + UpdatedComponent->GetForwardVector();

	TArray<FHitResult> Hits;
	const bool HitWall = GetWorld()->SweepMultiByChannel(Hits, Start, End, FQuat::Identity,
		  ECC_WorldStatic, CollisionShape, ClimbQueryParams);

	HitWall ? CurrentWallHits = Hits : CurrentWallHits.Reset();
}

void UZMASTMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;
	
	if (PlayerCharacter->WantsToClimb())
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UZMASTMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;

		//UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		//Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == CMOVE_Climbing;
	
	if (bWasClimbing)
	{
		bOrientRotationToMovement = true;

		//SetRotationToStand(GetCharacterOwner()->GetMesh());

		//UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		//Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);

		// After exiting climbing mode, reset velocity and acceleration
		StopMovementImmediately();
	}
	
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UZMASTMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::CMOVE_Climbing)
	{
		PhysClimbing(deltaTime, Iterations);
	}
	
	Super::PhysCustom(deltaTime, Iterations);
}

void UZMASTMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	ComputeSurfaceInfo();

	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	MoveAlongClimbingSurface(deltaTime);

	TryClimbUpLedge();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	SnapToClimbingSurface(deltaTime);
}

void UZMASTMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (CurrentWallHits.IsEmpty())
	{
		return;
	}

	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FCollisionShape CollisionSphere = FCollisionShape::MakeSphere(6);

	for (const FHitResult& WallHit : CurrentWallHits)
	{
		const FVector End = Start + (WallHit.ImpactPoint - Start).GetSafeNormal() * 120;
		
		FHitResult AssistHit;
		GetWorld()->SweepSingleByChannel(AssistHit, Start, End, FQuat::Identity,
										 ECC_WorldStatic, CollisionSphere, ClimbQueryParams);
		
		CurrentClimbingPosition += WallHit.ImpactPoint;
		CurrentClimbingNormal += WallHit.Normal;
	}

	CurrentClimbingPosition /= CurrentWallHits.Num();
	CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UZMASTMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	RestorePreAdditiveRootMotionVelocity();

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	ApplyRootMotionToVelocity(deltaTime);
}

bool UZMASTMovementComponent::ShouldStopClimbing() const
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return true;

	if (AnimInstance->Montage_IsPlaying(LedgeClimbMontage)) return false;
	
	return !PlayerCharacter->WantsToClimb() || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UZMASTMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return;

	PlayerCharacter->WantsToClimb(false);

	SetMovementMode(MOVE_Falling);
	StartNewPhysics(deltaTime, Iterations);
}

void UZMASTMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	if (Hit.Time < 1.f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UZMASTMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);

	constexpr bool bSweep = true;
	UpdatedComponent->MoveComponent(Offset * ClimbingSnapSpeed * deltaTime, Rotation, bSweep);
}

FQuat UZMASTMovementComponent::GetClimbingRotation(float deltaTime) const
{
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}
	
	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();
	const float RotationSpeed = ClimbingRotationSpeed * FMath::Max(1, Velocity.Length() / MaxClimbingSpeed);

	return FMath::QInterpTo(Current, Target, deltaTime, RotationSpeed);
}

bool UZMASTMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;
	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();

	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;

	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UZMASTMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;

	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryParams);
}

bool UZMASTMovementComponent::TryClimbUpLedge() const
{
	if (AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage))
	{
		return false;
	}

	const float UpSpeed = FVector::DotProduct(Velocity, UpdatedComponent->GetUpVector());
	const bool bIsMovingUp = UpSpeed >= MaxClimbingSpeed / 3;

	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		UE_LOG(LogZMASTMovementComponent, Error, TEXT("Reached the edge"));
		//SetRotationToStand(GetCharacterOwner()->GetMesh());
		
		const auto Duration = AnimInstance->Montage_Play(LedgeClimbMontage);
		//UE_LOG(LogZMASTMovementComponent, Display, TEXT("Anim Montage had been PLAYED: %f"), Duration);
		/*
		const auto Pawn = Cast<APawn>(GetOwner());
		const auto Controller = Pawn->GetController();

		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + ForwardDirection * 500, FColor::Green, false, 50, 0, 5);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		//Pawn->AddMovementInput(ForwardDirection, 1/);
		//Pawn->AddMovementInput(RightDirection, MovementVector.X);*/
		
		return true;
	}

	return false;
}

void UZMASTMovementComponent::SetRotationToStand(USkeletalMeshComponent* MeshComp) const
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter || PlayerCharacter->GetMesh() != MeshComp) return;
	
	const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
	UpdatedComponent->SetRelativeRotation(StandRotation);
	
	//const auto InterpolatedRotation = UKismetMathLibrary::RInterpTo(UpdatedComponent->GetRelativeRotation(), StandRotation, UpdatedComponent->GetComponentTickInterval(), 10.f);
	//UpdatedComponent->SetWorldRotation(InterpolatedRotation);
}

bool UZMASTMovementComponent::HasReachedEdge() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 1.5f;

	return !EyeHeightTrace(TraceDistance);
}

bool UZMASTMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
																	  ECC_WorldStatic, ClimbQueryParams);

	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UZMASTMovementComponent::CanMoveToLedgeClimbLocation() const
{
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	// Could use a property instead for fine-tuning.
	const FVector VerticalOffset = FVector::UpVector * 160.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 100.f;

	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	if (!IsLocationWalkable(CheckLocation))
	{
		return false;
	}

	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck,CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryParams);

	return !bBlocked;
}
