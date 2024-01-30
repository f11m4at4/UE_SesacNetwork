// Fill out your copyright notice in the Description page of Project Settings.


#include "NetActor.h"
#include "NetTPS.h"
#include <../../../../../../../Source/Runtime/Engine/Public/EngineUtils.h>
#include "NetTPSCharacter.h"
#include <Net/UnrealNetwork.h>

ANetActor::ANetActor()
{
	PrimaryActorTick.bCanEverTick = true;

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = meshComp;

	// 동기화처리 해달라고 설정
	bReplicates = true;

	//NetUpdateFrequency = 1.0f;
}

// Called when the game starts or when spawned
void ANetActor::BeginPlay()
{
	Super::BeginPlay();
	
	mat = meshComp->CreateDynamicMaterialInstance(0);
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(handle, FTimerDelegate::CreateLambda([&]
			{
				FLinearColor matColor = FLinearColor(FMath::RandRange(0, 1), FMath::RandRange(0, 1), FMath::RandRange(0, 1), 1);
				//OnRep_MatColor();
				ServerRPC_ChangeColor(matColor);
			}), 1, true);
	}
}

void ANetActor::OnRep_MatColor()
{
	if (mat)
	{
		mat->SetVectorParameterValue(TEXT("FloorColor"), matColor);
	}
}

void ANetActor::ServerRPC_ChangeColor_Implementation(const FLinearColor newColor)
{
	MultiRPC_ChangeColor(newColor);
}

bool ANetActor::ServerRPC_ChangeColor_Validate(const FLinearColor newColor)
{
	return true;
}

void ANetActor::ClientRPC_ChangeColor_Implementation(const FLinearColor newColor)
{
	if (mat)
	{
		mat->SetVectorParameterValue(TEXT("FloorColor"), newColor);
	}
}

void ANetActor::MultiRPC_ChangeColor_Implementation(const FLinearColor newColor)
{
	if (mat)
	{
		mat->SetVectorParameterValue(TEXT("FloorColor"), newColor);
	}
}

void ANetActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	GetWorldTimerManager().ClearTimer(handle);
}

// Called every frame
void ANetActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	PrintNetLog();

	FindOwner();

	// Yaw 축을 기준으로 P = P0 + vt 회전속도 50
	if (HasAuthority())
	{
		AddActorLocalRotation(FRotator(0, 50 * DeltaTime, 0));
		rotYaw = GetActorRotation().Yaw;
	}
	
	
	//else
	//{
	//	FRotator newRot = GetActorRotation();
	//	newRot.Yaw = rotYaw;
	//	SetActorRotation(newRot);
	//}
}

void ANetActor::OnRep_RotYaw()
{
	FRotator newRot = GetActorRotation();
	newRot.Yaw = rotYaw;
	SetActorRotation(newRot);
}



void ANetActor::PrintNetLog()
{
	const FString connStr = GetNetConnection() != nullptr ? TEXT("Valid Connection") : TEXT("Invalid Connection");
	const FString ownerStr = GetOwner() != nullptr ? GetOwner()->GetName() : TEXT("No Owner");

	const FString logStr = FString::Printf(TEXT("Connection : %s\nOwner:%s\nLocal Role:%s\nRemote Role:%s"), *connStr, *ownerStr, *LOCALROLE, *REMOTEROLE);

	DrawDebugString(GetWorld(), GetActorLocation(), logStr, nullptr, FColor::Yellow, 0, true, 1);
}

void ANetActor::FindOwner()
{
	// 영역안에 있는 가장 가까운 캐릭터 찾아서 owner 로 설정
	if (HasAuthority())
	{
		// 저장할 새 owner 변수
		AActor* newOwner = nullptr;
		float minDist = searchDistance;

		for (TActorIterator<ANetTPSCharacter> it(GetWorld()); it; ++it)
		{
			AActor* otherActor = *it;
			float dist = GetDistanceTo(otherActor);

			// 이전에 기록하고 있는 거리보다 작다면
			if (dist < minDist)
			{
				// -> 거리업데이트하고
				minDist = dist;
				// -> owner 설정
				newOwner = otherActor;
			}
		}

		// Owner 설정
		// 기존 owner 하고 다른 녀석이면 owner 업데이트
		if (GetOwner() != newOwner)
		{
			SetOwner(newOwner);
		}
	}

	// 영역 시각화
	DrawDebugSphere(GetWorld(), GetActorLocation(), searchDistance, 30, FColor::White, false, 0, 0, 1);
}



void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetActor, rotYaw);
	DOREPLIFETIME(ANetActor, matColor);
}
