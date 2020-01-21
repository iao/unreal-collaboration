// Fill out your copyright notice in the Description page of Project Settings.
#include "CubePawn.h"

// Sets default values
ACubePawn::ACubePawn() {
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bAddDefaultMovementBindings = false;
}

// Called when the game starts or when spawned
void ACubePawn::BeginPlay() {
	Super::BeginPlay();

	TextActor = GetWorld()->SpawnActor<ATextRenderActor>(ATextRenderActor::StaticClass(), GetActorLocation(), FRotator(0.f, 0.f, 0.f));
	TextActor->GetTextRender()->SetTextRenderColor(FColor::Red);
}

// Called every frame
void ACubePawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void ACubePawn::SetText(FText text) {
	TextActor->GetTextRender()->SetText(text);
}
