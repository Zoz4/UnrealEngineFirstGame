// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"

// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	ArrowComp = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComp"));

	// 载入资源文件 SkeletalMesh 与 AnimBlueprint
	static ConstructorHelpers::FObjectFinder<USkeletalMesh>MeshAsset(TEXT("/Script/Engine.SkeletalMesh'/Game/SCK_Casual01/Models/Premade_Characters/MESH_PC_03.MESH_PC_03'"));
	if (MeshAsset.Succeeded()) {
		GetMesh()->SetSkeletalMesh(MeshAsset.Object);
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
}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	FScriptDelegate PushDelegateSubscriber;
	PushDelegateSubscriber.BindUFunction(this, FName("OnPushEnded"));
	GetMesh()->GetAnimInstance()->OnMontageEnded.Add(PushDelegateSubscriber);

	FScriptDelegate AttackStartDelegateSubscriber;
	AttackStartDelegateSubscriber.BindUFunction(this, FName("OnAttackStarted"));
	GetMesh()->GetAnimInstance()->OnPlayMontageNotifyBegin.Add(AttackStartDelegateSubscriber);


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
		
	}
}


