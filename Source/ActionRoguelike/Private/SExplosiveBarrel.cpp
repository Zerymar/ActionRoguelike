// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"

#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMeshComp->SetSimulatePhysics(true);
	StaticMeshComp->SetCollisionProfileName("PhysicsActor");
	RootComponent = StaticMeshComp;

	// Set up radial force component for our barrel
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->Radius = 600.0f;
	RadialForceComp->ImpulseStrength = 2000.0f;
	RadialForceComp->ForceStrength = 10.0f;
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->SetAutoActivate(false);
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
	RadialForceComp->SetupAttachment(StaticMeshComp);

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(RootComponent);
	
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	StaticMeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnActorHit);
}

void ASExplosiveBarrel::OnActorHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// TEXT: Converts String literal to what Unreal expects
	//  Supports a wider set (unicode most likely?)
	// When you work in Strings, always use TEXT
	UE_LOG(LogTemp, Log, TEXT("OnActorHit in Explosive Barrel"));

	// GetNameSafe allows for null checks
	UE_LOG(LogTemp, Log, TEXT("OtherActor: %s, at game time: %f"), *GetNameSafe(OtherActor), GetWorld()->GetTimeSeconds());

	const FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
	
	// Only if actor is a projectile, should we fire an impulse
	if(OtherComp->GetCollisionProfileName().Compare("Projectile") == 0)
	{
		RadialForceComp->FireImpulse();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EffectComp->Template, RootComponent->GetComponentLocation());
		Destroy();
	}
	//UE_LOG(LogTemp, Warning, TEXT("Barrel Hit!!"));
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASExplosiveBarrel::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);
}

