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

		// player 의 속도를 가져와서
		FVector velocity = player->GetVelocity();
		// speed 에 할당하고 싶다.
		speed = FVector::DotProduct(velocity, player->GetActorForwardVector());
		// direction 에 할당하고 싶다.
		direction = FVector::DotProduct(velocity, player->GetActorRightVector());
	}
}
