// Fill out your copyright notice in the Description page of Project Settings.


#include "AExplodingBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SMagicProjectile.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AAExplodingBarrel::AAExplodingBarrel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->OnComponentHit.AddDynamic(this, &AAExplodingBarrel::OnHit);
	RootComponent = StaticMesh;

	// Radial impulse 
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->SetupAttachment(RootComponent);

	RadialForceComp->Radius = 500.0f; 
	RadialForceComp->ImpulseStrength = 2000.0f;
	RadialForceComp->bImpulseVelChange = true;
}

// Called when the game starts or when spawned
void AAExplodingBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAExplodingBarrel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAExplodingBarrel::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) // OtherComp is the thing hitting
{
	ASMagicProjectile* Projectile = Cast<ASMagicProjectile>(OtherActor); // To make an object react only when hit by another specific object, first cast it

	if (Projectile && OtherComp)
	{
		RadialForceComp->FireImpulse(); // Fires the impulse

		if (ExplosionEffect)
		{
			FActorSpawnParameters SpawnParams;

			GetWorld()->SpawnActor<AActor>(ExplosionEffect, GetActorLocation(), GetActorRotation(), SpawnParams);
		}

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Barrel hit. "));
		//}

		Destroy(); // Destroys object
	}

	//UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel")); // For debugging

	//UE_LOG(LogTemp, Log, TEXT("OtherActor: %s"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds); // Allows to safely get name of an actor. * converts fstring to character array

	//FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	//DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}
