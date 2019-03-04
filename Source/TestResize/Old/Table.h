// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "Scalable.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Table.generated.h"

class AHandle;
class ULegGeneratorComponent;

UCLASS()
class TESTRESIZE_API ATable : public AActor, public IScalable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATable();
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // MESH GENERATION RELATED:
    UPROPERTY(Category = MeshData, EditAnywhere, BlueprintReadOnly)
        TArray<FVector> Vertices;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "ChairsRow")
        TSubclassOf<class AChairsRow> ChairsRow;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "9PatchThreshold")
        float Patch9Threshold;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(Category = Components, EditDefaultsOnly, DisplayName = "LegGeneratorComponent")
        ULegGeneratorComponent* TableLegsGeneratorComponent;
    
    UPROPERTY(Category = Components, EditDefaultsOnly, DisplayName = "ProceduralMeshComponent")
        UProceduralMeshComponent* ProceduralMesh;

public:

    //UPROPERTY(VisibleAnywhere)
    //UStaticMeshComponent* TopStyle;

    void ScaleAlong(Direction direction, FVector amount) override;

private:

    TArray<FVector*> VertexPartitions[8];
    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FColor> VertexColors;

    void PartitionVertices();
    void CreateTableTop();
    void UpdateTableTop();

    void GetHandleCoordinates(FVector locations[8]);


    FVector InitialHandleCoordinates[8];
    FVector RelativeInitialHandleCoordinates[8];
    AHandle* ResizeHandles[8];
    //void SetHandleCoordinates(const FVector locations[8]);
    //void UpdateHandleCoordinates();
    //void UpdateChairsCoordinates();
};
