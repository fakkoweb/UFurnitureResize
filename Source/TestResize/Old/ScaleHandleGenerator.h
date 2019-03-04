// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Handle.h"
#include "ScaleHandleGenerator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTRESIZE_API UScaleHandleGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UScaleHandleGenerator();

    static void Init(UWorld* world);

protected:

    // Keeps track of pre-existing Actors in Scene
    static AHandle* ResizeHandles[8];

	// Called when the game starts
	virtual void BeginPlay() override;
    virtual void OnComponentCreated() override;
    virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

    FVector InitialHandleCoordinates[8];
    FVector RelativeInitialHandleCoordinates[8];

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void GetHandlesCoordinates(FVector locations[8], FRotator rotations[8]);
    void UpdateHandles();
	
};
