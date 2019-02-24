// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "UObject/Interface.h"
#include "Scalable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UScalable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class TESTRESIZE_API IScalable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    virtual void ScaleAlong(Direction direction, FVector amount) = 0;
	
};
