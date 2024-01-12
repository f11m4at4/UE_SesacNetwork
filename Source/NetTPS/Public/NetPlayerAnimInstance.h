// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NetPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NETTPS_API UNetPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	bool bHasPistol = false;

	// Blend space ���� ����� ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float speed;

	// ������ ���庯��
	UPROPERTY()
	class ANetTPSCharacter* player;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// �ѽ�⿡�� ����� ��Ÿ��
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* fireMontage;
	// �ѽ�� ����Լ�
	void PlayFireAnimation();

	// pitch ȸ����
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MyAnimSettings")
	float pitchAngle;

public:
	// --------- ������ -----------
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* reloadMontage;
	// ������ �ִϸ��̼� ����Լ�
	void PlayReloadAnimation();
	// ������ ������ �� ȣ��� Notify �̺�Ʈ�Լ�
	UFUNCTION()
	void AnimNotify_OnReloadFinish();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyAnimSettings")
	bool isDead = false;

};
