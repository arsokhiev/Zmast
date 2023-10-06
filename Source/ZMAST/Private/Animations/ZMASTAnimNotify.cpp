// ZMAST, All Rights Reserved


#include "Animations/ZMASTAnimNotify.h"

void UZMASTAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	OnNotified.Broadcast(MeshComp);
	Super::Notify(MeshComp, Animation, EventReference);
}
