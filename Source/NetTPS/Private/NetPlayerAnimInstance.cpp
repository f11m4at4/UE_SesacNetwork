// Fill out your copyright notice in the Description page of Project Settings.


#include "NetPlayerAnimInstance.h"
#include "NetTPSCharacter.h"

void UNetPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	player = Cast<ANetTPSCharacter>(TryGetPawnOwner());
}

void UNetPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (player)
	{
		// player �� �ӵ��� �����ͼ�
		FVector velocity = player->GetVelocity();
		// speed �� �Ҵ��ϰ� �ʹ�.
		speed = FVector::DotProduct(velocity, player->GetActorForwardVector());
		// direction �� �Ҵ��ϰ� �ʹ�.
		direction = FVector::DotProduct(velocity, player->GetActorRightVector());

		// ȸ���� ����
		pitchAngle = -player->GetBaseAimRotation().GetNormalized().Pitch;
		pitchAngle = FMath::Clamp(pitchAngle, -60, 60);

		bHasPistol = player->bHasPistol;
		isDead = player->isDead;
	}
}

void UNetPlayerAnimInstance::PlayFireAnimation()
{
	if (bHasPistol && fireMontage)
	{
		Montage_Play(fireMontage, 2);
	}
}

void UNetPlayerAnimInstance::PlayReloadAnimation()
{
	// �� ���� ���� �� ��Ÿ�� �÷���
	if (bHasPistol && reloadMontage)
	{
		Montage_Play(reloadMontage);
	}
}

void UNetPlayerAnimInstance::AnimNotify_OnReloadFinish()
{
	player->InitAmmoUI();
}
