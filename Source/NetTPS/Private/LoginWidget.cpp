// Fill out your copyright notice in the Description page of Project Settings.


#include "LoginWidget.h"
#include <Components/Button.h>
#include "NetGameInstance.h"
#include <Components/EditableText.h>
#include <Components/Slider.h>
#include <Components/TextBlock.h>
#include <Components/WidgetSwitcher.h>
#include "SessionSlotWidget.h"
#include <Components/ScrollBox.h>

void ULoginWidget::NativeConstruct()
{
	Super::NativeConstruct();

	gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());
	// �����߰� �̺�Ʈ �ݹ� ���
	gi->onSearchCompleted.AddDynamic(this, &ULoginWidget::AddSlotWidget);
	// ���� �˻� ������ ���� �̺�Ʈ �ݹ� ���
	gi->onSearchState.AddDynamic(this, &ULoginWidget::OnChangeButtonEnabled);


	btn_find->OnClicked.AddDynamic(this, &ULoginWidget::OnClickedFindSession);

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
	// GameInstance �� �ִ� CreateMyRoom() �Լ� ȣ��
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
	FString userName = edit_userName->GetText().ToString();
	if (userName.IsEmpty() == false)
	{
		gi->mySessionName = userName;
	}

	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void ULoginWidget::SwitchFindPanel()
{
	FString userName = edit_userName->GetText().ToString();
	if (userName.IsEmpty() == false)
	{
		gi->mySessionName = userName;
	}

	WidgetSwitcher->SetActiveWidgetIndex(2);
	// ���� �˻�
	OnClickedFindSession();
}

void ULoginWidget::BackToMain()
{
	WidgetSwitcher->SetActiveWidgetIndex(0);
}

void ULoginWidget::AddSlotWidget(const struct FSessionInfo& sessionInfo)
{
	// ���� ����
	auto slot = CreateWidget<USessionSlotWidget>(this, sessionSlotWidget);
	// ������ �Ҵ�
	slot->Set(sessionInfo);
	// ��ũ�ѹڽ��� �߰�
	scroll_roomList->AddChild(slot);
}

void ULoginWidget::OnClickedFindSession()
{
	// ���� ����
	scroll_roomList->ClearChildren();

	// ���� �˻�
	if (gi)
	{
		gi->FindOtherSessions();
	}
}

void ULoginWidget::OnChangeButtonEnabled(bool bIsSearching)
{
	btn_find->SetIsEnabled(!bIsSearching);
	if (bIsSearching)
	{
		txt_findingMsg->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		txt_findingMsg->SetVisibility(ESlateVisibility::Hidden);
	}
}
