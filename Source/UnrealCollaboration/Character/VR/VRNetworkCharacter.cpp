// By Paul Graham <p@ul.ms>

#include "VRNetworkCharacter.h"
#include "../BaseNetworkCharacter.h"
#include "../VR/VRNetworkPlayerController.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../HTTP/HTTPService.h"
#include "MotionControllerComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"

AVRNetworkCharacter::AVRNetworkCharacter() : ABaseNetworkCharacter() {
	if (!ControllerClass) ControllerClass = AVRControllerActor::StaticClass();
	
	// Setup rotation options (prevent sickness)
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Setup VR Origin
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(RootComponent);

	// Setup camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
	
	// Create a mesh component that will be used when being viewed from a '3rd person' view (when not controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetupAttachment(VROrigin);
	Mesh3P->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
	Mesh3P->SetRelativeLocation(FVector(0.0f, 0.0f, -91.7f)); // Position the mesh so that it aligns with the capsule
}

void AVRNetworkCharacter::BeginPlay() {
	Super::BeginPlay();

	// We are in standing mode
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	
	// Spawn the right controller
	RController = GetWorld()->SpawnActor<AVRControllerActor>(ControllerClass->GetAuthoritativeClass(), FVector(), FRotator());
	RController->SetOwner(this);
	RController->SetSource(FName("Right"));

	// Spawn the left controller
	LController = GetWorld()->SpawnActor<AVRControllerActor>(ControllerClass->GetAuthoritativeClass(), FVector(), FRotator());
	LController->SetOwner(this);
	LController->SetSource(FName("Left"));

	// Attach to the VR Origin
	// TODO: This still doesnt work (fix)
	RController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
	LController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
	
	// Show or hide meshes based on if we are locally controlling the character
	Mesh3P->SetVisibility(!IsLocallyControlled());
}

void AVRNetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	// If we are moving too fast, slow down
	if (GetLocalRole() == ROLE_Authority) {
		if (GetMovementComponent()->Velocity.X > XYMaxSpeed) {
			GetMovementComponent()->Velocity.X = XYMaxSpeed - (XYMaxSpeed / 10);
		} if (GetMovementComponent()->Velocity.X < -XYMaxSpeed) {
			GetMovementComponent()->Velocity.X = -XYMaxSpeed + (XYMaxSpeed / 10);
		} if (GetMovementComponent()->Velocity.Y > XYMaxSpeed) {
			GetMovementComponent()->Velocity.Y = XYMaxSpeed - (XYMaxSpeed / 10);
		} if (GetMovementComponent()->Velocity.Y < -XYMaxSpeed) {
			GetMovementComponent()->Velocity.Y = -XYMaxSpeed + (XYMaxSpeed / 10);
		} if (GetMovementComponent()->Velocity.Z > ZMaxSpeed) {
			GetMovementComponent()->Velocity.Z = ZMaxSpeed - (ZMaxSpeed / 10);
		} if (GetMovementComponent()->Velocity.Z < -ZMaxSpeed) {
			GetMovementComponent()->Velocity.Z = -ZMaxSpeed + (ZMaxSpeed / 10);
		}
	}
}