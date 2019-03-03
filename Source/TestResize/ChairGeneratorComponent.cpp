// Fill out your copyright notice in the Description page of Project Settings.

#include "ChairGeneratorComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"


// Sets default values for this component's properties
UChairGeneratorComponent::UChairGeneratorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UChairGeneratorComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
    // Minimum space occupied is *space*
    // Default disposition: *space* *chair* *space* --> *space* *chair* *space* *chair* *space* --> and so on...
    currentTopSpaceOccupied = ChairSpacing;

    // Create a SLIDER to which chairs stack will be attached
    // This way, I only need to act on slider to recenter all chairs to the table
    UWorld* world = GetWorld();
    if (world && Chair)
    {
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = GetOwner();

        for (int i = 0; i < 4; i++)
        {
            this->ChairRowSliders[i] = world->SpawnActor<AActor>(Empty, FVector(0, 0, 0), FRotator(0, 0, 0), spawnParams);
        }

        //FRotator rotation = GetOwner()->GetActorRotation();
        //this->ChairRowSliders[0] = world->SpawnActor<AActor>(Empty, FVector(0, 0, 0), FRotator(0, 0, 0), spawnParams);
        //FAttachmentTransformRules noscalewithparent(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);
        //this->ChairRowSlider->AttachToActor(GetOwner(), noscalewithparent);
    }
}


// Called every frame
void UChairGeneratorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}


unsigned int UChairGeneratorComponent::CanBeNChairs(float availableSpace, float chairsize, float chairspacing)
{
    float singleChairSpace = (chairspacing * 2) + chairsize;
    float additionalChairSpace = chairspacing + chairsize;
    if (availableSpace < singleChairSpace)
        return 0;
    else
        return 1 + FMath::FloorToInt((availableSpace - singleChairSpace) / additionalChairSpace);
}

void UChairGeneratorComponent::UpdateChairs(Direction direction, float Space)
{
    UWorld* world = GetWorld();
    if (world && Chair)
    {
        int side = (int)direction;
        unsigned int currentNumChairs = Chairs[side].size();
        unsigned int wannabeChairs = CanBeNChairs(Space, ChairWidth, ChairSpacing);

        if (currentNumChairs > wannabeChairs)
        {
            for (unsigned int i = 0; i < currentNumChairs - wannabeChairs; i++)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deleting Chair!");

                world->DestroyActor(Chairs[side].top());
                Chairs[side].pop();
                currentTopSpaceOccupied -= ChairSpacing + ChairWidth;
            }
        }
        else if (currentNumChairs < wannabeChairs)
        {
            for (unsigned int i = 0; i < wannabeChairs - currentNumChairs; i++)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Chair!");

                FActorSpawnParameters spawnParams;
                spawnParams.Owner = GetOwner();
                FRotator rotation = GetOwner()->GetActorRotation();
                FVector position = FVector(0, currentTopSpaceOccupied + (ChairWidth / 2), 0);
                AActor* chair = world->SpawnActor<AActor>(Chair, position, rotation, spawnParams);
                FAttachmentTransformRules noscalewithparent(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepRelative, false);
                chair->AttachToActor(this->ChairRowSliders[side], noscalewithparent);
                chair->SetActorRelativeLocation(FVector(ChairDistance, currentTopSpaceOccupied + (ChairWidth / 2), 0));

                Chairs[side].push(chair);

                currentTopSpaceOccupied += ChairWidth;
                currentTopSpaceOccupied += ChairSpacing;
            }
        }

        // Keep chairs centered in one easy step
        this->ChairRowSliders[side]->SetActorLocation(FVector(
            GetOwner()->GetActorLocation().X,
            GetOwner()->GetActorLocation().Y - Space / 2,
            0));

        // keep chairs properly scaled
        //this->ChairRowSlider->SetActorScale3D(FVector::OneVector);
        //TArray<AActor*> chairs;
        //this->ChairRowSlider->GetAllChildActors(chairs, false);
        //for (int i = 0; i < chairs.Num(); i++)
        //{
        //    chairs[i]->SetActorScale3D(FVector(ChairWidth, ChairWidth, 1));
        //}
        //FVector wantedDimension(ChairWidth, ChairWidth, 1);

        ////wantedDimension *= 0.01f; // use cm as base (since default side of model is 1m)

        //GetLegCoordinates(positions);
        //for (int i = 0; i < 4; i++)
        //{
        //    LegActors[i]->SetActorLocation(positions[i]);
        //    LegActors[i]->SetActorScale3D(wantedDimension);
        //}
        
    }
}

void UChairGeneratorComponent::GetChairSlidersCoordinates(FVector locations[4], FRotator rotations[4])
{
    AActor* owner = GetOwner();
    if (owner)
    {
        FVector origin, extension;
        owner->GetActorBounds(true, origin, extension);

        FVector ne = origin + extension;
        extension.X = -extension.X;
        FVector se = origin + extension;
        extension.Y = -extension.Y;
        FVector sw = origin + extension;
        extension.X = -extension.X;
        FVector nw = origin + extension;
        locations[Direction::E] = FVector((ne.X + se.X) / 2, ne.Y, ne.Z);
        locations[Direction::W] = FVector((nw.X + sw.X) / 2, nw.Y, nw.Z);
        locations[Direction::N] = FVector(ne.X, (nw.Y + ne.Y) / 2, ne.Z);
        locations[Direction::S] = FVector(se.X, (se.Y + sw.Y) / 2, se.Z);

        FRotator rotate90 = FRotator(0, 0, 0);
        for (int i = 0; i < 4; i++)
        {
            // Progressively rotate by 90
            rotations[i] = rotate90;
            rotate90.Yaw += 90;
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            locations[i] = FVector::ZeroVector;
            rotations[i] = FRotator::ZeroRotator;
        }
    }
}

