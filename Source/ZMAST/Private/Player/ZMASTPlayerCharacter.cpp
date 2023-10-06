// ZMAST, All Rights Reserved


#include "Player/ZMASTPlayerCharacter.h"
#include "AnimDataControllerActions.h"
#include "ZMASTPlayerController.h"
#include "Components/ZMASTSpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ZMAST/ZMASTUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerCharacter, All, All)

AZMASTPlayerCharacter::AZMASTPlayerCharacter(const FObjectInitializer& ObjInit) : Super(ObjInit)
{
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction (only controller rotates)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	GetCharacterMovement()->bAllowPhysicsRotationDuringAnimRootMotion = 1;

	SpringArmComponent = CreateDefaultSubobject<UZMASTSpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;
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
	
	ArmLengthTimelineProgress.BindDynamic(SpringArmComponent, &UZMASTSpringArmComponent::SetTargetArmLength);
	AimTimelineProgress.BindDynamic(SpringArmComponent, &UZMASTSpringArmComponent::SetTargetArmLength);

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

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AZMASTPlayerCharacter::EnableAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AZMASTPlayerCharacter::DisableAim);
	}
}

bool AZMASTPlayerCharacter::IsRunning() const
{
	return WantsToRun && !WeaponComponent->IsArmed() && !GetVelocity().IsZero();
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
	WeaponComponent->IsArmed() ? WeaponComponent->Disarm() : WeaponComponent->Arm();
}

void AZMASTPlayerCharacter::EnableAim(const FInputActionValue& Value)
{
	if (!WeaponComponent->IsArmed()) return;
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = true;

	if (SpringArmComponent->TargetArmLength == CurveAimShort->FloatCurve.GetFirstKey().Value)
	{
		AimTimeline.AddInterpFloat(CurveAimShort, AimTimelineProgress);
		AimTimeline.PlayFromStart();
	}
	else if (SpringArmComponent->TargetArmLength == CurveAimLong->FloatCurve.GetFirstKey().Value)
	{
		AimTimeline.AddInterpFloat(CurveAimLong, AimTimelineProgress);
		AimTimeline.PlayFromStart();
	}

	SpringArmComponent->SetFullViewPitch();
}

void AZMASTPlayerCharacter::DisableAim(const FInputActionValue& Value)
{
	if (!WeaponComponent->IsArmed()) return;
	
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	AimTimeline.AddInterpFloat(CurveAimShort, AimTimelineProgress);
	AimTimeline.ReverseFromEnd();
	
	SpringArmComponent->SetClampedViewPitch();
}