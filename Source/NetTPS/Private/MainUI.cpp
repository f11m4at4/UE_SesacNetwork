// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include <Components/Image.h>
#include <Components/UniformGridPanel.h>
#include <Components/HorizontalBox.h>
#include "NetPlayerController.h"
#include <Components/Button.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/GameStateBase.h>
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerState.h>
#include <../../../../../../../Source/Runtime/UMG/Public/Components/TextBlock.h>
#include <../../../../../../../Source/Runtime/UMG/Public/Components/EditableText.h>
#include "NetTPSCharacter.h"
#include "ChatWidget.h"
#include <../../../../../../../Source/Runtime/UMG/Public/Components/ScrollBox.h>
#include "NetTPS.h"

void UMainUI::ShowCrosshair(bool isShow)
{
	if (isShow)
	{
		img_crosshair->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		img_crosshair->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainUI::AddBullet()
{
	auto bulletWidget = CreateWidget(GetWorld(), bulletUIFactory);
	BulletPanel->AddChildToUniformGrid(bulletWidget, 0, BulletPanel->GetChildrenCount());
}

void UMainUI::PopBullet(int32 index)
{
	BulletPanel->RemoveChildAt(index);
}

void UMainUI::RemoveAllAmmo()
{
	/*for (auto bulletWidget : BulletPanel->GetAllChildren())
	{
		BulletPanel->RemoveChild(bulletWidget);
	}*/
	BulletPanel->ClearChildren();
}

void UMainUI::PlayDamageAnimation()
{
	PlayAnimation(DamageAnim);
}

void UMainUI::NativeConstruct()
{
	Super::NativeConstruct();

	// ��ư �̺�Ʈ ���
	btn_retry->OnClicked.AddDynamic(this, &UMainUI::OnRetry);
	btn_send->OnClicked.AddDynamic(this, &UMainUI::SendMsg);
}

void UMainUI::OnRetry()
{
	// Client �ڵ�
	// Sever ���� ������ ��û �ϰ�ʹ�.
	// 1. gameoverui hidden
	GameoverUI->SetVisibility(ESlateVisibility::Hidden);
	// 2. Mouse visibility
	auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		pc->SetShowMouseCursor(false);
		// 3. Server ���� Respawn ��û
		//pc->ServerRPCRespawnPlayer();
		pc->ServerRPCChangeToSpectator();
	}
}

void UMainUI::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// �ٸ� ����ڵ��� GameInstance �� �ִ� ���ǳ����� �������� �ʹ�.
	auto playerArr = GetWorld()->GetGameState()->PlayerArray;
	FString name;
	for (auto pState : playerArr)
	{
		name.Append(FString::Printf(TEXT("%s : %d\n"), *pState->GetPlayerName(), (int32)pState->GetScore()));
	}
	
	txt_users->SetText(FText::FromString(name));
}

void UMainUI::SendMsg()
{
	// �޽����� ������ �����ϰ� �ʹ�.
	FString msg = edit_input->GetText().ToString();
	edit_input->SetText(FText::GetEmpty());
	if (msg.IsEmpty() == false)
	{
		// 1. NetPlayerController �� �ʿ��ϴ�.
		auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
		// 2. NetTPSCharacter �� �ʿ�
		auto player = pc->GetPawn<ANetTPSCharacter>();
		// 3. ������ �����ϰ� �ʹ�.
		player->ServerRPC_SendMsg(msg);
	}
}

void UMainUI::ReceiveMsg(const FString& msg)
{
	// �޽��� �޾Ƽ� ChatWidget �� ���� �־��ֱ�
	auto msgWidget = CreateWidget<UChatWidget>(GetWorld(), chatWidget);
	msgWidget->txt_msg->SetText(FText::FromString(msg));
	// ��ũ�ѹڽ��� �߰����ֱ�
	scroll_msgList->AddChild(msgWidget);
	scroll_msgList->ScrollToEnd();
}
