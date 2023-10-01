// ZMAST, All Rights Reserved


#include "Player/ZMASTBaseCharacter.h"
#include "Components/ZMASTHealthComponent.h"
#include "Components/ZMASTWeaponComponent.h"

AZMASTBaseCharacter::AZMASTBaseCharacter()
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

