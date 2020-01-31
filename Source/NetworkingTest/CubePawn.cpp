// Fill out your copyright notice in the Description page of Project Settings.
#include "CubePawn.h"
#include "UnrealNetwork.h"

// Sets default values
ACubePawn::ACubePawn() {
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = false;

	SetReplicates(true);
}

// Called when the game starts or when spawned
void ACubePawn::BeginPlay() {
	Super::BeginPlay();

	// If we are the server, then spawn the text actor
	if (GetLocalRole() == ROLE_Authority) {
		TextActor = GetWorld()->SpawnActor<AMyTextRenderActor>(AMyTextRenderActor::StaticClass(), GetActorLocation(), FRotator(0.f, 0.f, 0.f));
		TextActor->text = FText::FromString("hello");
	}
}

// Called every frame
void ACubePawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	//if(TextActor) UE_LOG(LogTemp, Warning, TEXT("Text is '%s'"), *TextActor->text.ToString());
	//if (GetLocalRole() != ROLE_Authority) UE_LOG(LogTemp, Warning, TEXT("%hhd"), TextActor == nullptr);
}

// Called to set the text
void ACubePawn::SetText(FText text) {
	ServerSetText(text);
}

void ACubePawn::ServerSetText_Implementation(const FText& text) {
	TextActor->text = text;
}

bool ACubePawn::ServerSetText_Validate(const FText& text) {
	return true;
}