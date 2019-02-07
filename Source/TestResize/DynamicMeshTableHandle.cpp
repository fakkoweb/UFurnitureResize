// Fill out your copyright notice in the Description page of Project Settings.

#include "DynamicMeshTableHandle.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"

void ADynamicMeshTableHandle::Move(const FVector & position)
{
    FVector thisActorLocationBeforeMove = this->GetActorLocation();

    Super::Move(position);

    FVector thisActorLocationAfterMove = this->GetActorLocation();

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "SHOULD scale on direction: " + direction);
    if (scalable)
    {
        scalable->ScaleAlong(direction, thisActorLocationAfterMove - thisActorLocationBeforeMove);
    }
}

void ADynamicMeshTableHandle::SetDirection(const ADynamicMeshTable::Direction direction)
{
    this->direction = direction;

    RestrictZ = true;

    switch (direction)
    {
    case ADynamicMeshTable::Direction::N:
    case ADynamicMeshTable::Direction::S:
        RestrictY = true;
        break;

    case ADynamicMeshTable::Direction::E:
    case ADynamicMeshTable::Direction::W:
        RestrictX = true;
        break;
    }
}

void ADynamicMeshTableHandle::BeginPlay()
{

    Super::BeginPlay();

}
