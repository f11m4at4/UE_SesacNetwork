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

	// ��Ʈ��ũ �α���� �Լ�
	void PrintNetLog();

	// owner ���⿵��
	UPROPERTY(EditAnywhere)
	float searchDistance = 200;

	void FindOwner();

public:
	// Yaw �� ȸ���� ����ȭ ����
	//UPROPERTY(Replicated)
	UPROPERTY(ReplicatedUsing=OnRep_RotYaw)
	float rotYaw = 0;

	UFUNCTION()
	void OnRep_RotYaw();

public:
	UPROPERTY()
	class UMaterialInstanceDynamic* mat;
	// ������ ����ȭ�� ����
	UPROPERTY(ReplicatedUsing=OnRep_MatColor)
	FLinearColor matColor;
	UFUNCTION()
	void OnRep_MatColor();

	// ������ �����ų Server RPC �Լ�
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRPC_ChangeColor(const FLinearColor newColor);
	// ���� ����� �̺�Ʈ�� �޾Ƽ� ó���� Client RPC �Լ�
	UFUNCTION(Client, Unreliable)
	void ClientRPC_ChangeColor(const FLinearColor newColor);
	// ��� �������� ó���� NetMulticast RPC �Լ�
	UFUNCTION(NetMulticast, Unreliable)
	void MultiRPC_ChangeColor(const FLinearColor newColor);

};
