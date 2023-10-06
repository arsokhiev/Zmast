// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ZMASTWeaponComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZMAST_API UZMASTWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	bool EquipAnimInProgress = false;
	bool IsWeaponActive = false;

	void PlayAnimMontage(UAnimMontage* AnimMontage) const;
	void OnEquipActionFinished(USkeletalMeshComponent* MeshComp);

public:	
	UZMASTWeaponComponent();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsArmed() const { return IsWeaponActive; }

	bool IsAnimInProgress() const { return EquipAnimInProgress; }

	void Arm();
	void Disarm();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* ArmAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DisarmAnimMontage;
	
	virtual void BeginPlay() override;
	
};
