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
	
	// USpringArmComponent::SocketName: ��������ӵ�����ĩ��
	CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);

	SpringArmComp->SetRelativeLocation(FVector(0.0f, 0.0f, 90.0f));
	SpringArmComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bIgnoreBaseRotation = true;
	bUseControllerRotationYaw = false;

	ArrowComp->SetupAttachment(GetCapsuleComponent());
	ArrowComp->SetRelativeLocation(FVector(60.0f, 0.0f, 50.0f));

	// ����������ģ��
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
		// �ҳ���ǰ����
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ��ȡ��ǰʸ��
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void AMyPlayerCharacter::MoveRight(float AxisValue)
{
	if ((Controller != nullptr) && (AxisValue != 0.0f))
	{
		// �ҳ����ҷ���
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// ��ȡ����ʸ��
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// ��Ӹ÷�����
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
			GetWorld(), // ��ǰ����
			TraceStart, // ׷����ʼλ��
			TraceEnd, // ׷�ٽ���λ��
			TraceRadius, // ׷�ٰ뾶
			ObjectTypes, // ׷��Ŀ���������
			false, // �Ƿ�ִ�и��ӵ���ײ���
			IgnoreActors, // ���Ե���ײ��������
			EDrawDebugTrace::None, // �Ƿ���Ƶ�������
			OutHit, // ���ص���ײ���
			true // �Ƿ��ǲ�ѯ���Ӷ�
		);

		if (bHit)
		{
			// ��ӡ���н��
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
		// ��ӱ�������
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
	// ������Դ�ļ� SkeletalMesh �� AnimBlueprint
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


