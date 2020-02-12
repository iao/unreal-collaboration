// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "Components/TextRenderComponent.h"
#include "MyTextRenderActor.generated.h"

/**
 * 
 */
UCLASS()
class NETWORKINGTEST_API AMyTextRenderActor : public ATextRenderActor {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Disable")
	void HideActor(bool toHide);
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_Text)
	FText text;
	
	AMyTextRenderActor();

	UFUNCTION()
	void OnRep_Text();
	
};
