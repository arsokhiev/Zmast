// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Weapon/ZMASTBaseWeapon.h"
#include "ZMASTLauncherWeapon.generated.h"

class USoundCue;
class AZMASTProjectile;
class UNiagaraComponent;

UCLASS()
class ZMAST_API AZMASTLauncherWeapon : public AZMASTBaseWeapon
{
	GENERATED_BODY()

public:
	virtual void StartFire() override;
	void CompleteFire();
	virtual void StopFire() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AZMASTProjectile> ProjectileClass;

	virtual void MakeShoot() override;

private:
	FTimerHandle ShootTimerHandle;
	
	UPROPERTY()
	UNiagaraComponent* MuzzleFXComponent;
};
