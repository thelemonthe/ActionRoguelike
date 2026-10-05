// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"


ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap); 
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore); // This prevents the player from colliding with the projectile
	SphereComp->SetNotifyRigidBodyCollision(true);
	SphereComp->OnComponentHit.AddDynamic(this, &ASMagicProjectile::OnHit); // This lines binds the function to the collision event (very important)

	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) // Check if other actor is a null component
	{
		return;
	}

	if (OtherActor == GetInstigator()) // Check if overlap occurs with whoever fired it
	{
		return;
	}

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass())); // If we find a USAttributeComponent we then cast it to that type
	if (AttributeComp)
	{

		UE_LOG(LogTemp, Warning, TEXT("Found AttributeComponent on %s"),
			*OtherActor->GetName());

		AttributeComp->ApplyHealthChange(-10.0f);

		Destroy();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("NO AttributeComponent on %s"),
			*OtherActor->GetName());
	}
	
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASMagicProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{

	if(false)//if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                         // Key
			5.0f,                       // Display time
			FColor::Red,                // Text color
			TEXT("Projectile hit. ")
		);
	}

	// Destroy(); // Destroy projecile when it hits something
}