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
 * Interface classes are useful for ensuring that a set of (potentially) unrelated classes implement a common set of functions.
 * This is very useful in cases where some game functionality may be shared by large, complex classes that are otherwise dissimilar. 
 * For example, a game might have a system whereby entering a trigger volume can activate traps, alert enemies, or award points to the player. 
 * This might be implemented by a "ReactToTrigger" function on traps, enemies, and point-awarders. However, traps may be derived from AActor, 
 * enemies from a specialized APawn or ACharacter subclass, and point-awards from UDataAsset. All of these classes need shared functionality, 
 * but have no common ancestor other than UObject. In this case, an interface is recommended.
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
	
	// If we were doing a pure C++ implementation, this function would have to be virtual:
	// virtual void Interact(APawn* InstigatorPawn);
	
};
