// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum.h"
#include "Scalable.h"
#include "GameFramework/Actor.h"
#include "RestrictedMovementActor.h"
#include "Handle.generated.h"


class ScaleManager;

UCLASS(Blueprintable)
class TESTRESIZE_API AHandle : public ARestrictedMovementActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this currentScalingActor's properties
	AHandle();

    //UPROPERTY(VisibleAnywhere)
    //TScriptInterface<IScalable> ScalableActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:

    Direction movingDirection;
    ScaleManager* manager;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    virtual void MoveTo(const FVector& position) override;
    void SetDirection(const Direction direction);

    void SetManager(ScaleManager* const handleManager);
	
};
