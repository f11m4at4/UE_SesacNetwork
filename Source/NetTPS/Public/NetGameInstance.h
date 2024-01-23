// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

// 방정보(세션) 기록할 구조체
USTRUCT(BlueprintType)
struct FSessionInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString roomName;
	UPROPERTY(BlueprintReadOnly)
	FString hostName;
	UPROPERTY(BlueprintReadOnly)
	FString playerCount;
	UPROPERTY(BlueprintReadOnly)
	int32 pingSpeed;
	UPROPERTY(BlueprintReadOnly)
	int32 index;

	inline FString ToString()
	{
		return FString::Printf(TEXT("[%d] %s : %s - %s, %d ms"), index, *roomName, *hostName, *playerCount, pingSpeed);
	}
};


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
