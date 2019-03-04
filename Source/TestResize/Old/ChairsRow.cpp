// Fill out your copyright notice in the Description page of Project Settings.

#include "ChairsRow.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"



// Sets default values
AChairsRow::AChairsRow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ChairWidth = 50.f;
    ChairSpacing = 0.f;
    ChairDistance = 10.f;
}

// Called when the game starts or when spawned
void AChairsRow::BeginPlay()
{
	Super::BeginPlay();

    // Minimum space occupied is *space*
    // Default disposition: *space* *chair* *space* --> *space* *chair* *space* *chair* *space* --> and so on...
    currentTopSpaceOccupied = ChairSpacing;

    // Create a SLIDER to which chairs stack will be attached
    // This way, I only need to act on slider to recenter all chairs to the table
    UWorld* world = GetWorld();
    if (world && Chair)
    {
        FActorSpawnParameters spawnParams;
        spawnParams.Owner = this;
        FRotator rotation = this->GetActorRotation();
        this->ChairRowSlider = world->SpawnActor<AActor>(Empty, FVector(0, 0, 0), rotation, spawnParams);
        this->ChairRowSlider->AttachToActor(this, FAttachmentTransformRules::KeepRelativeTransform);    }
}

// Called every frame
void AChairsRow::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

unsigned int AChairsRow::CanBeNChairs(float availableSpace, float chairsize, float chairspacing)
{
    float singleChairSpace = (chairspacing * 2) + chairsize;
    float additionalChairSpace = chairspacing + chairsize;
    if (availableSpace < singleChairSpace)
        return 0;
    else
        return 1 + FMath::FloorToInt((availableSpace - singleChairSpace) / additionalChairSpace);
}

void AChairsRow::UpdateChairs(float Space)
{
    UWorld* world = GetWorld();
    if (world && Chair)
    {
        unsigned int currentNumChairs = Chairs.size();
        unsigned int wannabeChairs = CanBeNChairs(Space, ChairWidth, ChairSpacing);

        if (currentNumChairs > wannabeChairs)
        {
            for (unsigned int i = 0; i < currentNumChairs - wannabeChairs; i++)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deleting Chair!");

                world->DestroyActor(Chairs.top());
                Chairs.pop();
                currentTopSpaceOccupied -= ChairSpacing + ChairWidth;
            }
        }
        else if (currentNumChairs < wannabeChairs)
        {
            for (unsigned int i = 0; i < wannabeChairs - currentNumChairs; i++)
            {
                //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Chair!");

                FActorSpawnParameters spawnParams;
                spawnParams.Owner = this;
                FRotator rotation = this->GetActorRotation();
                FVector position = FVector(0, currentTopSpaceOccupied + (ChairWidth / 2), 0);
                AActor* chair = world->SpawnActor<AActor>(Chair, position, rotation, spawnParams);
                chair->AttachToActor(ChairRowSlider, FAttachmentTransformRules::KeepWorldTransform);
                chair->SetActorRelativeLocation( FVector(ChairDistance, currentTopSpaceOccupied + (ChairWidth / 2), 0) );

                Chairs.push(chair);

                currentTopSpaceOccupied += ChairWidth;
                currentTopSpaceOccupied += ChairSpacing;
            }
        }

        // Keep chairs centered in one easy step
        this->ChairRowSlider->SetActorRelativeLocation(FVector(0, -currentTopSpaceOccupied / 2, 0));

    }
}

