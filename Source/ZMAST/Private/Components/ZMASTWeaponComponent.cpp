// ZMAST, All Rights Reserved

#include "Components/ZMASTWeaponComponent.h"
#include "ZMASTPlayerCharacter.h"
#include "Animations/AnimUtils.h"
#include "Animations/ZMASTEquipFinishedAnimNotify.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

void UZMASTWeaponComponent::Arm()
{
	IsWeaponActive = true;
	
	PlayAnimMontage(ArmAnimMontage);
	EquipAnimInProgress = true;
}

void UZMASTWeaponComponent::Disarm()
{
	IsWeaponActive = false;
	
	PlayAnimMontage(DisarmAnimMontage);
	EquipAnimInProgress = true;
}

void UZMASTWeaponComponent::PlayAnimMontage(UAnimMontage* AnimMontage) const
{
	ACharacter* Character = Cast<AZMASTBaseCharacter>(GetOwner());
	if (!Character || !GetWorld()) return;

	Character->PlayAnimMontage(AnimMontage);
}

void UZMASTWeaponComponent::OnEquipActionFinished(USkeletalMeshComponent* MeshComp)
{
	const ACharacter* Character = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComp) return;

	EquipAnimInProgress = false;
	UE_LOG(LogWeaponComponent, Display, TEXT("Equip finished"));
}

UZMASTWeaponComponent::UZMASTWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UZMASTWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto ArmFinishedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTEquipFinishedAnimNotify>(ArmAnimMontage);
	const auto DisarmFinishedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTEquipFinishedAnimNotify>(DisarmAnimMontage);
	
	if (ArmFinishedAnimNotify && DisarmFinishedAnimNotify)
	{
		ArmFinishedAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnEquipActionFinished);
		DisarmFinishedAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnEquipActionFinished);
	}
	else
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is not set"));
	}
}