// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"

#include "SGameplayInterface.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void USInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void USInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void USInteractionComponent::PrimaryInteract(const FVector& Start, FVector& End,  const FRotator& Rotation, FCollisionObjectQueryParams ObjectQueryParams, float Range)
{
	// Only one query for now
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();
	TArray<FHitResult> Hits;

	float Radius = 30.0f;
	FCollisionShape Shape;
	Shape.SetSphere(Radius);
	
	// Sweeping -> "Moves" a sphere, tries to find overlaps
	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, Start, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColor = bBlockingHit ? FColor::Green : FColor::Red;
	
	for(FHitResult Hit : Hits)
	{
		// If something is hit
		AActor* HitActor = Hit.GetActor();
		if(HitActor)
		{
			// When implementing, use U and not I
			if(HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);
				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
			}
			End = Hit.ImpactPoint;
			DrawDebugSphere(GetWorld(), End, Radius, 32,LineColor, false, 2.0f);
		}
	}
	DrawDebugLine(GetWorld(), Start, End, LineColor, false, 2.0f, 0, 2.0f);
}
