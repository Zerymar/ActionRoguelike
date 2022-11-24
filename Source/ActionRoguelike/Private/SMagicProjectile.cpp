// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	// Our magic projectile should be faster than the default 500.0f
	MovementComp->InitialSpeed = 2000.0f;

}
/*
void ASMagicProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Projectile Collision, Destroying"));
	Super::OnActorHit(HitComp, OtherActor, OtherComp,NormalImpulse, Hit);
}*/

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

