// ZMAST, All Rights Reserved


#include "Weapon/ZMASTBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

AZMASTBaseWeapon::AZMASTBaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
	SetRootComponent(WeaponMesh);
}

void AZMASTBaseWeapon::StartFire()
{
	FireInProgress = true;
}

void AZMASTBaseWeapon::StopFire()
{
	FireInProgress = false;
}

bool AZMASTBaseWeapon::IsFiring() const
{
	return FireInProgress;
}

void AZMASTBaseWeapon::PlayShootFeedback()
{
	// In derived
}

void AZMASTBaseWeapon::BeginPlay()
{
	Super::BeginPlay();
	check(WeaponMesh);

	WeaponMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
}

void AZMASTBaseWeapon::MakeShoot()
{
	if (Cast<ACharacter>(GetOwner())->IsPlayerControlled())
	{
		PlayShootFeedback();
		OnShootMade.Broadcast();
	}
}

APlayerController* AZMASTBaseWeapon::GetPlayerController() const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return nullptr;

	return Character->GetController<APlayerController>();
}

bool AZMASTBaseWeapon::GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return false;

	if (Character->IsPlayerControlled())
	{
		APlayerController* PlayerController = GetPlayerController();
		if (!PlayerController) return false;

		PlayerController->GetPlayerViewPoint(ViewLocation, ViewRotation);
	}
	else
	{
		ViewLocation = GetMuzzleWorldLocation();
		ViewRotation = WeaponMesh->GetSocketRotation(MuzzleSocketName);
	}

	return true;
}

FVector AZMASTBaseWeapon::GetMuzzleWorldLocation() const
{
	return WeaponMesh->GetSocketLocation(MuzzleSocketName);
}

FVector AZMASTBaseWeapon::GetMuzzleForwardVector() const
{
	return WeaponMesh->GetSocketTransform(MuzzleSocketName).GetRotation().GetForwardVector();
}

bool AZMASTBaseWeapon::GetTraceData(FVector& TraceStart, FVector& TraceEnd) const
{
	FVector ViewLocation;
	FRotator ViewRotation;
	if (!GetPlayerViewPoint(ViewLocation, ViewRotation)) return false;

	TraceStart = ViewLocation;
	const FVector NormalizedShootDirection = ViewRotation.Vector();
	TraceEnd = TraceStart + (NormalizedShootDirection * TraceMaxDistance);
	return true;
}

void AZMASTBaseWeapon::MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd)
{
	if (!GetWorld()) return;

	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.bReturnPhysicalMaterial = true; // written in HitResult
	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility,
										 CollisionParams);
}

UNiagaraComponent* AZMASTBaseWeapon::SpawnMuzzleFX()
{
	return UNiagaraFunctionLibrary::SpawnSystemAttached(
		MuzzleFX,
		WeaponMesh,
		MuzzleSocketName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		true
	);
}
