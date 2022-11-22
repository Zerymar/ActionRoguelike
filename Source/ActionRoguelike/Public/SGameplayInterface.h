// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SGameplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class USGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONROGUELIKE_API ISGameplayInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	//BlueprintImplementableEvent -> Only use if you plan to implement in the Blueprint UI and not C++
	// BlueprintNativeEvent -> Expects C++ implementation but allows for Blueprint
	// BluePrintCallable allows us to call the function when in the blueprint editor
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void Interact(APawn* InstigatorPawn);
	
};
