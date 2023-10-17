// ZMAST, All Rights Reserved

#include "Components/ZMASTWeaponComponent.h"
#include "ZMASTBaseWeapon.h"
#include "ZMASTPlayerCharacter.h"
#include "ZMASTUtils.h"
#include "Animations/AnimUtils.h"
#include "Animations/ZMASTEquipFinishedAnimNotify.h"
#include "Animations/ZMASTGrabWeaponAnimNotify.h"
#include "Animations/ZMASTReleaseWeaponAnimNotify.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponComponent, All, All)

void UZMASTWeaponComponent::Arm()
{
	if (EquipAnimInProgress) return;
	
	IsWeaponActive = true;
	
	PlayAnimMontage(ArmAnimMontage);
	EquipAnimInProgress = true;
}

void UZMASTWeaponComponent::Disarm()
{
	if (EquipAnimInProgress) return;

	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter || !GetWorld()) return;

	if (PlayerCharacter->IsAiming())
	{
		PlayerCharacter->DisableAim(0);
	}
	
	IsWeaponActive = false;
	
	PlayAnimMontage(DisarmAnimMontage);
	EquipAnimInProgress = true;
}

void UZMASTWeaponComponent::StartFire()
{
	if (!CanFire()) return;

	SpawnedWeapon->StartFire();
}

void UZMASTWeaponComponent::StopFire()
{
	SpawnedWeapon->StopFire();
}

void UZMASTWeaponComponent::ChangeWeaponState()
{
	IsArmed() ? Disarm() : Arm();
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

void UZMASTWeaponComponent::OnGrabWeapon(USkeletalMeshComponent* MeshComp)
{
	const ACharacter* Character = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComp) return;

	AttachWeaponToSocket(SpawnedWeapon, Character->GetMesh(), WeaponEquipSocketName);
}

void UZMASTWeaponComponent::OnReleaseWeapon(USkeletalMeshComponent* MeshComp)
{
	const ACharacter* Character = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!Character || Character->GetMesh() != MeshComp) return;

	AttachWeaponToSocket(SpawnedWeapon, Character->GetMesh(), WeaponArmorySocketName);
}

void UZMASTWeaponComponent::PlayCameraShake() const
{
	AZMASTBaseCharacter* BaseCharacter = Cast<AZMASTBaseCharacter>(GetOwner());
	if (!BaseCharacter) return;

	APlayerController* Controller = BaseCharacter->GetController<APlayerController>();
	if (!Controller || !Controller->PlayerCameraManager) return;

	if (!SpawnedWeapon->CameraShake) return;
	Controller->PlayerCameraManager->StartCameraShake(SpawnedWeapon->CameraShake);
}

void UZMASTWeaponComponent::SpawnWeapon()
{
	AZMASTBaseCharacter* BaseCharacter = Cast<AZMASTBaseCharacter>(GetOwner());
	if (!BaseCharacter || !GetWorld()) return;

	SpawnedWeapon = GetWorld()->SpawnActor<AZMASTBaseWeapon>(WeaponData.WeaponClass);
	if (!SpawnedWeapon) return;

	SpawnedWeapon->OnShootMade.AddUObject(this, &UZMASTWeaponComponent::PlayCameraShake);
	SpawnedWeapon->SetOwner(BaseCharacter);

	AttachWeaponToSocket(SpawnedWeapon, BaseCharacter->GetMesh(), WeaponArmorySocketName);
}

void UZMASTWeaponComponent::InitAnimations()
{
	const auto ArmFinishedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTEquipFinishedAnimNotify>(ArmAnimMontage);
	const auto DisarmFinishedAnimNotify = AnimUtils::FindNotifyByClass<UZMASTEquipFinishedAnimNotify>(DisarmAnimMontage);

	const auto GrabWeaponAnimNotify = AnimUtils::FindNotifyByClass<UZMASTGrabWeaponAnimNotify>(ArmAnimMontage);
	const auto ReleaseWeaponAnimNotify = AnimUtils::FindNotifyByClass<UZMASTReleaseWeaponAnimNotify>(DisarmAnimMontage);
	
	if (ArmFinishedAnimNotify && DisarmFinishedAnimNotify
		&& GrabWeaponAnimNotify && ReleaseWeaponAnimNotify)
	{
		ArmFinishedAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnEquipActionFinished);
		DisarmFinishedAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnEquipActionFinished);

		GrabWeaponAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnGrabWeapon);
		ReleaseWeaponAnimNotify->OnNotified.AddUObject(this, &UZMASTWeaponComponent::OnReleaseWeapon);
	}
	else
	{
		UE_LOG(LogWeaponComponent, Error, TEXT("Equip anim notify is not set"));
	}
}

void UZMASTWeaponComponent::AttachWeaponToSocket(AZMASTBaseWeapon* Weapon, USceneComponent* SceneComponent,
                                                 const FName& SocketName)
{
	if (!Weapon || !SceneComponent) return;

	FAttachmentTransformRules AttachmentRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		false
		);
	Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

UZMASTWeaponComponent::UZMASTWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UZMASTWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	InitAnimations();
	SpawnWeapon();
}

bool UZMASTWeaponComponent::CanFire()
{
	const auto PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	if (!PlayerCharacter) return false;
	
	return !EquipAnimInProgress
	&& !ReloadAnimInProgress
	&& IsWeaponActive
	&& PlayerCharacter->IsAiming()
	&& PlayerCharacter->GetVelocity().IsNearlyZero();
}
