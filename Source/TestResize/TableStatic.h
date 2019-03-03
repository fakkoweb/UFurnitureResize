// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Scalable.h"
#include "TableStatic.generated.h"

class AHandle;
class ULegGeneratorComponent;
class UChairGeneratorComponent;

UCLASS()
class TESTRESIZE_API ATableStatic : public AActor, public IScalable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATableStatic();

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Surface Mesh")
        UStaticMesh* TopStyle;

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Top X Dimension")
        float SurfaceXDimension;

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Top Y Dimension")
        float SurfaceYDimension;

    UPROPERTY(Category = TableSettings, EditAnywhere, DisplayName = "Top Z Dimension")
        float SurfaceZDimension;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(Category = Components, EditDefaultsOnly, DisplayName = "SurfaceMeshComponent")
        UStaticMeshComponent* SurfaceMeshComponent;

    UPROPERTY(Category = Components, EditDefaultsOnly, DisplayName = "LegGeneratorComponent")
        ULegGeneratorComponent* TableLegsGeneratorComponent;

    UPROPERTY(Category = Components, EditDefaultsOnly, DisplayName = "ChairGeneratorComponent")
        UChairGeneratorComponent* TableChairsGeneratorComponent;

private:
    FVector lastTopCompensateLocation;
    FVector lastTopScale;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
    void PostActorCreated();
    void PostLoad();

    void ScaleAlong(Direction direction, FVector amount) override;

    void GetHandleCoordinates(FVector locations[8]);
	
	
};
