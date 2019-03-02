// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerLogic.h"
#include "ResizeTableGameMode.h"
#include "Movable.h"

#include "Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"


const float RAY_LENGTH = 2000;

APlayerLogic::APlayerLogic()
{
    lastDraggingElement = nullptr;

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

    // Is mouse left button down?
    if (IsInputKeyDown(EKeys::LeftMouseButton))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "LeftMouse is Down!");

        FVector2D mouseScreenPos;
        GetMousePosition(mouseScreenPos.X, mouseScreenPos.Y);

        // First frame with mouse pressed? Start dragging
        if (this->lastRaycastHit.GetComponent() == nullptr)
            DraggingStart(mouseScreenPos);

        // Else, update dragging
        else
            DraggingUpdate(mouseScreenPos);
    }

    // Button release
    else
        DraggingStop();
}

///////////////////////////////////////////////////////////////////////////
// Tries to pick an actor from given screen space coordinates and attaches a physic handle to it
void APlayerLogic::DraggingStart(FVector2D screenPos)
{
    // Retrieving mouse position in world
    FVector worldPos, worldDir;
    DeprojectScreenPositionToWorld(screenPos.X, screenPos.Y, worldPos, worldDir);

    // Ray-tracing to find a pickable actor
    this->lastRaycastHit.Reset();
    FVector rayStart = worldPos;
    FVector rayEnd = rayStart + worldDir * RAY_LENGTH;
    if (GetWorld()->LineTraceSingleByChannel(this->lastRaycastHit, rayStart, rayEnd, ECollisionChannel::ECC_Visibility))
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Traced");
        AActor* selectedActor = Cast<AActor>(this->lastRaycastHit.GetComponent()->GetOwner());
        if (selectedActor && selectedActor->Tags.Contains(USER_INTERACTIVE_TAG))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Selected interactive actor " + selectedActor->GetFName().ToString());

            // Updating the GameMode
            ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());
            gameMode->SetSelection(selectedActor);

            // Keep track of the actor if draggable
            lastDraggingElement = Cast<IMovable>(selectedActor);
        }
        else
        {
            // Picking a not-interactive actor
            this->lastRaycastHit.Reset();
            lastDraggingElement = nullptr;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// Updates physic handle using given screen space coordinates
void APlayerLogic::DraggingUpdate(FVector2D screenPos)
{
    FVector worldPos, worldDir;
    DeprojectScreenPositionToWorld(screenPos.X, screenPos.Y, worldPos, worldDir);
    FVector target = worldPos + worldDir * this->lastRaycastHit.Distance;
    lastDraggingElement->MoveTo(target);

#if defined(UE_BUILD_DEBUG)
    // Draw debug arrow
    DrawDebugDirectionalArrow(GetWorld(), this->lastRaycastHit.ImpactPoint, target, 10, FColor::Red, false, -1.0f, 0, 1.0f);
#endif
}

///////////////////////////////////////////////////////////////////////////
// Releases the physic handle
void APlayerLogic::DraggingStop()
{
    if (this->lastRaycastHit.GetComponent())
    {
        // Updating the GameMode
        ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());
        gameMode->ResetSelection();

        this->lastRaycastHit.Reset();
        lastDraggingElement = nullptr;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deselected actor");
    }
}

