// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include <stack>
#include "ChairGeneratorComponent.generated.h"

class AChairStatic;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TESTRESIZE_API UChairGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UChairGeneratorComponent();

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Width")
        float ChairWidth;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Height")
        float ChairHeight;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Spacing")
        float ChairSpacing;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Distance")
        float ChairDistance;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Element")
        TSubclassOf<class AChairStatic> Chair;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Slider")
        TSubclassOf<class AActor> Empty;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    unsigned int CanBeNChairs(float availableSpace, float chairsize, float chairspacing);

    AActor* ChairRowSliders[4];
    std::stack<AActor*> Chairs[4];
    float currentSpaceAvailable[4] = { 0 };
    float currentSpaceOccupied[4] = { 0 };
    bool needsUpdate[4] = { true };

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void SetChairsAvailableSpace(Direction side, float space);
    void UpdateChairs();
    void GetChairSlidersCoordinates(FVector locations[4], FRotator rotations[4]);
	
};
