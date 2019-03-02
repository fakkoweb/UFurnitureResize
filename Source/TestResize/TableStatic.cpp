// Fill out your copyright notice in the Description page of Project Settings.

#include "TableStatic.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"
#include "LegGeneratorComponent.h"


// Sets default values
ATableStatic::ATableStatic()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    SurfaceMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TableMesh"));
    RootComponent = SurfaceMeshComponent;

    TableLegsGeneratorComponent = CreateDefaultSubobject<ULegGeneratorComponent>(TEXT("TableLegGenerator"));
}

// Called when the game starts or when spawned
void ATableStatic::BeginPlay()
{
	Super::BeginPlay();

    SurfaceMeshComponent->SetStaticMesh(TopStyle);

    //SurfaceXDimension = 5.0f;
    //SurfaceYDimension = 2.0f;
    //SurfaceZDimension = 0.40f;

    FVector wantedDimension(SurfaceXDimension, SurfaceYDimension, SurfaceZDimension);
    //wantedDimension *= 0.01f; // use cm as base
    SurfaceMeshComponent->SetRelativeScale3D(wantedDimension);

    lastTopScale = wantedDimension;
    lastTopCompensateLocation = FVector::ZeroVector;

    TableLegsGeneratorComponent->UpdateLegs();
	
}

// Called every frame
void ATableStatic::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ATableStatic::PostActorCreated()
{
    Super::PostActorCreated();
}

// This is called when actor is already in level and map is opened
void ATableStatic::PostLoad()
{
    Super::PostLoad();
    //CreateSquare();
}

void ATableStatic::ScaleAlong(Direction direction, FVector amount)
{
    int i = 0;
    amount.Z = 0;
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
    case Direction::S:
        amount.Y = 0;
        goto calculate;
    case Direction::E:
    case Direction::W:
        amount.X = 0;
        goto calculate;

    calculate:
    default:

        lastTopScale = FVector(lastTopScale.X + amount.X / 2, lastTopScale.Y + amount.Y / 2, lastTopScale.Z + amount.Z / 2);
        lastTopCompensateLocation = FVector(lastTopCompensateLocation.X + amount.X / 2, lastTopCompensateLocation.Y + amount.Y / 2, lastTopCompensateLocation.Z + amount.Z / 2);

        SurfaceMeshComponent->SetRelativeLocation(lastTopCompensateLocation);
        SurfaceMeshComponent->SetRelativeScale3D(lastTopScale);

        break;
    }

    if (needUpdateMesh)
    {
        //if (TableLegsGeneratorComponent)
        //    TableLegsGeneratorComponent->UpdateLegs();
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