// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"

#include "SAttributeComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
	// Our magic projectile should be faster than the default 500.0f
	MovementComp->InitialSpeed = 2000.0f;
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);
	
	bDestroyOnHit = true;
	
}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if(OtherActor && OtherActor != GetInstigator())
	{
		// Iterate through all components, find the attributecomponent
		//Cast it to ensure that's what we get
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if(AttributeComp)
		{
			AttributeComp->ApplyHealthChange(-20.0f);
			Explode();
		}
	}
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

void ASMagicProjectile::LifeSpanExpired()
{
	Explode();
}

void ASMagicProjectile::Explode_Implementation()
{
	Super::Explode_Implementation();
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

