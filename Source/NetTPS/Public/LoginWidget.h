// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoginWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ULoginWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_createRoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UEditableText* edit_roomName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class USlider* slider_playerCount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_playerCount;

	UPROPERTY()
	class UNetGameInstance* gi;
public:
	virtual void NativeConstruct() override;
	UFUNCTION()
	void CreateRoom();

	// 슬라이더값 변경시 호출되는 콜백함수
	UFUNCTION()
	void OnValueChanged(float value);
};
