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

	// ����ȭó�� �ش޶�� ����
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

	// Yaw ���� �������� P = P0 + vt ȸ���ӵ� 50
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
	// �����ȿ� �ִ� ���� ����� ĳ���� ã�Ƽ� owner �� ����
	if (HasAuthority())
	{
		// ������ �� owner ����
		AActor* newOwner = nullptr;
		float minDist = searchDistance;

		for (TActorIterator<ANetTPSCharacter> it(GetWorld()); it; ++it)
		{
			AActor* otherActor = *it;
			float dist = GetDistanceTo(otherActor);

			// ������ ����ϰ� �ִ� �Ÿ����� �۴ٸ�
			if (dist < minDist)
			{
				// -> �Ÿ�������Ʈ�ϰ�
				minDist = dist;
				// -> owner ����
				newOwner = otherActor;
			}
		}

		// Owner ����
		// ���� owner �ϰ� �ٸ� �༮�̸� owner ������Ʈ
		if (GetOwner() != newOwner)
		{
			SetOwner(newOwner);
		}
	}

	// ���� �ð�ȭ
	DrawDebugSphere(GetWorld(), GetActorLocation(), searchDistance, 30, FColor::White, false, 0, 0, 1);
}



void ANetActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANetActor, rotYaw);
	DOREPLIFETIME(ANetActor, matColor);
}
