// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerState.h"
#include "NetGameInstance.h"

void ANetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	auto con = GetPlayerController();
	if (con && con->IsLocalController())
	{
		auto gi = GetWorld()->GetGameInstance<UNetGameInstance>();
		ServerRPC_SetUserName(gi->mySessionName);
	}
}

void ANetPlayerState::ServerRPC_SetUserName_Implementation(const FString& name)
{
	SetPlayerName(name);
}
