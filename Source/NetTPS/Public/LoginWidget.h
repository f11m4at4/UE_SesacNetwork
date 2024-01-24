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

	// �����̴��� ����� ȣ��Ǵ� �ݹ��Լ�
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
	// --------- ���ǽ��� ----------
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UScrollBox* scroll_roomList;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_find;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_findingMsg;

	// ���ǽ��� ����
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class USessionSlotWidget> sessionSlotWidget;

	UFUNCTION()
	void AddSlotWidget(const struct FSessionInfo& sessionInfo);

	// Find ��ư Ŭ������ �� ȣ��� �ݹ�
	UFUNCTION()
	void OnClickedFindSession();

	// ��ã�� ���� �̺�Ʈ �ݹ�
	UFUNCTION()
	void OnChangeButtonEnabled(bool bIsSearching);

};
