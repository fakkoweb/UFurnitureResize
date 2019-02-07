// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicMeshTable.h"
#include "DynamicMeshTableHandle.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "ChairsRow.h"
//#include "DrawDebugHelpers.h"

#include <string>

// Sets default values
ADynamicMeshTable::ADynamicMeshTable()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;
    //mesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);

    TableLegWidth = 20.f;
    //ChairWidth = 50.f;
    //ChairSpacing = 20.f;

    CreateSquare();
}

// Called when the game starts or when spawned
void ADynamicMeshTable::BeginPlay()
{
    Super::BeginPlay();

    // Compute where to place the handles
    GetHandleCoordinates(InitialHandleCoordinates);

    // Position 4 handles at the corners and 4 at the sides
    UWorld* world = GetWorld();
    if (world && ResizeHandle)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Resize Handles ready to be spawned!");
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = this;
        FRotator rotation = this->GetActorRotation();
        for (int i = 0; i < 8; i++)
        {
            FVector position = InitialHandleCoordinates[i];
            ResizeHandles[i] = world->SpawnActor<ADynamicMeshTableHandle>(ResizeHandle, position, rotation, spawnParams);
            ResizeHandles[i]->scalable = this;
            ResizeHandles[i]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            RelativeInitialHandleCoordinates[i] = ResizeHandles[i]->GetActorTransform().GetRelativeTransform(this->GetActorTransform()).GetLocation();
            ResizeHandles[i]->Tags.Add(("Draggable"));
            ResizeHandles[i]->SetDirection((Direction) i);
        }
    }

    // Partition vertices by side
    // All vertices within the range of a leg of the table will be moved when scaling from that side
    PartitionVertices();

    // Position a chair row for each side
    if (world && ChairsRow)
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ChairRows ready to be spawned!");
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = this;
        FRotator initrotation = this->GetActorRotation();
        FRotator rotate90 = FRotator(0, 0, 0);
        for (int i = 0; i < 4; i++)
        {
            FVector position = InitialHandleCoordinates[i];
            ChairsRows[i] = world->SpawnActor<AChairsRow>(ChairsRow, position, initrotation, spawnParams);
            ChairsRows[i]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            // Position at the middle handle
            ChairsRows[i]->SetActorRelativeLocation(RelativeInitialHandleCoordinates[i]);
            // Progressively rotate by 90
            ChairsRows[i]->SetActorRelativeRotation(rotate90);
            rotate90.Yaw += 90;
        }
    }
}

// Called every frame
void ADynamicMeshTable::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

//void ADynamicMeshTable::PostEditChangeProperty(struct FPropertyChangedEvent& e)
//{
//    FName PropertyName = (e.Property != NULL) ? e.Property->GetFName() : NAME_None;
//    if (PropertyName == GET_MEMBER_NAME_CHECKED(ADynamicMeshTable, Vertices))
//    {
//        //various uproperty tricks, see link
//        UpdateSquare();
//    }
//    Super::PostEditChangeProperty(e);
//}

//void ADynamicMeshTable::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
//{
//    //int32 index = e.GetArrayIndex(TEXT("Vertices")); //checks skipped
//    //UStaticMesh *mesh = Meshes[index]; //changed mesh
//    UpdateSquare();
//    UE_LOG(LogClass, Log, TEXT("PropertyChain index any"));
//    Super::PostEditChangeChainProperty(e);
//}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ADynamicMeshTable::PostActorCreated()
{
    Super::PostActorCreated();
    //CreateSquare();
}

// This is called when actor is already in level and map is opened
void ADynamicMeshTable::PostLoad()
{
    Super::PostLoad();
    //CreateSquare();
}

void ADynamicMeshTable::ScaleAlong(Direction direction, FVector amount)
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
        UpdateSquare();
        UpdateHandleCoordinates();
        UpdateChairsCoordinates();
    }

}

void ADynamicMeshTable::CreateTriangle()
{
    Vertices.Reset();
    Vertices.Add(FVector(0, 0, 0));
    Vertices.Add(FVector(0, 100, 0));
    Vertices.Add(FVector(0, 0, 100));

    TArray<int32> Triangles;
    Triangles.Add(0);
    Triangles.Add(1);
    Triangles.Add(2);

    TArray<FVector> normals;
    normals.Add(FVector(1, 0, 0));
    normals.Add(FVector(1, 0, 0));
    normals.Add(FVector(1, 0, 0));

    TArray<FVector2D> UV0;
    UV0.Add(FVector2D(0, 0));
    UV0.Add(FVector2D(10, 0));
    UV0.Add(FVector2D(0, 10));

    TArray<FProcMeshTangent> tangents;
    tangents.Add(FProcMeshTangent(0, 1, 0));
    tangents.Add(FProcMeshTangent(0, 1, 0));
    tangents.Add(FProcMeshTangent(0, 1, 0));

    TArray<FLinearColor> vertexColors;
    vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));
    vertexColors.Add(FLinearColor(0.75, 0.75, 0.75, 1.0));

    ProceduralMesh->CreateMeshSection_LinearColor(0, Vertices, Triangles, normals, UV0, vertexColors, tangents, true);

    // Enable collision data
    ProceduralMesh->ContainsPhysicsTriMeshData(true);
}

void ADynamicMeshTable::UpdateSquare()
{
    ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, TArray<FProcMeshTangent>(), false);
}


void ADynamicMeshTable::GetHandleCoordinates(FVector locations[8])
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

void ADynamicMeshTable::SetHandleCoordinates(const FVector locations[8])
{
    for (int i = 0; i < 8; i++)
    {
        ResizeHandles[i]->SetActorLocation(locations[i]);
    }
}

void ADynamicMeshTable::UpdateHandleCoordinates()
{
    FVector updatedLoc[8];
    GetHandleCoordinates(updatedLoc);
    SetHandleCoordinates(updatedLoc);
}

void ADynamicMeshTable::UpdateChairsCoordinates()
{
    float spaceOnX = FMath::Abs(ResizeHandles[Direction::N]->GetActorLocation().X - ResizeHandles[Direction::S]->GetActorLocation().X) - (TableLegWidth * 2);
    float spaceOnY = FMath::Abs(ResizeHandles[Direction::W]->GetActorLocation().Y - ResizeHandles[Direction::E]->GetActorLocation().Y) - (TableLegWidth * 2);
    ChairsRows[Direction::N]->UpdateChairs(spaceOnY);
    ChairsRows[Direction::S]->UpdateChairs(spaceOnY);
    ChairsRows[Direction::E]->UpdateChairs(spaceOnX);
    ChairsRows[Direction::W]->UpdateChairs(spaceOnX);

    ChairsRows[Direction::N]->SetActorLocation(ResizeHandles[Direction::N]->GetActorLocation());
    ChairsRows[Direction::S]->SetActorLocation(ResizeHandles[Direction::S]->GetActorLocation());
    ChairsRows[Direction::E]->SetActorLocation(ResizeHandles[Direction::E]->GetActorLocation());
    ChairsRows[Direction::W]->SetActorLocation(ResizeHandles[Direction::W]->GetActorLocation());
}

void ADynamicMeshTable::PartitionVertices()
{
    for(FVector& vertex : Vertices)
    {
        if (FMath::Abs(vertex.X - RelativeInitialHandleCoordinates[Direction::N].X) <= TableLegWidth)
            VertexPartitions[Direction::N].Add(&vertex);
        if (FMath::Abs(vertex.Y - RelativeInitialHandleCoordinates[Direction::E].Y) <= TableLegWidth)
            VertexPartitions[Direction::E].Add(&vertex);
        if (FMath::Abs(vertex.X - RelativeInitialHandleCoordinates[Direction::S].X) <= TableLegWidth)
            VertexPartitions[Direction::S].Add(&vertex);
        if (FMath::Abs(vertex.Y - RelativeInitialHandleCoordinates[Direction::W].Y) <= TableLegWidth)
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

void ADynamicMeshTable::CreateSquare()
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

    UpdateSquare();

}