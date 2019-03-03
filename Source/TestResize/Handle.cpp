// Fill out your copyright notice in the Description page of Project Settings.

#include "Handle.h"
#include "ScaleManager.h"

// Sets default values
AHandle::AHandle()
{
 	// Set this currentScalingActor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHandle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHandle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHandle::MoveTo(const FVector & position)
{
    FVector thisActorLocationBeforeMove = this->GetActorLocation();

    Super::MoveTo(position);

    FVector thisActorLocationAfterMove = this->GetActorLocation();

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "SHOULD scale on direction: " + direction);
    IScalable* currentScalingActor = manager->GetScalable();
    if (currentScalingActor)
    {
        // With Unreal Interfaces...
        IScalable* _scalable = Cast<IScalable>(currentScalingActor->_getUObject());
        _scalable->ScaleAlong(movingDirection, thisActorLocationAfterMove - thisActorLocationBeforeMove);
        manager->UpdateHandles(currentScalingActor);
    }
}

void AHandle::SetDirection(const Direction direction)
{
    this->movingDirection = direction;

    RestrictZ = true;

    switch (direction)
    {
    case Direction::N:
    case Direction::S:
        RestrictY = true;
        break;

    case Direction::E:
    case Direction::W:
        RestrictX = true;
        break;
    }
}

void AHandle::SetManager(ScaleManager* const handleManager)
{
    manager = handleManager;
}