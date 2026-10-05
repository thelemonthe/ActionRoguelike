// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "SBlackHoleProjectile.generated.h" // Should always be the last header

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class ACTIONROGUELIKE_API ASBlackHoleProjectile : public AActor
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="ProjectileVariables")
	float LifeSpan = 5.0f;

	UPROPERTY(VisibleAnywhere)
	URadialForceComponent* RadialForceComp;

	float MovementSpeed; 
	float RadialForceRadius;
	float RadialForceStrength;

	FTimerHandle TimerHandle_ProjectileDestroy;

	void ProjectileDestroy_TimeElapsed();
	

public:	
	// Sets default values for this actor's properties
	ASBlackHoleProjectile();

protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void ProjectileDestroy();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
