// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include <Components/Button.h>
#include "NetGameInstance.h"
#include <Components/EditableText.h>
#include <Components/Slider.h>
#include <Components/TextBlock.h>
#include <Components/WidgetSwitcher.h>

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());

	btn_createRoom->OnClicked.AddDynamic(this, &ULoginWidget::CreateRoom);
	slider_playerCount->OnValueChanged.AddDynamic(this, &ULoginWidget::OnValueChanged);

	// canvas switching event
	btn_createSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchCreatePanel);
	btn_findSession->OnClicked.AddDynamic(this, &ULoginWidget::SwitchFindPanel);
	btn_back->OnClicked.AddDynamic(this, &ULoginWidget::BackToMain);
	btn_back_1->OnClicked.AddDynamic(this, &ULoginWidget::BackToMain);
}

void ULoginWidget::CreateRoom()
{
	// GameInstance 에 있는 CreateMyRoom() 함수 호출
	// FString sessionName, int32 playerCount
	if (gi && edit_roomName->GetText().IsEmpty() == false)
	{
		FString roomName = edit_roomName->GetText().ToString();
		int32 playerCount = slider_playerCount->GetValue();
		gi->CreateMySession(roomName, playerCount);
	}
}

void ULoginWidget::OnValueChanged(float value)
{
	txt_playerCount->SetText(FText::AsNumber(value));
}

void ULoginWidget::SwitchCreatePanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::SwitchFindPanel()
{
	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void ULoginWidget::BackToMain()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}
