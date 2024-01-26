// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetTPSCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include <../../../../../../../Source/Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include "NetPlayerAnimInstance.h"
#include "MainUI.h"
#include <Components/WidgetComponent.h>
#include "HealthBar.h"
#include "NetTPS.h"
#include <Net/UnrealNetwork.h>
#include <Components/HorizontalBox.h>
#include "NetPlayerController.h"
#include "NetPlayerState.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// ANetTPSCharacter

ANetTPSCharacter::ANetTPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->TargetArmLength = 150.0f;
	CameraBoom->SetRelativeLocation(FVector(0, 40, 60));

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	gunComp = CreateDefaultSubobject<USceneComponent>(TEXT("GunComp"));
	gunComp->SetupAttachment(GetMesh(), TEXT("GunPosition"));
	gunComp->SetRelativeLocation(FVector(-14.232308f, 2.496198f, 4.253737f));
	gunComp->SetRelativeRotation(FRotator(20, 80, 10));

	// Health component
	hpUIComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	hpUIComp->SetupAttachment(GetMesh());

	// ���� ���ø����̼� ����ϰڴ�.
	bReplicates = true;
	SetReplicateMovement(true);
}

void ANetTPSCharacter::PossessedBy(AController* NewController)
{
	PRINTLOG(TEXT("Begin, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
	Super::PossessedBy(NewController);

	// ���� ���������� (���� �÷��̾�����) üũ
	if (IsLocallyControlled())
	{
		// UI �ʱ�ȭ
		InitUIWidget();
	}
	PRINTLOG(TEXT("End, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
}

void ANetTPSCharacter::BeginPlay()
{
	PRINTLOG(TEXT("Begin, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));
	// Call the base class  
	Super::BeginPlay();
	
	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Client â���� ����Ǵ� �ڵ忩�� �Ѵ�.
	// ���� �������̰�, Client ���� Ȯ��
	if (IsLocallyControlled() && HasAuthority() == false)
	{
		InitUIWidget();
	}

	PRINTLOG(TEXT("End, Controller : %s"), Controller ? TEXT("Valid") : TEXT("Invalid"));

}

void ANetTPSCharacter::TakePistol(const FInputActionValue& value)
{
	// ���� �����ϰ� ���� �ʴٸ� �������� �ȿ� �ִ� ���� ��´�.
	// �ʿ�Ӽ� : �Ѽ�������, ���� �� �ִ� ����
	// 1. ���� �����ϰ����� �����ϱ�
	if (bHasPistol == true || isDead)
	{
		return;
	}
	
	ServerRPCTakePistol();
}

void ANetTPSCharacter::AttachPistol(AActor* pistolActor)
{
	auto meshComp = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	meshComp->SetSimulatePhysics(false);
	meshComp->AttachToComponent(gunComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	if (mainUI)
	{
		mainUI->ShowCrosshair(true);
	}
}

void ANetTPSCharacter::ReleasePistol(const FInputActionValue& value)
{
	// ���� ����ִٸ� ����ʹ�.
	if (bHasPistol == false || isReloading || IsLocallyControlled() == false)
	{
		return;
	}
	
	ServerRPCReleasePistol();
}

void ANetTPSCharacter::DetachPistol(AActor* pistolActor)
{
	auto meshComop = pistolActor->GetComponentByClass<UStaticMeshComponent>();
	meshComop->SetSimulatePhysics(true);
	meshComop->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);

	if (mainUI)
	{
		mainUI->ShowCrosshair(false);
	}
}

void ANetTPSCharacter::Fire(const FInputActionValue& value)
{
	// ���� �����ϰ� ���� �ʴٸ� ó������ �ʴ´�.
	if (bHasPistol == false || bulletCount <= 0 || isReloading || isDead)
	{
		return;
	}

	ServerRPCFire();
}

void ANetTPSCharacter::InitUIWidget()
{
	// Player �ƴϸ� ó������ �ʵ��� ����
	auto pc = Cast<ANetPlayerController>(Controller);
	if (pc == nullptr)
	{
		return;
	}


	if (pc->mainUIWidget)
	{
		// mainui �� ���� ���� ������
		if (pc->mainUI == nullptr)
		{
			pc->mainUI = Cast<UMainUI>(CreateWidget(GetWorld(), pc->mainUIWidget));

			pc->mainUI->AddToViewport();
		}
		mainUI = pc->mainUI;
		mainUI->ShowCrosshair(false);

		hp = MaxHP;
		float percent = hp / MaxHP;
		mainUI->hp = percent;
		// �Ѿ� ��� ����
		mainUI->RemoveAllAmmo();

		// �Ѿ� UI ����
		bulletCount = maxBulletCount;
		// �Ѿ��߰�
		for (int i=0;i<maxBulletCount;i++)
		{
			mainUI->AddBullet();
		}

		if (hpUIComp)
		{
			hpUIComp->SetVisibility(false);
		}
	}
}


void ANetTPSCharacter::PostNetInit()
{
	PRINTLOG(TEXT("Begin"));
	Super::PostNetInit();

	if (bHasPistol && ownedPistol)
	{
		AttachPistol(ownedPistol);
	}
	PRINTLOG(TEXT("End"));
}

void ANetTPSCharacter::ReloadPistol(const FInputActionValue& value)
{
	// �� �������� �ƴ϶�� Ȥ�� ������ ���ϰ�� ó������ ���� 
	if (bHasPistol == false || isReloading)
	{
		return;
	}

	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayReloadAnimation();

	isReloading = true;
}

// server rpc call
void ANetTPSCharacter::InitAmmoUI()
{
	ServerRPCReload();
}

void ANetTPSCharacter::OnRep_HP()
{
	// ����ó��
	if (HP <= 0)
	{
		isDead = true;
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		// �׾����ϱ� ���� ����
		ReleasePistol(FInputActionValue());
	}

	// UI �� �� �Ҵ�
	float percent = hp / MaxHP;
	if (mainUI)
	{
		mainUI->hp = percent;
		// �ǰ�ó��ȿ��
		mainUI->PlayDamageAnimation();

		// ī�޶����ũȿ�� ���
		if (damageCameraShake)
		{
			auto pc = Cast<APlayerController>(Controller);
			pc->ClientStartCameraShake(damageCameraShake);
		}
	}
	else
	{
		auto hpUI = Cast<UHealthBar>(hpUIComp->GetWidget());
		hpUI->hp = percent;
	}
}

float ANetTPSCharacter::GetHP()
{
	return hp;
}

void ANetTPSCharacter::SetHP(float value)
{
	hp = value;
	
	OnRep_HP();
}

void ANetTPSCharacter::DamageProcess()
{
	HP--;

	
}

void ANetTPSCharacter::DieProcess()
{
	GetFollowCamera()->PostProcessSettings.ColorSaturation = FVector4(0, 0, 0, 1);

	auto pc = Cast<APlayerController>(Controller);
	pc->SetShowMouseCursor(true);

	// Die uiǥ��
	mainUI->GameoverUI->SetVisibility(ESlateVisibility::Visible);
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PrintNetLog();

	// HP UI ������ó��
	if (hpUIComp && hpUIComp->GetVisibleFlag())
	{
		FVector camLoc = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation();
		FVector direction = camLoc - hpUIComp->GetComponentLocation();
		direction.Z = 0;
		hpUIComp->SetWorldRotation(direction.GetSafeNormal().ToOrientationRotator());
	}
}

void ANetTPSCharacter::PrintNetLog()
{
	const FString connStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString logStr = FString::Printf(TEXT("Connection : %s\nOwner:%s\nLocal Role:%s\nRemote Role:%s"), *connStr, *ownerStr, *LOCALROLE, *REMOTEROLE);
	
	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
}

void ANetTPSCharacter::ServerRPCTakePistol_Implementation()
{
	// 2. �����־���Ѵ�.(���忡�� ��δ� ã�ƿ���)
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	for (auto tempPistol : allActors)
	{
		// 2-1. ������ �˻��ؾ��Ѵ�.
		if (tempPistol->GetName().Contains("BP_Pistol") == false)
		{
			continue;
		}
		// 3. �Ѱ��� �Ÿ��� ���ؾ��Ѵ�.
		float distance = FVector::Dist(GetActorLocation(), tempPistol->GetActorLocation());
		// 4. ���������ȿ� ���� ������
		if (distance > distanceToGun)
		{
			// �ٽõ���
			continue;
		}
		// 5. ���� �����ϰ�ʹ�.
		bHasPistol = true;
		ownedPistol = tempPistol;
		ownedPistol->SetOwner(this);
		
		// ��� Ŭ���̾�Ʈ���� ����
		MultiRPCTakePistol(ownedPistol);
		break;
	}
}

void ANetTPSCharacter::MultiRPCTakePistol_Implementation(AActor* pistolActor)
{
	// 6. ���� ���ʹ�.
	AttachPistol(pistolActor);
}

void ANetTPSCharacter::ServerRPCReleasePistol_Implementation()
{
	if (ownedPistol)
	{
		MultiRPCReleasePistol(ownedPistol);

		bHasPistol = false;
		ownedPistol->SetOwner(nullptr);
		ownedPistol = nullptr;
	}
}

void ANetTPSCharacter::MultiRPCReleasePistol_Implementation(AActor* pistolActor)
{
	DetachPistol(pistolActor);

}

void ANetTPSCharacter::ServerRPCFire_Implementation()
{
	// �ѽ��
	FHitResult hitInfo;
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 10000;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	if (bHit)
	{
		// ���� ����� ������ ��� ������ ó��
		auto otherPlayer = Cast<ANetTPSCharacter>(hitInfo.GetActor());
		if (otherPlayer)
		{
			otherPlayer->DamageProcess();

			// ����� ������ �� ���� 1���� ����
			auto ps = GetPlayerState<ANetPlayerState>();
			ps->SetScore(ps->GetScore() + 1);
		}
	}

	// �Ѿ� ����
	bulletCount--;
	
	MultiRPCFire(bHit, hitInfo, bulletCount);
}

void ANetTPSCharacter::MultiRPCFire_Implementation(bool bHit, const FHitResult& hitInfo, const int bc)
{
	bulletCount = bc;
	if (bHit)
	{
		// �����ڸ��� ��ƼŬȿ�� ���
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld()
			, gunEffect, hitInfo.Location, FRotator());
	}

	if (mainUI)
	{
		mainUI->PopBullet(bulletCount);
	}
	
	// �ѽ�� �ִϸ��̼� ���
	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayFireAnimation();
}


void ANetTPSCharacter::ServerRPCReload_Implementation()
{
	// �Ѿ� ���� �ʱ�ȭ
	bulletCount = maxBulletCount;
	// client rpc call
	ClientRPCReload();
}

void ANetTPSCharacter::ClientRPCReload_Implementation()
{
	// �Ѿ� ���� �ʱ�ȭ
	bulletCount = maxBulletCount;
	if (mainUI)
	{
		mainUI->RemoveAllAmmo();

		// �Ѿ� ������ŭ �Ѿ�UI �߰��ϱ�
		for (int i = 0; i < maxBulletCount; i++)
		{
			mainUI->AddBullet();
		}
	}

	isReloading = false;
}

void ANetTPSCharacter::StartVoiceChat()
{
	auto pc = GetController<ANetPlayerController>();
	pc->StartTalking();
}

void ANetTPSCharacter::StopVoiceChat()
{
	auto pc = GetController<ANetPlayerController>();
	pc->StopTalking();
}

void ANetTPSCharacter::ServerRPC_SendMsg_Implementation(const FString& msg)
{
	// ��� Ŭ���̾�Ʈ������ �޽��� ����
	MultiRPC_SendMsg(msg);
}

void ANetTPSCharacter::MultiRPC_SendMsg_Implementation(const FString& msg)
{
	// �÷��̾���Ʈ�ѷ� ã�ƿ���
	auto pc = Cast<ANetPlayerController>(GetWorld()->GetFirstPlayerController());
	if (pc)
	{
		pc->mainUI->ReceiveMsg(msg);
	}
}

void ANetTPSCharacter::OnRep_BulletCount()
{
	if (mainUI)
	{
		mainUI->PopBullet(bulletCount);
	}
}
//////////////////////////////////////////////////////////////////////////
// Input

void ANetTPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANetTPSCharacter::Look);

		EnhancedInputComponent->BindAction(takePistolActoin, ETriggerEvent::Started, this, &ANetTPSCharacter::TakePistol);
		EnhancedInputComponent->BindAction(releasePistolActoin, ETriggerEvent::Started, this, &ANetTPSCharacter::ReleasePistol);
		EnhancedInputComponent->BindAction(fireActoin, ETriggerEvent::Started, this, &ANetTPSCharacter::Fire);
		EnhancedInputComponent->BindAction(reloadActoin, ETriggerEvent::Started, this, &ANetTPSCharacter::ReloadPistol);

		EnhancedInputComponent->BindAction(voiceActoin, ETriggerEvent::Started, this, &ANetTPSCharacter::StartVoiceChat);
		EnhancedInputComponent->BindAction(voiceActoin, ETriggerEvent::Completed, this, &ANetTPSCharacter::StopVoiceChat);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void ANetTPSCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANetTPSCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANetTPSCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetTPSCharacter, bHasPistol);
	DOREPLIFETIME(ANetTPSCharacter, hp);
	DOREPLIFETIME(ANetTPSCharacter, ownedPistol);
	//DOREPLIFETIME(ANetTPSCharacter, isDead);
	//DOREPLIFETIME(ANetTPSCharacter, bulletCount);
}