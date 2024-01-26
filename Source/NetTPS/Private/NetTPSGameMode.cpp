// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSGameMode.h"
#include "NetTPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>

ANetTPSGameMode::ANetTPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

// GameMode �� �÷��̾ �����Ϸ��� �Ҷ� ��� PlayerStart ��ġ���� ����
// �����ϴ� �Լ�
AActor* ANetTPSGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	static int currentPlayer = 0;
	// ���忡 PlayerStart 2 �� �����
	// Blue�� 0 ��° ����, 1 ��°�� ����

	TArray<APlayerStart*> playerStarts;
	playerStarts.SetNum(2);
	for (TActorIterator<APlayerStart> it(GetWorld()); it ;++it)
	{
		APlayerStart* ps = *it;
		if (ps->ActorHasTag(TEXT("Blue")))
		{
			playerStarts[0] = ps;
		}
		else
		{
			playerStarts[1] = ps;
		}
	}
	currentPlayer = (currentPlayer + 1) % playerStarts.Num();
	return playerStarts[currentPlayer];
}
