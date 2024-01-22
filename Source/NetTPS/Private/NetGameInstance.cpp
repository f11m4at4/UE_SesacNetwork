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

	// 온라인세션인터페이스 만들기
	auto subsys = IOnlineSubsystem::Get();
	if (subsys)
	{
		sessionInterface = subsys->GetSessionInterface();
		// 콜백
		sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UNetGameInstance::OnCreateSessionComplete);
		// 검색 콜백
		sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UNetGameInstance::OnFindSessionsComplete);
	}
	
	////세션생성함수 호출
	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
		{
			FindOtherSessions();
		}
	), 2, false);
}

void UNetGameInstance::CreateMySession(const FString roomName, const int32 playerCount)
{
	// 세션 설정
	FOnlineSessionSettings sessionSettings;

	// 1. Dedicated server 사용여부
	sessionSettings.bIsDedicated = false;
	// 2. 랜선매칭(로컬), 스팀매칭 사용할지 여부
	FName subsysName = IOnlineSubsystem::Get()->GetSubsystemName();
	sessionSettings.bIsLANMatch = subsysName == "NULL";

	// 3. 매칭이 온라인을 통해 노출될지 여부
	// fals 일경우에는 초대를 통해서만 입장가능
	sessionSettings.bShouldAdvertise = true;

	// 4. 온라인 상태 정보를 활용하게 할지 여부
	sessionSettings.bUsesPresence = true;

	// 5. 게임진행중에 join 할수 있는지 여부
	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bAllowJoinViaPresence = true;

	// 6. 최대 허용인원수
	sessionSettings.NumPublicConnections = playerCount;

	// 7. 커스텀 옵션 -> 룸네임
	sessionSettings.Set(FName("ROOM_NAME"), roomName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("HOST_NAME"), mySessionName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	// 세션 생성
	// netid
	FUniqueNetIdPtr netID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	sessionInterface->CreateSession(*netID, FName(mySessionName), sessionSettings);
}

void UNetGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	PRINTLOG(TEXT("Create Session Start : %s, bWasSuccessful : %d"), *SessionName.ToString(), bWasSuccessful);
}

void UNetGameInstance::FindOtherSessions()
{
	// 검색조건설정
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	// 1. presence 설정이 true 로 되어 있는 세션만 검색
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	// 2. 랜 매칭 여부
	sessionSearch->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";

	// 3. 최대 검색 세션(방) 수
	sessionSearch->MaxSearchResults = 10;

	// 4. 검색
	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}

void UNetGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	// 찾기 실패시
	if (bWasSuccessful == false)
	{
		PRINTLOG(TEXT("Session search failed..."));
		return;
	}

	// 세션검색결과
	auto results = sessionSearch->SearchResults;
	PRINTLOG(TEXT("Search Result Count : %d"), results.Num());
}
