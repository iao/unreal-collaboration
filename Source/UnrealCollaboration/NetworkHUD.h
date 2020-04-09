// By Paul Graham <p@ul.ms>

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "Blueprint/UserWidget.h"
#include "SignWidget.h"
#include "SignPawn.h"
#include "NetworkHUD.generated.h"

UCLASS()
class ANetworkHUD : public AHUD {
	GENERATED_BODY()

	virtual void BeginPlay() override;

public:
	ANetworkHUD();
	
	UFUNCTION()
		void Confirm();

	UFUNCTION()
		void Delete();

	void SetVisible(bool visible);

protected:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<USignWidget> PlayerUIClass;

	ESlateVisibility fromBoolean(bool boolean);

	class USignWidget* PlayerUI;

	FText text;

};
