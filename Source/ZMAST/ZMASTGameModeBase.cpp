// ZMAST, All Rights Reserved


#include "ZMASTGameModeBase.h"
#include "ZMASTPlayerCharacter.h"
#include "ZMASTPlayerController.h"

AZMASTGameModeBase::AZMASTGameModeBase()
{
	DefaultPawnClass = AZMASTPlayerCharacter::StaticClass();
	PlayerControllerClass = AZMASTPlayerController::StaticClass();
}

void AZMASTGameModeBase::StartPlay()
{
	Super::StartPlay();
}
