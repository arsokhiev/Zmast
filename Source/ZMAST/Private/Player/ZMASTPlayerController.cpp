// ZMAST, All Rights Reserved


#include "Player/ZMASTPlayerController.h"

AZMASTPlayerController::AZMASTPlayerController()
{
}

void AZMASTPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AZMASTPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!InputComponent) return;
}
