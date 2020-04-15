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
	
	// Setup rotation options
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
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

	UE_LOG(LogTemp, Warning, TEXT("I am %d"), UGameplayStatics::GetPlayerControllerID(GetWorld()->GetFirstPlayerController()));

	// Set the floor
	UHeadMountedDisplayFunctionLibrary::SetTrackingOrigin(EHMDTrackingOrigin::Floor);
	
	// Spawn the controllers
	//if (IsLocallyControlled()) {
		// Spawn the right controller
		RController = GetWorld()->SpawnActor<AVRControllerActor>(ControllerClass->GetAuthoritativeClass(), VROrigin->GetComponentTransform());
		RController->SetOwner(this);
		RController->SetSource(FName("Right"));
	
		// Attach to the VR Origin
		RController->AttachToComponent(VROrigin, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false));
		//RController->AttachToComponent(VROrigin, FAttachmentTransformRules::KeepWorldTransform);
		//ServerSetController(controller, true);

		//RController = GetWorld()->SpawnActorDeferred<AVRControllerActor>(SpawnableClass->GetAuthoritativeClass(), FTransform());
		//RController->name = FName("Left");
		//UGameplayStatics::FinishSpawningActor(RController, FTransform());
		
		// Finally, reset our position
		UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
	//}
	
	// Show or hide meshes based on if we are locally controlling the character
	Mesh3P->SetVisibility(!IsLocallyControlled());
}

void AVRNetworkCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector location = GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("My location is %f %f %f"), location.X, location.Y, location.Z);
	
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

void AVRNetworkCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) {
	// Bind VR events (binding is always allowed on the server)
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind VR Actions
	check(PlayerInputComponent);
	if (PlayerInputComponent) {
		// Setup VR movement
		InputComponent->BindAxis("Move", this, &AVRNetworkCharacter::MoveVR);
	}
}

void AVRNetworkCharacter::MoveVR(float Value) {
	if (Value != 0.0f) {
		// Launch the player in the direction of the right motion controller
		AVRNetworkPlayerController* controller = Cast< AVRNetworkPlayerController>(GetWorld()->GetFirstPlayerController());
		if (controller) {
			FVector launch = RController->MotionController->GetComponentRotation().RotateVector(controller->PlayerCameraManager->GetActorForwardVector()) * LaunchSpeed * Value;
			ServerMoveVR(launch);
		}
	}
}

void AVRNetworkCharacter::ServerMoveVR_Implementation(FVector vector) {
	// Launch the player in the direction of the left motion controller
	LaunchCharacter(vector, false, !(GetMovementComponent()->Velocity.Z > 0.0f));
}

bool AVRNetworkCharacter::ServerMoveVR_Validate(FVector vector) {
	return true;
}