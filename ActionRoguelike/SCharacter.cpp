// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponent.h"


// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);
	
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationYaw = false;
	bIsAttacking = false;

	PrimaryAttackLineTraceDistance = 10000.0f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Forward movement
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);
//	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ASCharacter::StopJumping); // Not needed

	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("SecondaryAttack", IE_Pressed, this, &ASCharacter::SecondaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("Teleport", IE_Pressed, this, &ASCharacter::Teleport);
}

void ASCharacter::MoveForward(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	AddMovementInput(ControlRot.Vector(), Value);
}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;

	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);

	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true; 

	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, 0.2f); // After the delay, the function mentioned will be triggered

	//GetWorldTimeManager().ClearTimer(TimerHandle_PrimaryAttack); // How to clear timer
}

void ASCharacter::SecondaryAttack()
{
	if (bIsAttacking)
	{
		return;
	}

	bIsAttacking = true;

	PlayAnimMontage(AttackAnim);

	GetWorldTimerManager().SetTimer(TimerHandle_SecondaryAttack, this, &ASCharacter::SecondaryAttack_TimeElapsed, 0.2f);
}

void ASCharacter::Teleport()
{

}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	FCollisionObjectQueryParams ObjectQueryParams; // First line trace is performed to determine where the player is looking
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // This specifies what kind of objects we want the ray to collide with. 
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	//AActor* MyOwner = GetOwner();
	//UCameraComponent* Camera = MyOwner->FindComponentByClass<UCameraComponent>(); // We get a reference to the camera component from the player to then get its location
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();

	if (!Camera) {
		//UE_LOG(LogTemp, Error, TEXT("No camera found on %s"), *MyOwner->GetName());
		UE_LOG(LogTemp, Error, TEXT("Issue with Camera Component. "));
		return;
	}

	FVector Start = Camera->GetComponentLocation(); // Origin of line trace starts at player camera
	FVector Direction = Camera->GetForwardVector(); // Direction of camera to get the End location

	FVector End = Start + (Direction * PrimaryAttackLineTraceDistance);

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	// This if determines whether or not we can identify where the player is looking. If not, forward rotation is set to camera rotation 
	FRotator ForwardRotator;
	if (bBlockingHit)
	{
		FVector HitLocation = Hit.ImpactPoint; // Gets the point of impact from line trace
		FVector ForwardVector = (HitLocation - HandLocation).GetSafeNormal(); // Determines ForwardVector where Start: HandLocation, End: HitLocation
		ForwardRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator(); // Converts the ForwardVector to an FRotator to use in SpawnTM
		if (false)//if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Raycast has hit something. "));
		}
	}
	else {
		ForwardRotator = GetControlRotation(); // If we can't get a hit location, we use the camera rotation
		if (false)//if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Raycast hasn't hit anything. "));
		}
	}

	FTransform SpawnTM = FTransform(ForwardRotator, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this; // Allows to check who the projectile belongs to

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	bIsAttacking = false;
}

void ASCharacter::SecondaryAttack_TimeElapsed()
{
	FCollisionObjectQueryParams ObjectQueryParams; // First line trace is performed to determine where the player is looking
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // This specifies what kind of objects we want the ray to collide with. 
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	//AActor* MyOwner = GetOwner();
	//UCameraComponent* Camera = MyOwner->FindComponentByClass<UCameraComponent>(); // We get a reference to the camera component from the player to then get its location
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();

	if (!Camera) {
		//UE_LOG(LogTemp, Error, TEXT("No camera found on %s"), *MyOwner->GetName());
		UE_LOG(LogTemp, Error, TEXT("Issue with Camera Component. "));
		return;
	}

	FVector Start = Camera->GetComponentLocation(); // Origin of line trace starts at player camera
	FVector Direction = Camera->GetForwardVector(); // Direction of camera to get the End location

	FVector End = Start + (Direction * PrimaryAttackLineTraceDistance);

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	// This if determines whether or not we can identify where the player is looking. If not, forward rotation is set to camera rotation 
	FRotator ForwardRotator;
	if (bBlockingHit)
	{
		FVector HitLocation = Hit.ImpactPoint; // Gets the point of impact from line trace
		FVector ForwardVector = (HitLocation - HandLocation).GetSafeNormal(); // Determines ForwardVector where Start: HandLocation, End: HitLocation
		ForwardRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator(); // Converts the ForwardVector to an FRotator to use in SpawnTM
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Raycast has hit something. "));
		}
	}
	else {
		ForwardRotator = GetControlRotation(); // If we can't get a hit location, we use the camera rotation
		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Raycast hasn't hit anything. "));
		//}
	}

	FTransform SpawnTM = FTransform(ForwardRotator, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this; // Allows to check who the projectile belongs to

	GetWorld()->SpawnActor<AActor>(SecondaryProjectileClass, SpawnTM, SpawnParams);
	bIsAttacking = false;
}

void ASCharacter::Teleport_TimeElapsed()
{
	FCollisionObjectQueryParams ObjectQueryParams; // First line trace is performed to determine where the player is looking
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic); // This specifies what kind of objects we want the ray to collide with. 
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	//AActor* MyOwner = GetOwner();
	//UCameraComponent* Camera = MyOwner->FindComponentByClass<UCameraComponent>(); // We get a reference to the camera component from the player to then get its location
	UCameraComponent* Camera = FindComponentByClass<UCameraComponent>();

	if (!Camera) {
		//UE_LOG(LogTemp, Error, TEXT("No camera found on %s"), *MyOwner->GetName());
		UE_LOG(LogTemp, Error, TEXT("Issue with Camera Component. "));
		return;
	}

	FVector Start = Camera->GetComponentLocation(); // Origin of line trace starts at player camera
	FVector Direction = Camera->GetForwardVector(); // Direction of camera to get the End location

	FVector End = Start + (Direction * PrimaryAttackLineTraceDistance);

	FHitResult Hit;
	bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, Start, End, ObjectQueryParams);

	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");

	// This if determines whether or not we can identify where the player is looking. If not, forward rotation is set to camera rotation 
	FRotator ForwardRotator;
	if (bBlockingHit)
	{
		FVector HitLocation = Hit.ImpactPoint; // Gets the point of impact from line trace
		FVector ForwardVector = (HitLocation - HandLocation).GetSafeNormal(); // Determines ForwardVector where Start: HandLocation, End: HitLocation
		ForwardRotator = FRotationMatrix::MakeFromX(ForwardVector).Rotator(); // Converts the ForwardVector to an FRotator to use in SpawnTM
		if (false)//if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Raycast has hit something. "));
		}
	}
	else {
		ForwardRotator = GetControlRotation(); // If we can't get a hit location, we use the camera rotation
		if (false)//if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Raycast hasn't hit anything. "));
		}
	}

	FTransform SpawnTM = FTransform(ForwardRotator, HandLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this; // Allows to check who the projectile belongs to

	GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	bIsAttacking = false;
}

void ASCharacter::PrimaryInteract()
{

	FVector CameraLocation = CameraComp->GetComponentLocation(); // CameraLocation is taken to be the start of the Interact line trace

	if (InteractionComp) // Technically not necessary
	{
		InteractionComp->PrimaryInteract();

		//if (GEngine)
		//{
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Interact sent out by player. "));
		//}
	}
}