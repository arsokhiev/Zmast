// ZMAST, All Rights Reserved


#include "Components/ZMASTSpringArmComponent.h"
#include "ZMASTPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

void UZMASTSpringArmComponent::SetClampedViewPitch() const
{
	if (APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		PlayerCameraManager->ViewPitchMin = ClampedViewPitchMin;
		PlayerCameraManager->ViewPitchMax = ClampedViewPitchMax;
	}
}

void UZMASTSpringArmComponent::SetFullViewPitch() const
{
	if (APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		PlayerCameraManager->ViewPitchMin = AimViewPitchMin;
		PlayerCameraManager->ViewPitchMax = AimViewPitchMax;
	}
}
