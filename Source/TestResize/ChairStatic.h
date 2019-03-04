// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChairStatic.generated.h"

class ULegGeneratorComponent;

UCLASS()
class TESTRESIZE_API AChairStatic : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChairStatic();

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Width")
        float ChairWidth;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Thickness")
        float ChairThickness;

    UPROPERTY(Category = MeshData, EditAnywhere, DisplayName = "Chair Height")
        float ChairHeight;

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Surface Mesh")
        UStaticMesh* TopStyle;

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Back Mesh")
        UStaticMesh* BackStyle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(Category = Components, EditAnywhere, DisplayName = "BackMeshComponent")
        USceneComponent* BackMeshComponent;

    UPROPERTY(Category = Components, EditAnywhere, DisplayName = "SurfaceMeshComponent")
        UStaticMeshComponent* SurfaceMeshComponent;

    UPROPERTY(Category = Components, EditAnywhere, DisplayName = "LegGeneratorComponent")
        ULegGeneratorComponent* ChairLegsGeneratorComponent;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
