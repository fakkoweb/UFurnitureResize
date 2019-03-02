// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LegGeneratorComponent.generated.h"

class UStaticMesh;
class AStaticMeshActor;
/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TESTRESIZE_API ULegGeneratorComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:
    ULegGeneratorComponent(const FObjectInitializer& OI);

    UPROPERTY(Category = LegSettings, EditAnywhere, DisplayName = "Leg Mesh")
        UStaticMesh* LegStyle;

    UPROPERTY(Category = LegSettings, EditAnywhere, DisplayName = "Leg Width")
        float LegSideDimension;

    UPROPERTY(Category = LegSettings, EditAnywhere, DisplayName = "Leg Height")
        float LegHeight;

    UPROPERTY(Category = LegSettings, EditAnywhere, DisplayName = "Leg Spacing")
        float LegSpacing;

    void GetLegCoordinates(FVector coords[4]);
    void UpdateLegs();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    AStaticMeshActor* LegActors[4];
};
