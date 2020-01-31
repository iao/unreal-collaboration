// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DefaultPawn.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "MyTextRenderActor.h"
#include "CubePawn.generated.h"

UCLASS()
class NETWORKINGTEST_API ACubePawn : public ADefaultPawn {
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACubePawn();

	UPROPERTY(BlueprintReadOnly)
	AMyTextRenderActor* TextActor;
	
	void SetText(FText text);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSetText(const FText& text);
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};
