// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include <stack>

#include "ChairsRow.generated.h"

UCLASS()
class TESTRESIZE_API AChairsRow : public AActor
{
	GENERATED_BODY()
	
public:	

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Width")
        float ChairWidth;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Spacing")
        float ChairSpacing;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Distance")
        float ChairDistance;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair")
        TSubclassOf<class AActor> Chair;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Slider")
        TSubclassOf<class AActor> Empty;

	// Sets default values for this actor's properties
	AChairsRow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
    unsigned int CanBeNChairs(float availableSpace, float chairsize, float chairspacing);

    AActor* ChairRowSlider;
    std::stack<AActor*> Chairs;
    float currentTopSpaceOccupied;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void UpdateChairs(float Space);

};
