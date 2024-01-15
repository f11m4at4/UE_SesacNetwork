// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "NetTPSCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANetTPSCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	ANetTPSCharacter();
	

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	// 총붙일 컴포넌트
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* gunComp;

public:
	// Input Action 
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* takePistolActoin;
	// 필요속성 : 총소유여부, 
	UPROPERTY(Replicated)
	bool bHasPistol = false;
	// 잡을 수 있는 범위
	UPROPERTY(EditAnywhere, Category="Pistol")
	float distanceToGun = 200;
	// 소유중인 총
	UPROPERTY()
	AActor* ownedPistol = nullptr;

	// 총잡기 처리할 함수
	void TakePistol(const FInputActionValue& value);
	// 총을 컴포넌트로 붙이는 함수
	void AttachPistol(AActor* pistolActor);

	// ----------- 총 놓기 -------------
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* releasePistolActoin;
	// 총놓기 처리할 함수
	void ReleasePistol(const FInputActionValue& value);
	void DetachPistol(AActor* pistolActor);

	// ------------ 총쏘기 -------------
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* fireActoin;
	void Fire(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category="Pistol")
	class UParticleSystem* gunEffect;

// ------- main ui ---------
public:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UMainUI> mainUIWidget;
	UPROPERTY()
	class UMainUI* mainUI;

	void InitUIWidget();

public:
	// 최대 총알개수
	UPROPERTY(EditAnywhere, Category="Bullet")
	int32 maxBulletCount = 10;
	// 남은 총알개수
	int32 bulletCount = maxBulletCount;

// --------- 재장전 ---------
public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* reloadActoin;
	void ReloadPistol(const FInputActionValue& value);

	// 총알 UI 리셋
	void InitAmmoUI();
	// 재장전 중인지 여부 기억
	bool isReloading = false;

public:
	// --------------- 플레이어 체력 ---------------
	UPROPERTY(EditDefaultsOnly, Category="HP")
	float MaxHP = 3;
	// 현재 체력
	float hp = MaxHP;

	__declspec(property(get=GetHP, put=SetHP))
	float HP;
	float GetHP();
	void SetHP(float value);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* hpUIComp;

	// 피격처리함수
	void DamageProcess();

// ------- 죽음처리 --------
public:
	bool isDead = false;

public:
	virtual void Tick( float DeltaSeconds ) override;
	// 네트워크 로그찍는 함수
	void PrintNetLog();

// ----------------- Multiplayer 요소들 -----------------
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCTakePistol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCTakePistol(AActor* pistolActor);

	// 총놓기
	UFUNCTION(Server, Reliable)
	void ServerRPCReleasePistol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCReleasePistol(AActor* pistolActor);

};

