// Fill out your copyright notice in the Description page of Project Settings.


#include "SProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"

// Sets default values
ASProjectile::ASProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Projectile");
	// Our Project will be our "sphere object" in the world
	RootComponent = SphereComp;

	// Attach the sphere to the particle system
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);
	
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 500.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	bDestroyOnHit = false;

	
	
}

void ASProjectile::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//UE_LOG(LogTemp, Warning, TEXT("Projectile Collision, Destroying"));
	if(bDestroyOnHit)
	{
		Destroy();
	}
}

void ASProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	SphereComp->OnComponentHit.AddDynamic(this, &ASProjectile::OnActorHit);
}


// Called when the game starts or when spawned
void ASProjectile::BeginPlay()
{
	Super::BeginPlay();
	ProjectileOwner = GetInstigator();
	
}

// Called every frame
void ASProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

