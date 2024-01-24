// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NetGameInstance.generated.h"

// ������(����) ����� ����ü
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

// ���� �˻��� �������� ȣ��� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchSignature, const FSessionInfo&, sessionInfo);
// ���ǰ˻� ���� ��������Ʈ
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSearchStateSignature, bool, bIsSearching);

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

	// ��ã��Ϸ� �ݹ��� ����� ��������Ʈ
	FSearchSignature onSearchCompleted;

	// ��ã����� �ݹ� ��������Ʈ
	FSearchStateSignature onSearchState;

	// ����(��) ���� �Լ�
	void JoinSelectedSession(int32 roomIndex);

	// ���� ���� ��������Ʈ �ݹ�
	void OnJoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result);
};
