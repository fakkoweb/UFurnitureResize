// Fill out your copyright notice in the Description page of Project Settings.

#include "TestResizeGameModeBase.h"
#include "Engine/Classes/Components/PrimitiveComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "ScaleHandleGenerator.h"

///////////////////////////////////////////////////////////////////////////
// Utility that finds the StaticMeshComponent of an actor
inline UPrimitiveComponent* getMesh(AActor* actor)
{
    if (!actor)
        return nullptr;

    TArray<UPrimitiveComponent*> staticMeshes;
    actor->GetComponents<UPrimitiveComponent>(staticMeshes);
    return staticMeshes.Num() > 0 ? staticMeshes[0] : nullptr;
}
inline TArray<UPrimitiveComponent*> getMeshes(AActor* actor)
{
    if (!actor)
        return TArray<UPrimitiveComponent*>();

    TArray<UPrimitiveComponent*> staticMeshes;
    actor->GetComponents<UPrimitiveComponent>(staticMeshes,true);
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



///////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void ATestResizeGameModeBase::BeginPlay()
{
    UScaleHandleGenerator::Init(GetWorld());


}

///////////////////////////////////////////////////////////////////////////
// Called every frame
void ATestResizeGameModeBase::Tick(float deltaTime)
{
}



void ATestResizeGameModeBase::SetInteractive(AActor * element, bool interactive)
{
    if (interactive && !IsInteractive(element))
    {
        element->Tags.Add(USER_INTERACTIVE_TAG);
        getMesh(element)->SetRenderCustomDepth(true);
    }
    else
    {
        element->Tags.Remove(USER_INTERACTIVE_TAG);
        getMesh(element)->SetRenderCustomDepth(false);
    }
}

bool ATestResizeGameModeBase::IsInteractive(AActor * element)
{
    return element->Tags.Contains(USER_INTERACTIVE_TAG);
}


///////////////////////////////////////////////////////////////////////////
// Called to signal a selected element in current game mode
void ATestResizeGameModeBase::SetSelection(AActor* element)
{
    FString playerStr = "User";
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, playerStr + " picks " + element->GetName());

    // Save the picked element
    if(element)
        this->pickedElement = element;

    // Block interactivity on all blocks
    //for (const auto& jengaBlock : this->jengaBlocks)
    //    SetInteractive(jengaBlock, false);

    if (this->pickedElement)
    {
        // Enable highlight and interactivity on the picked one
        SetInteractive(this->pickedElement, true);
        for (const auto& mesh : getMeshes(this->pickedElement))
            mesh->SetRenderCustomDepth(true);
    }
}

///////////////////////////////////////////////////////////////////////////
// Called to signal a selected element in current game mode
void ATestResizeGameModeBase::ResetSelection()
{
    this->pickedElement = nullptr;
}

// Edit mode (on/off)
bool ATestResizeGameModeBase::RequestEditMode(bool active)
{
    if (pickedElement)
    {
        ;//TODO
    }
    return false;
}