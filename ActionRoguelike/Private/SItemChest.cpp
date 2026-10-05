// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
ASItemChest::ASItemChest()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = -120;
	bIsOpen = false;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                         // Key
			5.0f,                       // Display time
			FColor::Red,                // Text color
			TEXT("Interact activated for chest. ")
		);
	}

	if (bIsOpen == false) {
		LidMesh->SetRelativeRotation(FRotator(0, 0, TargetPitch)); // WARNING: Function takes in Pitch, Yaw, Roll NOT Roll, Pitch, Yaw
		bIsOpen = true;
	}
	else {
		LidMesh->SetRelativeRotation(FRotator(0, 0, 0)); // Closes chest
		bIsOpen = false;
	}
}

// Called when the game starts or when spawned
void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

