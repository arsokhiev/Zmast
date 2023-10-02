// ZMAST, All Rights Reserved


#include "Player/ZMASTBaseCharacter.h"
#include "ZMASTMovementComponent.h"
#include "Components/ZMASTHealthComponent.h"
#include "Components/ZMASTWeaponComponent.h"

AZMASTBaseCharacter::AZMASTBaseCharacter(const FObjectInitializer& ObjInit)
	: ACharacter(ObjInit.SetDefaultSubobjectClass<UZMASTMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	HealthComponent = CreateDefaultSubobject<UZMASTHealthComponent>("HealthComponent");
	WeaponComponent = CreateDefaultSubobject<UZMASTWeaponComponent>("WeaponComponent");
}

void AZMASTBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AZMASTBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AZMASTBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AZMASTBaseCharacter::IsRunning() const
{
	return false;
}

