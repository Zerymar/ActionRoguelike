// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectile.h"
#include "STeleportProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASTeleportProjectile : public ASProjectile
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	float ExplodeDelay;

	UPROPERTY(EditAnywhere)
	float TeleportAfterExplodeDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* EmitterEffectComp;

	FHitResult ProjectileHitResult;
	FTimerHandle ExplodeTimerHandle;
	FTimerHandle TeleportTimerHandle;

	
public:
	ASTeleportProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void SpawnEmitter();
	virtual void TeleportInstigator();

public:
	virtual void Tick(float DeltaTime) override;
};
