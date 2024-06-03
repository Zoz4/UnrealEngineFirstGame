// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCharacter.h"
#include "MyPlayerCharacter.h"

// Sets default values
AMyEnemyCharacter::AMyEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));
	AIControllerClass = AMyEnemyAIController::StaticClass();

	LoadAssets();

	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));
	ArrowComp->SetupAttachment(GetCapsuleComponent());
	ArrowComp->SetRelativeLocation(FVector(60.0f, 0.0f, 50.0f));
	GetCharacterMovement()->MaxWalkSpeed = 250.0f;


	// 防止与相机发生碰撞
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	

}

// Called when the game starts or when spawned
void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();


	FScriptDelegate PushDelegateSubscriber;
	PushDelegateSubscriber.BindUFunction(this, FName("OnPushEnded"));
	GetMesh()->GetAnimInstance()->OnMontageEnded.Add(PushDelegateSubscriber);

	FScriptDelegate AttackStartDelegateSubscriber;
	AttackStartDelegateSubscriber.BindUFunction(this, FName("OnAttackStarted"));
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(AttackStartDelegateSubscriber);
	
	ChasePlayer();
	
}

// Called every frame
void AMyEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void AMyEnemyCharacter::Push()
{
	if (bCanPush)
	{
		bCanPush = false;
		GetMesh()->GetAnimInstance()->Montage_Play(PushMontage);
	}
}

void AMyEnemyCharacter::OnPushEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == PushMontage)
		bCanPush = true;
}

void AMyEnemyCharacter::OnAttackStarted(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName == "StartPoint")
	{
		FVector TraceStart = ArrowComp->GetComponentLocation();
		FVector TraceEnd = TraceStart + ArrowComp->GetForwardVector() * 100.0f;
		float TraceRadius = 30.0f;

		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(this);


		FHitResult OutHit;

		bool bHit = UKismetSystemLibrary::SphereTraceSingleForObjects(
			GetWorld(), // 当前世界
			TraceStart, // 追踪起始位置
			TraceEnd, // 追踪结束位置
			TraceRadius, // 追踪半径
			ObjectTypes, // 追踪目标对象类型
			false, // 是否执行复杂的碰撞检测
			IgnoreActors, // 忽略的碰撞对象数组
			EDrawDebugTrace::None, // 是否绘制调试线条
			OutHit, // 返回的碰撞结果
			true // 是否考虑查询复杂度
		);

		if (bHit)
		{
			// 命中
			if (AMyPlayerCharacter* HitPlayerCharacter = Cast<AMyPlayerCharacter>(OutHit.GetActor()))
			{
				// UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *(OutHit.GetActor()->GetName()));
				ApplyDamage(OutHit.GetActor(), Damage);
			}
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("No Hit"));
		}
	}
}

void AMyEnemyCharacter::ApplyDamage(AActor* DamagedActor, float BaseDamage)
{
	UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, nullptr, this, nullptr);
}

float AMyEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//UE_LOG(LogTemp, Warning, TEXT("Enemy takes damage %f"), DamageAmount);
	Health -= ActualDamage;
	if (Health <= 0.0f)
	{
		GetMesh()->SetSimulatePhysics(true);
		GetCharacterMovement()->DisableMovement();
		GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
		GetWorld()->GetTimerManager().SetTimer(DestoryTimerHandle, this, &AMyEnemyCharacter::DestoryCharacter, 2.0f, false);
	}
	else
	{
		// 添加被攻击的动作
		AMyPlayerCharacter* PlayerCharacter = Cast<AMyPlayerCharacter>(DamageCauser);

		if (PlayerCharacter)
		{
			FVector AttackDirection = PlayerCharacter->GetActorForwardVector();
			FVector DamageDirection = AttackDirection * 150.0f + GetActorUpVector() * 100.0f;
			LaunchCharacter(DamageDirection, false, false);
		}

	}
	return ActualDamage;
}

void AMyEnemyCharacter::ChasePlayer()
{
	AMyEnemyAIController* AIController = Cast<AMyEnemyAIController>(GetController());
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AIController->MoveToActor(PlayerController->GetPawn());
}

void AMyEnemyCharacter::DestoryCharacter()
{
	Destroy();
}

void AMyEnemyCharacter::LoadAssets()
{
	static TArray<ConstructorHelpers::FObjectFinder<USkeletalMesh>>MeshAssets
	{
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_00.MESH_PC_00'")),
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_01.MESH_PC_01'")),
		ConstructorHelpers::FObjectFinder<USkeletalMesh>(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_03.MESH_PC_03'"))
	};
	bool bSucceeded = true;
	for (auto& MeshAsset : MeshAssets)
	{
		if (!MeshAsset.Succeeded())
		{
			bSucceeded = false;
			break;
		}
	}
	if (bSucceeded) {
		int32 index = FMath::RandRange(0, MeshAssets.Num() - 1);
		GetMesh()->SetSkeletalMesh(MeshAssets[index].Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Not Found!"));
	}
	static ConstructorHelpers::FObjectFinder<UAnimBlueprint> AnimBPAsset(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequin_UE4/OurAnimations/ABP_Player.ABP_Player'"));
	if (AnimBPAsset.Succeeded()) {
		GetMesh()->SetAnimInstanceClass(AnimBPAsset.Object->GeneratedClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimBlueprint Not Found!"));
	}
	static ConstructorHelpers::FObjectFinder<UAnimMontage>PushMontageAsset(TEXT("/Script/Engine.AnimMontage'/Game/Characters/Mannequin_UE4/OurAnimations/Push_Montage.Push_Montage'"));
	if (PushMontageAsset.Succeeded())
	{
		PushMontage = PushMontageAsset.Object;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PushMontageAsset Not Found!"));
	}

}


