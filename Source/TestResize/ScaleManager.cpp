// Fill out your copyright notice in the Description page of Project Settings.


#include "ScaleManager.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

ScaleManager::ScaleManager()
{
}

ScaleManager::~ScaleManager()
{
}

void ScaleManager::Init(UWorld* world)
{
    if (world)
    {
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = nullptr;
        for (int i = 0; i < 8; i++)
        {
            ResizeHandles[i] = world->SpawnActor<AHandle>(ResizeHandleActor, FVector::ZeroVector, FRotator::ZeroRotator, spawnParams);
            ResizeHandles[i]->Tags.Add("Interactive");
            ResizeHandles[i]->Tags.Add("Draggable");
            ResizeHandles[i]->SetDirection((Direction)i);
            //ResizeHandles[i]->->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
            ResizeHandles[i]->SetManager(this);
        }

        ResetHandles();
    }
}

void ScaleManager::SetScalable(IScalable * actor)
{
    if (actor)
    {
        if (actor != currentScalingActor)
        {
            currentScalingActor = actor;
            UpdateHandles(currentScalingActor);
        }
    }
    else
    {
        currentScalingActor = nullptr;
        ResetHandles();
    }
}

IScalable* ScaleManager::GetScalable()
{
    return currentScalingActor;
}

void ScaleManager::GetHandlesCoordinates(const AActor* currentScalingActor, FVector locations[8], FRotator rotations[8])
{
    if (currentScalingActor)
    {
        FVector origin, extension;
        currentScalingActor->GetActorBounds(true, origin, extension);

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
            rotations[i] = currentScalingActor->GetActorRotation();
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



void ScaleManager::UpdateHandles(const IScalable* currentScalingActor)
{
    FVector updatedLoc[8];
    FRotator updatedRot[8];
    GetHandlesCoordinates((AActor*)currentScalingActor, updatedLoc, updatedRot);
    for (int i = 0; i < 8; i++)
    {
        ResizeHandles[i]->SetActorLocation(updatedLoc[i]);
        ResizeHandles[i]->SetActorRotation(updatedRot[i]);
    }
}

void ScaleManager::ResetHandles()
{
    for (int i = 0; i < 8; i++)
    {
        ResizeHandles[i]->SetActorLocation(FVector(-1000,-1000,-1000));
        savedHandlesPositions[i] = FVector::ZeroVector;
        savedHandlesRotations[i] = FRotator::ZeroRotator;
    }
}

void ScaleManager::SaveState()
{
    for (int i = 0; i < 8; i++)
    {
        savedHandlesPositions[i] = ResizeHandles[i]->GetActorLocation();
        savedHandlesRotations[i] = ResizeHandles[i]->GetActorRotation();
    }
}

void ScaleManager::RevertState()
{
    for (int i = 0; i < 8; i++)
    {
        ResizeHandles[i]->MoveTo(savedHandlesPositions[i]);
        ResizeHandles[i]->SetActorRotation(savedHandlesRotations[i]);
    }
}
