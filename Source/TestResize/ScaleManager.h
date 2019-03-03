// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Handle.h"
/**
 * 
 */
class TESTRESIZE_API ScaleManager
{
public:
	ScaleManager();
	~ScaleManager();

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Handle Mesh")
        TSubclassOf<class AHandle> ResizeHandleActor;

    void Init(UWorld* world);
    void SetScalable(IScalable* currentScalingActor);
    IScalable* GetScalable();
    void GetHandlesCoordinates(const AActor* currentScalingActor, FVector locations[8], FRotator rotations[8]);
    void UpdateHandles(const IScalable* currentScalingActor);
    void ResetHandles();

    void SaveState();
    void RevertState();

protected:
    // Keeps track of pre-existing Actors in Scene
    AHandle* ResizeHandles[8];
    IScalable* currentScalingActor;

    FVector savedHandlesPositions[8];
    FRotator savedHandlesRotations[8];
};
