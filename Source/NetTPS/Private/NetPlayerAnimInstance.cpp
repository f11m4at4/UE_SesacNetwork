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
		bHasPistol = true;

		// player �� �ӵ��� �����ͼ�
		FVector velocity = player->GetVelocity();
		// speed �� �Ҵ��ϰ� �ʹ�.
		speed = FVector::DotProduct(velocity, player->GetActorForwardVector());
		// direction �� �Ҵ��ϰ� �ʹ�.
		direction = FVector::DotProduct(velocity, player->GetActorRightVector());
	}
}
