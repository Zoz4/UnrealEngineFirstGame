// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"

#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "MyPlayerCharacter.generated.h"

UCLASS()
class FIRSTGAME_API AMyPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

// Functions
public:
	void MoveForward(float InputAxis);
	void MoveRight(float InputAxis);
	void Push();

	UFUNCTION()
	void OnPushEnded(UAnimMontage* Montage, bool bInterrupted);
	UFUNCTION()
	void OnAttackStarted(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	void ApplyDamage(AActor* DamagedActor, float BaseDamage);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	void DestoryCharacter();

protected:
private:
	inline void LoadAssets();

// Properties
public:
	friend class AMyEnemyAIController;
protected:
	// 跟随玩家背后摄像机的弹簧臂组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class USpringArmComponent* SpringArmComp;
	// 玩家跟随摄像机
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UCameraComponent* CameraComp;
	// 箭头组件
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UArrowComponent* ArrowComp;

	UPROPERTY(BlueprintReadOnly)
	float MaxHealth = 101.0f;

	UPROPERTY(BlueprintReadOnly)
	float Health = MaxHealth;
	
private:
	UPROPERTY()
	bool bCanPush = true;
	class UAnimMontage* PushMontage;
	float Damage = 10.0f;
	FTimerHandle DestoryTimerHandle;
};
