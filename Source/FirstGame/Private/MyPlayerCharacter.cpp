// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"
#include "MyEnemyCharacter.h"
#include "MyHUD.h"

// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));

	LoadAssets();

	GetMesh()->SetupAttachment(GetCapsuleComponent());
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FQuat(FRotator(0.0f, -90.0f, 0.0f)));

	SpringArmComp->SetupAttachment(GetMesh());
	
	// USpringArmComponent::SocketName: 将相机附加到弹簧末端
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpringArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	bUseControllerRotationYaw = false;

	ArrowComp->SetupAttachment(GetCapsuleComponent());
	ArrowComp->SetRelativeLocation(FVector(60.0f, 0.0f, 50.0f));

	// 死亡物理动画模拟
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetMesh()->SetAnimInstanceClass(CharacterAnimationClass);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{

		PushDelegateSubscriber.BindUFunction(this, FName("OnPushEnded"));
		AnimInstance->OnMontageEnded.Add(PushDelegateSubscriber);

		//AttackStartDelegateSubscriber.BindUFunction(this, FName("OnAttackStarted"));
		//GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(AttackStartDelegateSubscriber);
		AnimInstance->OnPlayMontageNotifyBegin.AddDynamic(this, &AMyPlayerCharacter::OnAttackStarted);

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No AnimInstance"));
	}
}

// Called every frame
void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Push",IE_Pressed, this, &AMyPlayerCharacter::Push);
}

void AMyPlayerCharacter::MoveForward(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// 找出向前方向
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 获取向前矢量
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void AMyPlayerCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// 找出向右方向
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// 获取向右矢量
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// 添加该方向动作
		AddMovementInput(Direction, AxisValue);
	}
}

void AMyPlayerCharacter::Push()
{
	if (bCanPush)
	{
		bCanPush = false;
		GetMesh()->GetAnimInstance()->Montage_Play(PushMontage);

	}
}

void AMyPlayerCharacter::OnPushEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if(Montage == PushMontage)
		bCanPush = true;
}

void AMyPlayerCharacter::OnAttackStarted(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload)
{
	if (NotifyName=="StartPoint")
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
			// 打印命中结果
			//UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *(OutHit.GetActor()->GetName()));
			ApplyDamage(OutHit.GetActor(), Damage);
		}
		else
		{
			//UE_LOG(LogTemp, Warning, TEXT("No Hit"));
		}

	}
}

void AMyPlayerCharacter::ApplyDamage(AActor* DamagedActor, float BaseDamage)
{
	UGameplayStatics::ApplyDamage(DamagedActor, BaseDamage, nullptr, this, nullptr);
}

float AMyPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= DamageAmount;
	//UE_LOG(LogTemp, Warning, TEXT("Player takes damage %f"), DamageAmount);
	//UE_LOG(LogTemp, Warning, TEXT("%f"), Health);
	if (Health <= 0.0f)
	{
		GetMesh()->SetSimulatePhysics(true);

		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		DisableInput(PlayerController);

		if(!GetWorld()->GetTimerManager().IsTimerActive(DestoryTimerHandle))
			GetWorld()->GetTimerManager().SetTimer(DestoryTimerHandle, this, &AMyPlayerCharacter::DestoryCharacter, 2.0f, false);
	}
	else
	{
		// 添加被击动作
		AMyEnemyCharacter* EnemyCharacter = Cast<AMyEnemyCharacter>(DamageCauser);

		if (EnemyCharacter)
		{


			FVector AttackDirection = EnemyCharacter->GetActorForwardVector();
			FVector DamageDirection = AttackDirection * 150.0f + GetActorUpVector() * 100.0f;

			LaunchCharacter(DamageDirection, false, false);
		}
	}

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PlayerController)
		{
			AMyHUD* MyHUD = Cast<AMyHUD>(PlayerController->GetHUD());
			if (MyHUD)
			{
				//UE_LOG(LogTemp, Warning, TEXT("Found HUD"));
				UFunction* DamageScreenFunction = MyHUD->WavesWidget->FindFunction(FName("Damage Screen"));
				if (DamageScreenFunction)
				{
					//UE_LOG(LogTemp, Warning, TEXT("Found Function"));
					MyHUD->WavesWidget->ProcessEvent(DamageScreenFunction, nullptr);
				}
				
			}
		}
	}

	return ActualDamage;
}

void AMyPlayerCharacter::DestoryCharacter()
{
	Destroy();
}

void AMyPlayerCharacter::LoadAssets()
{
	// 载入资源文件 SkeletalMesh 与 AnimBlueprint
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_02.MESH_PC_02'"));
	if (MeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkeletalMesh Not Found!"));
	}
	static ConstructorHelpers::FClassFinder<UAnimInstance> AnimBPClass(TEXT("/Script/Engine.AnimBlueprint'/Game/Characters/Mannequin_UE4/OurAnimations/ABP_Player.ABP_Player_C'"));
	if (AnimBPClass.Succeeded()) {
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		CharacterAnimationClass = AnimBPClass.Class;
		GetMesh()->SetAnimInstanceClass(CharacterAnimationClass);
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


