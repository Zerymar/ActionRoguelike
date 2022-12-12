// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SProjectileBase.generated.h"
class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;

UCLASS()
class ACTIONROGUELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere)
	bool bDestroyOnHit;
	
	AActor* ProjectileOwner;
public:
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:
	
	// For collision, use a sphere component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	USphereComponent* SphereComp;

	// Movement Component that gives an object velocity to go in a straight line
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UProjectileMovementComponent* MovementComp;

	//Particle system component
	// TO show something in the world.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UParticleSystemComponent* EffectComp;

	UPROPERTY(EditDefaultsOnly,Category="Effects", BlueprintReadOnly)
	UParticleSystem* ImpactVFX;
	
	// Our audio component for every projectile
	UPROPERTY(EditDefaultsOnly, Category="Effects", BlueprintReadOnly)
	UAudioComponent* AudioComp;

	// Want to despawn when colliding
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Explode();
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
