// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Attach the SCharacter to our SpringArmComponent
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");

	// Allows camera movement
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	// Attach the SpringArmComponent to our CameraComponent
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	//Set up our interaction component
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");
	
	//Allows for the camera to not always be locked? So it's not always strafing
	// Allows orientation based on how the character is already moving, but based on camera
	// See notes about "move in the direction relative to the acceleration"
	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	// Action RPG Movement, character can look at camera
	// Booleans start with `b`
	bUseControllerRotationYaw = false;
	bCharacterSprinting = false;

	// Hold onto the standard speed
	CharacterMaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;

	// Attack inititialization
	AttackDelay = 0.3f;
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void ASCharacter::MoveForward(float Value)
{
	// Gets the vector pointing from the actor
	FRotator ControlRot = GetControlRotation();
	// Want to be anchored to the ground
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	
	// Originally, we were getting the vector relative to the pawn
	//AddMovementInput(GetActorForwardVector(), Value);
	
	// Relative to the camera
	AddMovementInput(ControlRot.Vector(), Value);
}

// When sprinting, increase walk speed
void ASCharacter::Sprint()
{
	this->bCharacterSprinting = !bCharacterSprinting;
	GetCharacterMovement()->MaxWalkSpeed = bCharacterSprinting ?
		CharacterMaxWalkSpeed * SprintMultiplier : CharacterMaxWalkSpeed;

}

void ASCharacter::MoveRight(float Value)
{
	FRotator ControlRot = GetControlRotation();
	// Want to be anchored to the ground
	ControlRot.Pitch = 0.0f;
	ControlRot.Roll = 0.0f;
	// Unreal
	/*
	 * X is forward (Red)
	 * Y is right (Green)
	 * Z is Up (Blue)
	 */
	
	
	// GetRightVector source code uses the rotation matrix
	// Originally, getRightVector would give us relative to the Pawn
	// Instead, we're going to apply the rotation matrix to the Control Rotation
	FVector RightVector = FRotationMatrix(ControlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, Value);
}

void ASCharacter::PrimaryAttack()
{

	// Pass in animation
	PlayAnimMontage(AttackAnim);

	// Add a timer so the animation syncs with spawn
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASCharacter::PrimaryAttack_TimeElapsed, AttackDelay);

	// If our character were to die, we want to clear out the timer
	// GetWorldTimerManager().ClearTimer(TimerHandle_PrimaryAttack);

}

void ASCharacter::PrimaryAttack_TimeElapsed()
{
	if(ensure(ProjectileClass))
	{
		// Gets the location of the hand to spawn the projectile
		FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
		// Spawn relative to the control rotation and currently just spawn on the actor
		FTransform SpawnTM = FTransform(GetControlRotation(),HandLocation );
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;
	
		// Whenever we spawn, spawn to the world
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		InteractionComp->PrimaryInteract();
	}
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
	// Name of our AxisComponent, Bind the movement to this character, call the function
	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	

	// When pressed, call the function PrimaryAttack
	PlayerInputComponent->BindAction("PrimaryAttack", IE_Pressed, this, &ASCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	
	//Allows the character to Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	// Run Speed
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::Sprint);

	// Interact
	
}

