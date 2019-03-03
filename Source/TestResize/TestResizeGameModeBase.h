// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Enum.h"
#include "ScaleManager.h"
#include "TestResizeGameModeBase.generated.h"

static const FName USER_INTERACTIVE_TAG = "Interactive";
static const FName USER_SELECTED_TAG = "Selected";


/**
 * 
 */
UCLASS()
class TESTRESIZE_API ATestResizeGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
private:
    AActor* SelectedElement = nullptr;
    GameMode currentMode;
    ScaleManager scaleMan;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;
    virtual void Tick(float deltaTime) override;

    void SetSelected(AActor* element, bool interactive);
    bool IsSelected(AActor* element);
    bool SetEditMode(bool active);

public:

    ATestResizeGameModeBase();

    UPROPERTY(Category = Style, EditAnywhere, DisplayName = "Handle Mesh")
        TSubclassOf<class AHandle> ResizeHandleActor;

    void SetSelection(AActor* element);
    void ResetSelection();
    bool RequestSwitchEditMode();
    bool RequestSaveEdit();
    bool RequestAbortEdit();

    void ClickOnActor(AActor* element);
    void DragOnActor(AActor* element, FVector toTarget);
};
