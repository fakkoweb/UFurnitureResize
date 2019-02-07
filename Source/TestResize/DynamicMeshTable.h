// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DynamicMeshTable.generated.h"

class ADynamicMeshTableHandle;

UCLASS()
class TESTRESIZE_API ADynamicMeshTable : public AActor
{
	GENERATED_BODY()
	
public:	
    // Sets default values for this actor's properties
    ADynamicMeshTable();

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Handle Mesh")
        TSubclassOf<class ADynamicMeshTableHandle> ResizeHandle;

    UPROPERTY(Category = MeshData, EditAnywhere, BlueprintReadOnly)
        TArray<FVector> Vertices;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Table Leg Width")
        float TableLegWidth;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair")
        TSubclassOf<class AActor> Chair;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    enum Direction
    {
        N, E, S, W, NE, SE, SW, NW
    };

    // Called every frame
    virtual void Tick(float DeltaTime) override;

    //void PostEditChangeProperty(FPropertyChangedEvent & e);

    void PostEditChangeChainProperty(FPropertyChangedChainEvent & e);

    void PostActorCreated();

    void PostLoad();

    void ScaleAlong(Direction direction, FVector amount);

private:

    UProceduralMeshComponent* ProceduralMesh;
    void CreateTriangle();
    void CreateSquare();
    void UpdateSquare();
    void GenerateHandleCoordinates();
    void UpdateHandleCoordinates();
    void PartitionVertices();

    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FColor> VertexColors;

    FVector InitialHandleCoordinates[8];
    FVector RelativeInitialHandleCoordinates[8];
    ADynamicMeshTableHandle* ResizeHandles[8];
    TArray<FVector*> VertexPartitions[8];




	
};
