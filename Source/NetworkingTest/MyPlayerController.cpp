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
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(ThePawn->GetInstigator());
		UE_LOG(LogTemp, Warning, TEXT("%d"), Actor == NULL);
		Possess(Actor);
		isPawn = false;
	} else {
		ANetworkingTestCharacter* Actor = Cast<ANetworkingTestCharacter>(GetPawnOrSpectator());
		
		const FVector Location = Actor->GetActorLocation() + FVector(10.f, 10.f, 0);
		FActorSpawnParameters Params = FActorSpawnParameters();
		Params.Owner = Cast<AActor>(Actor);
		Params.Instigator = Cast<APawn>(Actor);

		// Try to find a pawn within a distance, ensure it doesnt have an owner


		// If it doesn't exist -- make one
		ThePawn = GetWorld()->SpawnActor<ACubePawn>(ACubePawn::StaticClass(), Location, FRotator(0.f, 0.f, 0.f), Params);
		UE_LOG(LogTemp, Warning, TEXT("%d"), ThePawn->GetOwner() == NULL);
		Possess(ThePawn);
		isPawn = true;
	}
}

bool AMyPlayerController::ServerSpawn_Validate() {
	return true;
}