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

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UWidgetSwitcher* WidgetSwitcher;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_createSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_findSession;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_back_1;


	UFUNCTION()
	void SwitchCreatePanel();
	UFUNCTION()
	void SwitchFindPanel();
	UFUNCTION()
	void BackToMain();

public:
	// --------- 세션슬롯 ----------
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UScrollBox* scroll_roomList;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_find;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_findingMsg;

	// 세션슬롯 위젯
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> sessionSlotWidget;

	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& sessionInfo);

	// Find 버튼 클릭했을 때 호출될 콜백
	UFUNCTION()
	void OnClickedFindSession();

	// 방찾기 상태 이벤트 콜백
	UFUNCTION()
	void OnChangeButtonEnabled(bool bIsSearching);

};
