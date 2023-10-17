// ZMAST, All Rights Reserved


#include "Weapon/ZMASTProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/ZMASTWeaponFXComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AZMASTProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!GetWorld()) return;

	MovementComponent->StopMovementImmediately();
	
	UGameplayStatics::ApplyRadialDamage(GetWorld(),
										DamageAmount,
										GetActorLocation(),
										DamageRadius,
										UDamageType::StaticClass(),
										{GetOwner()},
										this,
										GetController(),
										DoFullDamage);

	WeaponFXComponent->PlayImpactFX(Hit);
	Destroy();
}

AController* AZMASTProjectile::GetController()
{
	const auto Pawn = Cast<APawn>(GetOwner());
	return Pawn ? Pawn->GetController() : nullptr;
}

AZMASTProjectile::AZMASTProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	CollisionComponent->InitSphereRadius(10.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //no physical collisions (only alerts)
	CollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block); //blocking by all channels
	CollisionComponent->bReturnMaterialOnMove = true;
	SetRootComponent(CollisionComponent);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	MovementComponent->InitialSpeed = 3000.0f;

	WeaponFXComponent = CreateDefaultSubobject<UZMASTWeaponFXComponent>("WeaponFXComponent");
}

void AZMASTProjectile::BeginPlay()
{
	Super::BeginPlay();

	check(CollisionComponent);
	check(MovementComponent);
	check(WeaponFXComponent);

	MovementComponent->Velocity = NormalizedShootDirection * MovementComponent->InitialSpeed;

	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AZMASTProjectile::OnProjectileHit);

	SetLifeSpan(LifeSeconds);
}