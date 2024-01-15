// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetActor.generated.h"

UCLASS()
class NETTPS_API ANetActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANetActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* meshComp;

	// 네트워크 로그찍는 함수
	void PrintNetLog();

	// owner 검출영역
	UPROPERTY(EditAnywhere)
	float searchDistance = 200;

	void FindOwner();

public:
	// Yaw 축 회전값 동기화 변수
	//UPROPERTY(Replicated)
	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float rotYaw = 0;

	UFUNCTION()
	void OnRep_RotYaw();

public:
	UPROPERTY()
	class UMaterialInstanceDynamic* mat;
	// 재질에 동기화될 색상
	UPROPERTY(ReplicatedUsing=OnRep_MatColor)
	FLinearColor matColor;
	UFUNCTION()
	void OnRep_MatColor();

	// 색상을 변경시킬 Server RPC 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ChangeColor(const FLinearColor newColor);
	// 색상 변경된 이벤트를 받아서 처리할 Client RPC 함수
	UFUNCTION(Client, Unreliable)
	void ClientRPC_ChangeColor(const FLinearColor newColor);
	// 모든 유저들이 처리할 NetMulticast RPC 함수
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_ChangeColor(const FLinearColor newColor);

};
