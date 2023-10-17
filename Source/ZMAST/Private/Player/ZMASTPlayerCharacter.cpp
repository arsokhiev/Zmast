// ZMAST, All Rights Reserved


#include "Player/ZMASTPlayerCharacter.h"
#include "AnimDataControllerActions.h"
#include "ZMASTPlayerController.h"
#include "Components/ZMASTSpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/ZMASTMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "ZMASTCoreTypes.h"
#include "ZMASTMovementComponent.h"
#include "ZMAST/Public/ZMASTUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, All, All)

AZMASTPlayerCharacter::AZMASTPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction (only controller rotates)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	MovementComponent = Cast<UZMASTMovementComponent>(GetCharacterMovement());
	MovementComponent->bOrientRotationToMovement = true; // Rotate character to moving direction
	MovementComponent->RotationRate = FRotator(0.f, 400.f, 0.f);
	MovementComponent->bAllowPhysicsRotationDuringAnimRootMotion = 1;

	SpringArmComponent = CreateDefaultSubobject<UZMASTSpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SocketOffset = FVector(0.0f, 80.0f, 60.0f);
	SpringArmComponent->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	SpringArmComponent->SetRelativeRotation(FRotator(-19.f, 0.f, 0.f));
	SpringArmComponent->TargetArmLength = 888.f;
	SpringArmComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	SpringArmComponent->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level
	SpringArmComponent->SetClampedViewPitch();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void AZMASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(CurveArmLength);
	check(CurveAimShort);
	check(CurveAimLong);
	check(CurveStartShootFOV);

	//CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AZMASTPlayerCharacter::OnCameraCollisionBeginOverlap);
	//CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &AZMASTPlayerCharacter::OnCameraCollisionEndOverlap);
	
	ArmLengthTimelineProgress.BindDynamic(SpringArmComponent, &UZMASTSpringArmComponent::SetTargetArmLength);
	AimTimelineProgress.BindDynamic(SpringArmComponent, &UZMASTSpringArmComponent::SetTargetArmLength);
	ShootFOVTimelineProgress.BindDynamic(CameraComponent, &UCameraComponent::SetFieldOfView);
	CompleteShootFOVTimelineProgress.BindDynamic(CameraComponent, &UCameraComponent::SetFieldOfView);

	OnStartMoving.AddUObject(WeaponComponent, &UZMASTWeaponComponent::StopFire);

	if (AZMASTPlayerController* ZMASTPlayerController = Cast<AZMASTPlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(ZMASTPlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

void AZMASTPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	ArmLengthTimeline.TickTimeline(DeltaSeconds);
	AimTimeline.TickTimeline(DeltaSeconds);
	ShootFOVTimeline.TickTimeline(DeltaSeconds);
	CompleteShootFOVTimeline.TickTimeline(DeltaSeconds);
}

void AZMASTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AZMASTPlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AZMASTPlayerCharacter::Look);
		
		// No jump
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Started, this, &AZMASTPlayerCharacter::Run);
		EnhancedInputComponent->BindAction(RunAction, ETriggerEvent::Completed, this, &AZMASTPlayerCharacter::Run);

		EnhancedInputComponent->BindAction(MouseWheelAction, ETriggerEvent::Triggered, this, &AZMASTPlayerCharacter::ChangeSpringArmTargetLength);

		EnhancedInputComponent->BindAction(EquipWeaponAction, ETriggerEvent::Triggered, this, &AZMASTPlayerCharacter::ChangeWeaponState);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AZMASTPlayerCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AZMASTPlayerCharacter::StopFire);
		
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AZMASTPlayerCharacter::EnableAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AZMASTPlayerCharacter::DisableAim);
	}
}

bool AZMASTPlayerCharacter::IsRunning() const
{
	return WantsToRun && !WeaponComponent->IsArmed() && !GetVelocity().IsZero();
}

bool AZMASTPlayerCharacter::IsAiming() const
{
	return SpringArmComponent->TargetArmLength == CurveAimShort->FloatCurve.GetLastKey().Value;
}

void AZMASTPlayerCharacter::StartShootFOVChange()
{
	//UE_LOG(LogPlayerCharacter, Display, TEXT("%f - %f"), CameraComponent->FieldOfView, CurveStartShootFOV->FloatCurve.GetLastKey().Value);
	if (CameraComponent->FieldOfView == CurveStartShootFOV->FloatCurve.GetFirstKey().Value)
	{
		ShootFOVTimeline.AddInterpFloat(CurveStartShootFOV, ShootFOVTimelineProgress);
		ShootFOVTimeline.PlayFromStart();
	}
	else
	{
		ShootFOVTimeline.AddInterpFloat(CurveStartShootFOV, ShootFOVTimelineProgress);
		ShootFOVTimeline.Play();
	}
}

void AZMASTPlayerCharacter::CancelShootFOVChange()
{
	ShootFOVTimeline.Reverse();
}

void AZMASTPlayerCharacter::CompleteShootFOVChange()
{
	CompleteShootFOVTimeline.AddInterpFloat(CurveCompleteShootFOV, CompleteShootFOVTimelineProgress);
	CompleteShootFOVTimeline.PlayFromStart();
}

void AZMASTPlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2d>();

	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}

	OnStartMoving.Broadcast();
}

void AZMASTPlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(-LookAxisVector.Y);
	}
}

void AZMASTPlayerCharacter::Run(const FInputActionValue& Value)
{
	WantsToRun = Value.Get<bool>();
}

void AZMASTPlayerCharacter::ChangeSpringArmTargetLength(const FInputActionValue& Value)
{
	ArmLengthTimeline.AddInterpFloat(CurveArmLength, ArmLengthTimelineProgress);
	
	if (-Value.Get<float>() > 0)
	{
		if (SpringArmComponent->TargetArmLength <= CurveArmLength->FloatCurve.GetFirstKey().Value)
		{
			ArmLengthTimeline.PlayFromStart();
		}
	}
	else if (-Value.Get<float>() < 0)
	{
		if (SpringArmComponent->TargetArmLength >= CurveArmLength->FloatCurve.GetLastKey().Value)
		{
			ArmLengthTimeline.ReverseFromEnd();
		}
	}
}

void AZMASTPlayerCharacter::ChangeWeaponState(const FInputActionValue& Value)
{
	WeaponComponent->ChangeWeaponState();
}

void AZMASTPlayerCharacter::StartFire(const FInputActionValue& Value)
{
	WeaponComponent->StartFire();
}

void AZMASTPlayerCharacter::StopFire(const FInputActionValue& Value)
{
	WeaponComponent->StopFire();
}

void AZMASTPlayerCharacter::EnableAim(const FInputActionValue& Value)
{
	if (!WeaponComponent->IsArmed()) return;
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	if (SpringArmComponent->TargetArmLength == CurveAimShort->FloatCurve.GetFirstKey().Value)
	{
		AimTimeline.AddInterpFloat(CurveAimShort, AimTimelineProgress);
		AimTimeline.Play();
	}
	else if (SpringArmComponent->TargetArmLength == CurveAimLong->FloatCurve.GetFirstKey().Value)
	{
		AimTimeline.AddInterpFloat(CurveAimLong, AimTimelineProgress);
		AimTimeline.Play();
	}

	SpringArmComponent->SetFullViewPitch();
}

void AZMASTPlayerCharacter::DisableAim(const FInputActionValue& Value)
{
	if (!WeaponComponent->IsArmed()) return;
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	AimTimeline.AddInterpFloat(CurveAimShort, AimTimelineProgress);
	AimTimeline.Reverse();
	
	SpringArmComponent->SetClampedViewPitch();
}

void AZMASTPlayerCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckCameraOverlap();
}

void AZMASTPlayerCharacter::OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	CheckCameraOverlap();
}

void AZMASTPlayerCharacter::CheckCameraOverlap()
{
	const auto bComponentsOverlap = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
	GetMesh()->SetOwnerNoSee(bComponentsOverlap);

	TArray<USceneComponent*> MeshChildren;
	GetMesh()->GetChildrenComponents(true, MeshChildren);

	for (auto MeshChild : MeshChildren)
	{
		const auto PrimitiveMeshChild = Cast<UPrimitiveComponent>(MeshChild);
		if (PrimitiveMeshChild)
		{
			PrimitiveMeshChild->SetOwnerNoSee(bComponentsOverlap);
		}
	}
}
