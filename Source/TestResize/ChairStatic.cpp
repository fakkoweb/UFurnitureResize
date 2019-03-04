// Fill out your copyright notice in the Description page of Project Settings.

#include "ChairStatic.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "LegGeneratorComponent.h"

// Sets default values
AChairStatic::AChairStatic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    //ChairMeshComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ChairMesh"))->GetRootComponent();
    //RootComponent = ChairMeshComponent;

    SurfaceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChairSurfaceMesh"));
    SurfaceMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
    //BackMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

    ChairLegsGeneratorComponent = CreateDefaultSubobject<ULegGeneratorComponent>(TEXT("ChairLegGenerator"));
}

// Called when the game starts or when spawned
void AChairStatic::BeginPlay()
{
	Super::BeginPlay();

    SurfaceMeshComponent->SetStaticMesh(TopStyle);

    FVector wantedDimension(ChairWidth*0.01f, ChairWidth*0.01f, ChairThickness);
    //wantedDimension *= 0.01f; // use cm as base
    SurfaceMeshComponent->SetRelativeScale3D(wantedDimension);

    RootComponent->UpdateBounds();

    ChairLegsGeneratorComponent->UpdateLegs();
}

// Called every frame
void AChairStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

