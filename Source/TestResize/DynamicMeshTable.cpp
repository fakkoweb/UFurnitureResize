// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicMeshTable.h"
#include "DynamicMeshTableHandle.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
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
    ChairWidth = 50.f;
    ChairSpacing = 20.f;

    CreateSquare();
}

// Called when the game starts or when spawned
void ADynamicMeshTable::BeginPlay()
{
    Super::BeginPlay();

    GenerateHandleCoordinates();

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

    PartitionVertices();

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

unsigned int ADynamicMeshTable::CanBeNChairs(float size, float chair, float spacer)
{
    float singleChairSpace = (spacer * 2) + chair;
    float additionalChairSpace = spacer + chair;
    if (size < singleChairSpace)
        return 0;
    else
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "size = " + FString::SanitizeFloat(size));
        //float a = (size - singleChairSpace) / additionalChairSpace;
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "div = " + FString::SanitizeFloat(a));
        return FMath::FloorToInt((size - singleChairSpace) / additionalChairSpace) + 1;
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "sedie = " + FString::SanitizeFloat((float)b));
    }
}

void ADynamicMeshTable::UpdateSquare()
{
    ProceduralMesh->CreateMeshSection(0, Vertices, Triangles, Normals, UV0, VertexColors, TArray<FProcMeshTangent>(), false);

    UWorld* world = GetWorld();
    if (world && Chair)
    {
        unsigned int currentNumChairs = Chairs.size();
        float currentSpaceAvailable = ResizeHandles[Direction::N]->GetActorLocation().X - ResizeHandles[Direction::S]->GetActorLocation().X;
        unsigned int wannabeChairs = CanBeNChairs(currentSpaceAvailable, ChairWidth, ChairSpacing);

        if (currentNumChairs > wannabeChairs)
        {
            for (unsigned int i = 0; i < currentNumChairs - wannabeChairs; i++)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deleting Chair!");

                world->DestroyActor(Chairs.top());
                Chairs.pop();
            }
        }
        else if (currentNumChairs < wannabeChairs)
        {
            for (unsigned int i = 0; i < wannabeChairs - currentNumChairs; i++)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Chair!");
                FActorSpawnParameters spawnParams;
                spawnParams.Owner = this;
                FRotator rotation = this->GetActorRotation();
                AActor* chair = world->SpawnActor<AActor>(ResizeHandle, ResizeHandles[Direction::W]->GetActorLocation(), rotation, spawnParams);
                chair->SetActorScale3D(FVector(1, ChairWidth, 1));
                chair->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);

                Chairs.push(chair);
            }
        }
        
        // Repositioning chairs...
        //for (AActor* a : Chairs)
        //{

        //}

    }
}

void ADynamicMeshTable::GenerateHandleCoordinates()
{
    FVector extension = ProceduralMesh->Bounds.BoxExtent;
    FVector origin = ProceduralMesh->Bounds.Origin;

    InitialHandleCoordinates[Direction::NE] = origin + extension;  // NE
    extension.X = -extension.X;
    InitialHandleCoordinates[Direction::SE] = origin + extension;  //SE
    extension.Y = -extension.Y;
    InitialHandleCoordinates[Direction::SW] = origin + extension;  //SW
    extension.X = -extension.X;
    InitialHandleCoordinates[Direction::NW] = origin + extension;  //NW
    InitialHandleCoordinates[Direction::E] = FVector((InitialHandleCoordinates[Direction::NE].X + InitialHandleCoordinates[Direction::SE].X) / 2, InitialHandleCoordinates[Direction::NE].Y, InitialHandleCoordinates[Direction::NE].Z);   // E
    InitialHandleCoordinates[Direction::W] = FVector((InitialHandleCoordinates[Direction::NW].X + InitialHandleCoordinates[Direction::SW].X) / 2, InitialHandleCoordinates[Direction::NW].Y, InitialHandleCoordinates[Direction::NW].Z);   // W
    InitialHandleCoordinates[Direction::N] = FVector(InitialHandleCoordinates[Direction::NE].X, (InitialHandleCoordinates[Direction::NW].Y + InitialHandleCoordinates[Direction::NE].Y) / 2, InitialHandleCoordinates[Direction::NE].Z);
    InitialHandleCoordinates[Direction::S] = FVector(InitialHandleCoordinates[Direction::SE].X, (InitialHandleCoordinates[Direction::SE].Y + InitialHandleCoordinates[Direction::SW].Y) / 2, InitialHandleCoordinates[Direction::SE].Z);
}

void ADynamicMeshTable::UpdateHandleCoordinates()
{
    FVector extension = ProceduralMesh->Bounds.BoxExtent;
    FVector origin = ProceduralMesh->Bounds.Origin;

    FVector updatedLoc[8];

    updatedLoc[Direction::NE] = origin + extension;  // NE
    extension.X = -extension.X;
    updatedLoc[Direction::SE] = origin + extension;  // NE
    extension.Y = -extension.Y;
    updatedLoc[Direction::SW] = origin + extension;
    extension.X = -extension.X;
    updatedLoc[Direction::NW] = origin + extension;
    updatedLoc[Direction::E] = FVector((updatedLoc[Direction::NE].X + updatedLoc[Direction::SE].X) / 2, updatedLoc[Direction::NE].Y, updatedLoc[Direction::NE].Z);   // E
    updatedLoc[Direction::W] = FVector((updatedLoc[Direction::NW].X + updatedLoc[Direction::SW].X) / 2, updatedLoc[Direction::NW].Y, updatedLoc[Direction::NW].Z);   // W
    updatedLoc[Direction::N] = FVector(updatedLoc[Direction::NE].X, (updatedLoc[Direction::NW].Y + updatedLoc[Direction::NE].Y) / 2, updatedLoc[Direction::NE].Z);
    updatedLoc[Direction::S] = FVector(updatedLoc[Direction::SE].X, (updatedLoc[Direction::SE].Y + updatedLoc[Direction::SW].Y) / 2, updatedLoc[Direction::SE].Z);

    for (int i = 0; i < 8; i++)
    {
        ResizeHandles[i]->SetActorLocation(updatedLoc[i]);
    }
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

    // Bounds only calculated once
    //ProceduralMesh->CalcBounds(FTransform(ProceduralMesh->Bounds.Origin, this->GetActorRotation(), this->GetActorScale()));
}