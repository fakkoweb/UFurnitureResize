// Fill out your copyright notice in the Description page of Project Settings.

#include "Table.h"
#include "LegGeneratorComponent.h"


// Sets default values
ATable::ATable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;

    CreateTableTop();

    TableLegsGeneratorComponent = CreateDefaultSubobject<ULegGeneratorComponent>(TEXT("TableLegGenerator"));
    //TableLegsGeneratorComponent->AttachTo(ProceduralMesh);
    //RootComponent = TableLegsGeneratorComponent;

    //TopStyle = CreateDefaultSubobject<UStaticMeshComponent>("Top style");

    //TableLegWidth = 20.f;
    //ChairWidth = 50.f;
    //ChairSpacing = 20.f;
}

// Called when the game starts or when spawned
void ATable::BeginPlay()
{
    Super::BeginPlay();

    // Partition vertices by side
    // All vertices within the range of a leg of the table will be moved when scaling from that side
    PartitionVertices();
}

// Called every frame
void ATable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ATable::ScaleAlong(Direction direction, FVector amount)
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
        for (FVector* vertex : VertexPartitions[direction])
        {
            //FString s = "SCALE VX " + i;
            //FString t = " of " + amount.ToString();
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, s + t);
            *vertex += amount;
        }
        needUpdateMesh = true;
        break;
    }

    if (needUpdateMesh)
    {
        UpdateTableTop();
        //if(TableLegsGeneratorComponent)
        //    TableLegsGeneratorComponent->ScaleAlong()
        //UpdateHandleCoordinates();
        //UpdateChairsCoordinates();
    }

}

void ATable::PartitionVertices()
{
    for (FVector& vertex : Vertices)
    {
        if (FMath::Abs(vertex.X - RelativeInitialHandleCoordinates[Direction::N].X) <= Patch9Threshold)
            VertexPartitions[Direction::N].Add(&vertex);
        if (FMath::Abs(vertex.Y - RelativeInitialHandleCoordinates[Direction::E].Y) <= Patch9Threshold)
            VertexPartitions[Direction::E].Add(&vertex);
        if (FMath::Abs(vertex.X - RelativeInitialHandleCoordinates[Direction::S].X) <= Patch9Threshold)
            VertexPartitions[Direction::S].Add(&vertex);
        if (FMath::Abs(vertex.Y - RelativeInitialHandleCoordinates[Direction::W].Y) <= Patch9Threshold)
            VertexPartitions[Direction::W].Add(&vertex);
    }

    // NE is merge of N and E vertices, and so on...
    VertexPartitions[Direction::NE].Append(VertexPartitions[Direction::N]);
    VertexPartitions[Direction::NE].Append(VertexPartitions[Direction::E]);
    VertexPartitions[Direction::SE].Append(VertexPartitions[Direction::S]);
    VertexPartitions[Direction::SE].Append(VertexPartitions[Direction::E]);
    VertexPartitions[Direction::SW].Append(VertexPartitions[Direction::S]);
    VertexPartitions[Direction::SW].Append(VertexPartitions[Direction::W]);
    VertexPartitions[Direction::NW].Append(VertexPartitions[Direction::N]);
    VertexPartitions[Direction::NW].Append(VertexPartitions[Direction::W]);
}

void ATable::CreateTableTop()
{
    Vertices.Reset();
    Vertices.Add(FVector(0, 0, 0));
    Vertices.Add(FVector(0, 100, 0));
    Vertices.Add(FVector(100, 0, 0));
    Vertices.Add(FVector(100, 100, 0));

    Triangles.Reset();
    Triangles.Add(0);
    Triangles.Add(1);
    Triangles.Add(2);
    Triangles.Add(3);
    Triangles.Add(2);
    Triangles.Add(1);

    //TArray<FVector> normals;
    //normals.Add(FVector(1, 0, 0));
    //normals.Add(FVector(1, 0, 0));
    //normals.Add(FVector(1, 0, 0));

    UV0.Reset();
    UV0.Add(FVector2D(0, 0));
    UV0.Add(FVector2D(0, 1));
    UV0.Add(FVector2D(1, 0));
    UV0.Add(FVector2D(1, 1));

    UpdateTableTop();

}

void ATable::UpdateTableTop()
{
    ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, TArray<FProcMeshTangent>(), false);
}

void ATable::GetHandleCoordinates(FVector locations[8])
{
    FVector extension = ProceduralMesh->Bounds.BoxExtent;
    FVector origin = ProceduralMesh->Bounds.Origin;

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