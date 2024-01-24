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
	
	virtual void PossessedBy(AController* NewController) override;

	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	// �Ѻ��� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* gunComp;

public:
	// Input Action 
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* takePistolActoin;
	// �ʿ�Ӽ� : �Ѽ�������, 
	UPROPERTY(Replicated)
	bool bHasPistol = false;
	// ���� �� �ִ� ����
	UPROPERTY(EditAnywhere, Category="Pistol")
	float distanceToGun = 200;
	// �������� ��
	UPROPERTY(Replicated)
	AActor* ownedPistol = nullptr;

	// ����� ó���� �Լ�
	void TakePistol(const FInputActionValue& value);
	// ���� ������Ʈ�� ���̴� �Լ�
	void AttachPistol(AActor* pistolActor);

	// ----------- �� ���� -------------
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* releasePistolActoin;
	// �ѳ��� ó���� �Լ�
	void ReleasePistol(const FInputActionValue& value);
	void DetachPistol(AActor* pistolActor);

	// ------------ �ѽ�� -------------
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* fireActoin;
	void Fire(const FInputActionValue& value);

	UPROPERTY(EditDefaultsOnly, Category="Pistol")
	class UParticleSystem* gunEffect;

// ------- main ui ---------
public:
	/*UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<class UMainUI> mainUIWidget;*/
	UPROPERTY()
	class UMainUI* mainUI;

	void InitUIWidget();

	// Client ������ ȣ��
	virtual void PostNetInit() override;
public:
	// �ִ� �Ѿ˰���
	UPROPERTY(EditAnywhere, Category="Bullet")
	int32 maxBulletCount = 10;
	// ���� �Ѿ˰���
	//UPROPERTY(ReplicatedUsing=OnRep_BulletCount)
	int32 bulletCount = maxBulletCount;
	UFUNCTION()
	void OnRep_BulletCount();
// --------- ������ ---------
public:
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* reloadActoin;
	void ReloadPistol(const FInputActionValue& value);

	// �Ѿ� UI ����
	void InitAmmoUI();
	// ������ ������ ���� ���
	bool isReloading = false;

public:
	// --------------- �÷��̾� ü�� ---------------
	UPROPERTY(EditDefaultsOnly, Category="HP")
	float MaxHP = 3;
	// ���� ü��
	UPROPERTY(ReplicatedUsing=OnRep_HP)
	float hp = MaxHP;
	UFUNCTION()
	void OnRep_HP();

	__declspec(property(get=GetHP, put=SetHP))
	float HP;
	float GetHP();
	void SetHP(float value);

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* hpUIComp;

	// �ǰ�ó���Լ�
	void DamageProcess();

	// �ǰ� ī�޶����ũ
	UPROPERTY(EditDefaultsOnly, Category="Damage")
	TSubclassOf<class UCameraShakeBase> damageCameraShake;

// ------- ����ó�� --------
public:
	//UPROPERTY(Replicated)
	bool isDead = false;

	// ����ó���Լ�
	void DieProcess();

public:
	virtual void Tick( float DeltaSeconds ) override;
	// ��Ʈ��ũ �α���� �Լ�
	void PrintNetLog();

// ----------------- Multiplayer ��ҵ� -----------------
public:
	UFUNCTION(Server, Reliable)
	void ServerRPCTakePistol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCTakePistol(AActor* pistolActor);

	// �ѳ���
	UFUNCTION(Server, Reliable)
	void ServerRPCReleasePistol();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCReleasePistol(AActor* pistolActor);

	// �ѽ��
	UFUNCTION(Server, Reliable)
	void ServerRPCFire();
	UFUNCTION(NetMulticast, Reliable)
	void MultiRPCFire(bool bHit, const FHitResult& hitInfo, const int bc);

	// ������
	UFUNCTION(Server, Reliable)
	void ServerRPCReload();
	UFUNCTION(Client, Reliable)
	void ClientRPCReload();

};

