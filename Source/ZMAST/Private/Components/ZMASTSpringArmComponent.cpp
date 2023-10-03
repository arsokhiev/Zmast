// ZMAST, All Rights Reserved


#include "Components/ZMASTSpringArmComponent.h"
#include "ZMASTPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/PlayerCameraManager.h"

void UZMASTSpringArmComponent::SetClampedViewPitch()
{
	if (APlayerCameraManager* PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		PlayerCameraManager->ViewPitchMin = ViewPitchMin_;
		PlayerCameraManager->ViewPitchMax = ViewPitchMax_;
	}
}