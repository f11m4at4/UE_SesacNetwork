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
		// player 의 속도를 가져와서
		FVector velocity = player->GetVelocity();
		// speed 에 할당하고 싶다.
		speed = FVector::DotProduct(velocity, player->GetActorForwardVector());
		// direction 에 할당하고 싶다.
		direction = FVector::DotProduct(velocity, player->GetActorRightVector());

		// 회전값 적용
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
	// 총 갖고 있을 때 몽타주 플레이
	if (bHasPistol && reloadMontage)
	{
		Montage_Play(reloadMontage);
	}
}

void UNetPlayerAnimInstance::AnimNotify_OnReloadFinish()
{
	player->InitAmmoUI();
}
