// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectileBase.h"
#include "STeleportProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROGUELIKE_API ASTeleportProjectile : public ASProjectileBase
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere)
	float ExplodeDelay;

	UPROPERTY(EditAnywhere)
	float TeleportAfterExplodeDelay;
	
	FHitResult ProjectileHitResult;
	FTimerHandle ExplodeTimerHandle;
	FTimerHandle TeleportTimerHandle;

	
public:
	ASTeleportProjectile();

protected:
	virtual void BeginPlay() override;
	virtual void Explode_Implementation() override;
	virtual void TeleportInstigator();
	

public:
	virtual void Tick(float DeltaTime) override;
};
