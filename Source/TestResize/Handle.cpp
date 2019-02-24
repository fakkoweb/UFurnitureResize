// Fill out your copyright notice in the Description page of Project Settings.

#include "Handle.h"


// Sets default values
AHandle::AHandle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

void AHandle::Move(const FVector & position)
{
    FVector thisActorLocationBeforeMove = this->GetActorLocation();

    Super::Move(position);

    FVector thisActorLocationAfterMove = this->GetActorLocation();

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "SHOULD scale on direction: " + direction);
    if (ScalableActor)
    {
        IScalable* _scalable = Cast<IScalable>(ScalableActor->_getUObject());
        _scalable->ScaleAlong(movingDirection, thisActorLocationAfterMove - thisActorLocationBeforeMove);
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