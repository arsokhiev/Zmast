// ZMAST, All Rights Reserved


#include "Weapon/ZMASTLauncherWeapon.h"

#include "ZMASTPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Weapon/ZMASTProjectile.h"

DEFINE_LOG_CATEGORY_STATIC(LogLauncherWeapon, All, All)

void AZMASTLauncherWeapon::StartFire()
{
	UE_LOG(LogLauncherWeapon, Display, TEXT("AZMASTLauncherWeapon::StartFire"));
	Super::StartFire();
	
	GetWorldTimerManager().SetTimer(ShootTimerHandle, this, &AZMASTLauncherWeapon::MakeShoot, 2, false);
	
	AZMASTPlayerCharacter* PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	PlayerCharacter->StartShootFOVChange();
}

void AZMASTLauncherWeapon::CompleteFire()
{
	AZMASTPlayerCharacter* PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
	PlayerCharacter->CompleteShootFOVChange();
	GetWorldTimerManager().ClearTimer(ShootTimerHandle);

	StopFire();
}

void AZMASTLauncherWeapon::StopFire()
{
	//UE_LOG(LogLauncherWeapon, Display, TEXT("AZMASTLauncherWeapon::StopFire()"));
	if	(GetWorldTimerManager().TimerExists(ShootTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(ShootTimerHandle);

		AZMASTPlayerCharacter* PlayerCharacter = Cast<AZMASTPlayerCharacter>(GetOwner());
		PlayerCharacter->CancelShootFOVChange();
	}
	Super::StopFire();
}

void AZMASTLauncherWeapon::MakeShoot()
{
	if (!GetWorld()) return;
	UE_LOG(LogLauncherWeapon, Display, TEXT("AZMASTLauncherWeapon::MakeShoot"));
	
	FVector TraceStart, TraceEnd;
	if (!GetTraceData(TraceStart, TraceEnd)) return;

	FHitResult HitResult;
	MakeHit(HitResult, TraceStart, TraceEnd);

	const FVector StartPoint = GetMuzzleWorldLocation();

	const FVector EndPoint = HitResult.bBlockingHit ? HitResult.ImpactPoint : TraceEnd;
	const FVector NormalizedDirection = (EndPoint - StartPoint).GetSafeNormal();

	const FTransform SpawnTransform(FRotator::ZeroRotator, GetMuzzleWorldLocation());
	if (AZMASTProjectile* Projectile = GetWorld()->SpawnActorDeferred<AZMASTProjectile>(ProjectileClass, SpawnTransform))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->SetShootDirection(NormalizedDirection);
		Projectile->FinishSpawning(SpawnTransform);
	}

	SpawnMuzzleFX();

	UGameplayStatics::SpawnSoundAttached(FireSound, WeaponMesh, MuzzleSocketName);

	CompleteFire();
	
	Super::MakeShoot();
}
