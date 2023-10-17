// ZMAST, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZMASTProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;
class UZMASTWeaponFXComponent;

UCLASS()
class ZMAST_API AZMASTProjectile : public AActor
{
	GENERATED_BODY()

private:
	FVector NormalizedShootDirection;

	UFUNCTION()
	void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
						 FVector NormalImpulse, const FHitResult& Hit);

	AController *GetController();
	
public:	
	AZMASTProjectile();

	void SetShootDirection(const FVector& NormalizedDirection) { NormalizedShootDirection = NormalizedDirection; }

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	UProjectileMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float DamageRadius = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float DamageAmount = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	bool DoFullDamage = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float LifeSeconds = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	UZMASTWeaponFXComponent* WeaponFXComponent;

	virtual void BeginPlay() override;

};
