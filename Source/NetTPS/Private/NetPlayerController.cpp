// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include <GameFramework/SpectatorPawn.h>

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

void ANetPlayerController::ServerRPCChangeToSpectator_Implementation()
{
	// 1. ���� ����ϴ� pawn ��������
	auto player = GetPawn();
	// 2. spawn ������ pawn
	//  -> Transform
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto sc = gm->SpectatorClass;
	auto trans = player->GetActorTransform();
	auto spectator = GetWorld()->SpawnActor<ASpectatorPawn>(sc, trans, params);
	// 3. Possess
	Possess(spectator);
	// 4. ���� pawn ����
	player->Destroy();

	// 5���Ŀ� ������ ó��
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &ANetPlayerController::ServerRPCRespawnPlayer_Implementation, 5, false);
}
