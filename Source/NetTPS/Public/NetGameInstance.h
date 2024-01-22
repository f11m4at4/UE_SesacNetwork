// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UNetGameInstance();

	virtual void Init() override;

public:
	// 세션 설정
	IOnlineSessionPtr sessionInterface;

	// 세션(호스트)이름
	FString mySessionName = "Brad";

	// 세션생성함수
	void CreateMySession(const FString roomName, const int32 playerCount);

	// 세션생성 완료 이벤트 콜백
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

public:
	// ---------- 방검색 ------------
	// 검색조건설정
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void FindOtherSessions();
	// 세션검색 완료시 이벤트 콜백
	void OnFindSessionsComplete(bool bWasSuccessful);
};
