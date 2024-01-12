// Fill out your copyright notice in the Description page of Project Settings.


#include "MainUI.h"
#include <Components/Image.h>
#include <Components/UniformGridPanel.h>

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
