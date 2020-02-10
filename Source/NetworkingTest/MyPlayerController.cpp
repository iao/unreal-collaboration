// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"
#include "ConstructorHelpers.h"
#include "Engine.h"
#include "UnrealNetwork.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	// stuff
	bShowMouseCursor = true;
	isPawn = false;
}

void AMyPlayerController::SetupInputComponent() {
	if (InputComponent == NULL) {
		InputComponent = NewObject<UInputComponent>(this, TEXT("Cool_InputComponent"));
		InputComponent->RegisterComponent();
	}
	
	check(InputComponent);
	if (InputComponent) {
		InputComponent->BindAction("Spawn", IE_Pressed, this, &AMyPlayerController::Spawn);
	}
}

void AMyPlayerController::Spawn() {
	ServerSpawn();
}

void AMyPlayerController::ServerSpawn_Implementation() {
	if (isPawn) {
		// Find the owner and possess
		Cast<ACubePawn>(ThePawn)->SetOwner(NULL);
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(ThePawn->GetInstigator());
		Possess(Actor);
		isPawn = false;
	} else {
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(GetPawnOrSpectator());

		// Try to find a pawn within a distance, ensure it doesnt have an owner
		for(TActorIterator<ACubePawn> CubeItr(GetWorld()); CubeItr; ++CubeItr) {
			ACubePawn* FoundPawn = Cast<ACubePawn>(*CubeItr);
			if(FoundPawn) {
				float distance = FVector::Dist(FoundPawn->GetActorLocation(), Actor->GetActorLocation());
				if(distance <= 500.0f && !FoundPawn->GetOwner()) {
					Possess(FoundPawn);
					isPawn = true;
					return;
				}
			}
		}
		
		// Otherwise Make the pawn from blueprinted pawn
		// Set the location etc of the new act
		const FVector Location = Actor->GetActorLocation() + FVector(0.f, 500.f, 0).RotateAngleAxis(Actor->GetActorRotation().Roll, FVector(0, 0, 1));
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Owner = Cast<AActor>(Actor);
		Params.Instigator = Cast<APawn>(Actor);

		// Spawn the pawn to the world
		ThePawn = GetWorld()->SpawnActor<APawn>(SpawnableClass->GetAuthoritativeClass(), Location, FRotator(0.f, 0.f, 360.f - Actor->GetActorRotation().Roll), Params);
		Possess(ThePawn);
		isPawn = true;
	}
}

bool AMyPlayerController::ServerSpawn_Validate() {
	return true;
}