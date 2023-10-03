// ZMAST, All Rights Reserved


#include "Player/ZMASTPlayerCharacter.h"
#include "ZMASTPlayerController.h"
#include "Components/ZMASTSpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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
	
	//GetCharacterMovement()->bConstrainToPlane = true;
	//GetCharacterMovement()->bSnapToPlaneAtStart = true;

	ZMASTSpringArmComponent = CreateDefaultSubobject<UZMASTSpringArmComponent>("SpringArmComponent");
	ZMASTSpringArmComponent->SetupAttachment(GetRootComponent());
	ZMASTSpringArmComponent->bUsePawnControlRotation = true;
	ZMASTSpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);
	ZMASTSpringArmComponent->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	ZMASTSpringArmComponent->SetRelativeRotation(FRotator(-19.f, 0.f, 0.f));
	ZMASTSpringArmComponent->TargetArmLength = 888.f;
	ZMASTSpringArmComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	ZMASTSpringArmComponent->bDoCollisionTest = true; // Don't want to pull camera in when it collides with level
	ZMASTSpringArmComponent->SetClampedViewPitch();

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(ZMASTSpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void AZMASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(CurveArmLength);
	OnArmLengthTimelineProgress.BindUFunction(ZMASTSpringArmComponent, FName("SetTargetArmLength"));

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
	}
}

bool AZMASTPlayerCharacter::IsRunning() const
{
	return WantsToRun && !GetVelocity().IsZero();
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
	ArmLengthTimeline.AddInterpFloat(CurveArmLength, OnArmLengthTimelineProgress);

	if (-Value.Get<float>() > 0)
	{
		if (ZMASTSpringArmComponent->TargetArmLength <= 888) ArmLengthTimeline.PlayFromStart();
	}
	else if (-Value.Get<float>() < 0)
	{
		if (ZMASTSpringArmComponent->TargetArmLength >= 1666) ArmLengthTimeline.ReverseFromEnd();
	}
}
