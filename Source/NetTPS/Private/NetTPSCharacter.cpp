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
#include <../../../../../../../Source/Runtime/UMG/Public/Components/WidgetComponent.h>
#include "HealthBar.h"
#include "NetTPS.h"
#include <../../../../../../../Source/Runtime/Engine/Public/Net/UnrealNetwork.h>

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

	// 액터 리플리케이션 사용하겠다.
	bReplicates = true;
	SetReplicateMovement(true);
}

void ANetTPSCharacter::BeginPlay()
{
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

	InitUIWidget();

}

void ANetTPSCharacter::TakePistol(const FInputActionValue& value)
{
	// 총을 소유하고 있지 않다면 일정범위 안에 있는 총을 잡는다.
	// 필요속성 : 총소유여부, 잡을 수 있는 범위
	// 1. 총을 소유하고있지 않으니까
	if (bHasPistol == true)
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
	// 총을 잡고있다면 놓고싶다.
	if (bHasPistol == false || isReloading)
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
	// 총을 소유하고 있지 않다면 처리하지 않는다.
	if (bHasPistol == false || bulletCount <= 0 || isReloading)
	{
		return;
	}

	// 총쏘기
	FHitResult hitInfo;
	FVector startPos = FollowCamera->GetComponentLocation();
	FVector endPos = startPos + FollowCamera->GetForwardVector() * 10000;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	bool bHit = GetWorld()->LineTraceSingleByChannel(hitInfo, startPos, endPos, ECC_Visibility, params);
	if (bHit)
	{
		// 맞은자리에 파티클효과 재생
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld()
		, gunEffect, hitInfo.Location, FRotator());

		// 맞은 대상이 상대방일 경우 데미지 처리
		auto otherPlayer = Cast<ANetTPSCharacter>(hitInfo.GetActor());
		if (otherPlayer)
		{
			otherPlayer->DamageProcess();
		}
	}

	// 총알 제거
	bulletCount--;
	mainUI->PopBullet(bulletCount);

	// 총쏘기 애니메이션 재생
	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayFireAnimation();
}

void ANetTPSCharacter::InitUIWidget()
{
	// Player 아니면 처리하지 않도록 하자
	auto pc = Cast<APlayerController>(Controller);
	if (pc == nullptr)
	{
		return;
	}

	if (mainUIWidget)
	{
		mainUI = Cast<UMainUI>(CreateWidget(GetWorld(), mainUIWidget));
		mainUI->AddToViewport();
		mainUI->ShowCrosshair(false);

		// 총알 UI 세팅
		bulletCount = maxBulletCount;
		// 총알추가
		for (int i=0;i<maxBulletCount;i++)
		{
			mainUI->AddBullet();
		}
	}
}

void ANetTPSCharacter::ReloadPistol(const FInputActionValue& value)
{
	// 총 소지중이 아니라면 혹은 재장전 중일경우 처리하지 말자 
	if (bHasPistol == false || isReloading)
	{
		return;
	}

	auto anim = Cast<UNetPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	anim->PlayReloadAnimation();

	isReloading = true;
}

void ANetTPSCharacter::InitAmmoUI()
{
	mainUI->RemoveAllAmmo();
	// 총알 개수 초기화
	bulletCount = maxBulletCount;
	// 총알 개수만큼 총알UI 추가하기
	for (int i=0;i<maxBulletCount;i++)
	{
		mainUI->AddBullet();
	}

	isReloading = false;
}

float ANetTPSCharacter::GetHP()
{
	return hp;
}

void ANetTPSCharacter::SetHP(float value)
{
	hp = value;
	// UI 에 값 할당
	float percent = hp / MaxHP;
	if (mainUI)
	{
		mainUI->hp = percent;
	}
	else
	{
		auto hpUI = Cast<UHealthBar>(hpUIComp->GetWidget());
		hpUI->hp = percent;
	}
}

void ANetTPSCharacter::DamageProcess()
{
	HP--;

	// 죽음처리
	if (HP <= 0)
	{
		isDead = true;
	}
}

void ANetTPSCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	PrintNetLog();
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
	// 2. 총이있어야한다.(월드에서 모두다 찾아오자)
	TArray<AActor*> allActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), allActors);
	for (auto tempPistol : allActors)
	{
		// 2-1. 총인지 검사해야한다.
		if (tempPistol->GetName().Contains("BP_Pistol") == false)
		{
			continue;
		}
		// 3. 총과의 거리를 구해야한다.
		float distance = FVector::Dist(GetActorLocation(), tempPistol->GetActorLocation());
		// 4. 일정범위안에 있지 않으면
		if (distance > distanceToGun)
		{
			// 다시돌기
			continue;
		}
		// 5. 총을 소유하고싶다.
		bHasPistol = true;
		ownedPistol = tempPistol;
		ownedPistol->SetOwner(this);
		
		// 모든 클라이언트한테 전송
		MultiRPCTakePistol(ownedPistol);
		break;
	}
}

void ANetTPSCharacter::MultiRPCTakePistol_Implementation(AActor* pistolActor)
{
	// 6. 총을 잡고싶다.
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
}