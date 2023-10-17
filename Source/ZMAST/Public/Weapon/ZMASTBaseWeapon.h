// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZMASTCoreTypes.h"
#include "ZMASTBaseWeapon.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundCue;

UCLASS()
class ZMAST_API AZMASTBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	AZMASTBaseWeapon();

	FOnShootMade OnShootMade;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<UCameraShakeBase> CameraShake;

	virtual void StartFire();
	virtual void StopFire();
	bool IsFiring() const;

	virtual void PlayShootFeedback();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	FName MuzzleSocketName = "MuzzleSocket";

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float TraceMaxDistance = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "VFX")
	UNiagaraSystem* MuzzleFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	USoundCue* FireSound;

	bool FireInProgress = false;
	
	virtual void BeginPlay() override;

	virtual void MakeShoot();
	APlayerController* GetPlayerController() const;
	bool GetPlayerViewPoint(FVector& ViewLocation, FRotator& ViewRotation) const;
	FVector GetMuzzleWorldLocation() const;
	FVector GetMuzzleForwardVector() const;
	virtual bool GetTraceData(FVector& TraceStart, FVector& TraceEnd) const;
	void MakeHit(FHitResult& HitResult, const FVector& TraceStart, const FVector& TraceEnd);

	UNiagaraComponent* SpawnMuzzleFX();
};
