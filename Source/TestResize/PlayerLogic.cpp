// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerLogic.h"
#include "ResizeTableGameMode.h"
#include "Movable.h"

#include "Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "DrawDebugHelpers.h"


const float RAY_LENGTH = 2000;

APlayerLogic::APlayerLogic()
{
    currentHitActor = nullptr;
    LeftMouseButton_WasDown = false;

    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}


///////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void APlayerLogic::BeginPlay()
{
    Super::BeginPlay();
}


///////////////////////////////////////////////////////////////////////////
// Called every frame
void APlayerLogic::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    FVector2D mouseScreenPos;
    GetMousePosition(mouseScreenPos.X, mouseScreenPos.Y);

    // Is mouse left button down?
    if (IsInputKeyDown(EKeys::LeftMouseButton))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "LeftMouse is Down!");

        // Was button already down?
        if (LeftMouseButton_WasDown)
            DraggingUpdate(mouseScreenPos);
        // First time it is down?
        else
        {
            MouseClick(mouseScreenPos);
            DraggingStart();
            LeftMouseButton_WasDown = true;
        }

        //if (this->lastRaycastHit.GetComponent() == nullptr)

    }
    // Button release
    else
    {

        if (LeftMouseButton_WasDown)
        {
            MouseRelease(mouseScreenPos);
            DraggingStop();
        }

        LeftMouseButton_WasDown = false;
    }
}

///////////////////////////////////////////////////////////////////////////
// Clicks on a specific position
void APlayerLogic::MouseClick(FVector2D screenPos)
{
    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());

    // Retrieving mouse position in world
    FVector worldPos, worldDir;
    DeprojectScreenPositionToWorld(screenPos.X, screenPos.Y, worldPos, worldDir);

    // Ray-tracing to find an interactive currentScalingActor
    this->lastRaycastHit.Reset();
    FVector rayStart = worldPos;
    FVector rayEnd = rayStart + worldDir * RAY_LENGTH;
    if (GetWorld()->LineTraceSingleByChannel(this->lastRaycastHit, rayStart, rayEnd, ECollisionChannel::ECC_Visibility))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Traced");
        AActor* hitActor = Cast<AActor>(this->lastRaycastHit.GetComponent()->GetOwner());
        if (hitActor && hitActor->Tags.Contains(USER_INTERACTIVE_TAG))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Clicked on interactive actor " + hitActor->GetFName().ToString());

            // Updating the GameMode
            gameMode->ClickOnActor(hitActor);

            // Keep track of the currentScalingActor if draggable
            currentHitActor = hitActor;
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Clicked on NON interactive actor " + hitActor->GetFName().ToString());

            // Updating the GameMode
            gameMode->ClickOnActor(nullptr);

            // Reset picking
            this->lastRaycastHit.Reset();
            currentHitActor = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// Releases the physic handle
void APlayerLogic::MouseRelease(FVector2D screenPos)
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Mouse released");

    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());

    // Reset picking
    this->lastRaycastHit.Reset();
    currentHitActor = nullptr;
}

///////////////////////////////////////////////////////////////////////////
// Starts dragging on the current hit currentScalingActor
void APlayerLogic::DraggingStart()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Drag started");

    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());

    if(currentHitActor)
        dragRaycast = FPlane(currentHitActor->GetActorLocation() , FVector::UpVector);
    //gameMode->DragOnActor(currentHitActor, currentHitActor->GetActorLocation());
}

///////////////////////////////////////////////////////////////////////////
// Updates physic handle using given screen space coordinates
void APlayerLogic::DraggingUpdate(FVector2D screenPos)
{
    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());

    if (currentHitActor)
    {
        FVector worldPos, worldDir;
        DeprojectScreenPositionToWorld(screenPos.X, screenPos.Y, worldPos, worldDir);   // finds intersection between mouse and near clip plane

        FVector target = worldPos + worldDir * RAY_LENGTH;
        FVector newPosition = FMath::LinePlaneIntersection(worldPos, target, dragRaycast);

        //if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), worldPos, target,))
        //{
        //}

        gameMode->DragOnActor(currentHitActor, newPosition);

#if defined(UE_BUILD_DEBUG)
        // Draw debug arrow
        DrawDebugDirectionalArrow(GetWorld(), this->lastRaycastHit.ImpactPoint, target, 10, FColor::Red, false, -1.0f, 0, 1.0f);
#endif
    }

}

///////////////////////////////////////////////////////////////////////////
// Releases the physic handle
void APlayerLogic::DraggingStop()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Dragging stopped on actor");

    if (currentHitActor)
    {

    }
}

