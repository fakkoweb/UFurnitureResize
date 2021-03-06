// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerElement.h"
#include "GameFramework/PlayerController.h"
#include "ResizeTableGameMode.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

#include <string>


///////////////////////////////////////////////////////////////////////////
// Sets default values
APlayerElement::APlayerElement()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    this->springArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    this->springArm->AttachTo(RootComponent);
    this->springArm->TargetArmLength = 250.0f;
    this->springArm->bDoCollisionTest = false;
    this->springArm->SetWorldRotation(FRotator(-40.f, 0.0f, 0.0f));

    this->camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    this->camera->AttachTo(this->springArm, USpringArmComponent::SocketName);

    AutoPossessPlayer = EAutoReceiveInput::Player0;

    upAndDown = 0.0f;
    rightAndLeft = 0.0f;
    zoom = 0.0f;
    navigationEnabled = false;
}

///////////////////////////////////////////////////////////////////////////
// Called when the game starts or when spawned
void APlayerElement::BeginPlay()
{
    Super::BeginPlay();
}

///////////////////////////////////////////////////////////////////////////
// Called to bind functionality to input
void APlayerElement::SetupPlayerInputComponent(UInputComponent* inputComponent)
{
    Super::SetupPlayerInputComponent(inputComponent);

    inputComponent->BindAction("EnableNavigation", IE_Pressed, this, &APlayerElement::EnableNavigation);
    inputComponent->BindAction("EnableNavigation", IE_Released, this, &APlayerElement::DisableNavigation);
    inputComponent->BindAction("GoUp", IE_Pressed, this, &APlayerElement::GoUp);
    inputComponent->BindAction("GoUp", IE_Released, this, &APlayerElement::GoDown);
    inputComponent->BindAction("GoDown", IE_Pressed, this, &APlayerElement::GoDown);
    inputComponent->BindAction("GoDown", IE_Released, this, &APlayerElement::GoUp);
    inputComponent->BindAction("PanRight", IE_Pressed, this, &APlayerElement::PanRight); // not used
    inputComponent->BindAction("PanRight", IE_Released, this, &APlayerElement::PanLeft); // not used
    inputComponent->BindAction("PanLeft", IE_Pressed, this, &APlayerElement::PanLeft);   // not used
    inputComponent->BindAction("PanLeft", IE_Released, this, &APlayerElement::PanRight); // not used
    inputComponent->BindAxis("CameraYaw", this, &APlayerElement::SetCameraYaw);
    inputComponent->BindAxis("CameraPitch", this, &APlayerElement::SetCameraPitch);
    inputComponent->BindAxis("Zoom", this, &APlayerElement::SetZoom);

    inputComponent->BindAction("EditMode", IE_Released, this, &APlayerElement::SwitchEditMode);
    inputComponent->BindAction("SaveEdit", IE_Released, this, &APlayerElement::SaveEdit);
    inputComponent->BindAction("AbortEdit", IE_Released, this, &APlayerElement::AbortEdit);


}

///////////////////////////////////////////////////////////////////////////
// Called every frame
void APlayerElement::Tick(float deltaTime)
{
    Super::Tick(deltaTime);

    FVector pawnLocation = GetActorLocation();
    pawnLocation.Z = FMath::Clamp(pawnLocation.Z + this->upAndDown, 0.0f, 255.0f);
    SetActorLocation(pawnLocation);

    if (navigationEnabled)
    {
        FRotator springArmRot = this->springArm->GetComponentRotation();
        springArmRot.Yaw += this->cameraRot.X;
        springArmRot.Pitch = FMath::Clamp(springArmRot.Pitch + this->cameraRot.Y, -85.0f, 0.0f);
        this->springArm->SetWorldRotation(springArmRot);
    }

    float springArmLength = this->springArm->TargetArmLength;

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Zoom is: %f"), this->springArm->TargetArmLength));

    springArmLength = FMath::Clamp(springArmLength - this->zoom, 250.0f, 1000.0f);
    this->springArm->TargetArmLength = springArmLength;

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Zoom is: %f"), this->springArm->TargetArmLength));
}

void APlayerElement::SwitchEditMode()
{
    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());
    gameMode->RequestSwitchEditMode();
}

void APlayerElement::SaveEdit()
{
    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());
    gameMode->RequestSaveEdit();
}

void APlayerElement::AbortEdit()
{
    // Get game mode
    ATestResizeGameModeBase* gameMode = (ATestResizeGameModeBase*)UGameplayStatics::GetGameMode(GetWorld());
    gameMode->RequestAbortEdit();
}
