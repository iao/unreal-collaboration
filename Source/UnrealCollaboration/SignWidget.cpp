// By Paul Graham <p@ul.ms>

#include "SignWidget.h"
#include "NetworkHUD.h"

bool USignWidget::Initialize() {
    bool isInit = Super::Initialize();
    if (isInit) {
        if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &USignWidget::Confirm);
        if (DeleteButton) DeleteButton->OnClicked.AddDynamic(this, &USignWidget::Delete);
    } return isInit;
}

FText USignWidget::GetText() {
    return TheBox->GetText();
}

void USignWidget::Confirm() {
    Owner->Confirm();
}

void USignWidget::Delete() {
    Owner->Delete();
}