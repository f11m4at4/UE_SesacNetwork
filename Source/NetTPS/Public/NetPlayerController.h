// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NetPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	// 리스폰 역할은 GameMode 담당을 한다.
	UPROPERTY()
	class ANetTPSGameMode* gm;

public:
	virtual void BeginPlay() override;

	// Respawn Server RPC
	UFUNCTION(Server, Reliable)
	void ServerRPCRespawnPlayer();

// ------- main ui ---------
public:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UMainUI> mainUIWidget;
	UPROPERTY()
	class UMainUI* mainUI;
};
