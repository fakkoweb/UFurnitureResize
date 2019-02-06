// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RestrictedMovementActor.generated.h"

UCLASS()
class TESTRESIZE_API ARestrictedMovementActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARestrictedMovementActor();

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Restrict X")
        bool RestrictX;

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Restrict Y")
        bool RestrictY;

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Restrict Z")
        bool RestrictZ;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    void Move(const FVector& position);

};
