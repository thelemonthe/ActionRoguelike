// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	InteractionDistance = 500;

	// ...
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // This specifies what kind of objects we want the ray to collide with. 

	AActor* MyOwner = GetOwner();
	UCameraComponent* Camera = MyOwner->FindComponentByClass<UCameraComponent>(); // We get a reference to the camera component from the player to then get its location

	FVector Start = Camera->GetComponentLocation(); // Origin of line trace starts at player camera
	FVector Direction = Camera->GetForwardVector(); // Direction of camera to get the End location


	FVector End = Start + (Direction * InteractionDistance);

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);

	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	//DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 0.1f, 0, 2.0f);

	AActor* HitActor = Hit.GetActor();

	if (false)//if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                         // Key
			5.0f,                       // Display time
			FColor::Red,                // Text color
			TEXT("Interact component starts scanning for objects. ")
		);
	}

	if (HitActor)
	{
		if (false)//if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,                         // Key
				5.0f,                       // Display time
				FColor::Red,                // Text color
				TEXT("Interact component has hit an object. ")
			);
		}

		if (HitActor->Implements<USGameplayInterface>()) // Checks if the hit actor implements ISGameplayInterface
		{
			if (false)//if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,                         // Key
					5.0f,                       // Display time
					FColor::Red,                // Text color
					TEXT("Object implements Interaction Interface. ")
				);
			}

			APawn* MyPawn = Cast<APawn>(MyOwner); // Owner must first be cast to pawn to be used

			ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
		}
	}
}