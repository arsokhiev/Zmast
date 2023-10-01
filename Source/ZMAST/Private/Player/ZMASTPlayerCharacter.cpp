// ZMAST, All Rights Reserved


#include "Player/ZMASTPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AZMASTPlayerCharacter::AZMASTPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmComponent");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0.0f, 100.0f, 80.0f);
	SpringArmComponent->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	SpringArmComponent->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // хуй знает пока не понял
	SpringArmComponent->TargetArmLength = 800.f;
	SpringArmComponent->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

}

void AZMASTPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AZMASTPlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AZMASTPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AZMASTPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AZMASTPlayerCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp", this, &AZMASTPlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnAround", this, &AZMASTPlayerCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &AZMASTPlayerCharacter::Jump);

}

void AZMASTPlayerCharacter::MoveForward(float Scale)
{
	if (Scale == 0.0f) return;
	AddMovementInput(this->GetActorForwardVector(), Scale);
}

void AZMASTPlayerCharacter::MoveRight(float Scale)
{
	if (Scale == 0.0f) return;
	AddMovementInput(this->GetActorRightVector(), Scale);
}
