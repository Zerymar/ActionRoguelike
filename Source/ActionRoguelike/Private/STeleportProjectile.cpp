// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASTeleportProjectile::ASTeleportProjectile()
{
	// Set our default delays
	ExplodeDelay = 0.2f;
	TeleportAfterExplodeDelay = 0.2f;
	MovementComp->InitialSpeed = 8500.0f;
	
}

void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(ProjectileOwner, true);
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle,this, &ASTeleportProjectile::Explode, 0.1f,  false, ExplodeDelay);
}

void ASTeleportProjectile::Explode_Implementation()
{
	GetWorldTimerManager().ClearTimer(ExplodeTimerHandle);
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactVFX, GetActorLocation(), GetActorRotation());

	// Start "turning off" simulations
	EffectComp->DeactivateSystem();
	MovementComp->StopMovementImmediately();
	SetActorEnableCollision(false);
	
	//Super::Explode_Implementation();
	GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ASTeleportProjectile::TeleportInstigator, 0.1f, false, TeleportAfterExplodeDelay);
}

void ASTeleportProjectile::TeleportInstigator()
{
	if(ensure(ProjectileOwner))
	{
		FVector TeleportLocation = GetActorLocation();
		const bool bTeleport = ProjectileOwner->TeleportTo(TeleportLocation, ProjectileOwner->GetActorRotation());

		// If failed, lets try to teleport along the same plane
		if(!bTeleport)
		{
			TeleportLocation.Z = ProjectileOwner->GetActorLocation().Z;
			ProjectileOwner->TeleportTo(TeleportLocation, ProjectileOwner->GetActorRotation(), false, false);
		}

		// Finally... Destroy our projectile
		Destroy();
	}
}


void ASTeleportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

