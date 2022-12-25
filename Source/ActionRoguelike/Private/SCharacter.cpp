// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"

#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
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
	

	//Set up our custom components
	InteractionComp = CreateDefaultSubobject<USInteractionComponent>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	
	
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
void ASCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AttributeComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
}


void ASCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth,
	float Delta)
{
	if (NewHealth <= 0.0f && Delta < 0.0f)
	{
		APlayerController* PC = Cast<APlayerController>(GetController());
		DisableInput(PC);
	}
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
	GetWorldTimerManager().SetTimer(TimerHandle_Attack,this, &ASCharacter::SpawnProjectile, 0.1f, false, AttackDelay); 

}

void ASCharacter::Attack_TimeElapsed()
{
	if(ensure(CurrentProjectileClass))
	{
		SpawnProjectile();
	}
}

void ASCharacter::PrimaryInteract()
{
	if(InteractionComp)
	{
		const float InteractRange = 500;
		
		const FCollisionObjectQueryParams QueryParams;
		FRotator Rotator;
		FVector Start;
		FVector End = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * InteractRange);
		GetActorEyesViewPoint(Start, Rotator);
		InteractionComp->PrimaryInteract(Start, End, Rotator, QueryParams, InteractRange);
	}
}

void ASCharacter::SpawnProjectile()
{
	// Spawn our muzzle
	// Gets the location of the hand to spawn the projectile
	const FVector SpawnLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	const FRotator SpawnRotation = GetMesh()->GetSocketRotation("Muzzle_01");
	UGameplayStatics::SpawnEmitterAttached(MuzzleVfx, GetMesh(), "HandMuzzle",SpawnLocation, SpawnRotation, EAttachLocation::KeepWorldPosition);
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Instigator = this;

	FCollisionShape Shape;
	Shape.SetSphere(10.0f);
	
	// Ignore player
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	// Lets query for any hits with Dynamic/static
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FVector TraceStart = CameraComp->GetComponentLocation();
	FVector TraceEnd = CameraComp->GetComponentLocation() + (GetControlRotation().Vector() * 5000);

	FHitResult Hit;
	//Return true if we got to a blocking hit
	if(GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectQueryParams, Shape, Params))
	{
		TraceEnd = Hit.ImpactPoint;
	}
	
	// Spawn projectile based on the rotation from the hand socket to its impact location
	// Find new direction/rotation from hand pointing to impact point in world
	FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - SpawnLocation).Rotator();

	FTransform SpawnTM = FTransform(ProjRotation, SpawnLocation);
	// Whenever we spawn, spawn to the world
	GetWorld()->SpawnActor<AActor>(CurrentProjectileClass, SpawnTM, SpawnParameters);
	
	if( bShowDebug)
	{
		DrawDebugDirectionalArrow(GetWorld(), SpawnLocation, TraceEnd, 90.0f, FColor::Blue, false, 2.0f, 0, 2);
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

