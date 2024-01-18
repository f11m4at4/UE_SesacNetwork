// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 서버일때만
	// GameMode 인스턴스 할당
	if (HasAuthority())
	{
		gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRPCRespawnPlayer_Implementation()
{
	// 1. 기존 플레이어가 있어야한다.
	auto player = GetPawn();
	//  2. 얘를 Unpossess
	UnPossess();
	//  3. Destroy
	player->Destroy();
	// 4. respawn
	gm->RestartPlayer(this);
}
