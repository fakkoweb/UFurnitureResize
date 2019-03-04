// Fill out your copyright notice in the Description page of Project Settings.

#include "LegGeneratorComponent.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMeshActor.h"

ULegGeneratorComponent::ULegGeneratorComponent(const FObjectInitializer& OI) : Super(OI)
{
    //LegSideDimension = FMath::Max(LegStyle->Bounds.BoxExtent.X, LegStyle->Bounds.BoxExtent.Y) * 2;
    //LegHeight = LegStyle->Bounds.BoxExtent.Z * 2;
    //LegSpacing = 5.0f;
}

void ULegGeneratorComponent::BeginPlay()
{

    // Position 4 legs at the corners of the parent actor
    UWorld* world = GetWorld();
    AActor* owner = GetOwner();
    if (world && owner)
    {
        FVector positions[4];
        LegHeight = owner->GetActorLocation().Z;
        GetLegCoordinates(positions);

        FRotator rotation = owner->GetActorRotation();
        FRotator rotate90 = FRotator(0, 0, 0);

        FActorSpawnParameters spawnParams;
        spawnParams.Owner = owner;

        for (int i = 0; i < 4; i++)
        {
            LegActors[i] = world->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), positions[i], rotation, spawnParams);
            LegActors[i]->GetStaticMeshComponent()->SetMobility(EComponentMobility::Movable);
            LegActors[i]->GetStaticMeshComponent()->SetStaticMesh(LegStyle);

            FAttachmentTransformRules noscalewithparent(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false);
            LegActors[i]->AttachToActor(owner, noscalewithparent);
            
            // Make sure you pass in 'this' or whatever Actor or Component as a valid outer object
            //UStaticMeshComponent* CreatedLeg = NewObject<UStaticMeshComponent>(this, UStaticMeshComponent::StaticClass(), *FString("Leg" + FString::FromInt(i + 1)));
            // Example of setting some properties
            //CreatedLeg->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            //CreatedLeg->SetWorldTransform(ComponentTransform);
            //CreatedLeg->SetStaticMesh(LegStyle);
            //CreatedLeg->SetCastShadow(bCastShadow);
            // Now, register the component.
            //CreatedLeg->RegisterComponent();
            // begin play will be called within RegisterComponent (which calls RegisterComponentWithWorld), you don't need to call it yourself
        }
    }

}

void ULegGeneratorComponent::GetLegCoordinates(FVector coords[4])
{
    AActor* owner = GetOwner();
    if (owner)
    {
        FVector origin, extension;
        owner->GetActorBounds(true, origin, extension);

        coords[0] = origin + extension;
        extension.X = -extension.X;
        coords[1] = origin + extension;
        extension.Y = -extension.Y;
        coords[2] = origin + extension;
        extension.X = -extension.X;
        coords[3] = origin + extension;

        // Correction taking into account Leg shape dimension
        coords[0] = FVector(coords[0].X - (LegSideDimension / 2), coords[0].Y - (LegSideDimension / 2), coords[0].Z - LegHeight - extension.Z*2);
        coords[1] = FVector(coords[1].X + (LegSideDimension / 2), coords[1].Y - (LegSideDimension / 2), coords[1].Z - LegHeight - extension.Z*2);
        coords[2] = FVector(coords[2].X + (LegSideDimension / 2), coords[2].Y + (LegSideDimension / 2), coords[2].Z - LegHeight - extension.Z*2);
        coords[3] = FVector(coords[3].X - (LegSideDimension / 2), coords[3].Y + (LegSideDimension / 2), coords[3].Z - LegHeight - extension.Z*2);
    }

}

void ULegGeneratorComponent::UpdateLegs()
{
    FVector positions[4];

    AActor* owner = GetOwner();
    if (owner)
    {
        LegHeight = owner->GetActorLocation().Z;
    }

    FVector wantedDimension(LegSideDimension, LegSideDimension, LegHeight);
    wantedDimension *= 0.01f; // use cm as base (since default side of model is 1m)

    GetLegCoordinates(positions);
    for (int i = 0; i < 4; i++)
    {
        // TODO: HARDFIX TO BE REMOVED: this should be called only once in BeginPlay, but it does not work when spawning runtime Actors!
        if(!LegActors[i]->GetStaticMeshComponent()->GetStaticMesh())
            LegActors[i]->GetStaticMeshComponent()->SetStaticMesh(LegStyle);

        LegActors[i]->SetActorLocation(positions[i]);
        LegActors[i]->SetActorScale3D(wantedDimension);
    }
}

