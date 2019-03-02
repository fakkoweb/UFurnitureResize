// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "Scalable.h"
#include "GameFramework/Actor.h"
#include "RestrictedMovementActor.h"
#include "Handle.generated.h"

UCLASS()
class TESTRESIZE_API AHandle : public ARestrictedMovementActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandle();

    UPROPERTY(VisibleAnywhere)
    TScriptInterface<IScalable> ScalableActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

    Direction movingDirection;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void MoveTo(const FVector& position) override;
    void SetDirection(const Direction direction);
	
};
