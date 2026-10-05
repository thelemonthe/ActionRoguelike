// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	Health = 100;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	Health += Delta; 

	OnHealthChanged.Broadcast(nullptr, this, Health, Delta);

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ApplyHealthChange occurred. "));
	//}

	return true; 
}



