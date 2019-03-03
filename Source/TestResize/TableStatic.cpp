// Fill out your copyright notice in the Description page of Project Settings.

#include "TableStatic.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "LegGeneratorComponent.h"
#include "ChairGeneratorComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"


// Sets default values
ATableStatic::ATableStatic()
{
 	// Set this currentScalingActor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SurfaceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
    RootComponent = SurfaceMeshComponent;

    TableLegsGeneratorComponent = CreateDefaultSubobject<ULegGeneratorComponent>(TEXT("TableLegGenerator"));
    TableChairsGeneratorComponent = CreateDefaultSubobject<UChairGeneratorComponent>(TEXT("TableChairGenerator"));
}

// Called when the game starts or when spawned
void ATableStatic::BeginPlay()
{
	Super::BeginPlay();

    SurfaceMeshComponent->SetStaticMesh(TopStyle);

    FVector wantedDimension(SurfaceXDimension, SurfaceYDimension, SurfaceZDimension);
    //wantedDimension *= 0.01f; // use cm as base
    SurfaceMeshComponent->SetRelativeScale3D(wantedDimension);

    lastTopScale = wantedDimension;
    lastTopCompensateLocation = GetActorLocation();

    TableLegsGeneratorComponent->UpdateLegs();
    TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::N, SurfaceYDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
    TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::S, SurfaceYDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
    TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::E, SurfaceXDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
    TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::W, SurfaceXDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
    TableChairsGeneratorComponent->UpdateChairs();
	
}

// Called every frame
void ATableStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This is called when currentScalingActor is spawned (at runtime or when you drop it into the world in editor)
void ATableStatic::PostActorCreated()
{
    Super::PostActorCreated();
}

// This is called when currentScalingActor is already in level and map is opened
void ATableStatic::PostLoad()
{
    Super::PostLoad();
    //CreateSquare();
}

void ATableStatic::ScaleAlong(Direction direction, FVector amount)
{
    int i = 0;
    amount.Z = 0;
    float compensateDirection = 1.0f;
    bool needUpdateMesh = false;

    switch (direction)
    {
        // if composite direction, recurse along two components
    case Direction::NE:
        ScaleAlong(E, amount);
        ScaleAlong(N, amount);
        needUpdateMesh = true;
        break;
    case Direction::SE:
        ScaleAlong(S, amount);
        ScaleAlong(E, amount);
        needUpdateMesh = true;
        break;
    case Direction::SW:
        ScaleAlong(S, amount);
        ScaleAlong(W, amount);
        needUpdateMesh = true;
        break;
    case Direction::NW:
        ScaleAlong(N, amount);
        ScaleAlong(W, amount);
        needUpdateMesh = true;
        break;

        // if simple direction, clamp along axis and calculate
    case Direction::N:
        amount.Y = 0;
        goto calculate;
    case Direction::S:
        amount.Y = 0;
        amount.X = -amount.X;
        compensateDirection = -compensateDirection;
        goto calculate;
    case Direction::E:
        amount.X = 0;
        goto calculate;
    case Direction::W:
        amount.X = 0;
        amount.Y = -amount.Y;
        compensateDirection = -compensateDirection;
        goto calculate;

    calculate:
    default:
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, "Amount is " + amount.ToString());
        lastTopScale = FVector(lastTopScale.X + (amount.X)*0.01f, lastTopScale.Y + (amount.Y)*0.01f, GetActorScale3D().Z);
        lastTopCompensateLocation = FVector(
            lastTopCompensateLocation.X + (compensateDirection*amount.X/2),
            lastTopCompensateLocation.Y + (compensateDirection*amount.Y/2),
            GetActorLocation().Z);

        SurfaceMeshComponent->SetRelativeLocation(lastTopCompensateLocation);
        SurfaceMeshComponent->SetRelativeScale3D(lastTopScale);

        SurfaceXDimension = SurfaceMeshComponent->RelativeScale3D.X;
        SurfaceYDimension = SurfaceMeshComponent->RelativeScale3D.Y;
        SurfaceZDimension = SurfaceMeshComponent->RelativeScale3D.Z;
    }
    
    if (true)
    {
        if (TableLegsGeneratorComponent)
            TableLegsGeneratorComponent->UpdateLegs();

        if (TableChairsGeneratorComponent)
        {
            TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::N, SurfaceYDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
            TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::S, SurfaceYDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
            TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::E, SurfaceXDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
            TableChairsGeneratorComponent->SetChairsAvailableSpace(Direction::W, SurfaceXDimension * 100 - TableLegsGeneratorComponent->LegSideDimension * 2);
            TableChairsGeneratorComponent->UpdateChairs();
        }
        //UpdateHandleCoordinates();
        //UpdateChairsCoordinates();
    }

}


void ATableStatic::GetHandleCoordinates(FVector locations[8])
{
    FVector extension = SurfaceMeshComponent->Bounds.BoxExtent;
    FVector origin = SurfaceMeshComponent->Bounds.Origin;

    locations[Direction::NE] = origin + extension;
    extension.X = -extension.X;
    locations[Direction::SE] = origin + extension;
    extension.Y = -extension.Y;
    locations[Direction::SW] = origin + extension;
    extension.X = -extension.X;
    locations[Direction::NW] = origin + extension;
    locations[Direction::E] = FVector((locations[Direction::NE].X + locations[Direction::SE].X) / 2, locations[Direction::NE].Y, locations[Direction::NE].Z);
    locations[Direction::W] = FVector((locations[Direction::NW].X + locations[Direction::SW].X) / 2, locations[Direction::NW].Y, locations[Direction::NW].Z);
    locations[Direction::N] = FVector(locations[Direction::NE].X, (locations[Direction::NW].Y + locations[Direction::NE].Y) / 2, locations[Direction::NE].Z);
    locations[Direction::S] = FVector(locations[Direction::SE].X, (locations[Direction::SE].Y + locations[Direction::SW].Y) / 2, locations[Direction::SE].Z);
}