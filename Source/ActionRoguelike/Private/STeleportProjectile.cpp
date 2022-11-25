// Fill out your copyright notice in the Description page of Project Settings.


#include "STeleportProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

ASTeleportProjectile::ASTeleportProjectile()
{
	EmitterEffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EmitterEffectComp");
	EmitterEffectComp->SetupAttachment(RootComponent);
	
	SphereComp->SetEnableGravity(false);

	// Set our default delays
	ExplodeDelay = 0.2f;
	TeleportAfterExplodeDelay = 0.2f;
}

void ASTeleportProjectile::BeginPlay()
{
	Super::BeginPlay();
	SphereComp->IgnoreActorWhenMoving(ProjectileOwner, true);
	
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle,this, &ASTeleportProjectile::SpawnEmitter, 0.1f,  false, ExplodeDelay);
	
}



void ASTeleportProjectile::SpawnEmitter()
{
	
	MovementComp->StopSimulating(ProjectileHitResult);
	// Destroy our base effect, no longer needed
	EffectComp->DestroyComponent();
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EmitterEffectComp->Template, RootComponent->GetComponentLocation());

	
	GetWorldTimerManager().SetTimer(TeleportTimerHandle, this, &ASTeleportProjectile::TeleportInstigator, 0.05f, false, TeleportAfterExplodeDelay);
	
}

void ASTeleportProjectile::TeleportInstigator()
{
	FVector TeleportLocation = ProjectileHitResult.GetActor() ? ProjectileHitResult.ImpactPoint : GetActorLocation();
	const bool bTeleport = ProjectileOwner->TeleportTo(TeleportLocation, ProjectileOwner->GetActorRotation());

	// If failed, lets try to teleport along the same plane
	if(!bTeleport)
	{
		TeleportLocation.Z = ProjectileOwner->GetActorLocation().Z;
		ProjectileOwner->TeleportTo(TeleportLocation, ProjectileOwner->GetActorRotation());
	}

	// Finally... Destroy our projectile
	Destroy();
}

void ASTeleportProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplodeTimerHandle.Invalidate();
	SpawnEmitter();
	
}

void ASTeleportProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

