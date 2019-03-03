// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RestrictedMovementActor.h"
#include "GameFramework/PlayerController.h"
#include "PlayerLogic.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMouseUp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMouseDown);

/**
 * 
 */
UCLASS()
class TESTRESIZE_API APlayerLogic : public APlayerController
{
    GENERATED_BODY()

public:
    APlayerLogic();

    UPROPERTY(Category = "Events", BlueprintAssignable)
        FOnMouseUp onMouseUpEvent;

    UPROPERTY(Category = "Events", BlueprintAssignable)
        FOnMouseDown onMouseDownEvent;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:

    // Click events
    void MouseClick(FVector2D screenPos);
    void MouseRelease(FVector2D screenPos);

    // Drag events
    void DraggingStart();
    void DraggingUpdate(FVector2D screenPos);
    void DraggingStop();

    FHitResult lastRaycastHit;
    AActor* currentHitActor;
    bool LeftMouseButton_WasDown;
    FVector2D lastMouseClickScreenPos;
    FPlane dragRaycast;
	
};

