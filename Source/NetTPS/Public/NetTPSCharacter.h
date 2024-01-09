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
	// �Ѻ��� ������Ʈ
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* gunComp;

public:
	// Input Action 
	UPROPERTY(EditDefaultsOnly, Category="Input")
	UInputAction* takePistolActoin;
	// �ʿ�Ӽ� : �Ѽ�������, 
	bool bHasPistol = false;
	// ���� �� �ִ� ����
	UPROPERTY(EditAnywhere, Category="Pistol")
	float distanceToGun = 200;
	// �������� ��
	UPROPERTY()
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
};

