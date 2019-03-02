// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RestrictedMovementActor.h"
#include "GameFramework/PlayerController.h"
#include "PlayerLogic.generated.h"


/**
 * 
 */
UCLASS()
class TESTRESIZE_API APlayerLogic : public APlayerController
{
    GENERATED_BODY()

public:
    APlayerLogic();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:

    // Drag events
    void DraggingStart(FVector2D screenPos);
    void DraggingUpdate(FVector2D screenPos);
    void DraggingStop();

    FHitResult lastRaycastHit;
    IMovable* lastDraggingElement;
	
};
