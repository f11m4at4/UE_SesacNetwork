// Fill out your copyright notice in the Description page of Project Settings.


#include "SessionSlotWidget.h"
#include "NetGameInstance.h"
#include <Components/TextBlock.h>
#include <Components/Button.h>

void USessionSlotWidget::Set(const FSessionInfo& sessionInfo)
{
	// ����(��)��ȣ
	txt_sessionNumber->SetText(FText::FromString(FString::Printf(TEXT("[%d]"), sessionInfo.index)));
	// ���̸�
	txt_roomName->SetText(FText::FromString(sessionInfo.roomName));
	txt_hostName->SetText(FText::FromString(sessionInfo.hostName));
	txt_playerCount->SetText(FText::FromString(sessionInfo.playerCount));
	// ping
	txt_pingSpeed->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), sessionInfo.pingSpeed)));

	sessionNumber = sessionInfo.index;
}

void USessionSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	btn_join->OnClicked.AddDynamic(this, &USessionSlotWidget::JoinSession);
}

void USessionSlotWidget::JoinSession()
{
	auto gi = Cast<UNetGameInstance>(GetWorld()->GetGameInstance());

	if (gi)
	{
		gi->JoinSelectedSession(sessionNumber);
	}
}
