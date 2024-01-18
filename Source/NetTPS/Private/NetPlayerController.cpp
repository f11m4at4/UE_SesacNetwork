// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"

void ANetPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// �����϶���
	// GameMode �ν��Ͻ� �Ҵ�
	if (HasAuthority())
	{
		gm = Cast<ANetTPSGameMode>(GetWorld()->GetAuthGameMode());
	}
}

void ANetPlayerController::ServerRPCRespawnPlayer_Implementation()
{
	// 1. ���� �÷��̾ �־���Ѵ�.
	auto player = GetPawn();
	//  2. �긦 Unpossess
	UnPossess();
	//  3. Destroy
	player->Destroy();
	// 4. respawn
	gm->RestartPlayer(this);
}
