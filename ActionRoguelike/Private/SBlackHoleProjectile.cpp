// Fill out your copyright notice in the Description page of Project Settings.

#include "SBlackHoleProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASBlackHoleProjectile::ASBlackHoleProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MovementSpeed = 1000.0f;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // This prevents the player from colliding with the projectile
	SphereComp->SetNotifyRigidBodyCollision(true);

	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = MovementSpeed;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	RadialForceRadius = 500.0f;
	RadialForceStrength = 1000.0f;

	//PrimitiveComponent->AddRadialForce(Origin, Radius, Strength, Falloff, bAccelChange);
}

// Called when the game starts or when spawned
void ASBlackHoleProjectile::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(TimerHandle_ProjectileDestroy, this, &ASBlackHoleProjectile::ProjectileDestroy_TimeElapsed, LifeSpan); // Destroys the actor after a delay
	
}

// Called every frame
void ASBlackHoleProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//SphereComp->AddRadialForce(GetActorLocation(), RadialForceRadius, RadialForceStrength, ERadialImpulseFalloff::RIF_Linear, false); // Adds a radial force every tick

	//FVector Direction = OtherComp->GetComponentLocation() - GetActorLocation();
	//Direction.Normalize();

	//OtherComp->AddForce(
	//	Direction * RadialForceStrength,
	//	NAME_None,
	//	false
	//);

}

void ASBlackHoleProjectile::ProjectileDestroy_TimeElapsed()
{
	Destroy();
}

