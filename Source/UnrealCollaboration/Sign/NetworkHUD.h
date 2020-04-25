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

	/** Called when the confirm button on the widget was clicked */
	UFUNCTION()
		void Confirm();

	/** Called when the delete button on the widget was clicked */
	UFUNCTION()
		void Delete();

	/** Set components of the UI to be visible, setup default text */
	void SetVisible(bool visible);

protected:
	/** If the selector controls deletion */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TextInfo)
		bool useSelectorControl = false;
	
	/** The maximum allowed text size */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = TextInfo)
		int32 MaximumTextSize = 100;

	/** The class which to use for widgets */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = SpawnInfo)
		TSubclassOf<USignWidget> PlayerUIClass;

	/** Converts boolean -> ESlateVisibility */
	ESlateVisibility fromBoolean(bool boolean);

	/** The PlayerUI widget*/
	class USignWidget* PlayerUI;

	/** The text last seen on a sign */
	FText text;

};
