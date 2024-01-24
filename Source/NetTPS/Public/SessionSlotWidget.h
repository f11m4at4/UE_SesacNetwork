// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SessionSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API USessionSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_roomName;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_hostName;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_playerCount;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_pingSpeed;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UTextBlock* txt_sessionNumber;
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	class UButton* btn_join;

	int32 sessionNumber = 0;

	void Set(const struct FSessionInfo& sessionInfo);

public:
	// ------------ 세션조인 ---------------
	virtual void NativeConstruct() override;

	UFUNCTION()
	void JoinSession();
};
