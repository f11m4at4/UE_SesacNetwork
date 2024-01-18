// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(NetLog, Log, All);

#define LOCALROLE (UEnum::GetValueAsString<ENetRole>(GetLocalRole()))
#define REMOTEROLE (UEnum::GetValueAsString<ENetRole>(GetRemoteRole()))

// 클라언트창에서 실행인지 서버인지
#define NETMODE (GetNetMode()==ENetMode::NM_Client ? TEXT("Client") : GetNetMode() == ENetMode::NM_Standalone ? TEXT("Standalone") : TEXT("Server"))

// 함수 호출되는 정보
#define CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__) + TEXT(")"))
// 정보 출력하는 함수 메크로
#define PRINTLOG(fmt, ...) \
UE_LOG(NetLog, Warning, TEXT("[%s]%s %s"), NETMODE, *CALLINFO, *FString::Printf(fmt, ##__VA_ARGS__))