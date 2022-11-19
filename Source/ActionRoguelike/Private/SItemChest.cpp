﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "SItemChest.h"


// Sets default values
ASItemChest::ASItemChest()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	TargetPitch = 110.0f;
}

void ASItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	// Rotate relative to the basemesh
	// the base is what it's attached to, so BaseMesh
	// 110 is the sweet spot
	LidMesh->SetRelativeRotation(FRotator(TargetPitch,0,0));
}

// Called when the game starts or when spawned
void ASItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASItemChest::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

