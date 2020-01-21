// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MyUserWidget.generated.h"

UCLASS()
class NETWORKINGTEST_API UMyUserWidget : public UUserWidget {
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* TheButton;

protected:
	virtual bool Initialize() override;

	UFUNCTION()
	void Test();
};
