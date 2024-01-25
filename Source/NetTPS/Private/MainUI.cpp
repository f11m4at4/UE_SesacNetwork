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
		name.Append(FString::Printf(TEXT("%s\n"), *pState->GetPlayerName()));
	}
	
	txt_users->SetText(FText::FromString(name));
}
