// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bShowDebug = false;

	// Attach the SCharacter to our SpringArmComponent
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");

	// Allows camera movement
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	// Attach the SpringArmComponent to our CameraComponent
	CameraComp = CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	//initialize field of view
	CameraComp->GetCameraView(0, CameraFieldOfView);

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

void ASCharacter::Attack(TSubclassOf<AActor> Projectile)
{

	// Pass in animation
	PlayAnimMontage(AttackAnim);
	CurrentProjectileClass = Projectile;
	// Add a timer so the animation syncs with spawn
	GetWorldTimerManager().SetTimer(TimerHandle_Attack,this, &ASCharacter::ShootProjectile, 0.1f, false, AttackDelay); 

}

void ASCharacter::Attack_TimeElapsed()
{
	if(ensure(CurrentProjectileClass))
	{
		ShootProjectile();
	}
}

void ASCharacter::CrosshairImpactRelativeRotation(FRotator& Rotator, const FVector& Start,  FVector& End,const FCollisionObjectQueryParams&  ObjectQueryParams, float Range)
{
	End = CameraLocation + (CameraFieldOfView.Rotation.Vector() * Range);
	Rotator = UKismetMathLibrary::FindLookAtRotation(Start, End);
	
	// Get our initial rotator without collision checking
	FHitResult Hit;
		
	// Only care about first hit to reconfigure our rotator
	GetWorld()->LineTraceSingleByObjectType(Hit, CameraLocation, End, ObjectQueryParams);

	// If there was a hit, reconfigure our rotator
	if(Hit.GetActor())
	{
		End = Hit.ImpactPoint;
		Rotator = UKismetMathLibrary::FindLookAtRotation(Start, Hit.ImpactPoint);
	}

	if( bShowDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), CameraLocation, End, 90.0f, FColor::White, false, 2.0f, 0, 2);
	}
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		const FCollisionObjectQueryParams QueryParams;
		FRotator CrosshairRotator;
		FVector Start;
		FVector End;
		const float InteractRange = 500;
		GetActorEyesViewPoint(Start, CrosshairRotator);
		CrosshairImpactRelativeRotation(CrosshairRotator, Start, End, QueryParams, InteractRange);
		InteractionComp->PrimaryInteract(Start, End, CrosshairRotator, QueryParams, InteractRange);
	}
}

void ASCharacter::ShootProjectile()
{
	// Gets the location of the hand to spawn the projectile
	const FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	
	// Lets query for any hits with Dynamic/static
	FVector ImpactLocation; 
	FRotator SpawnRotator;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);

	// Get the rotation to the closest "impact" Crosshair site
	CrosshairImpactRelativeRotation(SpawnRotator, SpawnLocation, ImpactLocation, ObjectQueryParams, 3000);
		
	// Spawn projectile based on the rotation from the hand socket to its impact location
	const FTransform SpawnTM = FTransform(SpawnRotator,SpawnLocation );
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;
		
	// Whenever we spawn, spawn to the world
	GetWorld()->SpawnActor<AActor>(CurrentProjectileClass, SpawnTM, SpawnParams);
	
	if( bShowDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, ImpactLocation, 90.0f, FColor::Blue, false, 2.0f, 0, 2);
	}
}


// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CameraComp->GetCameraView(DeltaTime, CameraFieldOfView);
	CameraLocation = CameraFieldOfView.Location;
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
	PlayerInputComponent->BindAction<FProjectileDelegate>("PrimaryAttack", IE_Pressed, this, &ASCharacter::Attack, PrimaryAttackProjectileClass);
	PlayerInputComponent->BindAction<FProjectileDelegate>("SecondaryAttack", IE_Pressed, this, &ASCharacter::Attack, SecondaryAttackProjectileClass);
	PlayerInputComponent->BindAction<FProjectileDelegate>("PrimaryUtility", IE_Pressed, this, &ASCharacter::Attack, UtilityProjectileClass);
	PlayerInputComponent->BindAction("PrimaryInteract", IE_Pressed, this, &ASCharacter::PrimaryInteract);
	
	//Allows the character to Jump
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	// Run Speed
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ASCharacter::Sprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ASCharacter::Sprint);

	// Interact
	
}

