// By Paul Graham <p@ul.ms>

#include "VRControllerActor.h"
#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"

// Constructor
AVRControllerActor::AVRControllerActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	// We have no collision
	SetActorEnableCollision(false);

	// Create the origin
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(RootComponent);
	
	// Create the controller
	MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("MotionController"));
	MotionController->SetupAttachment(VROrigin);
	MotionController->bDisplayDeviceModel = true;
	MotionController->CastShadow = true;
	
	// Create the mesh
	//ControllerMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ControllerMesh"));
	//ControllerMesh->SetupAttachment(MotionController);
}

void AVRControllerActor::SetSource(FName source) {
	MotionController->SetTrackingMotionSource(source);
}

void AVRControllerActor::BeginPlay() {
	Super::BeginPlay();
}

void AVRControllerActor::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}