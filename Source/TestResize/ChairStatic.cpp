// Fill out your copyright notice in the Description page of Project Settings.

#include "ChairStatic.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "LegGeneratorComponent.h"

// Sets default values
AChairStatic::AChairStatic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ChairMeshGroupComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ChairMeshGroup"));
    RootComponent = ChairMeshGroupComponent;

    SurfaceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairSurfaceMesh"));
    SurfaceMeshComponent->AttachToComponent(ChairMeshGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);

    BackMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairBackMesh"));
    BackMeshComponent->AttachToComponent(ChairMeshGroupComponent, FAttachmentTransformRules::KeepRelativeTransform);

    ChairLegsGeneratorComponent = CreateDefaultSubobject<ULegGeneratorComponent>(TEXT("ChairLegGenerator"));
}

// Called when the game starts or when spawned
void AChairStatic::BeginPlay()
{
	Super::BeginPlay();

    SurfaceMeshComponent->SetStaticMesh(TopStyle);
    BackMeshComponent->SetStaticMesh(BackStyle);

    FVector wantedSurfaceDimension(ChairWidth*0.01f, ChairWidth*0.01f, ChairThickness);
    //wantedDimension *= 0.01f; // use cm as base
    SurfaceMeshComponent->SetRelativeScale3D(wantedSurfaceDimension);

    FVector wantedBackDimension(0.3f, ChairWidth*0.01f, 2);
    //wantedDimension *= 0.01f; // use cm as base
    BackMeshComponent->SetRelativeLocation(FVector(10, 0, 0));
    BackMeshComponent->SetWorldScale3D(wantedBackDimension);


    RootComponent->UpdateBounds();

    ChairLegsGeneratorComponent->UpdateLegs();
}

// Called every frame
void AChairStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

