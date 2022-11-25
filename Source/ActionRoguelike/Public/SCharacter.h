// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;
UCLASS()
class ACTIONROGUELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()
protected:

	UPROPERTY(EditAnywhere, Category="Attack")
	UAnimMontage* AttackAnim;
	// Assigns from a class, MagicProjectile derives AActor
	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> PrimaryAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	TSubclassOf<AActor> SecondaryAttackProjectileClass;

	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackDelay;
	
	UPROPERTY(EditAnywhere, Category="Utility")
	TSubclassOf<AActor> UtilityProjectileClass;

	TSubclassOf<AActor> CurrentProjectileClass;
	
	DECLARE_DELEGATE_OneParam(FProjectileDelegate, TSubclassOf<AActor>);
	
	FTimerHandle TimerHandle_Attack;

	
public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	void CrosshairImpactRelativeRotation(FRotator& Rotator, const FVector& Start, FVector& End,const FCollisionObjectQueryParams&  ObjectQueryParams, float Range);

protected:
	// Show the variables anywhere in the editor. Important for being able to modify in Editor
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;
	
	bool bCharacterSprinting;

	UPROPERTY(EditAnywhere)
	float SprintMultiplier;

	UPROPERTY(VisibleAnywhere)
	float CharacterMaxWalkSpeed ;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(EditAnywhere)
	bool bShowDebug;
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Movement functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Sprint();
	void Attack(TSubclassOf<AActor> ClassToSpawn);
	void Attack_TimeElapsed();
	void PrimaryInteract();

	void SpawnProjectile();
	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
