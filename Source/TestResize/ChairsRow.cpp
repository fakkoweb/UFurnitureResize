// Fill out your copyright notice in the Description page of Project Settings.

#include "ChairsRow.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

// Sets default values
AChairsRow::AChairsRow()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    ChairWidth = 50.f;
    ChairSpacing = 20.f;

    currentTopSpaceOccupied = ChairSpacing;
}

// Called when the game starts or when spawned
void AChairsRow::BeginPlay()
{
	Super::BeginPlay();
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
    {
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "size = " + FString::SanitizeFloat(size));
        //float a = (size - singleChairSpace) / additionalChairSpace;
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "div = " + FString::SanitizeFloat(a));
        return FMath::FloorToInt((availableSpace - singleChairSpace) / additionalChairSpace) + 1;
        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "sedie = " + FString::SanitizeFloat((float)b));
    }
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
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Deleting Chair!");

                world->DestroyActor(Chairs.top());
                Chairs.pop();
                currentTopSpaceOccupied -= ChairSpacing + ChairWidth;
            }
        }
        else if (currentNumChairs < wannabeChairs)
        {
            for (unsigned int i = 0; i < wannabeChairs - currentNumChairs; i++)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Creating Chair!");

                currentTopSpaceOccupied += ChairSpacing;

                FActorSpawnParameters spawnParams;
                spawnParams.Owner = this;
                FRotator rotation = this->GetActorRotation();
                FVector position = FVector(0, currentTopSpaceOccupied + (ChairWidth / 2), 0);
                AActor* chair = world->SpawnActor<AActor>(Chair, position, rotation, spawnParams);
                //chair->SetActorScale3D(FVector(1, ChairWidth, 1));
                chair->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
                chair->SetActorRelativeLocation( FVector(0 , currentTopSpaceOccupied + (ChairWidth / 2), 0) );

                Chairs.push(chair);
                currentTopSpaceOccupied += ChairWidth;

            }
        }

        // Repositioning chairs...
        //for (AActor* a : Chairs)
        //{

        //}

    }
}

