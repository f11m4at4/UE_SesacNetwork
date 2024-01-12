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

	// Blend space 에서 사용할 변수선언
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float direction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MyAnimSettings")
	float speed;

	// 소유폰 저장변수
	UPROPERTY()
	class ANetTPSCharacter* player;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:
	// 총쏘기에서 사용할 몽타주
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* fireMontage;
	// 총쏘기 재생함수
	void PlayFireAnimation();

	// pitch 회전값
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="MyAnimSettings")
	float pitchAngle;

public:
	// --------- 재장전 -----------
	UPROPERTY(EditDefaultsOnly, Category="Anim")
	class UAnimMontage* reloadMontage;
	// 재장전 애니메이션 재생함수
	void PlayReloadAnimation();
	// 재장전 끝났을 때 호출될 Notify 이벤트함수
	UFUNCTION()
	void AnimNotify_OnReloadFinish();

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="MyAnimSettings")
	bool isDead = false;

};
