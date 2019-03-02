// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RestrictedMovementActor.h"
#include "DynamicMeshTable.h"
#include "DynamicMeshTableHandle.generated.h"

/**
 * 
 */
UCLASS()
class TESTRESIZE_API ADynamicMeshTableHandle : public ARestrictedMovementActor
{
	GENERATED_BODY()

public:
    ADynamicMeshTable* scalable;

    virtual void MoveTo(const FVector& position) override;
    void SetDirection(const ADynamicMeshTable::Direction direction);

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    ADynamicMeshTable::Direction direction;
	
};
