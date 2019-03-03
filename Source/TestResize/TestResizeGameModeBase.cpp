// Fill out your copyright notice in the Description page of Project Settings.

#include "TestResizeGameModeBase.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

///////////////////////////////////////////////////////////////////////////
// Utility that finds the StaticMeshComponent of an currentScalingActor
inline UPrimitiveComponent* getMesh(AActor* currentScalingActor)
{
    if (!currentScalingActor)
        return nullptr;

    TArray<UPrimitiveComponent*> staticMeshes;
    currentScalingActor->GetComponents<UPrimitiveComponent>(staticMeshes);
    return staticMeshes.Num() > 0 ? staticMeshes[0] : nullptr;
}
inline TArray<UPrimitiveComponent*> getMeshes(AActor* currentScalingActor)
{
    if (!currentScalingActor)
        return TArray<UPrimitiveComponent*>();

    TArray<UPrimitiveComponent*> staticMeshes;
    currentScalingActor->GetComponents<UPrimitiveComponent>(staticMeshes,true);
    return staticMeshes.Num() > 0 ? staticMeshes : TArray<UPrimitiveComponent*>();
}

//Adds a UActorComponent Subclass, that is based on the passed in Class, and added to the Outer(Actor) object
UActorComponent* AddComponentByClass(TSubclassOf<UActorComponent> Class, UObject* Outer)
{
    if (Class != nullptr && Outer != nullptr)
    {
        UActorComponent* Component = NewObject<UActorComponent>(Outer, *Class);
        if (Component != nullptr)
        {
            Component->RegisterComponent();
            return Component;
        }
        else
        {
            return nullptr;
        }
    }

    return nullptr;
}

//Removes a UActorComponent Subclass, that is based on the passed in Class, from an Outer(Actor) object
UActorComponent* RemoveComponentByClass(TSubclassOf<UActorComponent> Class, UObject* Outer)
{
    if (Class != nullptr && Outer != nullptr)
    {
        UActorComponent* Component = NewObject<UActorComponent>(Outer, *Class);
        if (Component != nullptr)
        {
            Component->RegisterComponent();
            return Component;
        }
        else
        {
            return nullptr;
        }
    }

    return nullptr;
}

///////////////////////////////////////////////////////////////////////////




ATestResizeGameModeBase::ATestResizeGameModeBase()
{
    currentMode = GameMode::SelectionMode;
}

///////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void ATestResizeGameModeBase::BeginPlay()
{
    scaleMan.ResizeHandleActor = ResizeHandleActor;
    scaleMan.Init(GetWorld());
}

///////////////////////////////////////////////////////////////////////////
// Called every frame
void ATestResizeGameModeBase::Tick(float deltaTime)
{
}



void ATestResizeGameModeBase::SetSelected(AActor * element, bool interactive)
{
    if (interactive && !IsSelected(element))
    {
        element->Tags.Add(USER_SELECTED_TAG);
        getMesh(element)->SetRenderCustomDepth(true);
        for (const auto& mesh : getMeshes(element))
            mesh->SetRenderCustomDepth(true);
    }
    else
    {
        element->Tags.Remove(USER_SELECTED_TAG);
        getMesh(element)->SetRenderCustomDepth(false);
        for (const auto& mesh : getMeshes(element))
            mesh->SetRenderCustomDepth(false);
    }
}

bool ATestResizeGameModeBase::IsSelected(AActor * element)
{
    return element->Tags.Contains(USER_SELECTED_TAG);
}

bool ATestResizeGameModeBase::SetEditMode(bool active)
{
    if (active)
    {
        scaleMan.SetScalable((IScalable*)SelectedElement);
        return true;
    }
    else
    {
        scaleMan.SetScalable(nullptr);
        return false;
    }
}


///////////////////////////////////////////////////////////////////////////
// Called to signal a selected element in current game mode
void ATestResizeGameModeBase::SetSelection(AActor* element)
{

    // Block interactivity on all blocks
    //for (const auto& jengaBlock : this->jengaBlocks)
    //    SetSelected(jengaBlock, false);

    // Save the picked element
    if (element)
    {
        FString playerStr = "User";
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, playerStr + " selects " + element->GetName());

        // Enable highlight and interactivity on the picked one
        SetSelected(element, true);

        // Save the picked element
        this->SelectedElement = element;
        currentMode = GameMode::SelectedMode;
    }
    else
    {
        if (this->SelectedElement)
        {
            FString playerStr = "User";
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, playerStr + " deselects " + this->SelectedElement->GetName());
        }
        
        // Reset the picked element
        this->SelectedElement = nullptr;
        currentMode = GameMode::SelectionMode;
    }
}

///////////////////////////////////////////////////////////////////////////
// Called to signal a selected element in current game mode
void ATestResizeGameModeBase::ResetSelection()
{
    this->SelectedElement = nullptr;
}

// Edit mode (on/off)
bool ATestResizeGameModeBase::RequestSwitchEditMode()
{
    if (SelectedElement)
    {
        if (currentMode == GameMode::SelectedMode && Cast<IScalable>(SelectedElement))
        {
            SetEditMode(true);
            currentMode = GameMode::EditMode;
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "EDIT MODE");
            return true;
        }
        else if (currentMode == GameMode::EditMode)
        {
            return RequestAbortEdit();
        }
    }
    return false;
}

// Edit mode (on/off)
bool ATestResizeGameModeBase::RequestSaveEdit()
{
    if (SelectedElement && currentMode==GameMode::EditMode)
    {
        SetEditMode(false);
        currentMode = GameMode::SelectedMode;
        return true;
    }
    return false;
}

// Edit mode (on/off)
bool ATestResizeGameModeBase::RequestAbortEdit()
{
    if (SelectedElement)
    {
        // TODO: REVERT EDIT
        SetEditMode(false);
        currentMode = GameMode::SelectedMode;
        return true;
    }
    return false;
}


void ATestResizeGameModeBase::ClickOnActor(AActor * element)
{
    switch (currentMode)
    {
    case GameMode::SelectionMode:
        SetSelection(element);
        break;
    case GameMode::SelectedMode:
        if (element != SelectedElement)
        {
            SetSelection(element);
        }
        break;
    case GameMode::EditMode:
        //Cast<IMovable>(element);
        break;

    default:
        break;
    }
}

void ATestResizeGameModeBase::DragOnActor(AActor * element, FVector toTarget)
{
    IMovable* movable = Cast<IMovable>(element);

    switch (currentMode)
    {
    case GameMode::EditMode:
        if (movable)
            movable->MoveTo(toTarget);
        break;

    default:
        break;
    }
}
