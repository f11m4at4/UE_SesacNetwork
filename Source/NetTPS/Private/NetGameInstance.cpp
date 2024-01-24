// Fill out your copyright notice in the Description page of Project Settings.


#include "NetGameInstance.h"
#include <OnlineSubsystem.h>
#include <OnlineSessionSettings.h>
#include "NetTPS.h"
#include <Online/OnlineSessionNames.h>


UNetGameInstance::UNetGameInstance()
{
	
}

void UNetGameInstance::Init()
{
	Super::Init();

	// �¶��μ����������̽� �����
	auto subsys = IOnlineSubsystem::Get();
	if (subsys)
	{
		sessionInterface = subsys->GetSessionInterface();
		// �ݹ�
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
		// �˻� �ݹ�
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionsComplete);
		sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnJoinSessionCompleted);
	}
	
	////���ǻ����Լ� ȣ��
	/*FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
		{
			FindOtherSessions();
		}
	), 2, false);*/
}

void UNetGameInstance::CreateMySession(const FString roomName, const int32 playerCount)
{
	// ���� ����
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated server ��뿩��
	sessionSettings.bIsDedicated = false;
	// 2. ������Ī(����), ������Ī ������� ����
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = subsysName == "NULL";

	// 3. ��Ī�� �¶����� ���� ������� ����
	// fals �ϰ�쿡�� �ʴ븦 ���ؼ��� ���尡��
	sessionSettings.bShouldAdvertise = true;

	// 4. �¶��� ���� ������ Ȱ���ϰ� ���� ����
	sessionSettings.bUsesPresence = true;

	// 5. ���������߿� join �Ҽ� �ִ��� ����
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;

	// 6. �ִ� ����ο���
	sessionSettings.NumPublicConnections = playerCount;

	// 7. Ŀ���� �ɼ� -> �����
	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("HOST_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// ���� ����
	// netid
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);
}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("Create Session Start : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);

	// �濡 ������ ó������
	if (bWasSuccessful)
	{
		GetWorld()->ServerTravel(TEXT("/Game/Net/Maps/BattleMap?listen"));
	}
}

void UNetGameInstance::FindOtherSessions()
{
	// �˻����Ǽ���
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. presence ������ true �� �Ǿ� �ִ� ���Ǹ� �˻�
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. �� ��Ī ����
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	// 3. �ִ� �˻� ����(��) ��
	sessionSearch->MaxSearchResults = 10;

	// 4. �˻�
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());

	// �� ��� ��˻����̴ٴ� �� �˾ƶ�~~
	onSearchState.Broadcast(true);
}

void UNetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// ã�� ���н�
	if (bWasSuccessful == false)
	{
		onSearchState.Broadcast(false);
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	// ���ǰ˻����
	auto results = sessionSearch->SearchResults;
	PRINTLOG(TEXT("Search Result Count : %d"), results.Num());

	for (int i=0;i<results.Num();i++)
	{
		auto sr = results[i];
		if (sr.IsValid() == false)
		{
			continue;
		}

		FSessionInfo sessionInfo;
		sessionInfo.index = i;

		// 1. ���̸�
		sr.Session.SessionSettings.Get(FName("ROOM_NAME"), sessionInfo.roomName);
		// 2. ȣ��Ʈ�̸�
		sr.Session.SessionSettings.Get(FName("HOST_NAME"), sessionInfo.hostName);
		// pc ������ �̸�
		FString ownerName = sr.Session.OwningUserName;
		// 3. �÷��̾�� (�ִ밡�ɼ� - �������尡�ɼ�)
		int32 maxPlayerCount = sr.Session.SessionSettings.NumPublicConnections;
		int32 currentPlayerCount = maxPlayerCount - sr.Session.NumOpenPublicConnections;

		sessionInfo.playerCount = FString::Printf(TEXT("(%d/%d)"), currentPlayerCount, maxPlayerCount);
		// 4. ������
		sessionInfo.pingSpeed = sr.PingInMs;

		PRINTLOG(TEXT("%s"), *sessionInfo.ToString());

		// ���� �߰�
		onSearchCompleted.Broadcast(sessionInfo);
	}

	onSearchState.Broadcast(false);
}

void UNetGameInstance::JoinSelectedSession(int32 roomIndex)
{
	auto sr = sessionSearch->SearchResults[roomIndex];

	sessionInterface->JoinSession(0, FName(mySessionName), sr);
}

void UNetGameInstance::OnJoinSessionCompleted(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	// Client �� ������ ���Ѵ�.
	// -> Client Travel
	if (result == EOnJoinSessionCompleteResult::Success)
	{
		// ������ ���� ������ url �� �ʿ�
		FString url;
		sessionInterface->GetResolvedConnectString(sessionName, url);

		PRINTLOG(TEXT("Client Travel url : %s"), *url);
		if (url.IsEmpty() == false)
		{
			// ���� ������!!!
			auto pc = GetWorld()->GetFirstPlayerController();
			pc->ClientTravel(url, ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		PRINTLOG(TEXT("Join session failed... %d"), result);
	}
}
