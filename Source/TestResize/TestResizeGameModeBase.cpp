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

    TArray<UPrimitiveComponent*> allStaticMeshes;
    currentScalingActor->GetComponents<UPrimitiveComponent>(allStaticMeshes);

    return allStaticMeshes.Num() > 0 ? allStaticMeshes[0] : nullptr;
}
inline TArray<UPrimitiveComponent*> getMeshes(AActor* currentScalingActor)
{
    if (!currentScalingActor)
        return TArray<UPrimitiveComponent*>();

    TArray<UPrimitiveComponent*> allStaticMeshes;
    currentScalingActor->GetComponents<UPrimitiveComponent>(allStaticMeshes);

    TArray<AActor*> childrenActors;
    currentScalingActor->GetAllChildActors(childrenActors, true);
    for (const auto& actor : childrenActors)
    {
        TArray<UPrimitiveComponent*> staticMeshes;
        actor->GetComponents<UPrimitiveComponent>(staticMeshes);
        allStaticMeshes.Append(staticMeshes);
    }

    return allStaticMeshes.Num() > 0 ? allStaticMeshes : TArray<UPrimitiveComponent*>();
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



void ATestResizeGameModeBase::SetSelected(AActor * element, bool selected)
{
    if (!element)
        return;

    TArray<UPrimitiveComponent*> elementMeshes = getMeshes(element);
    if (!IsSelected(element))
    {
        element->Tags.Add(USER_SELECTED_TAG);
        BackupMaterial = elementMeshes[0]->GetMaterial(0);
        for (const auto& mesh : elementMeshes)
        {
            //mesh->SetRenderCustomDepth(true);
            mesh->SetMaterial(0, SelectedMaterial);
        }

        // Save the picked element
        this->SelectedElement = element;
    }
    else
    {
        element->Tags.Remove(USER_SELECTED_TAG);
        for (const auto& mesh : elementMeshes)
        {
            //mesh->SetRenderCustomDepth(false);
            mesh->SetMaterial(0, BackupMaterial);
        }
        BackupMaterial = nullptr;

        // Reset the picked element
        this->SelectedElement = nullptr;
    }
}

bool ATestResizeGameModeBase::IsSelected(AActor * element)
{
    return element->Tags.Contains(USER_SELECTED_TAG);
}

bool ATestResizeGameModeBase::SetEditMode(bool active)
{
    TArray<UPrimitiveComponent*> elementMeshes = getMeshes(SelectedElement);

    if (active)
    {
        scaleMan.SetScalable((IScalable*)SelectedElement);

        // Save State that can be reverted
        scaleMan.SaveState();

        for (const auto& mesh : elementMeshes)
        {
            //mesh->SetRenderCustomDepth(true);
            mesh->SetMaterial(0, EditingMaterial);
        }

        return true;
    }
    else
    {
        scaleMan.SetScalable(nullptr);

        for (const auto& mesh : elementMeshes)
        {
            //mesh->SetRenderCustomDepth(true);
            mesh->SetMaterial(0, SelectedMaterial);
        }

        return false;
    }
}


///////////////////////////////////////////////////////////////////////////
// Called to signal a selected element in current game mode
void ATestResizeGameModeBase::SetSelection(AActor* element)
{
    // Save the picked element
    if (element)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "You have selected " + element->GetName());
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, "Element selected. Press E to switch to Edit Mode");

        // Enable highlight and interactivity on the picked one
        SetSelected(element, true);

        // Move to selected mode
        currentMode = GameMode::SelectedMode;
    }
    else
    {
        if (SelectedElement)
        {
            FString playerStr = "User";
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, playerStr + " deselects " + SelectedElement->GetName());
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "No elements selected. LeftClick with Mouse to select.");
            SetSelected(SelectedElement, false);
        }
        
        // Move back to selection mode
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
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "EDIT MODE");
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "EDIT MODE: Press ENTER to save, BACKSPACE to revert modifications.");
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

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, "Edits saved!");

        return true;
    }
    return false;
}

// Edit mode (on/off)
bool ATestResizeGameModeBase::RequestAbortEdit()
{
    if (SelectedElement)
    {
        // Revert saved state
        scaleMan.RevertState();

        SetEditMode(false);
        currentMode = GameMode::SelectedMode;

        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, "Edits reverted!");

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
