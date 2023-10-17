// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "ZMASTCoreTypes.h"
#include "Components/ActorComponent.h"
#include "ZMASTWeaponComponent.generated.h"

class AZMASTBaseWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZMAST_API UZMASTWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool EquipAnimInProgress = false;
	bool ReloadAnimInProgress = false;
	bool IsWeaponActive = false;

	void PlayAnimMontage(UAnimMontage* AnimMontage) const;
	
	void OnEquipActionFinished(USkeletalMeshComponent* MeshComp);
	void OnReloadActionFinished(USkeletalMeshComponent* MeshComp);

	void OnGrabWeapon(USkeletalMeshComponent* MeshComp);
	void OnReleaseWeapon(USkeletalMeshComponent* MeshComp);

	bool CanReload() const;

	void PlayCameraShake() const;

	void SpawnWeapon();
	void InitAnimations();
	void AttachWeaponToSocket(AZMASTBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

public:	
	UZMASTWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsArmed() const { return IsWeaponActive; }

	bool IsAnimInProgress() const { return EquipAnimInProgress; }

	void Arm();
	void Disarm();

	void StartFire();
	void StopFire();

	void ChangeWeaponState();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FWeaponData WeaponData;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponEquipSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName WeaponArmorySocketName = "ArmorySocket";

	UPROPERTY()
	AZMASTBaseWeapon* SpawnedWeapon = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ArmAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DisarmAnimMontage;
	
	virtual void BeginPlay() override;

	bool CanFire();
	
};
