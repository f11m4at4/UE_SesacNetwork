// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerController.h"
#include "NetTPSGameMode.h"
#include <GameFramework/SpectatorPawn.h>

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

void ANetPlayerController::ServerRPCChangeToSpectator_Implementation()
{
	// 1. 기존 사용하던 pawn 가져오기
	auto player = GetPawn();
	// 2. spawn 관전자 pawn
	//  -> Transform
	FActorSpawnParameters params;
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	auto sc = gm->SpectatorClass;
	auto trans = player->GetActorTransform();
	auto spectator = GetWorld()->SpawnActor<ASpectatorPawn>(sc, trans, params);
	// 3. Possess
	Possess(spectator);
	// 4. 기존 pawn 제거
	player->Destroy();

	// 5초후에 리스폰 처리
	FTimerHandle handle;
	GetWorldTimerManager().SetTimer(handle, this, &ANetPlayerController::ServerRPCRespawnPlayer_Implementation, 5, false);
}
