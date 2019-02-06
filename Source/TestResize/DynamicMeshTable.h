// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "DynamicMeshTable.generated.h"

UCLASS()
class TESTRESIZE_API ADynamicMeshTable : public AActor
{
	GENERATED_BODY()
	
public:	
    // Sets default values for this actor's properties
    ADynamicMeshTable();

    UPROPERTY(VisibleAnywhere)
        UProceduralMeshComponent* ProceduralMesh;

    UPROPERTY(Category = Vertices, EditAnywhere, BlueprintReadOnly)
        TArray<FVector> Vertices;

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Handle Mesh")
        TSubclassOf<class AActor> ResizeHandle;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    //void PostEditChangeProperty(FPropertyChangedEvent & e);

    void PostEditChangeChainProperty(FPropertyChangedChainEvent & e);

    void PostActorCreated();

    void PostLoad();

private:

    void CreateTriangle();
    void CreateSquare();
    void UpdateSquare();
    void GenerateHandleCoordinates();

    TArray<int32> Triangles;
    TArray<FVector> Normals;
    TArray<FVector2D> UV0;
    TArray<FColor> VertexColors;

    enum Direction
    {
        N, E, S, W, NE, SE, SW, NW
    };

    FVector InitialHandleCoordinates[8];
    AActor* ResizeHandles[8];



	
};
