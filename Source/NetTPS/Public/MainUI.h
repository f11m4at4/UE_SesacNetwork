// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainUI.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UMainUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class UImage* img_crosshair;

	// 크로스헤어 보일지여부
	void ShowCrosshair(bool isShow);

public:
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class UUniformGridPanel* BulletPanel;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Bullet")
	TSubclassOf<class UUserWidget> bulletUIFactory;

	// 총알위젯 추가
	void AddBullet();
	// 총알제거
	void PopBullet(int32 index);
	// 모든 총알UI 제거
	void RemoveAllAmmo();

public:
	// -------------- 체력 -------------
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="HP")
	float hp = 1.0f;
	UPROPERTY(EditDefaultsOnly, meta=(BindWidgetAnim), Transient)
	class UWidgetAnimation* DamageAnim;
	// 피격처리 애니메이션 재생
	void PlayDamageAnimation();

// --------- 죽음 UI ------------
public:
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class UHorizontalBox* GameoverUI;
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class UButton* btn_retry;
	UPROPERTY(BlueprintReadWrite, Category="UI", meta=(BindWidget))
	class UButton* btn_exit;

public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnRetry();
};
