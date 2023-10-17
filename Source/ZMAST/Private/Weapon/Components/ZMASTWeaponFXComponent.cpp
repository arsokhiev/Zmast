// ZMAST, All Rights Reserved


#include "Weapon/Components/ZMASTWeaponFXComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/DecalComponent.h"
#include "Sound/SoundCue.h"

UZMASTWeaponFXComponent::UZMASTWeaponFXComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UZMASTWeaponFXComponent::PlayImpactFX(const FHitResult& Hit)
{
	auto ImpactData = DefaultImpactData;
	if (Hit.PhysMaterial.IsValid())
	{
		const UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();
		if (ImpactDataMap.Contains(PhysMat))
		{
			ImpactData = ImpactDataMap[PhysMat];	
		}
	}

	//niagara
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(),
		ImpactData.NiagaraEffect,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation());

	//decal
	UDecalComponent *DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(),
		ImpactData.DecalData.Material,
		ImpactData.DecalData.Size,
		Hit.ImpactPoint,
		Hit.ImpactNormal.Rotation());

	if (DecalComponent)
	{
		DecalComponent->SetFadeOut(ImpactData.DecalData.LifeTime, ImpactData.DecalData.FadeOutTime);
	}

	//sound
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactData.Sound, Hit.ImpactPoint);
}