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
}

// Called every frame
void ACubePawn::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
