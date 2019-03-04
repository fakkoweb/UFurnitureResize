// Fill out your copyright notice in the Description page of Project Settings.

#include "ScaleHandleGenerator.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"


AHandle*  UScaleHandleGenerator::ResizeHandles[8] = { nullptr };

// Sets default values for this component's properties
UScaleHandleGenerator::UScaleHandleGenerator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


void UScaleHandleGenerator::Init(UWorld* world)
{
    if (world)
    {
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = nullptr;
        for (int i = 0; i < 8; i++)
        {
            ResizeHandles[i] = world->SpawnActor<AHandle>(AHandle::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
            ResizeHandles[i]->Tags.Add(("Draggable"));
            ResizeHandles[i]->SetDirection((Direction)i);
        }
    }
}

// Called when the game starts
void UScaleHandleGenerator::BeginPlay()
{
    Super::BeginPlay();

	// ...
    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ScaleHandleGenerator BeginPlay()");
}

void UScaleHandleGenerator::OnComponentCreated()
{
    Super::OnComponentCreated();

    for (int i = 0; i < 8; i++)
    {
        UScaleHandleGenerator::ResizeHandles[i]->AttachToActor(this->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
    }
    UpdateHandles();

}

void UScaleHandleGenerator::OnComponentDestroyed(bool bDestroyingHierarchy)
{
    Super::OnComponentDestroyed(bDestroyingHierarchy);

    for (int i = 0; i < 8; i++)
    {
        UScaleHandleGenerator::ResizeHandles[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    }
    UpdateHandles();

}


// Called every frame
void UScaleHandleGenerator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UScaleHandleGenerator::GetHandlesCoordinates(FVector locations[8], FRotator rotations[8])
{
    AActor* owner = GetOwner();
    if (owner)
    {
        FVector origin, extension;
        owner->GetActorBounds(true, origin, extension);

        locations[Direction::NE] = origin + extension;
        extension.X = -extension.X;
        locations[Direction::SE] = origin + extension;
        extension.Y = -extension.Y;
        locations[Direction::SW] = origin + extension;
        extension.X = -extension.X;
        locations[Direction::NW] = origin + extension;
        locations[Direction::E] = FVector((locations[Direction::NE].X + locations[Direction::SE].X) / 2, locations[Direction::NE].Y, locations[Direction::NE].Z);
        locations[Direction::W] = FVector((locations[Direction::NW].X + locations[Direction::SW].X) / 2, locations[Direction::NW].Y, locations[Direction::NW].Z);
        locations[Direction::N] = FVector(locations[Direction::NE].X, (locations[Direction::NW].Y + locations[Direction::NE].Y) / 2, locations[Direction::NE].Z);
        locations[Direction::S] = FVector(locations[Direction::SE].X, (locations[Direction::SE].Y + locations[Direction::SW].Y) / 2, locations[Direction::SE].Z);

        for (int i = 0; i < 8; i++)
        {
            rotations[i] = owner->GetActorRotation();
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            locations[i] = FVector::ZeroVector;
            rotations[i] = FRotator::ZeroRotator;
        }
    }
}

void UScaleHandleGenerator::UpdateHandles()
{
    FVector updatedLoc[8];
    FRotator updatedRot[8];
    GetHandlesCoordinates(updatedLoc, updatedRot);
    for (int i = 0; i < 8; i++)
    {
        UScaleHandleGenerator::ResizeHandles[i]->SetActorLocation(updatedLoc[i]);
        UScaleHandleGenerator::ResizeHandles[i]->SetActorRotation(updatedRot[i]);
    }
}

