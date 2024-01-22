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
	// ���� ����
	IOnlineSessionPtr sessionInterface;

	// ����(ȣ��Ʈ)�̸�
	FString mySessionName = "Brad";

	// ���ǻ����Լ�
	void CreateMySession(const FString roomName, const int32 playerCount);

	// ���ǻ��� �Ϸ� �̺�Ʈ �ݹ�
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

public:
	// ---------- ��˻� ------------
	// �˻����Ǽ���
	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	void FindOtherSessions();
	// ���ǰ˻� �Ϸ�� �̺�Ʈ �ݹ�
	void OnFindSessionsComplete(bool bWasSuccessful);
};
