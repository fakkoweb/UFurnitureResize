// Fill out your copyright notice in the Description page of Project Settings.

#include "RestrictedMovementActor.h"


// Sets default values
ARestrictedMovementActor::ARestrictedMovementActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    RestrictX = false;
    RestrictY = false;
    RestrictZ = false;
}



// Called when the game starts or when spawned
void ARestrictedMovementActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARestrictedMovementActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARestrictedMovementActor::Move(const FVector & position)
{
    FVector toSetPosition = position;
    if (RestrictX)
        toSetPosition.X = this->GetActorLocation().X;
    if (RestrictY)
        toSetPosition.Y = this->GetActorLocation().Y;
    if (RestrictZ)
        toSetPosition.Z = this->GetActorLocation().Z;

    this->SetActorLocation(toSetPosition);
}

