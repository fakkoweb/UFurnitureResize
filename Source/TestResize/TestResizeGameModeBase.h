// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TestResizeGameModeBase.generated.h"

static const FName USER_INTERACTIVE_TAG = "Interactive";

/**
 * 
 */
UCLASS()
class TESTRESIZE_API ATestResizeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
    AActor* pickedElement = nullptr;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void SetInteractive(AActor* element, bool interactive);
    bool IsInteractive(AActor* element);

public:
    void SetSelection(AActor* element);
    void ResetSelection();
    bool RequestEditMode(bool active);
	
};
