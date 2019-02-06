// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicMeshTable.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
//#include "DrawDebugHelpers.h"



// Sets default values
ADynamicMeshTable::ADynamicMeshTable()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ProceduralMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMesh"));
    RootComponent = ProceduralMesh;
    //mesh->AttachToComponent(RootComponent,FAttachmentTransformRules::KeepRelativeTransform);

    CreateSquare();


    
}

// Called when the game starts or when spawned
void ADynamicMeshTable::BeginPlay()
{
    Super::BeginPlay();

    FVector boxUpperLimits;
    FVector extension = ProceduralMesh->Bounds.BoxExtent;
    FVector origin = ProceduralMesh->Bounds.Origin;
    FVector positions[8];
    positions[Direction::NE] = origin + extension;  // NE
    extension.X = -extension.X;
    positions[Direction::SE] = origin + extension;  //SE
    extension.Y = -extension.Y;
    positions[Direction::SW] = origin + extension;  //SW
    extension.X = -extension.X;
    positions[Direction::NW] = origin + extension;  //NW
    positions[Direction::E] = FVector((positions[Direction::NE].X + positions[Direction::SE].X) / 2, positions[Direction::NE].Y, positions[Direction::NE].Z);   // E
    positions[Direction::W] = FVector((positions[Direction::NW].X + positions[Direction::SW].X) / 2, positions[Direction::NW].Y, positions[Direction::NW].Z);   // W
    positions[Direction::N] = FVector(positions[Direction::NE].X, (positions[Direction::NW].Y + positions[Direction::NE].Y) / 2, positions[Direction::NE].Z);
    positions[Direction::S] = FVector(positions[Direction::SE].X, (positions[Direction::SE].Y + positions[Direction::SW].Y) / 2, positions[Direction::SE].Z);

    UWorld* world = GetWorld();
    if (world)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "World exists");
    }
    if (ResizeHandle)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "ResizeHandle exists");
    }
    if (world && ResizeHandle)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Resize Handles ready to be spawned!");
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = this;
        FRotator rotation = this->GetActorRotation();
        for (int i = 0; i < 8; i++)
        {
            FVector position = positions[i];
            ResizeHandles[i] = world->SpawnActor<AActor>(ResizeHandle, position, rotation, spawnParams);
            ResizeHandles[i]->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
            ResizeHandles[i]->Tags.Add(("Draggable"));
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Spawned at "+position.ToString());
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

void ADynamicMeshTable::PostEditChangeChainProperty(struct FPropertyChangedChainEvent& e)
{
    //int32 index = e.GetArrayIndex(TEXT("Vertices")); //checks skipped
    //UStaticMesh *mesh = Meshes[index]; //changed mesh
    UpdateSquare();
    UE_LOG(LogClass, Log, TEXT("PropertyChain index any"));
    Super::PostEditChangeChainProperty(e);
}

// This is called when actor is spawned (at runtime or when you drop it into the world in editor)
void ADynamicMeshTable::PostActorCreated()
{
    Super::PostActorCreated();
    CreateSquare();

}

// This is called when actor is already in level and map is opened
void ADynamicMeshTable::PostLoad()
{
    Super::PostLoad();
    CreateSquare();
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
    //ProceduralMesh->boun
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